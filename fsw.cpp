#include "config.h"
#include "fsw.h"
#include "fsw_log.h"
#include <iostream>
#include <csignal>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <cerrno>
#include <vector>
#ifdef HAVE_GETOPT_LONG
#include <getopt.h>
#endif
// TODO: invert if
#ifndef HAVE_CORESERVICES_CORESERVICES_H
#include "fsevent_watcher.h"
#else
#include "kqueue_watcher.h"
#endif

using namespace std;

static watcher *watcher = nullptr;
static bool fflag = false;
static bool nflag = false;
static bool lflag = false;
static bool rflag = false;
static bool tflag = false;
static bool uflag = false;
static bool vflag = false;
static double lvalue = 1.0;
static string tformat = "%c";

bool is_verbose()
{
  return vflag;
}

static void usage()
{
#ifdef HAVE_GETOPT_LONG
  cout << PACKAGE_STRING << "\n\n";
  cout << "Usage:\n";
  cout << PACKAGE_NAME << " [OPTION] ... path ...\n";
  cout << "\n";
  cout << "Options:\n";
  cout << " -f, --format-time     Print the event time using the specified\n";
  cout << "                       format.\n";
  cout << " -h, --help            Show this message.\n";
  cout << " -l, --latency=DOUBLE  Set the latency.\n";
  cout << " -n, --numeric         Print numeric event mask.\n";
  cout << " -r, --recursive       Recurse subdirectories.\n";
  cout << " -t, --timestamp       Print the event timestamp.\n";
  cout << " -u, --utc-time        Print the event time as UTC time.\n";
  cout << " -v, --verbose         Print verbose output.\n";
  cout << "\n";
  cout << "See the man page for more information.";
  cout << endl;
#else
  cout << PACKAGE_STRING << "\n\n";
  cout << "Syntax:\n";
  cout << PACKAGE_NAME << " [-fhlnrtuv] path ...\n";
  cout << "\n";
  cout << "Usage:\n";
  cout << " -f  Print the event time stamp with the specified format.\n";
  cout << " -h  Show this message.\n";
  cout << " -l  Set the latency.\n";
  cout << " -n  Print numeric event masks.\n";
  cout << " -r  Recurse subdirectories.\n";
  cout << " -t  Print the event timestamp.\n";
  cout << " -u  Print the event time as UTC time.\n";
  cout << " -v  Print verbose output.\n";
  cout << "\n";
  cout << "See the man page for more information.";
  cout << endl;
#endif
  exit(FSW_EXIT_USAGE);
}

static void close_stream()
{
  if (watcher)
  {
    delete watcher;

    watcher = nullptr;
  }
}

static void close_handler(int signal)
{
  close_stream();

  fsw_log("Done.\n");
  exit(FSW_EXIT_OK);
}

static bool validate_latency(double latency, ostream &ost, ostream &est)
{
  if (lvalue == 0.0)
  {
    est << "Invalid value: " << optarg << endl;
    return false;
  }

  if (errno == ERANGE || lvalue == HUGE_VAL)
  {
    est << "Value out of range: " << optarg << endl;
    return false;
  }

  if (is_verbose())
  {
    ost << "Latency set to: " << lvalue << endl;
  }

  return true;
}

static void register_signal_handlers()
{
  struct sigaction action;
  action.sa_handler = close_handler;
  sigemptyset(&action.sa_mask);
  action.sa_flags = 0;

  if (sigaction(SIGTERM, &action, nullptr) == 0)
  {
    fsw_log("SIGTERM handler registered.\n");
  }
  else
  {
    cerr << "SIGTERM handler registration failed." << endl;
  }

  if (sigaction(SIGABRT, &action, nullptr) == 0)
  {
    fsw_log("SIGABRT handler registered.\n");
  }
  else
  {
    cerr << "SIGABRT handler registration failed." << endl;
  }

  if (sigaction(SIGINT, &action, nullptr) == 0)
  {
    fsw_log("SIGINT handler registered.\n");
  }
  else
  {
    cerr << "SIGINT handler registration failed" << endl;
  }
}

static vector<string> decode_event_flag_name(vector<event_flag> flags)
{
  vector<string> names;

  for (event_flag flag : flags)
  {
    switch (flag)
    {
    case event_flag::PlatformSpecific:
      names.push_back("PlatformSpecific");
      break;
    case event_flag::Created:
      names.push_back("Created");
      break;
    case event_flag::Updated:
      names.push_back("Updated");
      break;
    case event_flag::Removed:
      names.push_back("Removed");
      break;
    case event_flag::Renamed:
      names.push_back("Renamed");
      break;
    case event_flag::OwnerModified:
      names.push_back("OwnerModified");
      break;
    case event_flag::AttributeModified:
      names.push_back("AttributeModified");
      break;
    case event_flag::IsFile:
      names.push_back("IsFile");
      break;
    case event_flag::IsDir:
      names.push_back("IsDir");
      break;
    case event_flag::IsSymLink:
      names.push_back("IsSymLink");
      break;
    case event_flag::Link:
      names.push_back("Link");
      break;
    default:
      names.push_back("<Unknown>");
      break;
    }
  }

  return names;
}

static const unsigned int TIME_FORMAT_BUFF_SIZE = 128;

static void print_event_timestamp(const time_t &evt_time)
{
  char time_format_buffer[TIME_FORMAT_BUFF_SIZE];
  struct tm * tm_time = uflag ? gmtime(&evt_time) : localtime(&evt_time);

  string date =
      strftime(
          time_format_buffer,
          TIME_FORMAT_BUFF_SIZE,
          tformat.c_str(),
          tm_time) ? string(time_format_buffer) : string("<date format error>");

  cout << date << " - ";
}

static void process_events(const vector<event> &events)
{
  for (event evt : events)
  {
    vector<event_flag> flags = evt.get_flags();
    vector<string> flag_names = decode_event_flag_name(flags);

    if (tflag)
      print_event_timestamp(evt.get_time());

    cout << evt.get_path();
    cout << " - ";

    for (string name : flag_names)
    {
      cout << name << " ";
    }

    cout << endl;
  }
}

static void start_watcher(int argc, char ** argv, int optind)
{
  // parsing paths
  vector<string> paths;

  for (auto i = optind; i < argc; ++i)
  {
    fsw_log("Adding path: ");
    fsw_log(argv[i]);
    fsw_log("\n");

    paths.push_back(argv[i]);
  }

// TODO: invert if
#ifndef HAVE_CORESERVICES_CORESERVICES_H
  watcher = new fsevent_watcher(paths, process_events);
#else
  watcher = new kqueue_watcher(paths, process_events);
#endif
  watcher->set_latency(lvalue);
  watcher->set_recursive(rflag);

  watcher->run();
}

static void parse_opts(int argc, char ** argv)
{
  int ch;
  const char *short_options = "f:hl:nrtuv";

#ifdef HAVE_GETOPT_LONG
  int option_index = 0;
  static struct option long_options[] =
  {
  { "format-time", required_argument, nullptr, 'f' },
  { "help", no_argument, nullptr, 'h' },
  { "latency", required_argument, nullptr, 'l' },
  { "numeric", no_argument, nullptr, 'n' },
  { "recursive", no_argument, nullptr, 'r' },
  { "timestamp", no_argument, nullptr, 't' },
  { "utc-time", no_argument, nullptr, 'u' },
  { "verbose", no_argument, nullptr, 'v' },
  { nullptr, 0, nullptr, 0 } };

  while ((ch = getopt_long(
      argc,
      argv,
      short_options,
      long_options,
      &option_index)) != -1)
  {
#else
    while ((ch = getopt(argc, argv, short_options)) != -1)
    {
#endif

    switch (ch)
    {

    case 'f':
      fflag = true;
      tformat = string(optarg);
      break;

    case 'h':
      usage();
      exit(FSW_EXIT_USAGE);

    case 'l':
      lflag = true;
      lvalue = strtod(optarg, nullptr);

      if (!validate_latency(lvalue, cout, cerr))
      {
        exit(FSW_EXIT_LATENCY);
      }

      break;

    case 'n':
      nflag = true;
      break;

    case 'r':
      rflag = true;
      break;

    case 't':
      tflag = true;
      break;

    case 'u':
      uflag = true;
      break;

    case 'v':
      vflag = true;
      break;

    default:
      usage();
      exit(FSW_EXIT_UNK_OPT);
    }
  }
}

int main(int argc, char ** argv)
{
  parse_opts(argc, argv);

  if (optind == argc)
  {
    cerr << "Invalid number of arguments." << endl;
    exit(FSW_EXIT_UNK_OPT);
  }

  try
  {
    // registering handlers to clean up resources
    register_signal_handlers();

    // configure and start the event loop
    start_watcher(argc, argv, optind);
  }
  catch (exception & conf)
  {
    cerr << "An error occurred and the program will be terminated.\n";
    cerr << conf.what() << endl;

    return FSW_EXIT_ERROR;
  }
  catch (...)
  {
    cerr << "An unknown error occurred and the program will be terminated.\n";

    return FSW_EXIT_ERROR;
  }

  return FSW_EXIT_OK;
}

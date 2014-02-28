#include "config.h"
#include "fsw.h"
#include "fsw_log.h"
#include <iostream>
#include <sstream>
#include <csignal>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <cerrno>
#include <vector>
#ifdef HAVE_GETOPT_LONG
#include <getopt.h>
#endif
#ifdef HAVE_CORESERVICES_CORESERVICES_H
#include "fsevent_watcher.h"
#endif
#ifdef HAVE_SYS_EVENT_H
#include "kqueue_watcher.h"
#endif
#include "poll_watcher.h"

using namespace std;

static watcher *watcher = nullptr;
static vector<string> exclude_regex;
static bool _0flag = false;
static bool Eflag = false;
static bool fflag = false;
static bool iflag = false;
static bool kflag = false;
static bool nflag = false;
static bool lflag = false;
static bool pflag = false;
static bool rflag = false;
static bool tflag = false;
static bool uflag = false;
static bool vflag = false;
static bool xflag = false;
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
  cout
      << " -0, --print0          Use the ASCII NUL character (0) as line separator.\n";
#ifdef HAVE_REGCOMP
  cout << " -e, --exclude=REGEX   Exclude paths matching REGEX.\n";
  cout << " -E, --extended        Use exended regular expressions.\n";
#endif
  cout
      << " -f, --format-time     Print the event time using the specified format.\n";
  cout << " -h, --help            Show this message.\n";
#ifdef HAVE_REGCOMP
  cout << " -i, --insensitive     Use case insensitive regular expressions.\n";
#endif
#if defined(HAVE_SYS_EVENT_H)
  cout << " -k, --kqueue          Use the kqueue watcher.\n";
#endif
  cout << " -l, --latency=DOUBLE  Set the latency.\n";
  cout << " -n, --numeric         Print a numeric event mask.\n";
  cout << " -p, --poll            Use the poll watcher.\n";
  cout << " -r, --recursive       Recurse subdirectories.\n";
  cout << " -t, --timestamp       Print the event timestamp.\n";
  cout << " -u, --utc-time        Print the event time as UTC time.\n";
  cout << " -v, --verbose         Print verbose output.\n";
  cout << " -x, --event-flags     Print the event flags.\n";
  cout << "\n";
  cout << "See the man page for more information.";
  cout << endl;
#else
  string option_string = "[";
#ifdef HAVE_REGCOMP
  option_string += "eE";
#endif
  option_string += "fh";
#ifdef HAVE_REGCOMP
  option_string += "i";
#endif
#ifdef HAVE_SYS_EVENT_H
  option_string += "k";
#endif
  option_string += "lnprtuvx";
  option_string += "]";

  cout << PACKAGE_STRING << "\n\n";
  cout << "Syntax:\n";
  cout << PACKAGE_NAME << " " << option_string << " path ...\n";
  cout << "\n";
  cout << "Usage:\n";
  cout << " -0  Use the ASCII NUL character (0) as line separator.\n";
  cout << " -e  Exclude paths matching REGEX.\n";
  cout << " -E  Use exended regular expressions.\n";
  cout << " -f  Print the event time stamp with the specified format.\n";
  cout << " -h  Show this message.\n";
  cout << " -i  Use case insensitive regular expressions.\n";
#ifdef HAVE_SYS_EVENT_H
  cout << " -k  Use the kqueue watcher.\n";
#endif
  cout << " -l  Set the latency.\n";
  cout << " -n  Print a numeric event masks.\n";
  cout << " -p  Use the poll watcher.\n";
  cout << " -r  Recurse subdirectories.\n";
  cout << " -t  Print the event timestamp.\n";
  cout << " -u  Print the event time as UTC time.\n";
  cout << " -v  Print verbose output.\n";
  cout << " -x  Print the event flags.\n";
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

  cout << date << " ";
}

static void print_event_flags(const vector<event_flag> &flags)
{
  if (nflag)
  {
    int mask = 0;
    for (const event_flag &flag : flags)
    {
      mask += static_cast<int>(flag);
    }

    cout << " " << mask;
  }
  else
  {
    vector<string> flag_names = decode_event_flag_name(flags);

    for (string &name : flag_names)
    {
      cout << " " << name;
    }
  }
}

static void process_events(const vector<event> &events)
{
  for (const event &evt : events)
  {
    vector<event_flag> flags = evt.get_flags();

    if (tflag)
      print_event_timestamp(evt.get_time());

    cout << evt.get_path();

    if (xflag)
      print_event_flags(flags);

    if (_0flag)
    {
      cout << '\0';
      flush(cout);
    }
    else
    {
      cout << endl;
    }
  }
}

static void start_watcher(int argc, char ** argv, int optind)
{
  // parsing paths
  vector<string> paths;

  for (auto i = optind; i < argc; ++i)
  {
    char *real_path = ::realpath(argv[i], nullptr);
    string path = (real_path ? real_path : argv[i]);

    fsw_log("Adding path: ");
    fsw_log(path.c_str());
    fsw_log("\n");

    paths.push_back(path);
  }

  if (pflag)
  {
    watcher = new poll_watcher(paths, process_events);
  }
  else if (kflag)
  {
    watcher = new kqueue_watcher(paths, process_events);
  }
  else
  {
#if defined(HAVE_CORESERVICES_CORESERVICES_H)
    watcher = new fsevent_watcher(paths, process_events);
#elif defined(HAVE_SYS_EVENT_H)
    watcher = new kqueue_watcher(paths, process_events);
#else
    watcher = new poll_watcher(paths, process_events);
#endif
  }

  watcher->set_latency(lvalue);
  watcher->set_recursive(rflag);
  watcher->set_exclude(exclude_regex, !iflag, Eflag);

  watcher->run();
}

static void parse_opts(int argc, char ** argv)
{
  int ch;
  ostringstream short_options;

  short_options << "0f:hkl:nprtuvx";
#ifdef HAVE_REGCOMP
  short_options << "e:Ei";
#endif
#ifdef HAVE_SYS_EVENT_H
  short_options << "k";
#endif

#ifdef HAVE_GETOPT_LONG
  int option_index = 0;
  static struct option long_options[] =
  {
  { "print0", no_argument, nullptr, '0'},
#ifdef HAVE_REGCOMP
{ "exclude", required_argument, nullptr, 'e'},
{ "extended", no_argument, nullptr, 'E'},
#endif
{ "format-time", required_argument, nullptr, 'f'},
{ "help", no_argument, nullptr, 'h'},
#ifdef HAVE_REGCOMP
{ "insensitive", no_argument, nullptr, 'i'},
#endif
#ifdef HAVE_SYS_EVENT_H
{ "kqueue", no_argument, nullptr, 'k'},
#endif
{ "latency", required_argument, nullptr, 'l'},
{ "numeric", no_argument, nullptr, 'n'},
{ "poll", no_argument, nullptr, 'p'},
{ "recursive", no_argument, nullptr, 'r'},
{ "timestamp", no_argument, nullptr, 't'},
{ "utc-time", no_argument, nullptr, 'u'},
{ "verbose", no_argument, nullptr, 'v'},
{ "event-flags", no_argument, nullptr, 'x'},
{ nullptr, 0, nullptr, 0}};

  while ((ch = getopt_long(
      argc,
      argv,
      short_options.str().c_str(),
      long_options,
      &option_index)) != -1)
  {
#else
    while ((ch = getopt(argc, argv, short_options.str().c_str())) != -1)
    {
#endif

    switch (ch)
    {
    case '0':
      _0flag = true;
      break;

#ifdef HAVE_REGCOMP
    case 'e':
      exclude_regex.push_back(optarg);
      break;

    case 'E':
      Eflag = true;
      break;
#endif

    case 'f':
      fflag = true;
      tformat = string(optarg);
      break;

    case 'h':
      usage();
      exit(FSW_EXIT_USAGE);

#ifdef HAVE_REGCOMP
    case 'i':
      iflag = true;
      break;
#endif

#ifdef HAVE_SYS_EVENT_H
    case 'k':
      kflag = true;
      break;
#endif

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
      xflag = true;
      break;

      case 'p':
      pflag = true;
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

      case 'x':
      xflag = true;
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

    // configure and start the watcher loop
    start_watcher(argc, argv, optind);
  } catch (exception & conf)
  {
    cerr << "An error occurred and the program will be terminated.\n";
    cerr << conf.what() << endl;

    return FSW_EXIT_ERROR;
  } catch (...)
  {
    cerr << "An unknown error occurred and the program will be terminated.\n";

    return FSW_EXIT_ERROR;
  }

  return FSW_EXIT_OK;
}

#include "config.h"
#include "fsw.h"
#include "fsw_log.h"
#include <iostream>
#include <csignal>
#include <cstdlib>
#include <cmath>
#include <cerrno>
#include <vector>
#ifdef HAVE_GETOPT_LONG
#include <getopt.h>
#endif
#ifdef HAVE_CORESERVICES_CORESERVICES_H
#include "fsevent_watcher.h"
#else
#include "kqueue_watcher.h"
#endif

using namespace std;

static watcher *watcher = nullptr;
static bool nflag = false;
static bool lflag = false;
static bool tflag = false;
static bool uflag = false;
static bool vflag = false;
static double lvalue = 1.0;
static string tformat = "%c";

bool is_verbose()
{
  return vflag;
}

void usage()
{
#ifdef HAVE_GETOPT_LONG
  cout << PACKAGE_STRING << "\n\n";
  cout << "Usage:\n";
  cout << PACKAGE_NAME << " [OPTION] ... path ...\n";
  cout << "\n";
  cout << "Options:\n";
  cout << " -h, --help            Show this message.\n";
  cout << " -l, --latency=DOUBLE  Set the latency.\n";
  cout << " -n, --numeric         Print numeric event mask.\n";
  cout << " -t, --time-format     Print the event time using the specified\n";
  cout << "                       format.\n";
  cout << " -u, --utc-time        Print the event time as UTC time.\n";
  cout << " -v, --verbose         Print verbose output.\n";
  cout << "\n";
  cout << "See the man page for more information.";
  cout << endl;
#else
  cout << PACKAGE_STRING << "\n\n";
  cout << "Syntax:\n";
  cout << PACKAGE_NAME << " [-hlntuv] path ...\n";
  cout << "\n";
  cout << "Usage:\n";
  cout << " -h  Show this message.\n";
  cout << " -l  Set the latency.\n";
  cout << " -n  Print numeric event masks.\n";
  cout << " -t  Print the event time stamp with the specified format.\n";
  cout << " -u  Print the event time as UTC time.\n";
  cout << " -v  Print verbose output.\n";
  cout << "\n";
  cout << "See the man page for more information.";
  cout << endl;
#endif
  exit(FSW_EXIT_USAGE);
}

void close_stream()
{
  if (watcher)
  {
    delete watcher;

    watcher = nullptr;
  }
}

void close_handler(int signal)
{
  close_stream();

  fsw_log("Done.\n");
  exit(FSW_EXIT_OK);
}

bool validate_latency(double latency, ostream &ost, ostream &est)
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

void register_signal_handlers()
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

void start_event_loop(int argc, char ** argv, int optind)
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

#ifdef HAVE_CORESERVICES_CORESERVICES_H
  watcher = new fsevent_watcher(paths, nullptr);
#else
  watcher = new kqueue_watcher(paths, nullptr);
#endif
//  watcher->set_latency(lvalue);
//  watcher->set_numeric_event(nflag);
//  watcher->set_time_format(tformat);
//  watcher->set_utc_time(uflag);

  watcher->run();
}

int main(int argc, char ** argv)
{
  int ch;
  const char *short_options = "hl:nt:uv";

#ifdef HAVE_GETOPT_LONG
  int option_index = 0;
  static struct option long_options[] =
  {
  { "help", no_argument, 0, 'h' },
  { "latency", required_argument, 0, 'l' },
  { "numeric", no_argument, 0, 'n' },
  { "time-format", required_argument, 0, 't' },
  { "utc-time", no_argument, 0, 'u' },
  { "verbose", no_argument, 0, 'v' },
  { 0, 0, 0, 0 } };

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

    case 't':
      tflag = true;
      tformat = string(optarg);
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
    start_event_loop(argc, argv, optind);
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

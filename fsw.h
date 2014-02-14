#ifndef FSW_H
#define FSW_H

#include <exception>
#include <string>

using namespace std;

#define FSW_EXIT_OK      0
#define FSW_EXIT_UNK_OPT 1
#define FSW_EXIT_USAGE   2
#define FSW_EXIT_LATENCY 4
#define FSW_EXIT_STREAM  8
#define FSW_EXIT_ERROR  16

bool is_verbose();

class fsw_exception: public exception {
public:
  fsw_exception(string cause);
  virtual const char * what() const throw ();
  virtual ~fsw_exception() throw ();
private:
  const string cause;
};

#endif  /* FSW_H */

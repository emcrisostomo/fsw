#ifndef FSW__EXCEPTION_H
#define FSW__EXCEPTION_H

#include <exception>
#include <string>

using namespace std;

class fsw_exception: public exception {
public:
  fsw_exception(string cause);
  virtual const char * what() const throw ();
  virtual ~fsw_exception() throw ();
private:
  const string cause;
};

#endif  /* FSW__EXCEPTION_H */

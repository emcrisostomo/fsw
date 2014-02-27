#ifndef FSW_EXCEPTION_H
#define FSW_EXCEPTION_H

#include <exception>
#include <string>

class fsw_exception: public std::exception {
public:
  fsw_exception(std::string cause);
  virtual const char * what() const throw ();
  virtual ~fsw_exception() throw ();
private:
  const std::string cause;
};

#endif  /* FSW_EXCEPTION_H */

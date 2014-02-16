#include "fsw_exception.h"

using namespace std;

fsw_exception::fsw_exception(string cause) :
    cause(cause)
{
}

const char * fsw_exception::what() const throw ()
{
  return (string("Error: ") + this->cause).c_str();
}

fsw_exception::~fsw_exception() throw ()
{
}

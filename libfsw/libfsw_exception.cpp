/* 
 * Copyright (C) 2014, Enrico M. Crisostomo
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "libfsw_exception.h"

using namespace std;

libfsw_exception::libfsw_exception(string cause) :
  cause(cause)
{
}

const char * libfsw_exception::what() const throw ()
{
  return (string("Error: ") + this->cause).c_str();
}

libfsw_exception::~libfsw_exception() throw ()
{
}

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
#ifndef LIBFSW_EXCEPTION_H
#  define LIBFSW_EXCEPTION_H

#  include <exception>
#  include <string>

class libfsw_exception : public std::exception
{
public:
  libfsw_exception(std::string cause);
  virtual const char * what() const throw ();
  virtual ~libfsw_exception() throw ();
private:
  const std::string cause;
};

#endif  /* LIBFSW_EXCEPTION_H */

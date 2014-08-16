History
-------

[Alan Dipert][alan] wrote the first implementation of `fswatch` in 2009.  This 
version ran exclusively on OS X and relied on the [FSEvents][fse] API to get
change events from the OS.

At the end of 2013 [Enrico M. Crisostomo][enrico]
wrote `fsw` aiming at providing not only a drop-in replacement for `fswatch`,
but a common front-end from multiple file system change events APIs, including:

  * OS X FSEvents.
  * *BSD kqueue.
  * Linux inotify.

In April 2014 Alan and Enrico, in the best interest of users of either
`fswatch` and `fsw`, agreed on merging the two programs together.  At the same
time, Enrico was taking over `fswatch` as a maintainer.

As a consequence, development of `fswatch` will continue on its [main
repository][fswatchrepo] while the `fsw` repository will likely be frozen and
its documentation updated to redirect users to `fswatch`.

[alan]: http://alandipert.tumblr.com
[fse]: https://developer.apple.com/library/mac/documentation/Darwin/Reference/FSEvents_Ref/Reference/reference.html
[enrico]: http://thegreyblog.blogspot.com
[fswatchrepo]: https://github.com/emcrisostomo/fswatch

-----

Copyright (C) 2014 Enrico M. Crisostomo

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

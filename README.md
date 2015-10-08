# Redmine Command Line Interface

This project implements a command line interface (CLI) for the Redmine Project
Management Software. The primary goal is to allow developers to manipulating
Redmine issues although it also has limited support for manipulating Redmine
projects and users if the user has administrator access.

## Build

This project utilises the CMake build system, in order to build the project you
will need a C++11 capable compiler toolchain.

The project depends on libcurl to perform all HTTP requests and comes packaged
with curl source code, however the default build configuration will use the
system installed libcurl if it is available, however this is not the case on
systems such as Windows. You can optionally enable using the packaged version of
libcurl be setting the CMake option `REDMINE_BUILD_CURL=ON`, this is the default
on Windows.

In a Unix environment you can build the project as follows.

    git clone https://github.com:kbenzie/redmine.git
    cd redmine
    mkdir build && cd build
    cmake -DCMAKE_BUILD_TYPE=Release ..
    make

If you use a Windows environment you can build the project as follows.

    git clone https://github.com:kbenzie/redmine.git
    cd redmine
    mkdir build && cd build
    cmake -DCMAKE_BUILD_TYPE=Release ..
    MSBuild redmine.sln

# Contributions

Any pull requests are welcome, however do not expect frequent patches from
[myself](https://github.com/kbenzie) as this does what I need.

# Licence - MIT

Copyright (C) 2015 Kenenth Benzie

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

The curl sources have their own licence which is available
[here](https://github.com/bagder/curl/blob/master/COPYING).

The json source share the same licence which is available
[here](https://github.com/kbenzie/json/blob/master/licence.txt)

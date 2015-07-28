# A simple json library

The primary design goal of this library is to enable creation of new json
strings in as few lines of code as possible by making use of C++11 uniform
initialization. Secondary goals are to provide an iterator object interface and
to clearly follow the json specification.

## Build

CMake is used to generate platform specific build files, integration into other
CMake projects is easily supported using `add_directory`. Issue the following
commands to build the project after cloning.

### Unix

```
cd <json-dir>
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

### Windows

```
cd <json-dir>
mkdir build && cd build
cmake ..
JSON.sln
```

### Options

* `-DJSON_BUILD_TESTS=ON` enables building of the `UnitJSON` tests.
* `-DJSON_BUILD_TOOLS=ON` enabled building of the `jsonv` tool.

## License

Copyright (C) 2015 Kenneth Benzie

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

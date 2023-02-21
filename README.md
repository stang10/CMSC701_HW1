# cpp_sa_starter_proj

This is a simple C++ starter for Project 1.  It creates an executable that compiles and links against an efficient suffix array construction library [libsais](https://github.com/IlyaGrebnov/libsais). The libsais implementation comes from [https://github.com/IlyaGrebnov/libsais](https://github.com/IlyaGrebnov/libsais) and is licensed udner the Apache 2 license found [here](https://github.com/IlyaGrebnov/libsais/blob/master/LICENSE).

To use this project, you can do the following. First, clone this repo (below `$` represents the prompt of your shell):

```
$git clone https://github.com/rob-p/cpp_sa_starter_proj
$cd cpp_sa_starter_proj
```

Then, you can build the project from the command line — for this you will need [CMake](https://cmake.org/download/) and `make`.  Alternatively to building from the command line, if you are using VSCode or CLion, they should have builtin support or plugins for CMake which should let you build this project directly from within your IDE.  If you are using the command line, you should do the following:

```
$mkdir build
$cmake ..
$make
```

This should generate an executable in the current directory called `buildsa` which you can run:

```
$ ./buildsa
```

The output should be as follows:

```
suffix array construction successful!
[ 6, 5, 3, 1, 0, 4, 2 ]
```

Anyarg - a simple option parser for C++ 
=======================================

Current version: 0.11

### Summary
*Anyarg* supports both single letter options (like: -i) and long options (like: --help). The parsing of single letter options follows POSIX conventions and the parsing of long options follows GNU conventions (See URLs).

The mixup of short and long options in command line is allowed (like: -v --help).

The mixup of options and non-option arguments also is allowed, but it is a good practice to place options before non-option arguments in command line.

All non-option arugments are collected with the same order as they in the command line.

Argument -- stops the parsing of command line arguments. All the following arguments are treated as non-option arguments.

A single hypen - is parsed as an ordinary non-option arguments. It is usally used to specify input from standard input or output to the standant output. 


### Code example
```c++
#include <cstdio>
#include "anyarg.h"

int main(int argc, char **argv)
{
	// create a Anyarg object first
	Anyarg opt;
	
	// define usable options for this program, also set default values
	opt.add_flag("all", 'a', "List all stuffs.");
	opt.add_option_str("input-file", 'i', "-", "=FILE Input filename, default is -.");
	opt.add_option_int("buffer-size", 'b', 100, "=SIZE Set the buffer size, default is 100.");
	opt.add_option_double("min", 0, 0.9, "=FLOAT Minimal correlation coefficient, default is 0.9.");
	opt.add_flag('v', "Open verbose model.");
	opt.add_flag("help", 'h', "Display help information.");


	// parsing command line, collect command line arguments
	opt.parse_argv(argc, argv);

	// generate formatted usage information for all options
	if (opt.found_flag("help")) {
		printf("%s\n", opt.auto_usage());
		exit(0);
	}
	
	// access option values
	if (opt.found_flag("all"))
		printf("option --all is set in the command line\n");
	if (opt.found_flag('v'))
		printf("verbose mode is opened\n");
	printf("The value of option -b is %d\n", opt.get_value_int("buffer-size"));
	printf("The value of option --min is %f\n", opt.get_value_double("min"));

	// ...
	return 1;
}
```


### Motivation
Writting code to parsing command line options and formatted usage is non-trival, always is annoying and time wasting. 

*anyary* attempts to make option parsing much easier in C++ programming. It also can generate well formatted usage information for all options, really saving a lot of our life.

The formmatted usage is like this:
```
Options
  -a, --all                 List all stuffs.
  -i, --input-file=FILE     Input filename, default is -.
  -b, --buffer-size=SIZE    Set the buffer size, default is 100.
      --min=FLOAT           Minimal correlation coefficient, default is 0.9.
  -v                        Open verbose model.
  -h, --help                Display help information.
```

### Installation

No installation is needed. Just include anyarg.h in your source file. Compile anyarg.cpp together with your other source files.

### URLs
POSIX conventions:
http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap12.html

GNU extensions:
http://www.gnu.org/prep/standards/html_node/Command_002dLine-Interfaces.html


### Contributors
Li Qibin liqb036@gamil.com

Please contact if you have ideas to improve *Anyarg* or report a bug.


### License
GPL v3

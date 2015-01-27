#include <cstdio>
#include "anyarg.h"

int main(int argc, char **argv)
{
	// create a anyarg object first
	anyarg opt;
	
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

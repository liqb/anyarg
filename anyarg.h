/**
@file    anyarg.h
@brief   A light and powerful option parser for C++.
@author  Li Qibin (liqb036@gmail.com)
@version 0.10
@bug     no bug found yet
@date    2014/01/26

anyarg supports both single letter-options (like: -i) and long options (like: --help).

The mixup of short and long options in command line is allowed (like: -v --help).

Options can be given as:
\verbatim
--help
--long=value
--long value
-h
-ab
-c value
\endverbatim
where --help, -h, -a, -b are boolean flags, option --long and -c take an argument.

Although options can mix up with non-option arguments, it is a good practice to place options before non-option
arguments.

All non-option arugments are collected to a new array in the same order as they are in command line.

Argument -- stops the parsing of command line arguments. All the following arguments are treated as non-option
arguments.

A single hypen - is parsed as an ordinary non-option arguments. It is usally used to specify input from standard input or
output to the standant output.

Assign multiple values to an option is not allowed.
*/

/*
A single letter option begins with a hyphen '-'. The parsing of single letter options follows POSIX conventions.

A long option begins with two hyphens '--'. The parsing of long options follows GNU conventions.

Only specify/define a option once, otherwise you will incurr an error.

Example:
Supposing program \c foo has two flags (-a -all, -v --verbose) and two taking-value options (-s
--buffer-size, -n), you can turn on flag a and v, set option s to 100 and n to 50, and pass another two
non-option arguments (abc and xyz) to foo by:

foo -av -s 100 -n 50 abc xyz foo -av
--buffer-size=100 -n 50 abc xyz
foo --all --verbose --buffer-size=100 -n 50 abc xyz

POSIX conventions:
http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap12.html

GNU extensions:
http://www.gnu.org/prep/standards/html_node/Command_002dLine-Interfaces.html
*/


#ifndef __ANYARG_H__
#define __ANYARG_H__

#include <string>
#include <stdlib.h>
#include <string.h>

using std::string;
using std::vector;


/// Data structure of a program option
struct Option
{
	char type;    // type of option
	char letter;  // single-letter label of an option
	string name;   // long name of an option
//	char valtype;  // value type, [BSID], bool: B, string: S, int: I, double: D
	string value_str; // option value as a string
	union {           // option value as a bool, int or double value
		bool value_bool;
		int  value_int;
		double value_double;
	};
	string desc;   // a short sentence to describe an option
	string meta;   // a META word to specify the type of option argument, such as "FILE", "SIZE"

	Option():type(0), letter(), name(), value_str(), value_double(0.0), desc(), meta() {}
	bool set_value(const char *opt_value);
	bool set_desc_meta(const char *opt_desc);
};


/// Use this class to define program options and parse command line arguments.
class anyarg
{
  public:
	/// Construct a anyarg object.
	anyarg();

	//@{
	/**
	Add a boolean flag to a program.
	@param name   Long name of a flag. Using hypen to connect multiple words, such as "buffer-size".
	@param letter Single-letter label of a flag, such as 'i'. Set letter to 0 (not '0') if you do not need a short label.
	@param desc   Description of a flag, which will be used to generate usage. Set desc as "" if nothing to say.
	@return       \c true if success, \c false if the flag has been defined.
	@note The default value of flag is \c false.
	
	Examples:
	\verbatim
	opt.add_flag("help", 'h', "Display help information.");
	opt.add_flag('v', "Open the verbose mode.");
	\endverbatim
	opt is a anyarg object.
	*/
	bool add_flag(const char *name, char letter, const char *desc);
	bool add_flag(char letter, const char *desc);
	//@}


	/**
	Add a taking-value option to a program.
	@param name   Long name of a flag. Using hypen to connect multiple words, such as "buffer-size".
	@param letter Single-letter label of a flag, such as 'i'. Set letter to 0 if you do not need a short label.
	@param v0     The default value of this option.
	@param desc   Description of a flag, which will be used to generate usage. Set \c desc as "" if nothing to say.
	              If description starts with an \c = (like: "=FILE Name of output file"), the first word is treated as
	              a META word in the generation of option usage, e.g.: "-f FILE, --filename=FILE".
	@return       \c true if success, \c false if the flag has been defined.
	
	Examples:
	\verbatim
	opt.add_option_int("buffer-size", 0, 100, "=SIZE Buffer size, default=100");
	opt.add_option_str("output", 'o', "na", "=FILE Output file");
	\endverbatim
	*/
	// Add an option with a string value
	bool add_option_str(const char *name, char letter, const char *v0, const char *desc);
	bool add_option_str(char letter, const char *v0, const char *desc);

	// Add an option with an integer value
	bool add_option_int(char letter, int v0, const char *desc);
	bool add_option_int(const char *name, char letter, int v0, const char *desc);

	// Add an option with an double value
	bool add_option_double(char letter, double v0, const char *desc);
	bool add_option_double(const char *name, char letter, double v0, const char *desc);


	/**
	Parse command line arguments.
	@pre         Flags and options have to be defined by add_flag() and add_option_xxx() before the parsing of command line.
	@param argc  The number of arguments in command line.
	@param argv  Array including all command line arugments.
	@return      \c true if success, \c false if fail.
	@note        The frist elements of \c argv MUST be the name of the program.
	*/
	bool parse_argv(int argc, char **argv);

	
	/**
	Check whether a flag can be found in the command line.
	@param name   The same name used in add_flag().
	@param letter Single-letter label of a flag.
	@return       \c true if a flag is found, \c false if not.
	*/
	bool found_flag(const char *name) const;
	bool found_flag(char letter) const;

	
	/**
	Get the value of an option.
	@param name   The same name used in add_option_xxx().
	@param letter Single-letter label of a flag, such as 'i'.
	@return       The value of an option. If the value is not assigned in command line, the default value will be returned.
	
	Examples:
	\verbatim
	int buffer_size = opt.get_value_int("buffer-size");
	\endverbatim
	*/
	// Get the value of an option with string values
	const char *get_value_str(const char *name) const;
	const char *get_value_str(char letter) const;
	
	// Get the value of an option with integer values
	int get_value_int(const char *name) const;
	int get_value_int(char letter) const;
	
	// Get the value of an option with double values
	double get_value_double(const char *name) const;
	double get_value_double(char letter) const;


	/**
	Get the number of non-option arguments.
	@return The count of non-option arguments.
	*/
	int get_argc() const;


	/**
	Get a non-option argument by index
	@param i The index of a non-option arguments, ranging from 0 to get_argc() - 1.
	Non-option arguments are in the same order as they appeared in command line.
	*/
	const char *get_arg(int i) const;


	/**
	Generate well-formatted usage information for options defined for the program.
	@return Usage information as a c-string.
	*/
	const char *auto_usage();


  private:
	string prog_name_;  // name of the program
	int argc_;    // count of non-option arguments
	vector<string> argv_; // vector of non-option arguments

	vector<Option> options_; // vector of options
	
	string help_;     // formatted help for options

	anyarg(const anyarg &); // prevent the copy of a anyarg object

	anyarg & operator = (const anyarg &); // prevent assignment of anyarg object
	
	bool is_new_option(const char *name, char letter);

	int  get_optind(char letter) const;

	int  get_optind(const char *name) const;
	
	// Show information of all options, for debugging.
	void show_options() const;
};


/**
\example
\code
#include "anyarg.h"

int main(int argc, char **argv)
{
	anyarg opt;
	opt.add_flag("all", 'a', "List all.");
	opt.add_flag('v', "Open verbose model.");
	opt.add_flag("help", 'h', "Display help information.");
	opt.add_option_str("input-file", 'i', "-", "=FILE Input file name.");
	opt.add_option_int("buffer-size", 'b', 100, "=SIZE Set the buffer size.");
	opt.add_option_double("min", 0, 0.1, "=FLOAT Minimal identity.");
	opt.parse_argv(argc, argv);
	
//	printf("%s", opt.auto_usage());
	
	if (opt.found_flag("help")) {
		printf("%s\n", opt.auto_usage());
		exit(0);
	}
	
	if (opt.found_flag("all"))
		printf("option --all is set in the command line\n");
	if (opt.found_flag('v'))
		printf("verbose mode is opened\n");
	printf("The value of option -b is %d\n", opt.get_value_int("buffer-size"));
	printf("The value of option --min is %f\n", opt.get_value_double("min"));
	return 1;
}
\endcode
*/

#endif

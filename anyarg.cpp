#include <string>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cassert>
#include "anyarg.h"

using std::string;
using std::vector;

#define ANYARG_TYPE_FLAG  1
#define ANYARG_OPT_STR    2
#define ANYARG_OPT_INT    3
#define ANYARG_OPT_DOUBLE 4


// Set option value given a string
bool Option::set_value(const char *new_value)
{
	if (new_value == NULL || new_value[0] == 0) {
		fprintf(stderr, "invalid option value\n");
		exit(1);
	}

	switch (type)
	{
		case ANYARG_TYPE_FLAG: 
			if (strcmp(new_value, "true") == 0) value_bool = true;
			else value_bool = false;
			break;
		case ANYARG_OPT_STR:
			value_str = new_value;
			break;
		case ANYARG_OPT_INT:
			value_int = atoi(new_value);
			break;
		case ANYARG_OPT_DOUBLE:
			value_double = atof(new_value);
			break;
		default:
			fprintf(stderr, "invalid option value type\n");
			exit(1);
	}
	
	return true;
}


// set the desc and meta
bool Option::set_desc_meta(const char *s)
{
	if (s && s[0]) {
		if (s[0] == '=') { // has a meta word
			int i = 0;
			int len = strlen(s);
			while (i < len && !isspace(s[i])) ++i; //skip to first space
			meta.assign(s + 1, i - 1);
			while (i < len && isspace(s[i])) ++i;  //skip to a non-space character
			if (i < len) desc.assign(s + i);
			else desc = "No description.";
		} else desc = s;
	} else desc = "No description.";
	return true;
}


Anyarg::Anyarg(): prog_name_(), argc_(0), argv_(), options_(), help_() {}


int Anyarg::get_optind(const char *name) const
{
	if (name == NULL || name[0] == 0) {
		fprintf(stderr, "invalid option name\n");
		exit(1);
	}
	
	for (int i = 0; i < options_.size(); i++) {
		if (options_[i].name.empty()) continue;
		if (options_[i].name == name) return i;
	}
	return -1;
}


int Anyarg::get_optind(char letter) const
{
	if (letter == 0) {
		fprintf(stderr, "invalid option\n");
		exit(1);
	}

	for (int i = 0; i < options_.size(); i++)
		if (options_[i].letter == letter) return i;

	return -1;
}


bool Anyarg::is_new_option(const char *name, char letter)
{
	if ((name == NULL || name[0] == 0) && letter == 0) {
		fprintf(stderr, "%s line %d: invalid option\n", __FILE__, __LINE__);
		exit(1);
	}

	// check whether the option has been defined
	int found = 0;
	if (name && name[0]) {
		int j = get_optind(name);
		if (j >= 0) {
			found = 1;
			fprintf(stderr, "option --%s has been defined\n", name);
			exit(1);
		}
	}
	if (letter) {
		int j = get_optind(letter);
		if (j >= 0) {
			found = 1;
			fprintf(stderr, "option -%c has been defined\n", letter);
			exit(1);
		}
	}

//	printf("option %s, %c is new\n", name, letter);
	return true;
}


bool Anyarg::add_flag(const char *name, char letter, const char *desc)
{
	assert(is_new_option(name, letter));

	Option t;
	t.type = ANYARG_TYPE_FLAG;
	t.letter = letter;
	if (name && name[0]) t.name = name;
	else t.name = "";
	t.value_bool = false;
	t.desc = desc;
	t.meta = "";
	options_.push_back(t);
	return 1;
}


bool Anyarg::add_flag(char letter, const char *desc)
{
	return add_flag("", letter, desc);
}


bool Anyarg::add_option_str(const char *name, char letter, const char *v0, const char *desc)
{
	assert(is_new_option(name, letter));

	Option t;
	t.type = ANYARG_OPT_STR;
	t.letter = letter;
	if (name && name[0]) t.name = name;
	else t.name = "";
	t.value_str = v0;
	t.set_desc_meta(desc);
	
	options_.push_back(t);
	return 1;
}


bool Anyarg::add_option_str(char letter, const char *v0, const char *desc)
{
	return add_option_str("", letter, v0, desc);
}


bool Anyarg::add_option_int(const char *name, char letter, int v0, const char *desc)
{
	assert(is_new_option(name, letter));

	Option t;
	t.type = ANYARG_OPT_INT;
	t.letter = letter;
	if (name && name[0]) t.name = name;
	else t.name = "";
	t.value_int = v0;
	t.set_desc_meta(desc);
	
	options_.push_back(t);
	return 1;
}


bool Anyarg::add_option_int(char letter, int v0, const char *desc)
{
	return add_option_int("", letter, v0, desc);
}


bool Anyarg::add_option_double(const char *name, char letter, double v0, const char *desc)
{
	assert(is_new_option(name, letter));

	Option t;
	t.type = ANYARG_OPT_DOUBLE;
	t.letter = letter;
	if (name && name[0]) t.name = name;
	else t.name = "";
	t.value_double = v0;
	t.set_desc_meta(desc);
	
	options_.push_back(t);
	return 1;
}


bool Anyarg::add_option_double(char letter, double v0, const char *desc)
{
	return add_option_double("", letter, v0, desc);
}


// Turn on flags specified on command line
// Get option values specified on command line
// Collect non-option arguments
bool Anyarg::parse_argv(int argc, char **argv)
{
	prog_name_ = argv[0];
	
	if (prog_name_[0] == '-') {
		fprintf(stderr, "program name can not start with a -.\n");
		exit(1);
	}
	
	argc_ = 0;
	int stop_parsing = 0; // stop option parsing when meeting argument --
	int j = 0;
	string t;
	for (int i = 1; i < argc; i++) {
		if (stop_parsing) {
			t = argv[i];
			argv_.push_back(t); argc_++;
			continue;
		}
		
		if (argv[i][0] == '-' && argv[i][1] == 0) { // meet "-"
			t = argv[i];
			argv_.push_back(t); argc_++;
			continue;
		}
		
		if (argv[i][0] == '-' && argv[i][1] == '-' && argv[i][2] == 0) { // meet "--"
			stop_parsing = 1;
			continue;
		} 
		
		if (argv[i][0] == '-' && argv[i][1] == '-') { // long option
			char *x = strdup(argv[i] + 2);
			int len = strlen(x);
			int t = 0;
			while (x[t] && x[t] != '=') t++; // seek for '='
			if (t < len) { // a name=argument pair
				x[t] = '\0';
				j = get_optind(x);
				if (j < 0) {
					fprintf(stderr, "illegal option --%s.\n", x);
					exit(1);
				}

				if (options_[j].type == ANYARG_TYPE_FLAG) {
					fprintf(stderr, "option --%s is a flag, cannot taking a argument.\n", x);
					exit(1);
				}
				
				if (t >= len - 1) {
					fprintf(stderr, "the argument of option --%s is missed.\n", x);
					exit(1);
				}
				options_[j].set_value(x + t + 1);
			} else { // is --name or --name argument
				j = get_optind(x);
				if (j < 0) {
					fprintf(stderr, "illegal option --%s.\n", x);
					exit(1);
				}

				if (options_[j].type == ANYARG_TYPE_FLAG) {
					options_[j].set_value("true");
				} else {
					if (i >= argc - 1) {
						fprintf(stderr, "the argument of option --%s is missed.\n", x);
						exit(1);
					}
					options_[j].set_value(argv[++i]);
				}
			}
			free(x);
		} else if (argv[i][0] == '-') { // short option
			const char *x = argv[i] + 1;
			int len = strlen(x);
			if (len == 1) { // is a regular short option
				j = get_optind(x[0]);
				if (j < 0) {
					fprintf(stderr, "illegal option -%c.\n", x[0]);
					exit(1);
				}
				if (options_[j].type == ANYARG_TYPE_FLAG) {
					options_[j].set_value("true");
				} else {
					if (i >= argc - 1) {
						fprintf(stderr, "the argument of option -%c is missed.\n", x[0]);
						exit(1);
					}
					options_[j].set_value(argv[++i]);
				}
			} else { // bundled flags ('-lst') or option-value pair ('-ofoo');
				j = get_optind(x[0]);
				if (j < 0) {
					fprintf(stderr, "illegal option -%c.\n", x[0]);
					exit(1);
				}
				if (options_[j].type != ANYARG_TYPE_FLAG) { // is option-value pair
					options_[j].set_value(x + 1);
				} else { // is bundled flags
					for (int t = 0; t < len; t++) {
						j = get_optind(x[t]);
						if (j < 0) {
							fprintf(stderr, "illegal option -%c.\n", x[t]);
							exit(1);
						}
						if (options_[j].type != ANYARG_TYPE_FLAG) {
							fprintf(stderr, "-%c is not a flag, cannot be grouped.\n", x[t]);
							exit(1);
						}
						options_[j].set_value("true");
					}
				}
			}
		} else { // non-option arguments
			t = argv[i];
			argv_.push_back(t); argc_++;
		}
	}
	return 1;
}


bool Anyarg::found_flag(const char *name) const
{
	int j = get_optind(name);

	if (j < 0) {
		fprintf(stderr, "option --%s is undefined.\n", name);
		exit(1);
	}
	
	if (options_[j].type != ANYARG_TYPE_FLAG) {
		fprintf(stderr, "option --%s is not a flag.\n", name);
		exit(1);
	}
	return options_[j].value_bool;
}


bool Anyarg::found_flag(char letter) const
{
	int j = get_optind(letter);

	if (j < 0) {
		fprintf(stderr, "option -%c is undefined.\n", letter);
		exit(1);
	}

	if (options_[j].type != ANYARG_TYPE_FLAG) {
		fprintf(stderr, "option -%c is not a flag.\n", letter);
		exit(1);
	}
	
	return options_[j].value_bool;
}


const char *Anyarg::get_value_str(const char *name) const
{
	int j = get_optind(name);

	if (j < 0) {
		fprintf(stderr, "option --%s is undefined.\n", name);
		exit(1);
	}
	if (options_[j].type != ANYARG_OPT_STR) {
		fprintf(stderr, "option --%s is not a option with a string value.\n", name);
		exit(1);
	}
	return options_[j].value_str.c_str();
}


const char *Anyarg::get_value_str(char letter) const
{
	int j = get_optind(letter);

	if (j < 0) {
		fprintf(stderr, "option -%c is undefined.\n", letter);
		return 0;
	}
	
	if (options_[j].type != ANYARG_OPT_STR) {
		fprintf(stderr, "option -%c is not a option with a string value.\n", letter);
		return 0;
	}
	return options_[j].value_str.c_str();
}


int Anyarg::get_value_int(const char *name) const
{
	int j = get_optind(name);

	if (j < 0) {
		fprintf(stderr, "option --%s is undefined.\n", name);
		exit(1);
	}
	if (options_[j].type != ANYARG_OPT_INT) {
		fprintf(stderr, "option --%s is not a option with an integer value.\n", name);
		exit(1);
	}
	return options_[j].value_int;
}


int Anyarg::get_value_int(char letter) const
{
	int j = get_optind(letter);

	if (j < 0) {
		fprintf(stderr, "option -%c is undefined.\n", letter);
		return 0;
	}
	
	if (options_[j].type != ANYARG_OPT_INT) {
		fprintf(stderr, "option -%c is not a option with an integer value.\n", letter);
		return 0;
	}
	return options_[j].value_int;
}


double Anyarg::get_value_double(const char *name) const
{
	int j = get_optind(name);

	if (j < 0) {
		fprintf(stderr, "option --%s is undefined.\n", name);
		exit(1);
	}
	if (options_[j].type != ANYARG_OPT_DOUBLE) {
		fprintf(stderr, "option --%s is not a option with a double value.\n", name);
		exit(1);
	}
	return options_[j].value_double;
}


double Anyarg::get_value_double(char letter) const
{
	int j = get_optind(letter);

	if (j < 0) {
		fprintf(stderr, "option -%c is undefined.\n", letter);
		return 0;
	}
	
	if (options_[j].type != ANYARG_OPT_DOUBLE) {
		fprintf(stderr, "option -%c is not a option with a double value.\n", letter);
		return 0;
	}
	return options_[j].value_double;
}


int Anyarg::get_argc() const {return argc_;}


const char *Anyarg::get_arg(int i) const
{
	if (i < 0 || i >= argc_) {
		fprintf(stderr, "option index is out-of-range\n");
		exit(1);
	}
	return argv_[i].c_str();
}

//  -S, --buffer-size=SIZE    use

const char *Anyarg::auto_usage()
{
	int nindent = 28;
		  	  
	help_.clear();
	help_.append("Options\n");

	for (int i = 0; i < options_.size(); i++) {
		const Option &opt = options_[i];
		int k = 0;   // index
		char s[256]; // formatted option string
		for (int j = 0; j < 256; j++) s[j] = ' ';
		if (opt.letter) {
			s[2] = '-'; s[3] = opt.letter;
		}
		if (!opt.name.empty()) {
			if (opt.letter) s[4] = ',';
			s[6] = s[7] ='-';
			int l = opt.name.length();
			if (l > 64) l = 64; // truncate longname, almost never happen
			k = 8;
			strncpy(s + k, opt.name.c_str(), l); k += l;
			
			if (!opt.meta.empty()) {
				s[k++] = '=';
				l = opt.meta.length();
				if (l > 32) l = 32; // truncate META word, almost never happen
				strncpy(s + k, opt.meta.c_str(), l); k += l;
			}
		}
		
		if (k <= nindent - 2) {
			s[nindent] = '\0';
			help_.append(s);
		} else { // long option string
			s[k] = '\0';
			help_.append(s);
			help_.push_back('\n');
			help_.append(nindent, ' ');
		}

		help_.append(opt.desc);
		help_.push_back('\n');
	}
	return help_.c_str();
}


void Anyarg::show_options() const
{
	printf("Options (letter, long-name, type, value, META, desc):\n");
	for (int i = 0; i < options_.size(); i++) {
		const Option &t = options_[i];
		string name = t.name.empty() ? "NA" : t.name;
		char letter = t.letter ? t.letter : '#';
		string desc = t.desc.empty() ? "No desc": t.desc;
		string meta = t.meta.empty() ? "META" : t.meta;

		printf("%c%20s%4d", letter, name.c_str(), t.type);
		
		switch(t.type) {
			case ANYARG_TYPE_FLAG:
				if (t.value_bool) printf("%10s", "true");
				else printf("%10s", "false");
				break;
			case ANYARG_OPT_STR:
				printf("%10s", t.value_str.c_str());
				break;
			case ANYARG_OPT_INT:
				printf("%10d", t.value_int);
				break;
			case ANYARG_OPT_DOUBLE:
				printf("%10f", t.value_double);
				break;
			default:
				printf("%10s", "NOVALUE");
		}
		
		printf("%8s  %s\n", meta.c_str(), t.desc.c_str());
	}

	printf("\n%d non-option arguments\n", argc_);
	for (int i = 0; i < argc_; i++) printf("  %s\n", argv_[i].c_str());
}


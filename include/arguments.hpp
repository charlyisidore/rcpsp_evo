#ifndef ARGUMENTS_HPP
#define ARGUMENTS_HPP

#include <iostream>
#include <string>
#include <getopt.h>

/*
	Arguments parsing functions
	Uses getopt with long options.
*/
struct arguments
{
	/*
		Parse long and short options
	*/
	static void parse( int argc, char *argv[] );

	/*
		Display arguments parsed to see what the program has understood
	*/
	static void print( std::ostream & os = std::cout );

	/*
		Display usage
	*/
	static void usage( const char * program_name, std::ostream & os = std::cout );

	// Integer parameters
	static int
		population_size,
		print_problem,
		print_graph,
		print_table,
		print_plot,
		verbose,
		help;

	// Positive integer parameters
	static unsigned int
		random_seed;

	// Float parameters
	static double
		alpha;

	// Identifiers
	enum
	{
		id_random_seed = 0x100
	};

	// Instance file name
	static std::string filename;

	// getopt long options array
	static const struct option long_options[];
};

#endif

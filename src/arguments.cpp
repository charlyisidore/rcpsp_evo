#include "arguments.hpp"
#include <sstream>
#include <cstdlib>

int arguments::population_size( 100 );
int arguments::print_problem( 0 );
int arguments::print_graph( 0 );
int arguments::print_table( 0 );
int arguments::print_plot( 0 );
int arguments::verbose( 0 );
int arguments::help( 0 );
unsigned int arguments::random_seed( 0 );
double arguments::alpha( 0.75 );

std::string arguments::filename;

const struct option arguments::long_options[] = {
	{ "population-size", required_argument, 0,                         'p'                       },
	{ "alpha",           required_argument, 0,                         'a'                       },
	{ "random-seed",     required_argument, 0,                         arguments::id_random_seed },
	{ "print-problem",   no_argument,       &arguments::print_problem, 1                         },
	{ "print-graph",     no_argument,       &arguments::print_graph,   1                         },
	{ "print-table",     no_argument,       &arguments::print_table,   1                         },
	{ "print-plot",      no_argument,       &arguments::print_plot,    1                         },
	{ "verbose",         no_argument,       &arguments::verbose,       1                         },
	{ "brief",           no_argument,       &arguments::verbose,       0                         },
	{ "help",            no_argument,       &arguments::help,          1                         },
	{ 0, 0, 0, 0 }
};

// Parse long and short options
void arguments::parse( int argc, char *argv[] )
{
	int next_option;
	int option_index( 0 );

	const char * const short_options = "p:a:";

	do
	{
		next_option = getopt_long( argc, argv, short_options, arguments::long_options, &option_index );
		switch ( next_option )
		{
			case 'p':
				std::istringstream( optarg ) >> population_size;
				break;

			case 'a':
				std::istringstream( optarg ) >> alpha;
				break;

			case id_random_seed:
				std::istringstream( optarg ) >> random_seed;
				break;

			case 0:
			case -1:
				break;

			default:
				abort();
		}
	}
	while ( next_option != -1 );

	if ( optind < argc )
	{
		filename = argv[optind];
	}
}

void arguments::print( std::ostream & os )
{
	os
		<< "File: " << filename << std::endl
		<< "Options:" << std::endl
		<< "\tpopulation-size = " << population_size << std::endl
		<< "\talpha           = " << alpha           << std::endl
		<< "\trandom_seed     = " << random_seed     << std::endl
		<< std::endl;
}

void arguments::usage( const char * program_name, std::ostream & os )
{
	os << "Usage: " << program_name << " <instance-name> [-p size-of-population]" << std::endl;
}


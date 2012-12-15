#include "problem.hpp"
#include "solution.hpp"
#include "arguments.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <limits>

void usage( char * program_name )
{
	std::cout << "Usage: " << program_name << " <instance-name> [size-of-population]" << std::endl;
}

int main( int argc, char * argv[] )
{
	std::ifstream f;                   // Instance file
	Problem p;                         // Problem parameters
	std::vector<Solution> population;
	int bestCmax( std::numeric_limits<int>::max() );
	int bestIndex( 0 );


	// Initialization ------------------------------------------------------

	// Simple and powerful argument parser using standard POSIX getopt
	arguments::parse( argc, argv );

	// Print usage
	if ( arguments::filename.empty() )
	{
		arguments::usage( argv[0] );
		return 0;
	}

	// Set the initial random seed
	if ( arguments::random_seed == 0 )
	{
		arguments::random_seed = std::time( 0 );
	}
	std::srand( arguments::random_seed );

	// Print arguments to terminal
	arguments::print();

	// Open an instance file
	f.open( arguments::filename.c_str() );
	if ( !f.is_open() )
	{
		std::clog << "Invalid file name" << std::endl;
		return 0;
	}

	// Load the problem from the instance file
	f >> p;

	// Display the problem
	if ( arguments::print_problem )
		std::cout << p << std::endl;

	// Show precedence graph of the problem
	if ( arguments::print_graph )
		p.exportDotPrecedenceGraph();


	// Algorithm -----------------------------------------------------------

	// Generate an initial population
	for ( int i = 0; i < arguments::population_size; ++i )
	{
		Solution s( p );

		// Build a solution using GRASP
		s.grasp( arguments::alpha );

		// Possibility to manipulate solution as following
		//s[i] = j;

		// Update changes
		s.update();

		// We add the current solution in the population
		population.push_back( s );

		if ( s.getCmax() < bestCmax )
		{
			bestIndex = i;
			bestCmax = s.getCmax();
		}
	}


	// Result --------------------------------------------------------------

	std::cout << "Best solution: " << population[bestIndex] << std::endl;
	std::cout << "Best makespan: " << bestCmax << std::endl;

	// Show the schedulings
	if ( arguments::print_table )
		population[bestIndex].exportTable();

	if ( arguments::print_plot )
		population[bestIndex].exportGnuplot();

	return 0;
}


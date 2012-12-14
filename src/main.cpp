#include "problem.hpp"
#include "solution.hpp"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>

int main( int argc, char * argv[] )
{
	if ( argc <= 1 )
	{
		std::cout << "Please enter a file name" << std::endl;
		return 0;
	}

	std::srand( std::time( 0 ) );

	std::ifstream f; // Instance file
	Problem p;       // Problem parameters
	Solution s;      // A solution of the problem

	// Open an instance file
	f.open( argv[1] );
	if ( !f.is_open() )
	{
		std::cout << "Invalid file name" << std::endl;
		return 0;
	}

	// Load the problem from the instance file
	f >> p;

	// Display the problem
	std::cout << p << std::endl;

	// Show precedence graph of the problem
	p.exportDotPrecedenceGraph();

	// Set the solution compatible with the problem
	s.setProblem( p );

	// Build a solution using GRASP
	s.grasp( 0.7 );

	// Possibility to manipulate solution as following
	//s[i] = j;

	// Update changes
	s.update();

	// Display list of jobs in the solution
	std::cout << "Solution: " << s << std::endl;

	// Display Cmax
	std::cout << "Cmax = " << s.getCmax() << std::endl;

	// Show the schedulings
	s.exportTable();
	s.exportGnuplot();

	return 0;
}


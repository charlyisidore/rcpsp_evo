#include "problem.hpp"
#include "pstream.h"
#include <sstream>
#include <stdexcept>
#include <iomanip>
#include <cmath>

Problem::Problem() :
	_numJobs( 1 ),
	_numResources( 1 ),
	_resourceCapacity( 1 ),
	_jobDuration( 1 ),
	_jobRequest( 1, std::vector<int>( 1 ) ),
	_jobPredecessors( 1 ),
	_jobSuccessors( 1 )
{
}

int Problem::getUpperBound() const
{
	int d, sum( 0 ), max( 0 );
	for ( int j = 0; j < getNumJobs(); ++j )
	{
		d = getJobDuration( j );
		sum += d;
		max = std::max( d, max );
	}
	return std::max( sum, max );
}

void Problem::setNumJobs( int jmax )
{
	_numJobs = jmax;
	_jobDuration.resize( _numJobs );
	_jobRequest.clear();
	_jobRequest.resize( _numJobs, std::vector<int>( _numResources ) );
	_jobPredecessors.clear();
	_jobPredecessors.resize( _numJobs );
	_jobSuccessors.clear();
	_jobSuccessors.resize( _numJobs );
}

void Problem::setNumResources( int rmax )
{
	_numResources = rmax;
	_resourceCapacity.resize( _numResources );
	_jobRequest.clear();
	_jobRequest.resize( _numJobs, std::vector<int>( _numResources ) );
}

void Problem::exportDotPrecedenceGraph() const
{
	// Make a pipe an write the graphviz script on the fly
	redi::opstream out( "dot -Txlib" );
	exportDotPrecedenceGraph( out );
}

void Problem::exportDotPrecedenceGraph( std::ostream & os ) const
{
	os
		<< "digraph G {" << std::endl
		<< "\tsplines=true;" << std::endl;

	for ( int j = 0; j < _numJobs; ++j )
	{
		for ( int k = 0; k < (int)_jobSuccessors[j].size(); ++k )
		{
			os << '\t' << j+1 << " -> " << _jobSuccessors[j].at( k )+1 << std::endl;
		}
	}

	os << '}' << std::endl;
}

// Read a value on a line in the form "  key  :  value  "
int Problem::readValue( const std::string & line )
{
	std::size_t pos;
	int i;

	pos = line.find( ':' );
	std::istringstream( line.substr( pos+1 ) ) >> i;
	return i;
}

// Show on terminal or output
std::ostream & operator << ( std::ostream & os, const Problem & p )
{
	int intlen = std::floor( std::log( (float)p.getNumJobs() ) ) - 1;
	int max_num_successors( 0 );

	os
		<< "#jobs = " << p.getNumJobs() << std::endl
		<< "#resources = " << p.getNumResources() << std::endl;

	os << "Capacities:";
	for ( int k = 0; k < p.getNumResources(); ++k )
	{
		os << ' ' << p.getResourceCapacity( k );
	}
	os << std::endl;

	// max_num_successors is used to align predecessors
	for ( int j = 0; j < p.getNumJobs(); ++j )
	{
		if ( max_num_successors < (int)p.getJobSuccessors( j ).size() )
			max_num_successors = p.getJobSuccessors( j ).size();
	}

	for ( int j = 0; j < p.getNumJobs(); ++j )
	{
		os << "Job " << std::setw( intlen ) << j+1
		   << " | Duration: " << std::setw( 2 ) << p.getJobDuration( j )
		   << " | Req:";
		for ( int k = 0; k < p.getNumResources(); ++k )
		{
			os << ' ' << std::setw( intlen ) << p.getJobRequest( j, k );
		}
		os << " | Succ:";
		for ( int i = 0; i < (int)p.getJobSuccessors( j ).size(); ++i )
		{
			os << ' ' << std::setw( intlen ) << p.getJobSuccessors( j ).at( i )+1;
		}
		// Align predecessors
		os << std::setw( ( intlen + 1 ) * ( max_num_successors - p.getJobSuccessors( j ).size() ) ) << "";
#if 1
		os << " | Pred:";
		for ( int i = 0; i < (int)p.getJobPredecessors( j ).size(); ++i )
		{
			os << ' ' << std::setw( intlen ) << p.getJobPredecessors( j ).at( i )+1;
		}
#endif
		os << std::endl;
	}
	return os;
}

// Load from a file or input
std::istream & operator >> ( std::istream & is, Problem & p )
{
	int state = 0;

	for ( std::string line; std::getline( is, line ); )
	{
		switch ( state )
		{
			default: // Headers
				if ( line.find( "projects" ) != std::string::npos )
				{
					if ( Problem::readValue( line ) != 1 )
					{
						throw std::logic_error( "Number of projects is greater than supported" );
					}
				}

				else if ( line.find( "jobs" ) != std::string::npos )
				{
					p.setNumJobs( Problem::readValue( line ) );
				}

				else if ( line.find( "horizon" ) != std::string::npos )
				{
					p.setHorizon( Problem::readValue( line ) );
				}

				else if ( line.find( "- renewable" ) != std::string::npos )
				{
					p.setNumResources( Problem::readValue( line ) );
				}

				else if ( line.find( "PROJECT INFORMATION" ) != std::string::npos )
				{
					state = 1;
				}

				else if ( line.find( "PRECEDENCE RELATIONS" ) != std::string::npos )
				{
					state = 2;
				}

				else if ( line.find( "REQUESTS/DURATIONS" ) != std::string::npos )
				{
					state = 3;
				}

				else if ( line.find( "RESOURCEAVAILABILITIES" ) != std::string::npos )
				{
					state = 4;
				}
				break;

			case 1: // PROJECT INFORMATION
				int pronr, njobs, reldate, duedate, tardcost, MPM_Time;

				if ( std::getline( is, line ) ) // Skip the first line
				{
					std::istringstream( line )
						>> pronr >> njobs >> reldate >> duedate >> tardcost >> MPM_Time;
					p.setNumJobs( njobs+2 );
				}
				state = 0;
				break;

			case 2:{ // PRECEDENCE RELATIONS
				int jobnr, nmodes, nsuccessors, succ;

				for ( int j = 0; j < p.getNumJobs() && std::getline( is, line ); ++j )
				{
					std::istringstream iss( line );
					iss >> jobnr >> nmodes >> nsuccessors;
					for ( int i = 0; i < nsuccessors; ++i )
					{
						iss >> succ;
						p.addJobSuccessor( jobnr-1, succ-1 );
					}
				}
				state = 0;
				}break;

			case 3:{ // REQUESTS/DURATIONS
				int jobnr, mode, duration, req;

				if ( std::getline( is, line ) ) // Skip the first line
				{
					for ( int j = 0; j < p.getNumJobs() && std::getline( is, line ); ++j )
					{
						std::istringstream iss( line );
						iss >> jobnr >> mode >> duration;
						p.setJobDuration( jobnr-1, duration );
						for ( int k = 0; k < p.getNumResources(); ++k )
						{
							iss >> req;
							p.setJobRequest( jobnr-1, k, req );
						}
					}
				}
				state = 0;
				}break;

			case 4: // RESOURCEAVAILABILITIES
				if ( std::getline( is, line ) ) // Skip the first line
				{
					std::istringstream iss( line );
					for ( int k = 0; k < p.getNumResources(); ++k )
					{
						int c;
						iss >> c;
						p.setResourceCapacity( k, c );
					}
				}
				state = 0;
				break;
		}
	}
	return is;
}


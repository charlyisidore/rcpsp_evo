#include "solution.hpp"
#include <algorithm>
#include <stdexcept>

Solution::Solution()
{
}

Solution::Solution( const Problem & p )
{
	setProblem( p );
}

Solution::Solution( const Solution & s ) :
	_sequence( s._sequence ),
	_jobs(),
	_resources( s._resources )
{
	// Copy the jobs (new allocation), but the predecessors/successors pointers will not be copied
	for ( int j = 0; j < (int)s._jobs.size(); ++j )
	{
		_jobs.push_back( new Job( *s._jobs[j] ) );
	}

	// "Copy" predecessors/successors pointers (w r t new pointers)
	for ( int j = 0; j < (int)s._jobs.size(); ++j )
	{
		for ( int k = 0; k < (int)s._jobs[j]->getPredecessors().size(); ++k )
		{
			int i = s._jobs[j]->getPredecessors().at( k )->getId();
			_jobs[j]->addPredecessor( _jobs[i] );
		}

		for ( int k = 0; k < (int)s._jobs[j]->getSuccessors().size(); ++k )
		{
			int i = s._jobs[j]->getSuccessors().at( k )->getId();
			_jobs[j]->addSuccessor( _jobs[i] );
		}
	}
}

Solution::~Solution()
{
	clear();
}

void Solution::setProblem( const Problem & p )
{
	int jmax( p.getNumJobs() ), tmax( p.getUpperBound() ), rmax( p.getNumResources() );

	// Clear all existing memory
	clear();

	// Allocate a sequence according to the size of the problem
	_sequence.resize( jmax );

	// Allocate job objects
	for ( int j = 0; j < jmax; ++j )
	{
		Job * job = new Job( j, rmax );
		job->setDuration( p.getJobDuration( j ) );
		for ( int k = 0; k < rmax; ++k )
		{
			job->setRequest( k, p.getJobRequest( j, k ) );
		}
		_jobs.push_back( job );
	}

	// Make the precedence graph with job pointers
	for ( int j = 0; j < jmax; ++j )
	{
		for ( int k = 0; k < (int)p.getJobPredecessors( j ).size(); ++k )
		{
			int i = p.getJobPredecessors( j ).at( k );
			_jobs[j]->addPredecessor( _jobs[i] );
		}
		for ( int k = 0; k < (int)p.getJobSuccessors( j ).size(); ++k )
		{
			int i = p.getJobSuccessors( j ).at( k );
			_jobs[j]->addSuccessor( _jobs[i] );
		}
	}

	// Allocate resource objects
	for ( int k = 0; k < rmax; ++k )
	{
		_resources.push_back( Resource( tmax, p.getResourceCapacity( k ) ) );
	}
}

void Solution::grasp( double alpha )
{
	// Build a valid solution w r t precedence constraints
	std::vector<int> RCL, u;
	std::vector<bool> selected( _jobs.size(), false );

	// Save the jobs with no predecessors in a list
	for ( int j = 0; j < (int)_jobs.size(); ++j )
	{
		if ( _jobs[j]->getPredecessors().empty() )
		{
			RCL.push_back( _jobs[j]->getId() );
		}
	}

	for ( int pos = 0; pos < (int)_sequence.size(); ++pos )
	{
#if VERBOSE
		for ( int i = 0; i < (int)RCL.size(); ++i )
		{
			std::cout << "-- " << i << ' ' << RCL[i] << std::endl;
		}
#endif

		// Sort utility
		sortByUtility( RCL, u );

#if VERBOSE
		for ( int i = 0; i < (int)RCL.size(); ++i )
		{
			std::cout << "__ " << i << ' ' << RCL[i] << ' ' << u[i] << std::endl;
		}
#endif

		int umin = u.back();
		int umax = u.front();
		int ulimit = umin + alpha * ( umax - umin );

		int s = 0;
		while ( s < (int)u.size() && u[s] >= ulimit ) ++s;

#if VERBOSE
		std::cout << "umin = " << umin << std::endl
			<< "umax = " << umax << std::endl
			<< "ulimit = " << ulimit << std::endl
			<< "s = " << s << std::endl;
#endif

		// Choose an eligible job (with only selected predecessors or no predecessor)
		int p = std::rand() % s;
		int q = RCL[p];

		// Append the job to the list
		selected[q] = true;
		_sequence[pos] = q;

		// Remove job from eligible list
		RCL.erase( RCL.begin() + p );

		Job * current_job = _jobs.at( q );

		// For the new selected job, find its successors
		for ( int i = 0; i < (int)current_job->getSuccessors().size(); ++i )
		{
			int j = current_job->getSuccessors().at( i )->getId();

			// To become eligible, a job must be unselected (and not already in eligible list)
			if ( !selected[j] && std::find( RCL.begin(), RCL.end(), j ) == RCL.end() )
			{
				// Successor has only selected predecessors ?
				bool has_only_selected_predecessors = true;
				for ( int k = 0; k < (int)_jobs[j]->getPredecessors().size() && has_only_selected_predecessors; ++k )
				{
					int l = _jobs[j]->getPredecessors().at( k )->getId();
					if ( selected[l] == false )
					{
						has_only_selected_predecessors = false;
					}
				}

				if ( has_only_selected_predecessors )
				{
					RCL.push_back( j );
				}
			}
		}
	}

	update();
}

void Solution::reset()
{
	// Reset resources
	for ( int k = 0; k < (int)_resources.size(); ++k )
	{
		_resources[k].reset();
	}

	// Reset jobs
	for ( int j = 0; j < (int)_jobs.size(); ++j )
	{
		_jobs[j]->reset();
	}
}

int Solution::getMinStartTimeOfJob( int j ) const
{
	int t = _jobs[j]->getPredMaxFinishTime();

	for ( int k = 0; k < (int)_resources.size(); ++k )
	{
		// Find the leftmost available space in all resources
		t = std::max( t, _resources[k].findSpace( t, _jobs[j]->getRequest( k ), _jobs[j]->getDuration() ) );
	}

	return t;
}

void Solution::update()
{
	for ( int j = 0; j < size(); ++j )
	{
		int t( 0 ), i( _sequence[j] );

		t = getMinStartTimeOfJob( i );
		_jobs[i]->setStartTime( t );

		for ( int k = 0; k < (int)_resources.size(); ++k )
		{
			// Add the current job to the scheduling
			_resources[k].assign( t, _jobs[i]->getRequest( k ), _jobs[i]->getDuration(), i+1 );
		}
	}
}

void Solution::exportGnuplot() const
{
	for ( int k = 0; k < (int)_resources.size(); ++k )
	{
		_resources[k].exportGnuplot();
	}
}

void Solution::exportGnuplot( std::ostream & os ) const
{
	for ( int k = 0; k < (int)_resources.size(); ++k )
	{
		_resources[k].exportGnuplot( os );
	}
}

void Solution::exportTable() const
{
	exportTable( std::cout );
}

void Solution::exportTable( std::ostream & os ) const
{
	for ( int k = 0; k < (int)_resources.size(); ++k )
	{
		os << "Resource " << k+1 << ':' << std::endl
		   << _resources[k] << std::endl;
	}
}

void Solution::clear()
{
	for ( int j = 0; j < (int)_jobs.size(); ++j )
	{
		delete _jobs[j];
	}
}

void Solution::sortByUtility( std::vector<int> & RCL, std::vector<int> & u ) const
{
	u.clear();
	for ( int i = 0; i < (int)RCL.size(); ++i )
	{
		int j = RCL[i];
		int r = 0;
		for ( int k = 0; k < (int)_resources.size(); ++k )
		{
			r += _jobs[k]->getRequest( k );
		}
		int v = _jobs[j]->getDuration() / r;

		bool placed = false;
		for ( int k = 0; k < i && !placed; ++k )
		{
			if ( v > u[k] )
			{
				u.insert( u.begin() + k, v );
				RCL.erase( RCL.begin() + i );
				RCL.insert( RCL.begin() + k, j );
				placed = true;
			}
		}

		if ( !placed )
			u.push_back( v );
	}
}

std::ostream & operator << ( std::ostream & os, const Solution & s )
{
	for ( int i = 0; i < s.size(); ++i )
	{
		if ( i > 0 ) os << ' ';
		os << s[i]+1;
	}
	return os;
}


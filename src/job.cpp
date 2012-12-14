#include "job.hpp"
#include <algorithm>
#include <limits>

Job::Job( int id, int rmax ) :
	_id( id ),
	_duration( 0 ),
	_start( 0 ),
	_finish( 0 ),
	_request( rmax )
{
}

Job::Job( const Job & job ) :
	_id( job._id ),
	_duration( job._duration ),
	_start( job._start ),
	_finish( job._finish ),
	_request( job._request ),
	_predecessors(), // Don't copy pointers to predecessors/successors
	_successors()
{
}

int Job::getPredMaxFinishTime() const
{
	int f( 0 );
	for ( int k = 0; k < (int)_predecessors.size(); ++k )
	{
		f = std::max( f, _predecessors[k]->getFinishTime() );
	}
	return f;
}

int Job::getSuccMinStartTime() const
{
	int s( std::numeric_limits<int>::max() );
	for ( int k = 0; k < (int)_successors.size(); ++k )
	{
		s = std::min( s, _successors[k]->getStartTime() );
	}
	return s;
}

void Job::reset()
{
	_start = 0;
	_finish = 0;
}

std::ostream & operator << ( std::ostream & os, const Job & s )
{
	return os;
}


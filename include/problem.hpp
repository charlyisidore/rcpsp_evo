#ifndef PROBLEM_HPP
#define PROBLEM_HPP

#include <vector>
#include <iostream>
#include <algorithm>

/*
	A class to store all the instance parameters which are meant to be constant.
	The variables of this class should not be modified during the solve procedure.
*/
class Problem
{
public:
	Problem();

	/*
		Return the number of jobs of the problem
	*/
	int getNumJobs() const;

	/*
		Return the number of resources of the problem
	*/
	int getNumResources() const;

	/*
		Return the horizon of the problem
	*/
	int getHorizon() const;

	/*
		Return the duration of the job j
	*/
	int getJobDuration( int j ) const;

	/*
		Return the request of the job j for the resource k
	*/
	int getJobRequest( int j, int k ) const;

	/*
		Return the capacity of the resource k
	*/
	int getResourceCapacity( int k ) const;

	/*
		Compute an upper bound of Cmax with sum of durations
	*/
	int getUpperBound() const;

	/*
		Return the predecessors of the job j
	*/
	const std::vector<int> & getJobPredecessors( int j ) const;

	/*
		Return the successors of the job j
	*/
	const std::vector<int> & getJobSuccessors( int j ) const;

	/*
		Set the number of jobs (allocate jmax jobs)
	*/
	void setNumJobs( int jmax );

	/*
		Set the number of resources (allocate rmax resources)
	*/
	void setNumResources( int rmax );

	/*
		Set the horizon of the problem
	*/
	void setHorizon( int tmax );

	/*
		Set the duration of the job j
	*/
	void setJobDuration( int j, int duration );

	/*
		Set the request of the job j for the resource k
	*/
	void setJobRequest( int j, int k, int request );

	/*
		Set the capacity of the resource k
	*/
	void setResourceCapacity( int k, int rmax );

	/*
		Add a successor s for a job j (and a predecessor j for s)
	*/
	void addJobSuccessor( int j, int s );

	/*
		Show the precedence graph on a window with graphviz
	*/
	void exportDotPrecedenceGraph() const;

	/*
		Export a DOT script for precedence graph in an output stream
	*/
	void exportDotPrecedenceGraph( std::ostream & os ) const;

	// Instance value reader
	static int readValue( const std::string & line );

private:
	int _numJobs,
	    _numResources,
	    _horizon;

	std::vector<int> _resourceCapacity,
	                 _jobDuration;

	std::vector< std::vector<int> > _jobRequest,
	                                _jobPredecessors,
	                                _jobSuccessors;
};

/*
	Show the problem on an output stream
*/
std::ostream & operator << ( std::ostream & os, const Problem & p );

/*
	Load the problem from an input stream
*/
std::istream & operator >> ( std::istream & is, Problem & p );

////////////////////////////////////////////////////////////////////////////////

inline int Problem::getNumJobs() const
{
	return _numJobs;
}

inline int Problem::getNumResources() const
{
	return _numResources;
}

inline int Problem::getHorizon() const
{
	return _horizon;
}

inline int Problem::getJobDuration( int j ) const
{
	return _jobDuration[j];
}

inline int Problem::getJobRequest( int j, int k ) const
{
	return _jobRequest[j][k];
}

inline int Problem::getResourceCapacity( int k ) const
{
	return _resourceCapacity[k];
}

inline const std::vector<int> & Problem::getJobPredecessors( int j ) const
{
	return _jobPredecessors[j];
}

inline const std::vector<int> & Problem::getJobSuccessors( int j ) const
{
	return _jobSuccessors[j];
}

inline void Problem::setHorizon( int tmax )
{
	_horizon = tmax;
}

inline void Problem::setJobDuration( int j, int duration )
{
	_jobDuration[j] = duration;
}

inline void Problem::setJobRequest( int j, int k, int request )
{
	_jobRequest[j][k] = request;
}

inline void Problem::setResourceCapacity( int k, int rmax )
{
	_resourceCapacity[k] = rmax;
}

inline void Problem::addJobSuccessor( int j, int s )
{
	_jobSuccessors[j].push_back( s );
	_jobPredecessors[s].push_back( j );
}

#endif

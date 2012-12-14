#ifndef JOB_HPP
#define JOB_HPP

#include <vector>
#include <iostream>

/*
	A class to handle jobs and precedence constraints.
	This class helps solving procedure with a precedence graph included in.
*/
class Job
{
public:
	Job( int id, int rmax );
	Job( const Job & job );

	/*
		Retrieve the id of the job
	*/
	int getId() const;

	/*
		Retrieve the duration of the job
	*/
	int getDuration() const;

	/*
		Retrieve the request of the job for the resource k
	*/
	int getRequest( int k ) const;

	/*
		Retrieve the start time of the job if set
	*/
	int getStartTime() const;

	/*
		Retrieve the finish time of the job if set
	*/
	int getFinishTime() const;

	/*
		Retrieve the max finish time of the predecessors
	*/
	int getPredMaxFinishTime() const;

	/*
		Retrieve the min start time of the successors
	*/
	int getSuccMinStartTime() const;

	/*
		Set the duration of the job
	*/
	void setDuration( int duration );

	/*
		Set the duration of the job
	*/
	void setRequest( int k, int req );

	/*
		Set the start time of the job
	*/
	void setStartTime( int t );

	/*
		Reset the variables of the job
	*/
	void reset();

	/*
		Get the list of predecessors
	*/
	const std::vector<Job *> & getPredecessors() const;

	/*
		Get the list of successors
	*/
	const std::vector<Job *> & getSuccessors() const;

	/*
		Add a predecessor
	*/
	void addPredecessor( Job * s );

	/*
		Add a successor
	*/
	void addSuccessor( Job * s );

private:
	int _id, _duration, _start, _finish;
	std::vector<int> _request;
	std::vector<Job *> _predecessors, _successors; // Array of jobs
};

std::ostream & operator << ( std::ostream & os, const Job & s );

////////////////////////////////////////////////////////////////////////////////

inline int Job::getId() const
{
	return _id;
}

inline int Job::getDuration() const
{
	return _duration;
}

inline int Job::getRequest( int k ) const
{
	return _request[k];
}

inline int Job::getStartTime() const
{
	return _start;
}

inline int Job::getFinishTime() const
{
	return _finish;
}

inline void Job::setDuration( int duration )
{
	_duration = duration;
}

inline void Job::setRequest( int k, int req )
{
	_request[k] = req;
}

inline void Job::setStartTime( int t )
{
	_start = t;
	_finish = t + _duration;
}

inline const std::vector<Job *> & Job::getPredecessors() const
{
	return _predecessors;
}

inline const std::vector<Job *> & Job::getSuccessors() const
{
	return _successors;
}

inline void Job::addPredecessor( Job * s )
{
	_predecessors.push_back( s );
}

inline void Job::addSuccessor( Job * s )
{
	_successors.push_back( s );
}

#endif

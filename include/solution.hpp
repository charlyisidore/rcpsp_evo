#ifndef SOLUTION_HPP
#define SOLUTION_HPP

#include "problem.hpp"
#include "job.hpp"
#include "resource.hpp"
#include <vector>
#include <iostream>

/*
	A class to handle a solution, i.e. a list of jobs.
	Contains helper procedures to compute a valid scheduling from a list.
*/
class Solution
{
public:
	Solution();
	Solution( const Problem & p );
	Solution( const Solution & s );
	virtual ~Solution();

	/*
		Set the problem of the solution
	*/
	void setProblem( const Problem & p );

	/*
		Reset the solution to 1,...,n
	*/
	void reset();

	/*
		Get the Cmax of the solution (must call s->update() to update Cmax)
	*/
	int getCmax() const;

	/*
		Get the minimum start time possible for job j
	*/
	int getMinStartTimeOfJob( int j ) const;

	/*
		Build a solution with GRASP procedure
	*/
	void grasp( double alpha );

	/*
		Get the source job
	*/
	Job * getSource() const;

	/*
		Get the source job
	*/
	Job * getSink() const;

	/*
		Get the job object at position j in the solution
	*/
	Job * getJobAt( int j ) const;

	/*
		Get the job id at position i in the solution
	*/
	int operator [] ( int i ) const;

	/*
		Get the job id at position i in the solution (editable)
	*/
	int & operator [] ( int i );

	/*
		Get the size of the solution
	*/
	int size() const;

	/*
		Update variables of the solution
	*/
	void update();

	/*
		Show the resource usage with gnuplot
	*/
	void exportGnuplot() const;

	/*
		Export a gnuplot script in an output stream for resource usage
	*/
	void exportGnuplot( std::ostream & os ) const;

	/*
		Show the resource usage with terminal
	*/
	void exportTable() const;

	/*
		Export a plain text table in an output stream for resource usage
	*/
	void exportTable( std::ostream & os ) const;

	/*
		Clear ALL memory of the solution (including problem)
	*/
	void clear();

private:
	std::vector<int> _sequence; // The solution
	std::vector<Job *> _jobs;   // Array of jobs
	std::vector<Resource> _resources;   // Array of resources

	void sortByUtility( std::vector<int> & RCL, std::vector<int> & u ) const;
};

std::ostream & operator << ( std::ostream & os, const Solution & s );

////////////////////////////////////////////////////////////////////////////////

inline int Solution::getCmax() const
{
	return getSink()->getFinishTime();
}

inline Job * Solution::getSource() const
{
	return _jobs.front();
}

inline Job * Solution::getSink() const
{
	return _jobs.back();
}

inline Job * Solution::getJobAt( int j ) const
{
	return _jobs[_sequence[j]];
}

inline int Solution::operator [] ( int i ) const
{
	return _sequence[i];
}

inline int & Solution::operator [] ( int i )
{
	return _sequence[i];
}

inline int Solution::size() const
{
	return _sequence.size();
}

#endif

#ifndef RESOURCE_HPP
#define RESOURCE_HPP

#include <vector>
#include <iostream>

/*
	A class to handle resources and capacity constraints.
	This class helps solving procedure with a table of resources used.
*/
class Resource
{
public:
	Resource();
	Resource( int tmax, int rmax );

	/*
		Reset all the variables in the resource
	*/
	void reset();

	/*
		Return the maximal t we can reach (defined by allocated memory size)
	*/
	int getMaxTime() const;

	/*
		Return the maximal resource usage feasible
	*/
	int getCapacity() const;

	/*
		Return the id of the job using the square at coordinates (t,r)
	*/
	int getUsage( int t, int r ) const;

	/*
		Return the number of squares available at period t
	*/
	int getAvailable( int t ) const;

	/*
		Return the leftmost period for which we can allocate req*duration squares
	*/
	int findSpace( int t, int req, int duration ) const;

	/*
		Add a job to the resource scheduling
	*/
	void assign( int t, int req, int duration, int j );

	/*
		Allocate a period interval [0,tmax-1]
	*/
	void setMaxTime( int tmax );

	/*
		Allocate a resource usage interval [0,rmax-1]
	*/
	void setCapacity( int rmax );

	/*
		Set the id of the job using the square at coordinates (t,r)
	*/
	void setUsage( int t, int r, int j );

	/*
		Allocate the period interval [0,tmax-1] and the resource usage interval [0,rmax-1]
	*/
	void resize( int tmax, int rmax );

	/*
		Open a gnuplot window to show the scheduling
	*/
	void exportGnuplot() const;

	/*
		Write a gnuplot script into a stream
	*/
	void exportGnuplot( std::ostream & out ) const;

private:
	std::vector< std::vector<int> > _use; // use[t][r] is the id of the job using the square (t,r)
};

std::ostream & operator << ( std::ostream & os, const Resource & s );

////////////////////////////////////////////////////////////////////////////////

inline void Resource::reset()
{
	resize( getMaxTime(), getCapacity() );
}

inline int Resource::getMaxTime() const
{
	return _use.size();
}

inline int Resource::getCapacity() const
{
	return _use.empty() ? 0 : _use[0].size();
}

inline int Resource::getUsage( int t, int r ) const
{
	return _use[t][r];
}

inline void Resource::setMaxTime( int tmax )
{
	resize( tmax, getCapacity() );
}

inline void Resource::setCapacity( int rmax )
{
	resize( getMaxTime(), rmax );
}

inline void Resource::setUsage( int t, int r, int j )
{
	_use[t][r] = j;
}

inline void Resource::resize( int tmax, int rmax )
{
	_use.clear();
	_use.resize( tmax, std::vector<int>( rmax ) );
}

#endif

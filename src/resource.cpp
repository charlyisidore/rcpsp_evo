#include "resource.hpp"
#include "pstream.h"
#include <set>
#include <iomanip>
#include <algorithm>
#include <functional>
#include <cmath>

Resource::Resource() :
	_use()
{
}

Resource::Resource( int tmax, int rmax ) :
	_use( tmax, std::vector<int>( rmax ) )
{
}

int Resource::getAvailable( int t ) const
{
	// Function: int -> bool, return true if the argument equals to 0, false otherwise
	std::binder2nd< std::equal_to<int> > is_zero( std::equal_to<int>(), 0 );

	// Count the number of squares which are equal to 0 in a specific period t
	return std::count_if( _use[t].begin(), _use[t].end(), is_zero );
}

// Find a rectangle of dimensions req*duration
//     +----------+
// req |          |
//     +----------+
//       duration
int Resource::findSpace( int t, int req, int duration ) const
{
	// One dimension is zero, no need to find a space
	if ( req == 0 || duration == 0 )
		return t;

	bool feasible( false );
	for ( int i = t; i < getMaxTime() && !feasible; ++i )
	{
		// To find a space, we have to have enough capacity during "len" consecutive iterations
		if ( req <= getAvailable( i ) )
		{
			feasible = ( ( 1 + i - t ) >= duration );
		}
		else
		{
			// Otherwise:
			// Update t, the current candidate beginning of an available space
			t = i+1;
		}
	}
	// Return -1 if no space has been found
	return feasible ? t : -1;
}

void Resource::assign( int t, int req, int duration, int j )
{
	for ( int i = t; i < t+duration; ++i )
	{
		// Fill units until charge reaches req.
		int charge( 0 );
		for( int r = 0; r < getCapacity(); ++r )
		{
			if ( getUsage( i, r ) == 0 )
			{
				setUsage( i, r, j );
				++charge;
			}

			if ( charge >= req )
				break;
		}
	}
}

void Resource::exportGnuplot() const
{
	// Make a pipe an write the gnuplot script on the fly
	redi::opstream out( "gnuplot -persistent" );
	exportGnuplot( out );
}

// Write a script for gnuplot
void Resource::exportGnuplot( std::ostream & os ) const
{
	std::set<int> styles;
	int k( 0 ), tmax( getMaxTime() ), rmax( getCapacity() ), xtics, ytics;

	// Get the bound of the scheduling (tmax)
	bool limit( false );
	for ( int t = tmax-1; t > 0 && !limit; --t )
	{
		for ( int r = 0; r < (int)getCapacity(); ++r )
		{
			if ( getUsage( t, r ) != 0 )
			{
				limit = true;
				tmax = t+1;
				break;
			}
		}
	}

	// To have ticks with space between
	xtics = std::pow( 1 + (int)std::log10( tmax ), 2 );
	ytics = std::pow( 1 + (int)std::log10( rmax ), 2 );

	// Options
	os
		<< "set key outside" << std::endl                  // Position of the legend
		<< "set xtics " << xtics << std::endl                      // 1 tic every time
		<< "set ytics " << ytics << std::endl                      // 1 tic every resource usage
		<< "set xrange [0:" << tmax << "]" << std::endl    // Width
		<< "set yrange [0:" << rmax << "]" << std::endl    // Height
		<< "set size ratio " << ( (double)rmax / (double)tmax ) << std::endl; // Aspect ratio

	// Draw all the filled squares in the scheduling
	for ( int t = 0; t < (int)getMaxTime(); ++t )
	{
		for ( int r = 0; r < (int)getCapacity(); ++r )
		{
			if ( getUsage( t, r ) != 0 )
			{
				++k;
				os
					<< "set object " << k
					<< " rectangle from " << t << ',' << r
					<< " to " << t+1 << ',' << r+1
					<< " fillstyle noborder fillcolor lt " << getUsage( t, r ) << std::endl;

				// Save the existing job numbers
				styles.insert( getUsage( t, r ) );
			}
		}
	}

	// Plot NaN display nothing, but this makes a legend appear.
	os << "plot NaN notitle";
	for ( std::set<int>::const_iterator it = styles.begin(); it != styles.end(); ++it )
	{
		os << ", NaN linecolor " << *it << " title '" << *it << "'";
	}
	os << std::endl;
}

// Display resource usage
std::ostream & operator << ( std::ostream & os, const Resource & s )
{
	int tmax( s.getMaxTime() ),
	    rmax( s.getCapacity() ),
	    jmax( 0 ),
	    c0w( std::log10( tmax )+2 ), // Size of the first column
	    crw( std::log10( rmax )+1 ); // Size of the other columns

	// Get the bound of the scheduling (tmax)
	bool limit( false );
	for ( int t = tmax-1; t > 0 && !limit; --t )
	{
		for ( int r = 0; r < (int)s.getCapacity(); ++r )
		{
			if ( s.getUsage( t, r ) != 0 )
			{
				limit = true;
				tmax = t+1;
				break;
			}
		}
	}

	// Compute maximal size of a column
	for ( int t = 0; t < tmax; ++t )
	{
		for ( int r = 0; r < rmax; ++r )
		{
			jmax = std::max( jmax, s.getUsage( t, r ) );
		}
	}
	crw = std::max( crw, (int)std::log10( jmax )+1 );

	// Header: t | 0 1 2 ...
	os << std::setfill( ' ' );
	os << std::setw( c0w ) << std::right << "t" << " |";
	for ( int r = 0; r < rmax; ++r )
	{
		os << ' ' << std::setw( crw ) << std::right << r;
	}

	// Separator : ---------...
	os << std::endl << std::setfill( '-' ) << std::setw( c0w+2 ) << std::right << "|"
		<< std::setw( rmax*(crw+1)+1 ) << std::left << "" << std::endl;

	// All the table : <t> | <ut0> <ut1> <ut2> ...
	os << std::setfill( ' ' );
	for ( int t = 0; t < tmax; ++t )
	{
		os << std::setw( c0w ) << std::right << t << " |";
		for ( int r = 0; r < rmax; ++r )
		{
			os << ' ' << std::setw( crw ) << std::right << s.getUsage( t, r );
		}
		os << std::endl;
	}
	return os;
}


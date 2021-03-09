#ifndef QWT_POINT_EXT_HPP_
#define QWT_POINT_EXT_HPP_

#include "qwt_global.h"
#include <qpoint.h>
#include <QColor>
#ifndef QT_NO_DEBUG_STREAM
#include <qdebug.h>
#endif

/*!
  \brief QwtPointExt class defines a extended point in double coordinates
*/

class QWT_EXPORT QwtPointExt
{
public:
	QwtPointExt();
	QwtPointExt( double x, double y, double color, double size );
	QwtPointExt( const QwtPointExt& );
	QwtPointExt( const QPointF& );

	bool isNull() const;

	double x() const;
	double y() const;
	double color() const;
	double size() const;

	double& rx();
	double& ry();
	double& rcolor();
	double& rsize();

	void setX( double x );
	void setY( double y );
	void setColor( double color );
	void setSize( double size );

	QPointF toPoint() const;

	bool operator==( const QwtPointExt& ) const;
	bool operator!=( const QwtPointExt& ) const;

private:
	double m_x;
	double m_y;
	double m_color;
	double m_size;
}; // class QWT_EXPORT

Q_DECLARE_TYPEINFO( QwtPointExt, Q_MOVABLE_TYPE );

#ifndef QT_NO_DEBUG_STREAM
QWT_EXPORT QDebug operator<<( QDebug, const QwtPointExt& );
#endif

/*!
	Constructs a null point.
	\sa isNull()
*/
inline QwtPointExt::QwtPointExt() :
	m_x    ( qQNaN() ),
	m_y    ( qQNaN() ),
	m_color( 0.0 ),
	m_size ( 5.0 )
{
}


//! Constructs a point with coordinates specified by x, y, color and size.
inline QwtPointExt::QwtPointExt( double x, double y, double color, double size ) :
	m_x    ( x ),
	m_y    ( y ),
	m_color( color ),
	m_size ( size )
{
}


/*!
	Copy constructor.
	Constructs a point using the values of the point specified.
*/
inline QwtPointExt::QwtPointExt( const QwtPointExt& other ) :
	m_x    ( other.m_x ),
	m_y    ( other.m_y ),
	m_color( other.m_color ),
	m_size ( other.m_size )
{
}


/*!
	Constructs a point with x and y coordinates from a 2D point,
	and a z coordinate of 0.
*/
inline QwtPointExt::QwtPointExt( const QPointF& other ) :
	m_x    ( other.x() ),
	m_y    ( other.y() ),
	m_color( 0.0 ),
	m_size ( 5.0 )
{
}


/*!
	\return True if the point is null; otherwise returns false.

	A point is considered to be null if x, y and z-coordinates
	are equal to zero.
*/
inline bool QwtPointExt::isNull() const
{
	return( qIsNaN( m_x ) && qIsNaN( m_y ) );
}


//! \return The x-coordinate of the point.
inline double QwtPointExt::x() const
{
	return( m_x );
}


//! \return The y-coordinate of the point.
inline double QwtPointExt::y() const
{
	return( m_y );
}


//! \return The color of the point.
inline double QwtPointExt::color() const
{
	return( m_color );
}


//! \return The size of the point.
inline double QwtPointExt::size() const
{
	return( m_size );
}


//! \return A reference to the x-coordinate of the point.
inline double& QwtPointExt::rx()
{
	return( m_x );
}


//! \return A reference to the y-coordinate of the point.
inline double& QwtPointExt::ry()
{
	return( m_y );
}


//! \return A reference to the color of the point.
inline double& QwtPointExt::rcolor()
{
	return( m_color );
}


//! \return A reference to the size of the point.
inline double& QwtPointExt::rsize()
{
	return( m_size );
}


//! Sets the x-coordinate of the point to the value specified by x.
inline void QwtPointExt::setX( double x )
{
	m_x = x;
}


//! Sets the y-coordinate of the point to the value specified by y.
inline void QwtPointExt::setY( double y )
{
	m_y = y;
}


//! Sets the color of the point to the value specified by color.
inline void QwtPointExt::setColor( double color )
{
	m_color = color;
}


//! Sets the size of the point to the value specified by size.
inline void QwtPointExt::setSize( double size )
{
	m_size = size;
}


/*!
   \return 2D point.
*/
inline QPointF QwtPointExt::toPoint() const
{
	return( QPointF( m_x, m_y ) );
}


//! \return True, if this point and other are equal; otherwise returns false.
inline bool QwtPointExt::operator==( const QwtPointExt& other ) const
{
	return( ( m_x == other.m_x ) && ( m_y == other.m_y ) && ( m_color == other.m_color ) && ( m_size == other.m_size ) );
}


//! \return True if this rect and other are different; otherwise returns false.
inline bool QwtPointExt::operator!=( const QwtPointExt& other ) const
{
	return( !operator==( other ) );
}


#endif // ifndef QWT_POINT_EXT_HPP_

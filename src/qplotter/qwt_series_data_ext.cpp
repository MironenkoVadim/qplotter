#include "qwt_series_data_ext.hpp"

#include <qwt_series_data.h>

static inline QRectF qwtBoundingRect( const QwtPointExt& sample )
{
	return( QRectF( sample.x(), sample.y(), 0.0, 0.0 ) );
}


/*!
  \brief Calculate the bounding rectangle of a series subset

  Slow implementation, that iterates over the series.

  \param series Series
  \param from Index of the first sample, <= 0 means from the beginning
  \param to Index of the last sample, < 0 means to the end

  \return Bounding rectangle
*/
QRectF qwtBoundingRect( const QwtSeriesData< QwtPointExt >& series, int from, int to )
{
	QRectF boundingRect( 1.0, 1.0, -2.0, -2.0 ); // invalid;

	if ( from < 0 )
	{
		from = 0;
	}

	if ( to < 0 )
	{
		to = static_cast< int >( series.size() ) - 1;
	}

	if ( to < from )
	{
		return( boundingRect );
	}

	int i;
	for ( i = from; i <= to; i++ )
	{
		const QRectF rect = qwtBoundingRect( series.sample( i ) );
		if ( ( rect.width() >= 0.0 ) && ( rect.height() >= 0.0 ) )
		{
			boundingRect = rect;
			i++;
			break;
		}
	}

	for ( ; i <= to; i++ )
	{
		const QRectF rect = qwtBoundingRect( series.sample( i ) );
		if ( ( rect.width() >= 0.0 ) && ( rect.height() >= 0.0 ) )
		{
			boundingRect.setLeft( qMin( boundingRect.left(), rect.left() ) );
			boundingRect.setRight( qMax( boundingRect.right(), rect.right() ) );
			boundingRect.setTop( qMin( boundingRect.top(), rect.top() ) );
			boundingRect.setBottom( qMax( boundingRect.bottom(), rect.bottom() ) );
		}
	}

	return( boundingRect );
} // qwtBoundingRect


/*!
   Constructor
   \param samples Samples
*/
QwtPointExtSeriesData::QwtPointExtSeriesData(
	const QVector< QwtPointExt >& samples ) :
	QwtArraySeriesData< QwtPointExt >( samples )
{
}


/*!
  \brief Calculate the bounding rectangle

  The bounding rectangle is calculated once by iterating over all
  points and is stored for all following requests.

  \return Bounding rectangle
*/
QRectF QwtPointExtSeriesData::boundingRect() const
{
	if ( d_boundingRect.width() < 0.0 )
	{
		d_boundingRect = qwtBoundingRect( *this );
	}

	return( d_boundingRect );
}

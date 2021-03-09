#include "qwt_plot_extcurve.hpp"
#include "qwt_series_data_ext.hpp"

#include <qwt_color_map.h>
#include <qwt_painter.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_map.h>


class QwtPlotExtCurve::PrivateData
{
public:
	PrivateData() :
		colorRange     ( 0.0, 1.0 ),
		paintAttributes( QwtPlotExtCurve::ClipPoints )
	{
		colorMap = new QwtLinearColorMap();
	}


	~PrivateData()
	{
		delete colorMap;
	}


	QwtColorMap*                     colorMap;
	QwtInterval                      colorRange;
	QVector< QRgb >                  colorTable;
	QwtPlotExtCurve::PaintAttributes paintAttributes;
}; // class QwtPlotExtCurve::PrivateData

/*!
  Constructor
  \param title Title of the curve
*/
QwtPlotExtCurve::QwtPlotExtCurve( const QwtText& title ) :
	QwtPlotSeriesItem( title )
{
	init();
}


/*!
  Constructor
  \param title Title of the curve
*/
QwtPlotExtCurve::QwtPlotExtCurve( const QString& title ) :
	QwtPlotSeriesItem( QwtText( title ) )
{
	init();
}


//! Destructor
QwtPlotExtCurve::~QwtPlotExtCurve()
{
	delete d_data;
}


/*!
  \brief Initialize data members
*/
void QwtPlotExtCurve::init()
{
	setItemAttribute( QwtPlotItem::Legend );
	setItemAttribute( QwtPlotItem::AutoScale );

	d_data = new PrivateData;
	setData( new QwtPointExtSeriesData() );

//	setZ( 20.0 );
}


//! \return QwtPlotItem::Rtti_PlotSpectroCurve
int QwtPlotExtCurve::rtti() const
{
	return( QwtPlotItem::Rtti_PlotSpectroCurve );
}


/*!
  Specify an attribute how to draw the curve

  \param attribute Paint attribute
  \param on On/Off
  /sa PaintAttribute, testPaintAttribute()
*/
void QwtPlotExtCurve::setPaintAttribute( PaintAttribute attribute, bool on )
{
	if ( on )
	{
		d_data->paintAttributes |= attribute;
	}
	else
	{
		d_data->paintAttributes &= ~attribute;
	}
}


/*!
	\return True, when attribute is enabled
	\sa PaintAttribute, setPaintAttribute()
*/
bool QwtPlotExtCurve::testPaintAttribute( PaintAttribute attribute ) const
{
	return ( d_data->paintAttributes & attribute );
}


/*!
  Initialize data with an array of samples.
  \param samples Vector of points
*/
void QwtPlotExtCurve::setSamples( const QVector< QwtPointExt >& samples )
{
	setData( new QwtPointExtSeriesData( samples ) );
}


/*!
  Assign a series of samples

  setSamples() is just a wrapper for setData() without any additional
  value - beside that it is easier to find for the developer.

  \param data Data
  \warning The item takes ownership of the data object, deleting
		   it when its not used anymore.
*/
void QwtPlotExtCurve::setSamples(
	QwtSeriesData< QwtPointExt >* data )
{
	setData( data );
}


/*!
  Change the color map

  Often it is useful to display the mapping between intensities and
  colors as an additional plot axis, showing a color bar.

  \param colorMap Color Map

  \sa colorMap(), setColorRange(), QwtColorMap::color(),
	  QwtScaleWidget::setColorBarEnabled(), QwtScaleWidget::setColorMap()
*/
void QwtPlotExtCurve::setColorMap( QwtColorMap* colorMap )
{
	if ( colorMap != d_data->colorMap )
	{
		delete d_data->colorMap;
		d_data->colorMap = colorMap;
	}

	legendChanged();
	itemChanged();
}


/*!
   \return Color Map used for mapping the intensity values to colors
   \sa setColorMap(), setColorRange(), QwtColorMap::color()
*/
const QwtColorMap* QwtPlotExtCurve::colorMap() const
{
	return( d_data->colorMap );
}


/*!
   Set the value interval, that corresponds to the color map

   \param interval interval.minValue() corresponds to 0.0,
				   interval.maxValue() to 1.0 on the color map.

   \sa colorRange(), setColorMap(), QwtColorMap::color()
*/
void QwtPlotExtCurve::setColorRange( const QwtInterval& interval )
{
	if ( interval != d_data->colorRange )
	{
		d_data->colorRange = interval;

		legendChanged();
		itemChanged();
	}
}


/*!
  \return Value interval, that corresponds to the color map
  \sa setColorRange(), setColorMap(), QwtColorMap::color()
*/
QwtInterval& QwtPlotExtCurve::colorRange() const
{
	return( d_data->colorRange );
}


/*!
  Draw a subset of the points

  \param painter Painter
  \param xMap Maps x-values into pixel coordinates.
  \param yMap Maps y-values into pixel coordinates.
  \param canvasRect Contents rectangle of the canvas
  \param from Index of the first sample to be painted
  \param to Index of the last sample to be painted. If to < 0 the
		 series will be painted to its last sample.

  \sa drawDots()
*/
void QwtPlotExtCurve::drawSeries( QPainter* painter,
                                  const QwtScaleMap& xMap, const QwtScaleMap& yMap,
                                  const QRectF& canvasRect, int from, int to ) const
{
	if ( !painter || ( dataSize() <= 0 ) )
	{
		return;
	}

	if ( to < 0 )
	{
		to = static_cast< int >( dataSize() ) - 1;
	}

	if ( from < 0 )
	{
		from = 0;
	}

	if ( from > to )
	{
		return;
	}

	drawDots( painter, xMap, yMap, canvasRect, from, to );
} // QwtPlotExtCurve::drawSeries


/*!
  Draw a subset of the points

  \param painter Painter
  \param xMap Maps x-values into pixel coordinates.
  \param yMap Maps y-values into pixel coordinates.
  \param canvasRect Contents rectangle of the canvas
  \param from Index of the first sample to be painted
  \param to Index of the last sample to be painted. If to < 0 the
		 series will be painted to its last sample.

  \sa drawSeries()
*/
void QwtPlotExtCurve::drawDots( QPainter* painter,
                                const QwtScaleMap& xMap, const QwtScaleMap& yMap,
                                const QRectF& canvasRect, int from, int to ) const
{
	if ( !d_data->colorRange.isValid() )
	{
		return;
	}

	const QwtColorMap::Format format = d_data->colorMap->format();
	if ( format == QwtColorMap::Indexed )
	{
		d_data->colorTable = d_data->colorMap->colorTable( d_data->colorRange );
	}

	const QwtSeriesData< QwtPointExt >* series = data();

	for ( int i = from; i <= to; i++ )
	{
		const QwtPointExt sample = series->sample( i );

		double xi = xMap.transform( sample.x() );
		double yi = yMap.transform( sample.y() );

		if ( d_data->paintAttributes & QwtPlotExtCurve::ClipPoints )
		{
			if ( !canvasRect.contains( xi, yi ) )
			{
				continue;
			}
		}

		auto size  = sample.size();
		auto shift = size / 2;
		if ( format == QwtColorMap::RGB )
		{
			painter->setPen( QPen( QBrush( QColor::fromHsv( static_cast< int >( sample.color() * 240.0 ), 255, 255 ) ), size ) );
		}
		else
		{
			const unsigned char index = d_data->colorMap->colorIndex(
				d_data->colorRange, sample.color() );
			painter->setPen( QPen( QColor::fromRgba( d_data->colorTable[index] ), size ) );
		}

		QwtPainter::drawEllipse( painter, QRectF( xi - shift, yi - shift, size, size ) );
	}

	d_data->colorTable.clear();
} // QwtPlotExtCurve::drawDots

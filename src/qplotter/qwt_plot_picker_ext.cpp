#include "qwt_plot_picker_ext.hpp"

QwtPlotPickerExt::QwtPlotPickerExt( int xAxis, int yAxis, RubberBand rubberBand, DisplayMode trackerMode, QWidget* canvas ) :
	QwtPlotPicker( xAxis, yAxis, rubberBand, trackerMode, canvas )
{
}


QwtText QwtPlotPickerExt::trackerText( const QPoint& pos ) const
{
	emit mouseMoved( pos );
	auto p = invTransform( pos );
	return( QwtText( QString::number( static_cast< int >( p.x() ) ) + ": " + QString::number( p.y(), 'f', 1 ) ) );
}

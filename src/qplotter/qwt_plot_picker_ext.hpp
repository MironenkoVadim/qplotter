#ifndef QWT_PLOT_PICKER_EXT_HPP_
#define QWT_PLOT_PICKER_EXT_HPP_

#include <qwt_plot_canvas.h>
#include <qwt_plot_picker.h>


class QwtPlotPickerExt : public QwtPlotPicker
{
	Q_OBJECT

public:
	QwtPlotPickerExt( int xAxis, int yAxis, RubberBand rubberBand, DisplayMode trackerMode, QWidget* canvas );
	QPointF point( QPoint pos ) { return( invTransform( pos ) ); }

signals:
	void mouseMoved( const QPoint& pos ) const;

protected:
	virtual QwtText trackerText( const QPoint& pos ) const override;
};

#endif // ifndef QWT_PLOT_PICKER_EXT_HPP_

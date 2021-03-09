#ifndef MARKS_PLOT_HPP_
#define MARKS_PLOT_HPP_

#include "qwt_point_ext.hpp"

#include <qwt_plot.h>
#include <qwt_plot_curve.h>

#include <QLabel>

class QwtPlotExtCurve;
class QwtPlotDirectPainter;

class MarksPlot : public QwtPlot
{
	Q_OBJECT

public:
	MarksPlot( QLabel* labelX, QLabel* labelY, QWidget* parent = nullptr );
	virtual ~MarksPlot();

	void appendPoint( QwtPointExt );
	void clearPoints();

	Q_SLOT void showSymbols( bool );
	QwtPlotCurve* y0Line() { return( m_y0Line ); }

protected:
	virtual void enterEvent( QEvent* event ) override final;
	virtual void leaveEvent( QEvent* event ) override final;

private:
	QwtPlotExtCurve*      m_curve { nullptr };
	QwtPlotCurve*         m_y0Line { nullptr };
	QwtPlotDirectPainter* m_directPainter { nullptr };
	QLabel*               m_labelX { nullptr };
	QLabel*               m_labelY { nullptr };
}; // class MarksPlot

#endif // ifndef MARKS_PLOT_HPP_

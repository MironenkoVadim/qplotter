#ifndef QWT_PLOT_EXTCURVE_HPP_
#define QWT_PLOT_EXTCURVE_HPP_

#include "qwt_point_ext.hpp"

#include <qwt_global.h>
#include <qwt_plot_seriesitem.h>
#include <qwt_series_data.h>

class QwtSymbol;
class QwtColorMap;

/*!
	\brief Curve that displays 3D points as dots, where the z coordinate is
		   mapped to a color.
*/
class QWT_EXPORT QwtPlotExtCurve : public QwtPlotSeriesItem, public QwtSeriesStore< QwtPointExt >
{
public:
	//! Paint attributes
	enum PaintAttribute
	{
		//! Clip points outside the canvas rectangle
		ClipPoints = 1
	};

	//! Paint attributes
	typedef QFlags< PaintAttribute > PaintAttributes;

	explicit QwtPlotExtCurve( const QString& title = QString() );
	explicit QwtPlotExtCurve( const QwtText& title );

	virtual ~QwtPlotExtCurve();

	virtual int rtti() const override;

	void setPaintAttribute( PaintAttribute, bool on = true );
	bool testPaintAttribute( PaintAttribute ) const;

	void setSamples( const QVector< QwtPointExt >& );
	void setSamples( QwtSeriesData< QwtPointExt >* );


	void setColorMap( QwtColorMap* );
	const QwtColorMap* colorMap() const;

	void setColorRange( const QwtInterval& );
	QwtInterval& colorRange() const;

	virtual void drawSeries( QPainter*,
	                         const QwtScaleMap& xMap, const QwtScaleMap& yMap,
	                         const QRectF& canvasRect, int from, int to ) const override;

protected:
	virtual void drawDots( QPainter*,
	                       const QwtScaleMap& xMap, const QwtScaleMap& yMap,
	                       const QRectF& canvasRect, int from, int to ) const;

private:
	void init();

	class PrivateData;
	PrivateData* d_data;
}; // class QWT_EXPORT

Q_DECLARE_OPERATORS_FOR_FLAGS( QwtPlotExtCurve::PaintAttributes )

#endif // ifndef QWT_PLOT_EXTCURVE_HPP_

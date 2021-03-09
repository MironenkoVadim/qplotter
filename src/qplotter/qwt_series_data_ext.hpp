#ifndef QWT_SERIES_DATA_EXT_HPP_
#define QWT_SERIES_DATA_EXT_HPP_

#include "qwt_point_ext.hpp"

#include <qwt_series_data.h>

//! Interface for iterating over an array of ext points
class QWT_EXPORT QwtPointExtSeriesData : public QwtArraySeriesData< QwtPointExt >
{
public:
	QwtPointExtSeriesData(
		const QVector< QwtPointExt >& = QVector< QwtPointExt >( ) );
	virtual QRectF boundingRect() const override;
};

QWT_EXPORT QRectF qwtBoundingRect(
	const QwtSeriesData< QwtPointExt >&, int from = 0, int to = -1 );

#endif // ifndef QWT_SERIES_DATA_EXT_HPP_

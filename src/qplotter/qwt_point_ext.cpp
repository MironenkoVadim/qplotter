#include "qwt_point_ext.hpp"

#ifndef QT_NO_DEBUG_STREAM

QDebug operator<<( QDebug debug, const QwtPointExt& point )
{
	debug.nospace() << "QwtPointExt(" << point.x() << "," << point.y() << ","
	                << point.color() << "," << point.size() << ")";
	return( debug.space() );
}


#endif

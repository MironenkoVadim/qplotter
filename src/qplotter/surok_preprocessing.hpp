#ifndef SUROK_PREPROCESSING_HPP_
#define SUROK_PREPROCESSING_HPP_

#include "ui_surok_preprocessing.h"
#include "surok_exchange.hpp"
#include "marks_plot.hpp"
#include "check_list.hpp"

#include <QTimer>
#include <QWidget>
#include <iostream>
#include <memory>

#include <qwt3d_color.h>
#include <qwt3d_drawable.h>
#include <qwt3d_plot.h>
#include <qwt3d_surfaceplot.h>
#include <qwt_point_3d.h>
#include "marks_plot.hpp"

#include <veer/protocols/preproc/marks_packet.hpp>
#include <veer/protocols/preproc/spectrums_packet.hpp>
#include <veer/protocols/preproc/spectrums_request.hpp>

#include <qwt_plot_curve.h>
#include <qwt_legend.h>
#include <qwt_plot_grid.h>
#include <qwt_symbol.h>

namespace VTR = veer::types::radar;
namespace VPP = veer::protocols::preproc;

class Spectrum3D;

class SurokPreprocessing : public QWidget, private Ui::SurokPreprocessing
{
	Q_OBJECT

public:
	explicit SurokPreprocessing( QWidget* parent = nullptr );
	void setSurokExchange( SurokExchange* value ) { m_surokExchange = value; }
	SurokExchange* surokExchange() { return( m_surokExchange ); }

	Q_SLOT void receiveData();
	Q_SIGNAL void update3D();

private:
	std::pair< int, int >   m_indexTable { 0, 0 };
	std::complex< double >* m_dataDrawing { nullptr };
	SurokExchange*          m_surokExchange { nullptr };

	int                                 m_axis { 0 };
	QPolygonF                           m_listPointTop;
	QPolygonF                           m_listPointBottom;
	QHash< int, std::vector< double > > m_vecBottom;
	QHash< int, std::vector< double > > m_vecTop;

	Qwt3D::SurfacePlot  m_surface;
	Spectrum3D*         m_spectrum3d;
	CheckList*          m_receivePrimaryChannels;
	CheckList*          m_receiveSecondaryChannels;
	QStandardItemModel* m_modelTableInfoMarks;
//	void paintEvent( QPaintEvent* event );

	QwtPlotGrid*  m_grid;
	QwtPlotCurve* m_curvBottom, * m_curvTop;

	void onTableClicked( const QModelIndex& index );
	void cuttingX();
	void cuttingY();
	void drawPoints();
	void createInfoModelMarks();
	void sendDataToDrawing();
	void draw3DGraph();
	void createSpectrum3D();
	void setupConnections();
	void splitter();

	Q_SLOT void slSendSpectrumRequest();
	Q_SLOT void slReceiveCheckboxToggled();
	Q_SIGNAL void valueChanged( veer::protocols::preproc::SpectrumsRequest );

	friend class Spectrum3D;
}; // class SurokPreprocessing

#endif // ifndef SUROK_PREPROCESSING_HPP_

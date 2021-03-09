#ifndef SUROK_MARKS_PLOTTER_HPP_
#define SUROK_MARKS_PLOTTER_HPP_

#include "marks_plot.hpp"
#include "qwt_plot_picker_ext.hpp"

#include "ui_main_window.h"
#include "ui_surok_marks_plotter.h"

#include "surok_exchange.hpp"
#include "surok_preprocessing.hpp"

#include <veer/common/ports.hpp>
#include <veer/protocols/base/header.hpp>
#include <veer/protocols/preproc/marks_packet.hpp>
#include <veer/protocols/preproc/spectrums_packet.hpp>
#include <veer/types/radar/zone_index.hpp>

#include <QFile>
#include <QTimer>
#include <iostream>
#include <QStandardItemModel>

namespace VTR = veer::types::radar;
namespace VTC = veer::types::common;
namespace VTP = veer::types::preproc;

struct Marks
{
	uint16_t periodsCount { 0 };           // кол-во каналов скорости
	uint16_t distanceChannels { 0 };       // кол-во каналов дальности
	uint16_t distanceIndex { UINT16_MAX }; // Номер дальностного фильтра
	uint16_t dopplerIndex { UINT16_MAX };
	uint16_t azimuthIndex { UINT16_MAX };

	uint32_t                cycleNumber { 0 };
	float                   az { 0.0 };
	float                   r { 0.0 };
	float                   el { 0.0 };
	float                   vel { 0.0 };
	float                   noise { 0.0 };
	float                   periodDuration { 0.0 };
	std::complex< double >* dataSpectr { nullptr };
	VTR::ZoneIndex          zoneNum;
};


inline float degreeToRadian( const float angle )
{
	constexpr float toRadian = static_cast< float >( M_PI / 180.0 );
	return( angle * toRadian );
}


inline float radianToDegree( const float angle )
{
	constexpr float toDegree = static_cast< float >( 180.0F / M_PI );
	return( angle * toDegree );
}


namespace VPB = veer::protocols::base;
namespace VPP = veer::protocols::preproc;

class SurokMarksPlotter : public QWidget, private Ui::SurokMarksPlotter
{
	Q_OBJECT


public:
	explicit SurokMarksPlotter( QWidget* parent = nullptr );
	QWidget* azimuthWidget() const;
	QWidget* distanceWidget() const;
	QWidget* velocityWidget() const;
	QWidget* elevationWidget() const;

	bool getEnableTab() const;
	void setEnableTab( bool value );

	Q_SLOT void onDisconnected();
	Q_SLOT void onConnected();
	Q_SLOT void onError( QAbstractSocket::SocketError );

	Q_SLOT void receiveFromSocketMark();
	Q_SLOT void receiveFromSocketSpectr();

	void setSurokExchange( SurokExchange* value ) { m_surokExchange = value; }

private:
	MarksPlot*     m_plotDistance;
	MarksPlot*     m_plotVelocity;
	MarksPlot*     m_plotAzimuth;
	MarksPlot*     m_plotElevation;
	SurokExchange* m_surokExchange { nullptr };

	bool    m_enableTab;
	QFile   m_file;
	QString m_marksUrl;
	quint16 m_marksPort;

	QList < QList< Marks > > m_packetList;

	QTimer m_timer;
	QTimer m_timerRequest;
	QTimer m_connectionChecker;

	QDataStream         m_stream;
	QStandardItemModel* m_modelTable;

	QwtPlotPickerExt* m_pickerAzimuth;
	QwtPlotPickerExt* m_pickerDistance;
	QwtPlotPickerExt* m_pickerVelocity;
	QwtPlotPickerExt* m_pickerElevation;

	QwtPlotCurve m_minDistanceLine;
	QwtPlotCurve m_midDistanceLine;
	QwtPlotCurve m_maxDistanceLine;

	void drawPoints();
	template< typename T >
	void assigment( T& );
	void assigmentSpectr( VPP::SpectrumsData& );
	void assigmentMark( VPP::MarksData& );
	void createInfoModel();

	void setViewFileNameRB();
	void setViewHostRB();
	void defaultSettings();

	void splitter();
	void clearGraph();
	void spinboxSize();

	void connectToHost();

	void legendColorMax();
	void legendColorMin();
	void updateMinSNRRadius();
	void updateMaxSNRRadius();

	void visiblePeriodsUpdate( MarksPlot* plot, QSpinBox* sb );
	QwtPlotPickerExt* createPicker( MarksPlot* plot );
	std::tuple< MarksPlot*, QwtPlotPickerExt* > createPlot( QWidget* parent, QLabel* coordsLabelX, QLabel* coordsLabelY );

	Q_SLOT void updateMax( MarksPlot* plot, QSpinBox* min, QSpinBox* max );
	Q_SLOT void updateMin( MarksPlot* plot, QSpinBox* min, QSpinBox* max );

	Q_SLOT void readFromFile();

	Q_SLOT void setFileName();
	Q_SLOT void protocolSettings();
	Q_SLOT void setupConnections();
//	Q_SLOT void modeRB( MarksPlot* plot, double size );
	Q_SLOT void sliderValueUpdate( MarksPlot* plot, QwtPlotPickerExt* picker );
}; // class SurokMarksPlotter

#endif // ifndef SUROK_MARKS_PLOTTER_HPP_

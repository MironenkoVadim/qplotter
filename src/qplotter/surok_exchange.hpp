#ifndef SUROK_EXCHANGE_HPP_
#define SUROK_EXCHANGE_HPP_

#include <QTcpSocket>
#include <iostream>

#include "ui_main_window.h"
#include "ui_surok_marks_plotter.h"

#include <veer/protocols/preproc/marks_packet.hpp>
#include <veer/protocols/preproc/spectrums_packet.hpp>

namespace VTR = veer::types::radar;
namespace VTC = veer::types::common;
namespace VTP = veer::types::preproc;
namespace VPB = veer::protocols::base;
namespace VPP = veer::protocols::preproc;

class SurokExchange : public QObject
{
	Q_OBJECT

public:
	SurokExchange();

	QList< VPP::MarksData >&     marksDataList();
	QList< VPP::SpectrumsData >& spectrumsDataList();

	Q_SIGNAL void newUpdateMark();
	Q_SIGNAL void newUpdateSpectr();
	Q_SIGNAL void socketConnected();
	Q_SLOT void onReadyRead();

	QTcpSocket* socket();

private:

	Q_SLOT void onConnected();
	Q_SLOT void onDisconnected();
	Q_SLOT void onError( QAbstractSocket::SocketError );
	Q_SLOT void setupConnections();

private:
	QTcpSocket m_socket;

	QList< VPP::MarksData >     m_marksDataList;
	QList< VPP::SpectrumsData > m_spectrumsDataList;
}; // class SurokExchange

#endif // SUROK_EXCHANGE_HPP_

#include "surok_exchange.hpp"

SurokExchange::SurokExchange()
{
	setupConnections();
}


void SurokExchange::setupConnections()
{
	connect( &m_socket, &QTcpSocket::connected, this, &SurokExchange::onConnected );
	connect( &m_socket, &QTcpSocket::readyRead, this, &SurokExchange::onReadyRead );
	connect( &m_socket, &QTcpSocket::disconnected, this, &SurokExchange::onDisconnected );
	connect( &m_socket, QOverload< QAbstractSocket::SocketError >::of( &QTcpSocket::error ), this, &SurokExchange::onError );
}


QTcpSocket* SurokExchange::socket()
{
	return( &m_socket );
}


QList< VPP::SpectrumsData >& SurokExchange::spectrumsDataList()
{
	return( m_spectrumsDataList );
}


QList< VPP::MarksData >& SurokExchange::marksDataList()
{
	return( m_marksDataList );
}


void SurokExchange::onConnected()
{
	qWarning() << tr( "Connect to server" ) << "\n";
	Q_EMIT socketConnected();
	qDebug() << "Q_EMIT socketConnected()";
}


void SurokExchange::onDisconnected()
{
	qWarning() << tr( "Disconnect to server" ) << "\n";
}


void SurokExchange::onError( QAbstractSocket::SocketError err )
{
	qWarning() << "Error: " + ( err == QAbstractSocket::HostNotFoundError ?
	                            "The host was not found." :
	                            err == QAbstractSocket::RemoteHostClosedError ?
	                            "The remote host is closed." :
	                            err == QAbstractSocket::ConnectionRefusedError ?
	                            "The connection was refused." :
	                            QString( m_socket.errorString() )
	                            );
}


void SurokExchange::onReadyRead()
{
	auto ba = m_socket.bytesAvailable();
	qDebug() << "bytesAvailable: " << ba;
	if ( ba < static_cast< int >( sizeof( VPB::Header ) ) ) { return; }

	QByteArray  block = m_socket.read( sizeof( VPB::Header ) );
	QDataStream in( block );
	VPB::Header header;
	in >> header;

	if ( header.size() <= ba )
	{
		block.append( m_socket.read( header.size() - sizeof( VPB::Header ) ) );
	}

	while ( header.size() > block.size() )
	{
		auto waitTime = m_socket.waitForReadyRead( 30000 );
		if ( !waitTime )
		{
			m_socket.disconnectFromHost();
			return;
		}
		block.append( m_socket.read( header.size() - block.size() ) );
	}

	QDataStream in2( block );
	if ( header.type() == VTC::CodogramType::MarksPack )
	{
		VPP::MarksData marksData;
		in2 >> marksData;
		m_marksDataList.append( std::move( marksData ) );
		Q_EMIT newUpdateMark();
	}

	if ( header.type() == VTC::CodogramType::NoiseMarksPack )
	{
		VPP::MarksData marksData { true };
		in2 >> marksData;
		m_marksDataList.append( std::move( marksData ) );
		Q_EMIT newUpdateMark();
	}

	if ( header.type() == VTC::CodogramType::SpectrumsPack )
	{
		VPP::SpectrumsData spectrumsData;
		in2 >> spectrumsData;
		m_spectrumsDataList.append( std::move( spectrumsData ) );
		Q_EMIT newUpdateSpectr();
	}
} // SurokExchange::onReadyRead

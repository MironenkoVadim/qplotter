#include "application.hpp"
#include "cmlib_private_config.hpp"
#include "main_window.hpp"

#include <myx/qt/translators.hpp>
#include <myx/qt/posix_signal_watcher.hpp>

#include <QCoreApplication>


namespace MQ = myx::qt;

int main( int argc, char** argv )
{
	Application app( argc, argv );
	QCoreApplication::setOrganizationName( CMLIB_ORGANIZATION_NAME );
	QCoreApplication::setApplicationName( CMLIB_PROJECT_NAME );

	// Подключение переводов
	MQ::QTranslatorsList tl;
	MQ::append_translators( tl, QStringLiteral( CMLIB_PROJECT_NAME ) );

	// Обработка сигналов POSIX
	MQ::PosixSignalWatcher signalWatcher;
	signalWatcher.watchForSignal( SIGINT );
	signalWatcher.watchForSignal( SIGTERM );
	signalWatcher.watchForSignal( SIGHUP );
	QObject::connect( &signalWatcher, &MQ::PosixSignalWatcher::posixSignal, &app, &Application::handlePosixSignal );

	MainWindow mw;
	mw.show();

	QTimer starter;
	starter.setSingleShot( true );
	QObject::connect( &starter, &QTimer::timeout, &app, &Application::start );
	starter.start( 0 );

	return ( Application::exec() );
} // main

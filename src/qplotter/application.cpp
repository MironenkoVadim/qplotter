#include "application.hpp"
#include "cmlib_private_config.hpp"

#include <myx/filesystem/paths.hpp>

#include <csignal>

#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDebug>
#include <QSettings>

namespace MF = myx::filesystem;

Application::Application( int& argc, char** argv ) :
	QApplication( argc, argv ),
	m_startTime ( QDateTime::currentDateTime() )
{
}


Application::~Application() = default;


void Application::readSettings()
{
	auto& paths = MF::Paths::instance();
	paths.init( QStringLiteral( CMLIB_PROJECT_NAME ) );
	auto* settings = new QSettings( paths.configFilePath(), QSettings::IniFormat );

	settings->sync();
	settings->deleteLater();
} // Application::readSettings


void Application::start()
{
	readSettings();
} // Application::start


void Application::handlePosixSignal( int signal )
{
	qDebug() << "Got signal: " << signal;
	if ( signal != SIGHUP )
	{
		QApplication::quit();
	}
} // Application::handlePosixSignal

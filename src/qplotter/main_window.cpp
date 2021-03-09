#include "main_window.hpp"

#include <QFileDialog>
#include <QStandardItemModel>
#include <QTableView>

MainWindow::MainWindow( QMainWindow* parent ) :
	QMainWindow   ( parent ),
	Ui::MainWindow()
{
	setupUi( this );
	defaultSettings();
	connection();
//	drawPointsTab->layout()->addWidget( &m_surokDrawPoints );
	surokMarksTab->layout()->addWidget( &m_surokMarkPlotter );
	surokPreprocessingTab->layout()->addWidget( &m_surokPreprocessing );
	m_surokMarkPlotter.setSurokExchange( &m_surokExchange );
	m_surokPreprocessing.setSurokExchange( &m_surokExchange );
	mainTabs->setCurrentWidget( surokMarksTab );
}


MainWindow::~MainWindow() = default;


void MainWindow::connection()
{
	connect( &m_surokExchange, &SurokExchange::newUpdateSpectr, &m_surokPreprocessing, &SurokPreprocessing::receiveData );

	connect( m_surokExchange.socket(), &QTcpSocket::connected, &m_surokMarkPlotter, &SurokMarksPlotter::onConnected );
	connect( m_surokExchange.socket(), &QTcpSocket::readyRead, &m_surokMarkPlotter, &SurokMarksPlotter::onDisconnected );
	connect( &m_surokExchange, &SurokExchange::newUpdateMark,  &m_surokMarkPlotter, &SurokMarksPlotter::receiveFromSocketMark );
	connect( &m_surokExchange, &SurokExchange::newUpdateMark,  this, [=] {
		mainTabs->setTabEnabled( mainTabs->indexOf( surokPreprocessingTab ), false );
	} );
	connect( &m_surokExchange, &SurokExchange::newUpdateSpectr, &m_surokMarkPlotter, &SurokMarksPlotter::receiveFromSocketSpectr );
	connect( &m_surokExchange, &SurokExchange::newUpdateSpectr, this, [=] {
		mainTabs->setTabEnabled( mainTabs->indexOf( surokPreprocessingTab ), true );
	} );

	connect( actionAzimuth,   &QAction::triggered, this, &MainWindow::graphDisplay );
	connect( actionDistance,  &QAction::triggered, this, &MainWindow::graphDisplay );
	connect( actionElevation, &QAction::triggered, this, &MainWindow::graphDisplay );
	connect( actionVelocity,  &QAction::triggered, this, &MainWindow::graphDisplay );
	connect( actionExit, &QAction::triggered, this, &QApplication::quit );
	enabledTimer.start( 5000 );
} // MainWindow::connection


void MainWindow::graphDisplay()
{
	m_surokMarkPlotter.azimuthWidget()->setVisible( actionAzimuth->isChecked() );
	m_surokMarkPlotter.distanceWidget()->setVisible( actionDistance->isChecked() );
	m_surokMarkPlotter.velocityWidget()->setVisible( actionVelocity->isChecked() );
	m_surokMarkPlotter.elevationWidget()->setVisible( actionElevation->isChecked() );
} // MainWindow::graphDisplay


void MainWindow::defaultSettings()
{
	actionAzimuth->setChecked( true );
	actionElevation->setChecked( true );
	actionDistance->setChecked( true );
	actionVelocity->setChecked( true );
	actionExit->setShortcut( static_cast< int >( Qt::CTRL ) +
	                         static_cast< int >( Qt::Key_Q ) );
} // MainWindow::mainSetting


void MainWindow::enabledTab()
{
//	surokPreprocessingTab->setEnabled( m_surokMarkPlotter.getEnableTab() );
//	if ( !m_surokMarkPlotter.getEnableTab() )
//	{
//		surokPreprocessingTab->hide();
//	}
//	if ( m_surokMarkPlotter.getEnableTab() )
//	{
//		surokPreprocessingTab->show();
//	}
}

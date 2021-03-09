#include "main_window.hpp"

#include <QFileDialog>
#include <QMouseEvent>
#include <QSplitter>
#include <QStandardItemModel>
#include <QTableView>

#include <qwt_picker_machine.h>
#include <qwt_plot_picker.h>
#include <qwt_scale_draw.h>
#include <qwt_scale_widget.h>

#include <tuple>


SurokMarksPlotter::SurokMarksPlotter( QWidget* parent ) :
	QWidget              ( parent ),
	Ui::SurokMarksPlotter()
{
	setupUi( this );
	spinboxSize();
	splitter();
	defaultSettings();
	createInfoModel();
	setupConnections();
}


void SurokMarksPlotter::setupConnections()
{
	connect( openPB, &QPushButton::clicked, this, &SurokMarksPlotter::setFileName );

	connect( clearWidgetsPB, &QPushButton::clicked, this, &SurokMarksPlotter::clearGraph );
	connect( azimuthMax, QOverload< int >::of( &QSpinBox::valueChanged ), [=]( int i )
	{ updateMax( m_plotAzimuth, azimuthMin, azimuthMax ); } );
	connect( azimuthMin, QOverload< int >::of( &QSpinBox::valueChanged ), [=]( int i )
	{ updateMin( m_plotAzimuth, azimuthMin, azimuthMax ); } );
	connect( distanceMax, QOverload< int >::of( &QSpinBox::valueChanged ), [=]( int i )
	{ updateMax( m_plotDistance, distanceMin, distanceMax ); } );
	connect( distanceMin, QOverload< int >::of( &QSpinBox::valueChanged ), [=]( int i )
	{ updateMin( m_plotDistance, distanceMin, distanceMax ); } );
	connect( velocityMax, QOverload< int >::of( &QSpinBox::valueChanged ), [=]( int i )
	{ updateMax( m_plotVelocity, velocityMin, velocityMax ); } );
	connect( velocityMin, QOverload< int >::of( &QSpinBox::valueChanged ), [=]( int i )
	{ updateMin( m_plotVelocity, velocityMin, velocityMax ); } );
	connect( elevationMax, QOverload< int >::of( &QSpinBox::valueChanged ), [=]( int i )
	{ updateMax( m_plotElevation, elevationMin, elevationMax ); } );
	connect( elevationMin, QOverload< int >::of( &QSpinBox::valueChanged ), [=]( int i )
	{ updateMin( m_plotElevation, elevationMin, elevationMax ); } );
	connect( maxSnrRadius, QOverload< double >::of( &QDoubleSpinBox::valueChanged ), [=]
	{ updateMaxSNRRadius(); } );
	connect( minSnrRadius, QOverload< double >::of( &QDoubleSpinBox::valueChanged ), [=]
	{ updateMinSNRRadius(); } );

	connect( velocityLegendMax, QOverload< int >::of( &QSpinBox::valueChanged ), [=]
	{ legendColorMax(); } );
	connect( velocityLegendMin, QOverload< int >::of( &QSpinBox::valueChanged ), [=]
	{ legendColorMin(); } );
	connect( visiblePeriods, QOverload< int >::of( &QSpinBox::valueChanged ), [=]( int i )
	{ visiblePeriodsUpdate( m_plotAzimuth, visiblePeriods ); } );
	connect( visiblePeriods, QOverload< int >::of( &QSpinBox::valueChanged ), [=]( int i )
	{ visiblePeriodsUpdate( m_plotDistance, visiblePeriods ); } );
	connect( visiblePeriods, QOverload< int >::of( &QSpinBox::valueChanged ), [=]( int i )
	{ visiblePeriodsUpdate( m_plotElevation, visiblePeriods ); } );
	connect( visiblePeriods, QOverload< int >::of( &QSpinBox::valueChanged ), [=]( int i )
	{ visiblePeriodsUpdate( m_plotVelocity, visiblePeriods ); } );
	connect( plotsBackground, QOverload< int >::of( &QSlider::valueChanged ), [=]
	{ sliderValueUpdate( m_plotAzimuth, m_pickerAzimuth ); } );
	connect( plotsBackground, QOverload< int >::of( &QSlider::valueChanged ), [=]
	{ sliderValueUpdate( m_plotDistance, m_pickerDistance ); } );
	connect( plotsBackground, QOverload< int >::of( &QSlider::valueChanged ), [=]
	{ sliderValueUpdate( m_plotElevation, m_pickerElevation ); } );
	connect( plotsBackground, QOverload< int >::of( &QSlider::valueChanged ), [=]
	{ sliderValueUpdate( m_plotVelocity, m_pickerVelocity ); } );
	connect( protocolCB, &QCheckBox::clicked,   this, &SurokMarksPlotter::protocolSettings );
	connect( connectPB,  &QPushButton::clicked, this, &SurokMarksPlotter::connectToHost );
	connect( &m_timer, &QTimer::timeout, this, &SurokMarksPlotter::connectToHost );

	connect( infoTableView->horizontalHeader(), QOverload< int >::of( &QHeaderView::sectionClicked ), [=]( int index ) {
		m_modelTable->sort( infoTableView->horizontalHeader()->sortIndicatorSection(), infoTableView->horizontalHeader()->sortIndicatorOrder() );
	} );
	connect( hostsList, QOverload< int >::of( &QComboBox::currentIndexChanged ), [=]( int index ){
		qDebug() << "Disconnected from host";
		m_surokExchange->socket()->disconnectFromHost();
	} );
} // SurokMarksPlotter::setupConnections


void SurokMarksPlotter::spinboxSize()
{
	label_18->adjustSize();
	label_15->adjustSize();
	label_17->adjustSize();
	label_16->adjustSize();

	label_18->setMinimumSize( label_16->size() );
	label_15->setMinimumSize( label_16->size() );
	label_17->setMinimumSize( label_16->size() );
} // SurokMarksPlotter::spinboxSize


void SurokMarksPlotter::sliderValueUpdate( MarksPlot* plot, QwtPlotPickerExt* picker )
{
	int sliderVal = plotsBackground->value();
	QBrush brush  = plot->canvasBackground();
	brush.setColor( QColor( sliderVal, sliderVal, sliderVal ) );
	plot->setCanvasBackground( brush );
	if ( sliderVal < 96 )
	{
		picker->setRubberBandPen( QColor( Qt::white ) );
		picker->setTrackerPen( QColor( Qt::white ) );
		plot->y0Line()->setPen( QPen( QColor( Qt::white ), 0.5, Qt::SolidLine ) );
	}
	else
	{
		picker->setRubberBandPen( QColor( Qt::black ) );
		picker->setTrackerPen( QColor( Qt::black ) );
		plot->y0Line()->setPen( QPen( QColor( Qt::black ), 0.5, Qt::SolidLine ) );
	}

	plot->replot();
} // SurokMarksPlotter::sliderValueUpdate


QWidget* SurokMarksPlotter::azimuthWidget() const
{
	return( widgetGrAz );
}


QWidget* SurokMarksPlotter::distanceWidget() const
{
	return( widgetGrDis );
}


QWidget* SurokMarksPlotter::velocityWidget() const
{
	return( widgetGrVel );
}


QWidget* SurokMarksPlotter::elevationWidget() const
{
	return( widgetGrElev );
}


bool SurokMarksPlotter::getEnableTab() const
{
	return( m_enableTab );
}


void SurokMarksPlotter::setEnableTab( bool value )
{
	m_enableTab = value;
}


void SurokMarksPlotter::visiblePeriodsUpdate( MarksPlot* plot, QSpinBox* sb )
{
	int valueSB = sb->value();

	plot->setAxisScale( MarksPlot::xBottom, 0, valueSB, valueSB / 10 );
	plot->replot();
} // SurokMarksPlotter::visiblePeriodsUpdate


void SurokMarksPlotter::updateMax( MarksPlot* plot, QSpinBox* min, QSpinBox* max )
{
	int maxV = max->value();
	int minV = min->value();
	if ( maxV <= minV )
	{
		max->setValue( minV + 1 /*+ 5*/ );
		max->setMinimum( minV );
		min->setMaximum( minV + 1 /*+ 5*/ );
	}
	else
	{
		max->setMinimum( minV );
	}
	if ( min->value() < 0 )
	{
		plot->setAxisScale( MarksPlot::yLeft, min->value(), max->value(), ( max->value() - min->value() ) / 4 );
	}
	else
	{
		plot->setAxisScale( MarksPlot::yLeft, min->value(), max->value(), ( max->value() - min->value() ) / 5 );
	}
	plot->replot();
} // SurokMarksPlotter::updateMax


void SurokMarksPlotter::updateMin( MarksPlot* plot, QSpinBox* min, QSpinBox* max )
{
	int maxV = max->value();
	int minV = min->value();
	if ( maxV <= minV )
	{
		min->setValue( maxV - 1 /*- 5*/ );
		min->setMaximum( maxV );
		max->setMinimum( maxV - 1 /*- 5*/ );
	}
	else
	{
		min->setMaximum( maxV );
	}

	if ( min->value() < 0 )
	{
		plot->setAxisScale( MarksPlot::yLeft, min->value(), max->value(), ( max->value() - min->value() ) / 4 );
	}
	else
	{
		plot->setAxisScale( MarksPlot::yLeft, min->value(), max->value(), ( max->value() - min->value() ) / 5 );
	}
	plot->replot();
} // SurokMarksPlotter::updateMin


template< typename T >
void SurokMarksPlotter::assigment( T& structMark )
{
	int rowNumber { 0 };
	int countMarks { 0 };
	QList< Marks > marksList;
	if ( structMark.isEmpty() ) { return; }
	if ( protocolCB->isChecked() )
	{
		distanceMin->setValue( qFloor( std::get< 0 >( structMark.last().zones().front() ).nearEdge() / 1e3 ) );
		distanceMax->setValue( qCeil( std::get< 0 >( structMark.last().zones().back() ).farEdge() / 1e3 ) );
	}

	snNearZoneDSB->setValue( std::get< 0 >( structMark.last().zones().front() ).snr() );
	snFarZoneDSB->setValue( std::get< 0 >( structMark.last().zones().back() ).snr() );

	m_modelTable->removeRows( 0, m_modelTable->rowCount() );
	for ( auto pair : structMark.last().zones() )
	{
		countMarks += std::get< 0 >( pair ).marksCount();
		for ( auto mark : std::get< 1 >( pair ) )
		{
			Marks tmpSpectr;
			tmpSpectr.az      = radianToDegree( mark.position().az() );
			tmpSpectr.r       = mark.position().r() / 1e3F;
			tmpSpectr.vel     = mark.velocity().v();
			tmpSpectr.el      = radianToDegree( mark.position().el() );
			tmpSpectr.noise   = mark.snr();
			tmpSpectr.zoneNum = mark.zoneNumber();

			QStandardItem* item_col_00 = new QStandardItem();
			QStandardItem* item_col_01 = new QStandardItem();
			QStandardItem* item_col_02 = new QStandardItem();
			QStandardItem* item_col_03 = new QStandardItem();
			QStandardItem* item_col_04 = new QStandardItem();
			QStandardItem* item_col_05 = new QStandardItem();

			rowNumber++;
			item_col_00->setText( QString::number( rowNumber ) );
			item_col_01->setText( QString::number( static_cast< double >( radianToDegree( mark.position().az() ) ), 'f', 2 ) );
			item_col_02->setText( QString::number( static_cast< double >( mark.position().r() / 1e3F ), 'f', 2 ) );
			item_col_03->setText( QString::number( static_cast< double >( mark.velocity().v() ), 'f', 2 ) );
			item_col_04->setText( QString::number( static_cast< double >( radianToDegree( mark.position().el() ) ), 'f', 2 ) );
			item_col_05->setText( QString::number( static_cast< double >( mark.snr() ), 'f', 2 ) );
			m_modelTable->appendRow( QList< QStandardItem* >() << item_col_00 << item_col_01 << item_col_02
			                                                   << item_col_03 << item_col_04 << item_col_05 );
			marksList.append( std::move( tmpSpectr ) );
		}
	}
	m_packetList.append( marksList );
	marksCount->setText( QString::number( countMarks ) );
	currentPeriod->setText( QString::number( structMark.last().header().cycleNumber() ) );

	nearFieldFrequency->setText( QString::number( static_cast< int >( std::get< 0 >( structMark.last().zones().front() ).carrier() ) / 1e3 ) + tr( " kHz" ) );
	farFieldFrequency->setText( QString::number( static_cast< int >( std::get< 0 >( structMark.last().zones().back() ).carrier() ) / 1e3 ) + tr( " kHz" ) );

	QColor lineColor = ( plotsBackground->value() > 96 ) ? QColor( Qt::black ) : QColor( Qt::white );
	if ( nearFieldCheckbox->checkState() == Qt::Checked )
	{
		double xNearStart[2] = { 0, 3000 };
		double yNearStart[2] = { std::get< 0 >( structMark.last().zones().front() ).nearEdge() / 1e3,
			                 std::get< 0 >( structMark.last().zones().front() ).nearEdge() / 1e3 };
		m_minDistanceLine.setSamples( xNearStart, yNearStart, 2 );
		m_minDistanceLine.setPen( QPen( lineColor, 0.5, Qt::DashDotLine ) );

		double xNearEnd[2] = { 0, 3000 };
		double yNearEnd[2] = { std::get< 0 >( structMark.last().zones().front() ).farEdge() / 1e3,
			               std::get< 0 >( structMark.last().zones().front() ).farEdge() / 1e3 };
		m_midDistanceLine.setSamples( xNearEnd, yNearEnd, 2 );
		m_midDistanceLine.setPen( QPen( lineColor, 0.5, Qt::DashDotLine ) );
	}

	if ( farFieldCheckbox->checkState() == Qt::Checked )
	{
		double xFarStart[2] = { 0, 3000 };
		double yFarStart[2] = { std::get< 0 >( structMark.last().zones().back() ).nearEdge() / 1e3,
			                std::get< 0 >( structMark.last().zones().back() ).nearEdge() / 1e3 };
		m_maxDistanceLine.setSamples( xFarStart, yFarStart, 2 );
		m_maxDistanceLine.setPen( QPen( lineColor, 0.5, Qt::DashDotDotLine ) );

		double xFarEnd[2] = { 0, 3000 };
		double yFarEnd[2] = { std::get< 0 >( structMark.last().zones().back() ).farEdge() / 1e3,
			              std::get< 0 >( structMark.last().zones().back() ).farEdge() / 1e3 };
		m_maxDistanceLine.setSamples( xFarEnd, yFarEnd, 2 );
		m_maxDistanceLine.setPen( QPen( lineColor, 0.5, Qt::DashDotDotLine ) );
	}
	drawPoints();
} // SurokMarksPlotter::drawPoints


void SurokMarksPlotter::drawPoints()
{
	int beginPlace = 0;
	if ( m_packetList.size() > visiblePeriods->value() )
	{
		beginPlace = m_packetList.size() - visiblePeriods->value();
	}

	for ( int i = beginPlace; i < m_packetList.size(); i++ )
	{
		int stepX = i - beginPlace;
		for ( auto& mark : m_packetList[i] )
		{
			if ( ( mark.vel > static_cast< float >( velocityLegendMax->value() ) ) ||
			     ( mark.vel < static_cast< float >( velocityLegendMin->value() ) ) )
			{
				mark.vel = ( mark.vel > 0 ) ? static_cast< float >( velocityLegendMax->value() ) : static_cast< float >( velocityLegendMin->value() );
			}
			if ( ( mark.noise > 10 * maxSnrRadius->value() ) || ( mark.noise < 10 * minSnrRadius->value() ) )
			{
				mark.noise = ( mark.noise > static_cast< float >( maxSnrRadius->value() ) ) ? static_cast< float >( maxSnrRadius->value() ) : static_cast< float >( minSnrRadius->value() );
			}
			double velocityColor = mark.vel / static_cast< float >( velocityLegendMax->value() - velocityLegendMin->value() ) + 0.5F;
			double sizeMarks     = mark.noise / ( 100.0 / ( maxSnrRadius->value() - minSnrRadius->value() ) ) + minSnrRadius->value();
			if ( ( nearFieldCheckbox->checkState() == Qt::Checked ) && ( mark.zoneNum == VTR::ZoneIndex::Near ) )
			{
				m_plotAzimuth->appendPoint( QwtPointExt( stepX, mark.az, velocityColor, sizeMarks ) );
				m_plotDistance->appendPoint( QwtPointExt( stepX, mark.r, velocityColor, sizeMarks ) );
				m_plotVelocity->appendPoint( QwtPointExt( stepX, mark.vel, velocityColor, sizeMarks ) );
				m_plotElevation->appendPoint( QwtPointExt( stepX, mark.el, velocityColor, sizeMarks ) );
			}
			if ( ( farFieldCheckbox->checkState() == Qt::Checked ) && ( mark.zoneNum == VTR::ZoneIndex::Far ) )
			{
				m_plotAzimuth->appendPoint( QwtPointExt( stepX, mark.az, velocityColor, sizeMarks ) );
				m_plotDistance->appendPoint( QwtPointExt( stepX, mark.r, velocityColor, sizeMarks ) );
				m_plotVelocity->appendPoint( QwtPointExt( stepX, mark.vel, velocityColor, sizeMarks ) );
				m_plotElevation->appendPoint( QwtPointExt( stepX, mark.el, velocityColor, sizeMarks ) );
			}
		}
	}
} // SurokMarksPlotter::drawPoints


void SurokMarksPlotter::readFromFile()
{
	m_plotAzimuth->clearPoints();
	m_plotDistance->clearPoints();
	m_plotVelocity->clearPoints();
	m_plotElevation->clearPoints();
	VPP::MarksData marksData;
	VPP::SpectrumsData spectrumsData;

//	m_stream >> _head;
//	m_stream.device() -> reset();
	if ( m_stream.status() == QDataStream::Ok )
	{
//		if ( _head.type() == VTC::CodogramType::MarksPack )
//		{
		m_stream >> marksData;
//		assigment( _mark );
		m_connectionChecker.start( 3000 );
//		}
//		if ( _head.type() == VTC::CodogramType::SpectrumsPack )
//		{
//			m_stream >> _spectrMark;
//			spectrAssigment( _spectrMark );
//			m_connectionChecker.start( 3000 );
//		}
	}
} // SurokMarksPlotter::readFromFile


void SurokMarksPlotter::onConnected()
{
	if ( m_surokExchange->socket()->state() == QAbstractSocket::ConnectedState )
	{
		connectPB->setStyleSheet( "QPushButton {color: green;}" );
	}
} // MarksDataPrint::onConnected


void SurokMarksPlotter::onDisconnected()
{
	if ( m_surokExchange->socket()->state() != QAbstractSocket::ConnectedState )
	{
		connectPB->setStyleSheet( "QPushButton {color: red;}" );
	}
//	Q_EMIT discon();
}


void SurokMarksPlotter::onError( QAbstractSocket::SocketError err )
{
	qWarning() << "Error: " + ( err == QAbstractSocket::HostNotFoundError ?
	                            "The host was not found." :
	                            err == QAbstractSocket::RemoteHostClosedError ?
	                            "The remote host is closed." :
	                            err == QAbstractSocket::ConnectionRefusedError ?
	                            "The connection was refused." :
	                            QString( m_surokExchange->socket()->errorString() )
	                            );
}


void SurokMarksPlotter::receiveFromSocketSpectr()
{
	m_plotAzimuth->clearPoints();
	m_plotDistance->clearPoints();
	m_plotVelocity->clearPoints();
	m_plotElevation->clearPoints();

	auto spectList = m_surokExchange->spectrumsDataList();
	assigment( spectList );
}


void SurokMarksPlotter::receiveFromSocketMark()
{
	m_plotAzimuth->clearPoints();
	m_plotDistance->clearPoints();
	m_plotVelocity->clearPoints();
	m_plotElevation->clearPoints();

	auto marksList = m_surokExchange->marksDataList();
	assigment( marksList );
} // MarksDataPrint::onReadyRead


void SurokMarksPlotter::legendColorMax()
{
	velocityLegendMin->setValue( -velocityLegendMax->value() );
}


void SurokMarksPlotter::legendColorMin()
{
	velocityLegendMax->setValue( -velocityLegendMin->value() );
}


void SurokMarksPlotter::setViewFileNameRB()
{
	fileNameLE->setEnabled( true );
	openPB->setEnabled( true );
	hostsList->setEnabled( false );
	portSB->setEnabled( false );
} // SurokMarksPlotter::setViewFileNameRB


void SurokMarksPlotter::setViewHostRB()
{
	fileNameLE->setEnabled( false );
	openPB->setEnabled( false );
	hostsList->setEnabled( true );
	portSB->setEnabled( true );
} // SurokMarksPlotter::setViewHostRB


void SurokMarksPlotter::createInfoModel()
{
	QStringList horizontal;

	horizontal.append( tr( "№" ) );
	horizontal.append( tr( "Azimuth" ) );
	horizontal.append( tr( "Distance" ) );
	horizontal.append( tr( "Velocity" ) );
	horizontal.append( tr( "Elevation" ) );
	horizontal.append( tr( "SNR (dB)" ) );

	m_modelTable = new QStandardItemModel;
	m_modelTable->setHorizontalHeaderLabels( horizontal );

	infoTableView->verticalHeader()->setVisible( false );
	infoTableView->setModel( m_modelTable );

	infoTableView->horizontalHeader()->setSortIndicatorShown( true );
	infoTableView->setColumnWidth( 0, infoTableView->fontMetrics().width( horizontal[0] + "WW" ) );
	infoTableView->setColumnWidth( 1, infoTableView->fontMetrics().width( horizontal[1] + "WW" ) );
	infoTableView->setColumnWidth( 2, infoTableView->fontMetrics().width( horizontal[2] + "WW" ) );
	infoTableView->setColumnWidth( 3, infoTableView->fontMetrics().width( horizontal[3] + "WW" ) );
	infoTableView->setColumnWidth( 4, infoTableView->fontMetrics().width( horizontal[4] + "WW" ) );
	infoTableView->setColumnWidth( 5, infoTableView->fontMetrics().width( horizontal[5] + "WW" ) );
	infoTableView->horizontalHeader()->setStretchLastSection( true );
} // SurokMarksPlotter::createInfoModel


void SurokMarksPlotter::defaultSettings()
{
	formingTime->setDateTime( QDateTime::currentDateTime() );
	receivingTime->setDateTime( QDateTime::currentDateTime() );

	int widgetsWidth = visiblePeriods->fontMetrics().width( "9999999 KHZ" );
	minSnrRadius->setMaximumWidth( widgetsWidth );
	maxSnrRadius->setMaximumWidth( widgetsWidth );
	visiblePeriods->setMaximumWidth( widgetsWidth );
	farFieldFrequency->setMaximumWidth( widgetsWidth );
	nearFieldFrequency->setMaximumWidth( widgetsWidth );
	marksCount->setMaximumWidth( widgetsWidth );

	currentPeriod->setMaximumWidth( widgetsWidth );
	plotsBackground->setMaximumWidth( widgetsWidth );
	velocityLegend->setMaximumWidth( widgetsWidth );
	velocityLegendMax->setMaximumWidth( widgetsWidth );
	velocityLegendMin->setMaximumWidth( widgetsWidth );

	velocityMax->setValue( velocityLegendMax->value() );
	velocityMin->setValue( velocityLegendMin->value() );

	auto plotAzimuth = createPlot( widgetAzimuth, azimuthCoordsx, azimuthCoordsy );
	m_plotAzimuth   = std::get< 0 >( plotAzimuth );
	m_pickerAzimuth = std::get< 1 >( plotAzimuth );
	updateMax( m_plotAzimuth, azimuthMin, azimuthMax );
	updateMin( m_plotAzimuth, azimuthMin, azimuthMax );

	auto plotDistance = createPlot( widgetDistance, distanceCoordsx, distanceCoordsy );
	m_plotDistance   = std::get< 0 >( plotDistance );
	m_pickerDistance = std::get< 1 >( plotDistance );
	updateMax( m_plotDistance, distanceMin, distanceMax );
	updateMin( m_plotDistance, distanceMin, distanceMax );

	auto plotElevation = createPlot( widgetElevation, elevationCoordsx, elevationCoordsy );
	m_plotElevation   = std::get< 0 >( plotElevation );
	m_pickerElevation = std::get< 1 >( plotElevation );
	updateMax( m_plotElevation, elevationMin, elevationMax );
	updateMin( m_plotElevation, elevationMin, elevationMax );

	auto plotVelocity = createPlot( widgetVelocity, velocityCoordsx, velocityCoordsy );
	m_plotVelocity   = std::get< 0 >( plotVelocity );
	m_pickerVelocity = std::get< 1 >( plotVelocity );
	updateMax( m_plotVelocity, velocityMin, velocityMax );
	updateMin( m_plotVelocity, velocityMin, velocityMax );

	m_minDistanceLine.attach( m_plotDistance );
	m_midDistanceLine.attach( m_plotDistance );
	m_maxDistanceLine.attach( m_plotDistance );
} // SurokMarksPlotter::defaultSettings


void SurokMarksPlotter::protocolSettings()
{
	if ( protocolCB->isChecked() )
	{
		azimuthMax->setEnabled( false );
		azimuthMin->setEnabled( false );
		distanceMax->setEnabled( false );
		distanceMin->setEnabled( false );
		velocityMax->setEnabled( false );
		velocityMin->setEnabled( false );
		elevationMax->setEnabled( false );
		elevationMin->setEnabled( false );
	}
	else
	{
		azimuthMax->setEnabled( true );
		azimuthMin->setEnabled( true );
		distanceMax->setEnabled( true );
		distanceMin->setEnabled( true );
		velocityMax->setEnabled( true );
		velocityMin->setEnabled( true );
		elevationMax->setEnabled( true );
		elevationMin->setEnabled( true );
	}
} // SurokMarksPlotter::protocolSettings


void SurokMarksPlotter::updateMaxSNRRadius()
{
	double maxV = maxSnrRadius->value();
	double minV = minSnrRadius->value();
	if ( maxV <= minV )
	{
		maxSnrRadius->setValue( minV );
		maxSnrRadius->setMinimum( minV );
		minSnrRadius->setMaximum( minV );
	}
	else
	{
		maxSnrRadius->setMinimum( minV );
	}
} // SurokMarksPlotter::updateMaxSNRRadius


void SurokMarksPlotter::updateMinSNRRadius()
{
	double maxV = maxSnrRadius->value();
	double minV = minSnrRadius->value();
	if ( maxV <= minV )
	{
		minSnrRadius->setValue( maxV );
		minSnrRadius->setMaximum( maxV );
		maxSnrRadius->setMinimum( maxV );
	}
	else
	{
		minSnrRadius->setMaximum( maxV );
	}
} // SurokMarksPlotter::updateMinSNRRadius


void SurokMarksPlotter::setFileName()
{
	QString dirOpenFile = QFileDialog::getOpenFileName( this, tr( "Open file" ), "/home/user/work",
	                                                    nullptr, nullptr,
	                                                    QFileDialog::DontUseNativeDialog );
	fileNameLE->setText( dirOpenFile );
	m_file.setFileName( fileNameLE->text() );
	if ( !m_file.open( QIODevice::ReadOnly ) )
	{
		qCritical() << tr( "Can't open adsb-file " ) << m_file.fileName();
	}
	if ( m_file.isOpen() )
	{
		m_stream.setDevice( &m_file );
		if ( m_stream.device()->atEnd() ) { return; }
		connect( &m_connectionChecker, &QTimer::timeout, this, &SurokMarksPlotter::readFromFile );
		readFromFile();
	}
} // SurokMarksPlotter::setFileName


std::tuple< MarksPlot*, QwtPlotPickerExt* > SurokMarksPlotter::createPlot( QWidget* parent, QLabel* coordsLabelX, QLabel* coordsLabelY )
{
	auto plot = new MarksPlot( coordsLabelX, coordsLabelY, parent );
	plot->setMinimumHeight( 1 );
	plot->setMinimumWidth( 1 );

	parent->layout()->addWidget( plot );
	auto picker = createPicker( plot );
	connect( picker, &QwtPlotPickerExt::mouseMoved, [=]( const QPoint& point ) {
		auto p = picker->point( point );
		coordsLabelX->setText( QString::number( p.x(), 'f', 1 ) );
		coordsLabelY->setText( QString::number( p.y(), 'f', 1 ) );
	} );
	plot->setAxisScale( MarksPlot::xBottom, 0, visiblePeriods->value(), 100 );
	sliderValueUpdate( plot, picker );
	return( std::make_tuple( plot, picker ) );
} // SurokMarksPlotter::createPlot


void SurokMarksPlotter::clearGraph()
{
	m_plotAzimuth->clearPoints();
	m_plotElevation->clearPoints();
	m_plotDistance->clearPoints();
	m_plotVelocity->clearPoints();
	m_packetList.clear();
} // SurokMarksPlotter::clearGraph


QwtPlotPickerExt* SurokMarksPlotter::createPicker( MarksPlot* plot )
{
	auto picker = new QwtPlotPickerExt( QwtPlot::xBottom, QwtPlot::yLeft,
	                                    QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn,
	                                    plot->canvas() );
	picker->setStateMachine( new QwtPickerDragPointMachine() );
	picker->setRubberBand( QwtPicker::CrossRubberBand );
	return( picker );
} // SurokMarksPlotter::createPicker


void SurokMarksPlotter::splitter()
{
	QList< int > splitter = splitter_4->sizes();

	splitter.replace( 1, static_cast< int >( this->height() / 0.99 ) );
	splitter.replace( 0, static_cast< int >( this->height() / 0.01 ) );
	splitter_4->setSizes( splitter );

	QList< int > splitter12 = splitter_1->sizes();
	splitter12.replace( 1, static_cast< int >( this->height() / 0.5 ) );
	splitter12.replace( 0, static_cast< int >( this->height() / 0.5 ) );
	splitter_1->setSizes( splitter12 );

	QList< int > splitter34 = splitter_2->sizes();
	splitter34.replace( 1, static_cast< int >( this->height() / 0.5 ) );
	splitter34.replace( 0, static_cast< int >( this->height() / 0.5 ) );
	splitter_2->setSizes( splitter34 );
} // SurokMarksPlotter::splitter


void SurokMarksPlotter::connectToHost()
{
	if ( connectPB->isChecked() )
	{
		if ( m_surokExchange->socket()->state() != QAbstractSocket::ConnectedState )
		{
			connectPB->setStyleSheet( "QPushButton {color: red;}" );
			m_surokExchange->socket()->flush();
			m_surokExchange->socket()->connectToHost( hostsList->currentText(), static_cast< quint16 >( portSB->text().toUInt() ) );
		}
	}
	else
	{
		connectPB->setStyleSheet( "" );
		m_surokExchange->socket()->disconnectFromHost();
	}
	m_timer.start( 3000 );
}

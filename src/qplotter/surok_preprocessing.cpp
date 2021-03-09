#include "surok_preprocessing.hpp"
#include <qwt3d_function.h>
#include <QListWidgetItem>
#include <QStandardItemModel>
#include "check_list.hpp"


class QPaintWidget : public QWidget
{
	QPaintWidget( QWidget* parent ) :
		QWidget( parent )
	{
	}


	void paintEvent( QPaintEvent* event )
	{
		QPainter painter( this );
		std::vector< QPoint > points;
		painter.setPen( QPen( Qt::red, 100 ) ); // Настройки рисования
		painter.drawPoint( QPoint( 130, 4000 ) );
	}


	friend class SurokPreprocessing;
}; // class QPaintWidget

class Spectrum3D : public Qwt3D::Function
{
	SurokPreprocessing* m_gui { nullptr };

public:
	Spectrum3D( Qwt3D::SurfacePlot* pw ) :
		Function( pw )
	{
	}


	void setGui( SurokPreprocessing* value )
	{
		m_gui = value;
	}


	double operator()( double x, double y ) final
	{
		auto f = m_gui->surokExchange()->spectrumsDataList().back().zones().front();

		auto distChannel = std::get< 0 >( f ).distanceChannels();
		if ( std::get< 0 >( f ).spectrumsCount() > 0 )
		{
			auto data = m_gui->m_dataDrawing;
			if ( data != nullptr )
			{
				auto value = data[ static_cast< int >( std::round( x ) ) * distChannel + static_cast< int >( std::round( y ) ) ];
				auto v     = std::hypot( value.real(), value.imag() );
				if ( m_gui->linORlogViewCB->checkState() == Qt::Checked )
				{
					v = 10 * log10( v );
				}
				return( v );
			}
		}
		return( 0 );
	}
}; // class Spectrum3D

SurokPreprocessing::SurokPreprocessing( QWidget* parent ) :
	QWidget               ( parent ),
	Ui::SurokPreprocessing()
{
	setupUi( this );
	createSpectrum3D();
	draw3DGraph();
	createInfoModelMarks();
	cuttingX();
	cuttingY();
	splitter();
	QStringList channels;
	for ( int i = 1; i <= 12; ++i ) channels.append( QString::number( i ) );
	m_receivePrimaryChannels = new CheckList();
	m_receivePrimaryChannels->addItems( channels );
	layoutPrimary->addWidget( m_receivePrimaryChannels );

	for ( int i = 13; i <= 20; ++i ) channels.append( QString::number( i ) );
	m_receiveSecondaryChannels = new CheckList();
	m_receiveSecondaryChannels->addItems( channels );
	layoutSecondary->addWidget( m_receiveSecondaryChannels );

	receivePrimary->setChecked( true );
	receiveSecondary->setChecked( false );

	setupConnections();
}


void SurokPreprocessing::setupConnections()
{
	connect( linORlogViewCB, &QCheckBox::stateChanged, this, &SurokPreprocessing::draw3DGraph );

	connect( receivePrimary, &QCheckBox::toggled, this, &SurokPreprocessing::slReceiveCheckboxToggled );
	connect( receiveSecondary, &QCheckBox::toggled, this, &SurokPreprocessing::slReceiveCheckboxToggled );
	connect( m_receivePrimaryChannels, &CheckList::updated, this, &SurokPreprocessing::slReceiveCheckboxToggled );
	connect( m_receiveSecondaryChannels, &CheckList::updated, this, &SurokPreprocessing::slReceiveCheckboxToggled );

	connect( m_surokExchange, &SurokExchange::socketConnected, this, &SurokPreprocessing::slSendSpectrumRequest );

	connect( infoMarksTable, &QAbstractItemView::clicked, this, &SurokPreprocessing::onTableClicked );
	connect( infoMarksTable->horizontalHeader(), QOverload< int >::of( &QHeaderView::sectionClicked ), [=]( int index ) {
		m_modelTableInfoMarks->sort( infoMarksTable->horizontalHeader()->sortIndicatorSection(), infoMarksTable->horizontalHeader()->sortIndicatorOrder() );
	} );
//	connect( infoMarksTable->selectionModel(), &QItemSelectionModel::selectionChanged, [=]( const QModelIndex& current, const QModelIndex& previous )
//	{
//		const auto& zones = m_surokExchange->spectrumsDataList().back().zones();
//		for ( const auto& zone : zones )
//		{
//			QwtPoint3D q;
//			q.setX( std::get< 1 >( zone ).at( current.row() ).distanceIndex() );
//			q.setY( std::get< 1 >( zone ).at( current.row() ).dopplerIndex() );
//			q.setZ( 0 );
//			//		plot3D->layout()->

//			QwtPlot a;
//			//		a.
//			//		auto plot = new MarksPlot( coordsLabelX, coordsLabelY, parent );
//			//		plot->setMinimumHeight( 1 );
//			//		plot->setMinimumWidth( 1 );
//			//		parent->layout()->addWidget( plot );

////			curv1 = new QwtPlotCurve(QString("U1(t)"));
////			curv1->setRenderHint(QwtPlotItem::RenderAntialiased);
////			curv1->setPen(QPen(Qt::red));

////			QwtSymbol *symbol1 = new QwtSymbol();
////			symbol1->setStyle(QwtSymbol::Ellipse);
////			symbol1->setPen(QColor(Qt::black));
////			symbol1->setSize(4);
////			curv1->setSymbol(symbol1);
////			curv1->attach( m_surface );
////			plot3D->layout()->addWidget( curv1 );
//		}
//	} );
} // SurokPreprocessing::setupConnections


void SurokPreprocessing::onTableClicked( const QModelIndex& index )
{
	if ( index.isValid() )
	{
		QString cellText = index.data().toString();
	}
	qDebug() << "Table index" << index.row();
	const auto& zones = m_surokExchange->spectrumsDataList().back().zones();
	for ( const auto& zone : zones )
	{
		QwtPoint3D q;
		q.setX( std::get< 1 >( zone ).at( index.row() ).distanceIndex() );
		q.setY( std::get< 1 >( zone ).at( index.row() ).dopplerIndex() );
		q.setZ( 0 );
		m_indexTable = std::make_pair( q.x(), q.y() );
		qDebug() << "coord: " << q.x() << q.y();
	}
}


void SurokPreprocessing::drawPoints()
{
	const auto& zones = m_surokExchange->spectrumsDataList().back().zones();
	for ( const auto& zone : zones )
	{
		for ( const auto& mark : std::get< 1 >( zone ) )
		{
			QwtPoint3D q;
			q.setX( mark.distanceIndex() );
			q.setY( mark.dopplerIndex() );
			q.setZ( 0 );
//			plot3D->layout()

//			QPaintWidget wgt;
//			plot3D->layout()->addWidget( wgt );
//			a.
//			auto plot = new MarksPlot( coordsLabelX, coordsLabelY, parent );
//			plot->setMinimumHeight( 1 );
//			plot->setMinimumWidth( 1 );

//			parent->layout()->addWidget( plot );
		}
	}
} // SurokPreprocessing::drawPoints


void SurokPreprocessing::slSendSpectrumRequest()
{
	qDebug() << "slSendSpectrumRequest";
	VPP::SpectrumsRequest request;
	if ( receivePrimary->isChecked() )
	{
		const auto listPrimary = m_receivePrimaryChannels->getCheckedItems();
		for ( const auto& l: listPrimary )
		{
			request.enablePrimaryChannel( l.toInt() );
		}
	}

	if ( receiveSecondary->isChecked() )
	{
		const auto listSecondary = m_receiveSecondaryChannels->getCheckedItems();
		for ( const auto& l: listSecondary )
		{
			request.enableSecondaryChannel( l.toInt() );
		}
	}
	QByteArray  ba;
	QDataStream ds( &ba, QIODevice::WriteOnly );
	ds << request;
	if ( m_surokExchange->socket()->write( ba ) == -1 )
	{
		qWarning() << "Can't send TCP packet";
	}
} // SurokPreprocessing::slSendSpectrumRequest


void SurokPreprocessing::receiveData()
{
	m_vecBottom.clear();
	m_vecTop.clear();
	m_listPointTop.clear();
	m_listPointBottom.clear();
	draw3DGraph();
	if ( ( m_surokExchange != nullptr ) && ( !m_surokExchange->spectrumsDataList().isEmpty() ) )
	{
		const auto& zones = m_surokExchange->spectrumsDataList().back().zones();
//		auto distanceChannels = std::get< 0 >( m_surokExchange->spectrumsDataList().back().zones().front() ).distanceChannels();
		auto distanceChannels = spinBox_4->value();
		auto periodsCount     = spinBox_3->value();
//		auto periodsCount     = m_surokExchange->spectrumsDataList().back().header().periodsCount();
		auto data = m_dataDrawing;
		if ( data != nullptr )
		{
			for ( int id = spinBox->value(); id < distanceChannels; ++id )
			{
				std::vector< double > tmp;
				for ( int ix = spinBox_2->value(); ix < periodsCount; ++ix )
				{
					auto vX    = data[ static_cast< int >( std::round( ix ) ) * distanceChannels ];
					auto xPlot = std::hypot( vX.real(), vX.imag() );
					tmp.push_back( xPlot );
				}
				m_vecBottom.insert( id, std::move( tmp ) );
			}
			for ( int id = spinBox_2->value(); id < periodsCount; ++id )
			{
				std::vector< double > tmp;
				for ( int iy = spinBox->value(); iy < distanceChannels; ++iy )
				{
					auto vY    = data[ static_cast< int >( std::round( iy ) ) ];
					auto yPlot = std::hypot( vY.real(), vY.imag() );
					tmp.push_back( yPlot );
				}
				m_vecTop.insert( id, std::move( tmp ) );
			}
			m_axis = 0;
			auto indexDist      = m_indexTable.first;
			auto trueVectorDist = m_vecBottom.take( indexDist );
			for ( const auto& step : trueVectorDist )
			{
				QPointF qX;
				qX.setY( step );
				qX.setX( m_axis );
				m_listPointBottom.append( qX );
				m_axis++;
			}
			m_axis = 0;
			auto indexDoppler      = m_indexTable.second;
			auto trueVectorDoppler = m_vecTop.take( indexDoppler );
			for ( const auto& step : trueVectorDoppler )
			{
				QPointF qY;
				qY.setX( step );
				qY.setY( m_axis );
				m_listPointTop.append( qY );
				m_axis++;
			}

			qwtPlot_top->setAxisScale( 0, 0, distanceChannels - 1 );
			qwtPlot_bottom->setAxisScale( 2, 0, periodsCount - 1 );

			m_curvTop->setSamples( m_listPointTop );
			qwtPlot_top->replot();

			m_curvBottom->setSamples( m_listPointBottom );
			qwtPlot_bottom->replot();
		}
	}
} // SurokPreprocessing::receiveData


void SurokPreprocessing::sendDataToDrawing()
{
	int indexView { 0 };
	int rowNumber { 0 };
	m_modelTableInfoMarks->removeRows( 0, m_modelTableInfoMarks->rowCount() );
	indexView = showChannel->itemData( showChannel->currentIndex() ).toInt();
	const auto& zones = m_surokExchange->spectrumsDataList().back().zones();

	for ( const auto& zone : zones )
	{
		for ( const auto& spectrum : std::get< 2 >( zone ) )
		{
			if ( ( indexView < 0 ) && !spectrum.isPrimary() )
			{
				indexView = std::abs( indexView );
				if ( indexView == spectrum.number() )
				{
					m_dataDrawing = spectrum.data();
				}
			}
			if ( ( indexView > 0 ) && spectrum.isPrimary() )
			{
				if ( indexView == spectrum.number() )
				{
					m_dataDrawing = spectrum.data();
				}
			}
		}
		for ( const auto& mark : std::get< 1 >( zone ) )
		{
			QStandardItem* item_col_00 = new QStandardItem();
			QStandardItem* item_col_01 = new QStandardItem();
			QStandardItem* item_col_02 = new QStandardItem();

			rowNumber++;
			item_col_00->setText( QString::number( rowNumber ) );
			item_col_01->setText( QString::number( static_cast< double >( mark.distanceIndex() ), 'f', 2 ) );
			item_col_02->setText( QString::number( static_cast< double >( mark.dopplerIndex() ), 'f', 2 ) );

			m_modelTableInfoMarks->appendRow( QList< QStandardItem* >() << item_col_00 << item_col_01 << item_col_02 );
		}
	}
} // SurokPreprocessing::sendDataToDrawing


void SurokPreprocessing::createSpectrum3D()
{
	m_surface.setTitle( tr( "Surok 3D spectrum" ) );
	m_surface.setRotation( 30, 0, 15 );
	m_surface.setScale( 1, 40, 400 );
	m_surface.setShift( 0, 0, 0 );
	m_surface.setZoom( 1 );

	for ( unsigned i = 0; i != m_surface.coordinates()->axes.size(); ++i )
	{
		m_surface.coordinates()->axes[i].setMajors( 7 );
		m_surface.coordinates()->axes[i].setMinors( 5 );
	}

	m_surface.coordinates()->axes[Qwt3D::X1].setLabelString( tr( "distance" ) );
	m_surface.coordinates()->axes[Qwt3D::Y1].setLabelString( tr( "doppler" ) );
	m_surface.coordinates()->axes[Qwt3D::Z1].setLabelString( tr( "amplitude" ) );

	m_surface.setCoordinateStyle( Qwt3D::BOX );
	m_surface.setBackgroundColor( Qwt3D::RGBA( 60, 60, 60, 0 ) );

	m_spectrum3d = new Spectrum3D( &m_surface );
	m_spectrum3d->setMinZ( -10 );
	m_spectrum3d->setMaxZ( 50 );
	m_spectrum3d->setMesh( 41, 31 );
	m_spectrum3d->setGui( this );
	plot3D->layout()->addWidget( &m_surface );
} // SurokPreprocessing::createSpectrum3D


void SurokPreprocessing::draw3DGraph()
{
	if ( ( m_surokExchange != nullptr ) && ( !m_surokExchange->spectrumsDataList().isEmpty() ) )
	{
		sendDataToDrawing();
		auto distanceChannels = std::get< 0 >( m_surokExchange->spectrumsDataList().back().zones().front() ).distanceChannels();
		auto periodsCount     = m_surokExchange->spectrumsDataList().back().header().periodsCount();
		if ( std::get< 0 >( m_surokExchange->spectrumsDataList().back().zones().front() ).spectrumsCount() > 0 )
		{
			drawPoints(); // для отображения точек на 3d графике

			m_spectrum3d->setDomain( spinBox->value(), spinBox_3->value() - 1,
			                         spinBox_2->value(), spinBox_4->value() - 1 );
			m_spectrum3d->create();
			m_surface.updateData();
			m_surface.updateGL();
		}
	}
} // SurokPreprocessing::draw3DGraph


void SurokPreprocessing::slReceiveCheckboxToggled()
{
	int currentChannel = 0;
	if ( showChannel->currentIndex() >= 0 ) { currentChannel = showChannel->itemData( showChannel->currentIndex() ).toInt(); }
	showChannel->clear();

	if ( receivePrimary->isChecked() )
	{
		m_receivePrimaryChannels->getCheckedItems().clear();
		layoutPrimary->setEnabled( true );
		layoutSecondary->setEnabled( false );

		const auto listPrimary = m_receivePrimaryChannels->getCheckedItems();
		for ( const auto& l: listPrimary )
		{
			showChannel->addItem( tr( "Primary: " ) + l, l.toInt() );
		}
	}

	if ( receiveSecondary->isChecked() )
	{
		m_receiveSecondaryChannels->getCheckedItems().clear();
		layoutPrimary->setEnabled( false );
		layoutSecondary->setEnabled( true );

		const auto listSecondary = m_receiveSecondaryChannels->getCheckedItems();
		for ( const auto& l: listSecondary )
		{
			showChannel->addItem( tr( "Secondary: " ) + l, -l.toInt() );
		}
	}

	if ( receiveSecondary->isChecked() && receivePrimary->isChecked() )
	{
		layoutSecondary->setEnabled( true );
		layoutPrimary->setEnabled( true );
	}

	if ( !receiveSecondary->isChecked() && !receivePrimary->isChecked() )
	{
		m_receivePrimaryChannels->getCheckedItems().clear();
		m_receiveSecondaryChannels->getCheckedItems().clear();
		layoutSecondary->setEnabled( false );
		layoutPrimary->setEnabled( false );
	}

	auto currentIndex = showChannel->findData( currentChannel );
	if ( currentIndex >= 0 ) { showChannel->setCurrentIndex( currentIndex ); }
	slSendSpectrumRequest();
} // SurokPreprocessing::slReceiveCheckboxToggled


void SurokPreprocessing::createInfoModelMarks()
{
	QStringList horizontal;

	horizontal.append( tr( "№" ) );
	horizontal.append( tr( "Range channel" ) );
	horizontal.append( tr( "Speed channel" ) );
	m_modelTableInfoMarks = new QStandardItemModel;
	m_modelTableInfoMarks->setHorizontalHeaderLabels( horizontal );
	infoMarksTable->verticalHeader()->setVisible( false );

	infoMarksTable->setModel( m_modelTableInfoMarks );
	infoMarksTable->resizeRowsToContents();
	infoMarksTable->resizeColumnsToContents();

	infoMarksTable->horizontalHeader()->setSortIndicatorShown( true );
	infoMarksTable->setColumnWidth( 0, infoMarksTable->fontMetrics().width( horizontal[0] + "WW" ) );
	infoMarksTable->setColumnWidth( 1, infoMarksTable->fontMetrics().width( horizontal[1] + "WWWWWWWWWWWWWW" ) );
	infoMarksTable->setColumnWidth( 2, infoMarksTable->fontMetrics().width( horizontal[2] + "WW" ) );

	infoMarksTable->horizontalHeader()->setStretchLastSection( true );
} // SurokPreprocessing::createInfoModelMarks


void SurokPreprocessing::cuttingY()
{
	qwtPlot_top->setTitle( "Doppler cut " );
	qwtPlot_top->setCanvasBackground( Qt::white );
	qwtPlot_top->setAxisTitle( QwtPlot::yLeft, tr( "m/c" ) );
	qwtPlot_top->setAxisTitle( QwtPlot::xBottom, tr( "amplitude" ) );

	m_grid = new QwtPlotGrid();
	m_grid->enableXMin( true );
	m_grid->setMajorPen( QPen( Qt::gray, 1 ) );
	m_grid->attach( qwtPlot_top );

	m_curvTop = new QwtPlotCurve();
	m_curvTop->setTitle( "distanceIndex" );
	m_curvTop->setPen( Qt::blue, 1 );
	m_curvTop->setRenderHint( QwtPlotItem::RenderAntialiased, true );

	QwtSymbol* symbol = new QwtSymbol( QwtSymbol::Ellipse,
	                                   QBrush( Qt::yellow ), QPen( Qt::red, 2 ), QSize( 1, 1 ) );
	m_curvTop->setSymbol( symbol );
	m_curvTop->attach( qwtPlot_top );
} // SurokPreprocessing::cuttingX


void SurokPreprocessing::cuttingX()
{
	qwtPlot_bottom->setTitle( "Distance cut " );
	qwtPlot_bottom->setCanvasBackground( Qt::white );
	qwtPlot_bottom->setAxisTitle( QwtPlot::yLeft, tr( "amplitude" ) );
	qwtPlot_bottom->setAxisTitle( QwtPlot::xBottom, tr( "km" ) );

	m_grid = new QwtPlotGrid();
	m_grid->enableXMin( true );
	m_grid->setMajorPen( QPen( Qt::gray, 1 ) );
	m_grid->attach( qwtPlot_bottom );

	m_curvBottom = new QwtPlotCurve();
	m_curvBottom->setTitle( "dopplerIndex" );
	m_curvBottom->setPen( Qt::blue, 1 );
	m_curvBottom->setRenderHint( QwtPlotItem::RenderAntialiased, true );

	QwtSymbol* symbol = new QwtSymbol( QwtSymbol::Ellipse,
	                                   QBrush( Qt::yellow ), QPen( Qt::red, 2 ), QSize( 1, 1 ) );
	m_curvBottom->setSymbol( symbol );
	m_curvBottom->attach( qwtPlot_bottom );
} // SurokPreprocessing::cuttingY


void SurokPreprocessing::splitter()
{
	QList< int > lst = splitter_2->sizes();

	lst.replace( 0, this->height() / 0.2 );
	lst.replace( 1, this->height() / 0.8 );
	splitter_2->setSizes( lst );

	QList< int > lst2 = splitter_3->sizes();

	lst2.replace( 0, this->height() / 0.2 );
	lst2.replace( 1, this->height() / 0.8 );
	splitter_3->setSizes( lst2 );
}

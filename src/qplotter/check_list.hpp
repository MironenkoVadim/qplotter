#ifndef CHECKLIST_HPP_
#define CHECKLIST_HPP_

#include <QComboBox>
#include <QLineEdit>
#include <QStandardItemModel>


class CheckList : public QComboBox
{
	Q_OBJECT

public:
	CheckList( QWidget* parent = nullptr ) :
		QComboBox( parent )
	{
		m_model = new QStandardItemModel;
		setEditable( true );
		slotUpdateText();
		connect( m_model, &QStandardItemModel::itemChanged, this, &CheckList::slotUpdate );
	}


	void addItem( const QString& text )
	{
		QStandardItem* item = new QStandardItem();
		item->setText( text );
		item->setFlags( Qt::ItemIsUserCheckable | Qt::ItemIsEnabled );
		item->setData( static_cast< int >( Qt::Unchecked ), Qt::CheckStateRole );
		m_model->setItem( m_model->rowCount(), 0, item );
		this->setModel( m_model );
	}


	void addItems( const QStringList& texts )
	{
		for ( const auto& t: texts )
		{
			addItem( t );
		}
	}


	QStringList getCheckedItems() const
	{
		QStringList checkedItems;

		for ( int i = 0; i < m_model->rowCount(); i++ )
		{
			if ( m_model->item( i, 0 )->checkState() == Qt::Checked )
			{
				checkedItems << m_model->item( i, 0 )->text();
			}
		}

		return( checkedItems );
	}


	void setCheckedItems( const QStringList& items )
	{
		for ( int i = 0; i < items.count(); i++ )
		{
			int index = findText( items.at( i ) );
			if ( index != -1 )
			{
				m_model->item( index )->setData( static_cast< int >( Qt::Checked ), Qt::CheckStateRole );
			}
		}

		slotUpdate();
	}


	Q_SLOT void slotUpdateText()
	{
		lineEdit()->setText( m_displayText );
	}


	Q_SLOT void slotUpdate()
	{
		m_displayText.clear();

		for ( int i = 0; i < m_model->rowCount(); i++ )
		{
			if ( m_model->item( i, 0 )->checkState() == Qt::Checked )
			{
				m_displayText.append( m_model->item( i, 0 )->text() );
				m_displayText.append( "; " );
			}
		}
		slotUpdateText();
		Q_EMIT updated();
	}


	Q_SIGNAL void updated();

private:
	QStandardItemModel* m_model { nullptr };
	QString             m_displayText;
}; // class CheckList

#endif // ifndef CHECKLIST_HPP_

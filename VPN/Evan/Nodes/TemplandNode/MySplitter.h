#ifndef MYSPLITTER_H
#define MYSPLITTER_H

#include <QtGui/QSplitter>
#include <QMessageBox>

//! Templand MySplitter class
/*!
	This class modifies a behavior of the standard QT Splitter control.
*/
class MySplitter : public QSplitter
{
	Q_OBJECT
public:
    MySplitter( Qt::Orientation orientation, QWidget * parent = 0 ) : QSplitter( orientation, parent )
    {
    }

public slots:

    //! Moves the splitter to a specific location. This is useful if you want one splitter moving another splitter.
	/*!
		\param pos New position of the splitter.
		\param index The splitter handle index from left or top edge.
	*/
    void moveSplitterPosition( int pos, int index )
    {
        blockSignals( true );
        moveSplitter( pos, index );
        blockSignals( false );
    }
};

#endif

#ifndef TEMPLANDNODE_H
#define TEMPLANDNODE_H

#ifndef TEMPLAND_TOOLKIT_BUILD
#define TEMPLAND_TOOLKIT_BUILD 1
#endif

#include <map>
#include "Designer/ui_TemplandWindow.h"
#if TEMPLAND_TOOLKIT_BUILD
#include "../INode.h"
#include "../../DataTypes/LandmarkSet.h"
#include "../../DataTypes/SurfaceVector.h"
#include "../../DataTypes/Loadings.h"

#endif

#include "ew/View3Landmarks.h"

#include "TableauLayout.h"


#if TEMPLAND_TOOLKIT_BUILD
#include "../../DataTypes/Specimens.h"
#endif

class SurfaceItem;
class FormItem;

#if TEMPLAND_TOOLKIT_BUILD
class TemplandNode : public QMainWindow, public Ui::TemplandWindow , public INode
#else
class TemplandNode : public QMainWindow, public Ui::TemplandWindow
#endif
{
Q_OBJECT
private:
#if TEMPLAND_TOOLKIT_BUILD

	LandmarkSet* m_targetLmks;
	SurfaceVector* m_targetSurfaces;
	LandmarkSet* m_templateLmks;
	SurfaceVector* m_templateSurfaces;
	LandmarkSet* m_targetLmksSpline;
	LandmarkSet* m_templateLmksSpline;
	Specimens* m_specimens;
	bool m_open;

#endif

    QGLWidget* m_share;

    unsigned int m_tableauIndex;

	void connectActions();

	TableauLayout* getCurrentTableauFile()
	{
	     QMdiSubWindow * cwin = mdiArea->currentSubWindow();

        if( cwin == 0 )
            return 0;

        return (TableauLayout*)cwin->widget();
	}

	void GetSpecimens();

protected:
    void closeEvent(QCloseEvent *event);
#if TEMPLAND_TOOLKIT_BUILD
	void setOutputPort0(LandmarkSet* p){setOutputData(0,p);}
	void setOutputPort1(SurfaceVector* p){setOutputData(1,p);}
	void setOutputPort2(LandmarkSet* p){setOutputData(2,p);}
	void setOutputPort3(SurfaceVector* p){setOutputData(3,p);}
	void setOutputPort4(LandmarkSet* p){setOutputData(4,p);}
	void setOutputPort5(LandmarkSet* p){setOutputData(5,p);}
	void setOutputPort6(Specimens* p){setOutputData(6,p);}

#endif

public:
	TemplandNode(QWidget* parent=0);
	~TemplandNode();
#if TEMPLAND_TOOLKIT_BUILD
	void process();
	QString toString() const;
	void fromString(const QString&);
#endif
	void showGUI() {activateWindow();show();m_open = true;}
	const QObject* getDialog() const {return this;}
	bool isUpdated() const {return false;}
	bool canDelete() {return !m_open;}

public slots:
    TableauLayout* newTableau( bool createNewTab = true );
    void openTableau( const QString& cmd_filename = "" );
    void saveAsTableau();
    void saveTableau();

    void filenameChanged( QString filename );
    void subWindowChanged( QMdiSubWindow * window );

    void typeLandmark();
    void typeSemiLandmark();
    void typeNonLandmark();

    void changeBckgndColor();

    void templateHide();

	void status( QString msg ) { statusbar->showMessage( msg ); }
	void status( QString msg, int val ) { statusbar->showMessage( msg, val ); }

    void refreshOutputs();

    void selectPoints( bool tog );

signals:
	void dataReady();
};
#endif

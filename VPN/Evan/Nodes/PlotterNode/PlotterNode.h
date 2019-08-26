#ifndef PLOTTERNODE_H
#define PLOTTERNODE_H

#include "../INode.h"
#include "../../DataTypes/Loadings.h"
#include "../../DataTypes/FloatVector.h"
#include <QMainWindow>
#include <QMouseEvent>
#include <QAction>
#include <QTreeWidgetItem>
#include <qwt_symbol.h>
#include <qwt_plot.h>
#include <qwt_plot_picker.h>

#include "Designer/ui_axesSettings.h"

#include "../../Utilities/Logger.h"

class QwtPlotMarker;
class QwtPlotGrid;
class QwtPlotRescaler;
class QwtPlotScaleItem;
class QwtPlotCurve;
class QComboBox;
class QFrame;
class GroupTreeWidget;
class GroupTreeItem;
class MemberTreeItem;

class PlotPicker : public QwtPlotPicker
{
    Q_OBJECT
    private:
        bool m_mouseButton;
    public:
        PlotPicker( int   	xAxis,
                    int  	yAxis,
                    int  	selectionFlags,
                    RubberBand  	rubberBand,
                    DisplayMode  	trackerMode,
                    QwtPlotCanvas *  	canvas) : QwtPlotPicker(xAxis,yAxis,selectionFlags,rubberBand,trackerMode,canvas),m_mouseButton(true)
        {}
        bool getMouseButton()const{return m_mouseButton;}
    protected:
        virtual void widgetMousePressEvent (QMouseEvent *);
};

class AxesSettings;

class PlotterNode : public QMainWindow , public INode
{
Q_OBJECT

private:
    Variables* m_xAxisInput;
	Variables* m_yAxisInput;
	Loadings* m_loadingsInput;
    Loadings* m_loadingsOutput;
    FloatVector* m_clickOutput;

    GroupTreeWidget* m_groupsTree;
    QwtPlot* m_plotArea;
    QwtPlotRescaler* m_plotRescaler;
    PlotPicker* m_plotPicker;
    QwtPlotGrid* m_plotGrid;
    QwtPlotScaleItem* m_xAxisScale;
	QwtPlotScaleItem* m_yAxisScale;
	QwtSymbol m_plotSymbol;
	QwtPlotMarker* m_leftMark;
	QwtPlotMarker* m_rightMark;

    QFrame* m_plotFrame;
    QMenu* m_symbolMenu;
    QMenu* m_labelMenu;
    QMenu* m_labelAlignMenu;
    QMenu* m_labelOrientMenu;
    QComboBox* m_xAxisCombo;
    QComboBox* m_yAxisCombo;
    QMap<QAction*, Qt::Alignment> m_markerAlignments;
    QMap<QAction*, Qt::Orientation> m_markerOrientations;
    QMap<MemberTreeItem*, QwtPlotMarker*> m_markerItems;
    QVector<QwtPlotMarker*> m_selectedMarkers;
    QVector<QwtPlotMarker*> m_allMarkers;
    QVector<int> m_rowIndices;
    QMenu *m_axesMenu;
    QMenu *m_titleMenu;

    double m_axesScaleFactor;
    AxesSettings * m_axesSettings;

    //VD
    QMenu* viewMenu;
    QwtDoublePoint lastPos;
    FloatVector* m_translatedClickOutput;
    bool m_mouseCoordinates;
    bool m_hideCoordinates;

    bool m_fitPlot;
    bool m_showLabels;
    bool m_loadGroups;

//    int m_cachedSize;
//    QwtSymbol::Style m_cachedSymbol;
//    QColor m_cachedColor;
    QColor m_cachedBGColor;

    QList<GroupTreeItem*> m_loadedGroupItems;

    void clearLoadedGroups();
    void checkLoadedGroups();
    bool getBounds(double& xMin, double& xMax, double& yMin, double& yMax);
    void assignAxisValues();
    void calculateLoadingsOutput();
    void highlightMember(MemberTreeItem*);
    void clearSelectedMarkers();
    void scaleAxes();
    void updatePlot();

protected:
	Variables* getInputPortData0()const { return dynamic_cast<Variables*>( getInputData( 0 ) ); }
	Variables* getInputPortData1()const { return dynamic_cast<Variables*>( getInputData( 1 ) ); }
	Loadings* getInputPortData2()const { return dynamic_cast<Loadings *>( getInputData( 2 ) ); }

	void setOutputPort0(Loadings* p){setOutputData(0,p);}
	void setOutputPort1(FloatVector* p){setOutputData(1,p);}
	void setOutputPort2(FloatVector* p){setOutputData(2,p);}

public:
	PlotterNode(QWidget* parent=0);
	~PlotterNode();
	void showGUI()
    {
        QWidget::activateWindow();
        QWidget::show();
    }
	const QObject* getDialog() const {return this;}
	void process();
	bool isUpdated() const {return false;}

	QString toString() const;
	void fromString(const QString& params);

    void setAxesLimits();

public slots:
    void labelAlignAction(QAction*);
    void labelOrientAction(QAction*);
    void markerInColor(QColor);
    void bkgndColor();
    void toggleGrid(bool);
    void toggleCartesian(bool);
    void toggleLabels(bool);
    void toggleFit()    {m_fitPlot=!m_fitPlot; assignAxisValues();}
    void copyPlotToClipboard();
    void plot();
    void axisComboChanged();
    void setMarkersSymbol(int);
    void setMarkersSize(float);
    void markersSelected(const QwtDoubleRect&);
    void changePlotTitle(const QString& text) {m_plotArea->setTitle(text);}
    void fillClickOutput(const QwtDoublePoint&);
    bool exportPlot();
    void disableMouse();
    void hideCoord();
    void selectMarkers();
    void refreshMarkers();
    void showAxesSettings();
    void showLabelFontsSettings();
    void showAxisFontsSettings();
    void changeXAxisLabel();
    void changeYAxisLabel();
    void changeTitleFont();
    void changeTitleText();
    void changeTitleColor();
    void changeLabel(QTreeWidgetItem* item, int num);
    void changeXAxisColor();
    void changeYAxisColor();
    void changeAxisThickness();
    void changeGridColor();
    void changeMargin();
    void keyPressEvent(QKeyEvent *event);

signals:
	void dataReady();
};

class AxesSettings : public QDialog , public Ui::AxisDialog
{
    Q_OBJECT

private:
    double m_xMin;
    double m_xMax;
    double m_yMin;
    double m_yMax;
    bool m_ready;
    PlotterNode * m_plotter;

public:
    AxesSettings(QWidget* parent=0)
    {
    	m_xMin = m_xMax = 0.0;
    	m_yMin = m_yMax = 0.0;
    	m_plotter = NULL;
        setupUi(this);
        m_ready = false;
    }
    ~AxesSettings(){}

    void setPlotter(PlotterNode * plotter)
    {
        m_plotter = plotter;
    }

    bool isReady()
    {
        return m_ready;
    }
    void setMinX(double x)
    {
        m_xMin = x;
        QString txt;
        txt.setNum(x);
        lineEdit->setText(txt);
    }
    void setMaxX(double x)
    {
        m_xMax = x;
        QString txt;
        txt.setNum(x);
        lineEdit_2->setText(txt);
    }
    void setMinY(double y)
    {
        m_yMin = y;
        QString txt;
        txt.setNum(y);
        lineEdit_3->setText(txt);
    }
    void setMaxY(double y)
    {
        m_yMax = y;
        QString txt;
        txt.setNum(y);
        lineEdit_4->setText(txt);
    }
    void getBounds(double &xMin, double &xMax, double &yMin, double &yMax)
    {
        xMin = m_xMin;
        xMax = m_xMax;
        yMin = m_yMin;
        yMax = m_yMax;
    }
    void setBounds(double xMin, double xMax, double yMin, double yMax)
    {
        m_xMin = xMin;
        m_xMax = xMax;
        m_yMin = yMin;
        m_yMax = yMax;
    }

public slots:
    void accept()
    {
        bool ok;
        double tmp;
        tmp = lineEdit->text().toDouble(&ok);
        if(ok)
        {
            m_xMin = tmp;
        }
        tmp = lineEdit_2->text().toDouble(&ok);
        if(ok)
        {
            m_xMax = tmp;
        }
        tmp = lineEdit_3->text().toDouble(&ok);
        if(ok)
        {
            m_yMin = tmp;
        }
        tmp = lineEdit_4->text().toDouble(&ok);
        if(ok)
        {
            m_yMax = tmp;
        }
        m_ready = true;
        m_plotter->setAxesLimits();

        QDialog::accept();
    }
};

#endif

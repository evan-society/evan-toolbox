#include "PlotterNode.h"
#include "../../Utilities/Logger.h"
#include "../GroupNode/GroupTreeWidget.h"
#include "../GroupNode/PlotSymbolCombo.h"
#include <QtGui>
#include <qwt_scale_engine.h>
#include <qwt_scale_div.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_picker.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_scaleitem.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_rescaler.h>
#include <qwt_scale_widget.h>

#include <QtSvg/QGraphicsSvgItem>
#include <QtSvg/QSvgGenerator>
#include <QtSvg/QSvgRenderer>
#include <QtSvg/QSvgWidget>
#include <QPrinter>
#include <limits>
#include <QSettings>


QwtSymbol::Style getSymbolFromIndex(int index)
{
    QwtSymbol::Style result = QwtSymbol::NoSymbol;
    switch(index)
    {
        case 0: result=QwtSymbol::Ellipse;break;
        case 1: result=QwtSymbol::Rect;break;
        case 2: result=QwtSymbol::Diamond;break;
        case 3: result=QwtSymbol::Triangle;break;
        case 4: result=QwtSymbol::Cross;break;
        case 5: result=QwtSymbol::XCross;break;
        case 6: result=QwtSymbol::HLine;break;
        case 7: result=QwtSymbol::VLine;break;
        case 8: result=QwtSymbol::Star1;break;
        case 9: result=QwtSymbol::Star2;break;
        case 10: result=QwtSymbol::Hexagon;break;
        default: result=QwtSymbol::Ellipse;
    }
    return result;
}

void PlotPicker::widgetMousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::RightButton)
    {
        QwtPlotPicker::widgetMousePressEvent(e);
        QPoint pos = parentWidget()->mapFromGlobal(QCursor::pos());
        m_mouseButton=false;
        emit moved(invTransform(pos));
    }
    else if ( e->button() == Qt::LeftButton )
    {
        QwtPlotPicker::widgetMousePressEvent(e);
        m_mouseButton=true;
    }
}

PlotterNode::PlotterNode(QWidget* parent) : QMainWindow(parent),  INode(),
                                        m_plotSymbol(QwtSymbol::Ellipse, QColor(Qt::black), QColor(Qt::black), QSize(8, 8))
{
    m_xAxisInput = NULL;
	m_yAxisInput = NULL;
	m_loadingsInput = NULL;

	m_loadingsOutput = new Loadings;
    m_clickOutput = new FloatVector;
    m_translatedClickOutput = new FloatVector;

    m_axesScaleFactor = 1.5;

	m_fitPlot = false;
	m_showLabels = true;

	m_cachedBGColor = Qt::transparent;

    m_plotArea = new QwtPlot(this);
    m_plotArea->setCanvasBackground(Qt::white);
    m_plotArea->setFrameShape(QFrame::NoFrame);
    m_plotArea->setCanvasLineWidth(0);
    //m_plotArea->plotLayout()->setAlignCanvasToScales(true);
    m_plotArea->setMargin(50);
    //m_plotArea->setAutoReplot(true);

    m_groupsTree = new GroupTreeWidget(this);
    m_groupsTree->headerItem()->setText(0, QApplication::translate("PlotterNode", "Name", 0, QApplication::UnicodeUTF8));
    m_groupsTree->headerItem()->setText(1, QApplication::translate("PlotterNode", "Size", 0, QApplication::UnicodeUTF8));
    m_groupsTree->headerItem()->setText(2, QApplication::translate("PlotterNode", "Plot Symbol", 0, QApplication::UnicodeUTF8));
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(m_groupsTree->sizePolicy().hasHeightForWidth());
    m_groupsTree->setSizePolicy(sizePolicy);
    m_groupsTree->hide();
    connect(m_groupsTree, SIGNAL(itemSelectionChanged()), this, SLOT(selectMarkers()));
    connect(m_groupsTree, SIGNAL(sizeChanged(float)), this, SLOT(setMarkersSize(float)));
    connect(m_groupsTree, SIGNAL(colorChanged(QColor)), this, SLOT(markerInColor(QColor)));
    connect(m_groupsTree, SIGNAL(symbolChanged(int)), this, SLOT(setMarkersSymbol(int)));
    connect(m_groupsTree, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(changeLabel(QTreeWidgetItem*,int)));

    m_plotRescaler = new QwtPlotRescaler(m_plotArea->canvas());
    m_plotRescaler->setRescalePolicy(QwtPlotRescaler::Fixed);
    m_plotRescaler->setExpandingDirection(QwtPlot::yLeft, QwtPlotRescaler::ExpandBoth);
    m_plotRescaler->setEnabled(false);

    m_plotGrid = new QwtPlotGrid;
    m_plotGrid->enableXMin(true);
    m_plotGrid->enableYMin(true);
    m_plotGrid->setMajPen(QPen(Qt::black, 0, Qt::DotLine));
    m_plotGrid->setMinPen(QPen(Qt::gray, 0 , Qt::DotLine));
    m_plotGrid->attach(m_plotArea);
    m_plotGrid->setVisible(false);

    m_plotPicker = new PlotPicker(QwtPlot::xBottom, QwtPlot::yLeft,
                        QwtPicker::RectSelection | QwtPicker::CornerToCorner ,
                        QwtPicker::NoRubberBand, QwtPicker::AlwaysOn,
                        m_plotArea->canvas());

    m_xAxisScale = new QwtPlotScaleItem(QwtScaleDraw::BottomScale);
    m_xAxisScale->setScaleDivFromAxis(true);
    m_xAxisScale->attach(m_plotArea);

    m_yAxisScale = new QwtPlotScaleItem(QwtScaleDraw::LeftScale);
    m_yAxisScale->setScaleDivFromAxis(true);
    m_yAxisScale->attach(m_plotArea);

    m_xAxisScale->hide();
    m_yAxisScale->hide();

    //setCentralWidget(m_plotArea);
    QHBoxLayout* hLayout = new QHBoxLayout(this);
    m_plotFrame = new QFrame(this);
    m_plotFrame->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 255);"));
    m_plotFrame->setFrameShape(QFrame::NoFrame);
    QVBoxLayout* frameLayout = new QVBoxLayout(m_plotFrame);
    frameLayout->addWidget(m_plotArea);
    m_plotFrame->setLayout(frameLayout);
    hLayout->addWidget(m_plotFrame);
    hLayout->addWidget(m_groupsTree);

    QVBoxLayout* verticalLayout = new QVBoxLayout(this);
    verticalLayout->addLayout(hLayout);

    m_xAxisCombo = new QComboBox(this);
    m_xAxisCombo->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_yAxisCombo = new QComboBox(this);
    m_yAxisCombo->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    QHBoxLayout* horizontalLayout = new QHBoxLayout();
    QSpacerItem* horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    QLabel* xlabel = new QLabel("X Axis", this);
    QLabel* ylabel = new QLabel("Y Axis", this);

    horizontalLayout->addItem(horizontalSpacer);
    horizontalLayout->addWidget(xlabel);
    horizontalLayout->addWidget(m_xAxisCombo);
    horizontalLayout->addWidget(ylabel);
    horizontalLayout->addWidget(m_yAxisCombo);

    verticalLayout->addLayout(horizontalLayout);

    QMenuBar* menubar = new QMenuBar();
    viewMenu = new QMenu("Plot");

    ////////////////////////////////////////////////////
    m_titleMenu = new QMenu("Title");
    m_axesMenu = new QMenu("Axes");
    m_labelMenu = new QMenu("Labels");

    QAction* labelAction = new QAction("Show/Hide", m_labelMenu);
    labelAction->setObjectName(QString::fromUtf8("LabelAction"));
    labelAction->setCheckable(true);
    labelAction->setChecked(true);
    m_labelMenu->addAction(labelAction);
    connect(labelAction, SIGNAL(toggled(bool)), this, SLOT(toggleLabels(bool)));

    m_labelAlignMenu = new QMenu("Alignment");

    QAction* alignTop = new QAction("Top", m_labelAlignMenu);
    alignTop->setCheckable(true);
    alignTop->setChecked(true);
    m_markerAlignments.insert(alignTop, Qt::AlignTop);
    m_labelAlignMenu->addAction(alignTop);

    QAction* alignBottom = new QAction("Bottom", m_labelAlignMenu);
    alignBottom->setCheckable(true);
    m_markerAlignments.insert(alignBottom, Qt::AlignBottom);
    m_labelAlignMenu->addAction(alignBottom);

    QAction* alignLeft = new QAction("Left", m_labelAlignMenu);
    alignLeft->setCheckable(true);
    m_markerAlignments.insert(alignLeft, Qt::AlignLeft);
    m_labelAlignMenu->addAction(alignLeft);

    QAction* alignRight = new QAction("Right", m_labelAlignMenu);
    alignRight->setCheckable(true);
    m_markerAlignments.insert(alignRight, Qt::AlignRight);
    m_labelAlignMenu->addAction(alignRight);

    QAction* alignCenter = new QAction("Center", m_labelAlignMenu);
    alignCenter->setCheckable(true);
    m_markerAlignments.insert(alignCenter, Qt::AlignCenter);
    m_labelAlignMenu->addAction(alignCenter);

    QActionGroup* alignGroup = new QActionGroup(this);
    alignGroup->addAction(alignTop);
    alignGroup->addAction(alignBottom);
    alignGroup->addAction(alignLeft);
    alignGroup->addAction(alignRight);
    alignGroup->addAction(alignCenter);

    connect(m_labelAlignMenu, SIGNAL(triggered(QAction*)), this, SLOT(labelAlignAction(QAction*)));
    m_labelMenu->addMenu(m_labelAlignMenu);

    m_labelOrientMenu = new QMenu("Orientation");

    QAction* orientHorz = new QAction("Horizontal", m_labelOrientMenu);
    orientHorz->setCheckable(true);
    orientHorz->setChecked(true);
    m_markerOrientations.insert(orientHorz, Qt::Horizontal);
    m_labelOrientMenu->addAction(orientHorz);

    QAction* orientVert = new QAction("Vertical", m_labelOrientMenu);
    orientVert->setCheckable(true);
    m_markerOrientations.insert(orientVert, Qt::Vertical);
    m_labelOrientMenu->addAction(orientVert);

    QActionGroup* orientGroup = new QActionGroup(this);
    orientGroup->addAction(orientHorz);
    orientGroup->addAction(orientVert);

    connect(m_labelOrientMenu, SIGNAL(triggered(QAction*)), this, SLOT(labelOrientAction(QAction*)));
    m_labelMenu->addMenu(m_labelOrientMenu);

    QAction* labelFontAction = new QAction("Set Font", m_labelMenu);
    labelFontAction->setObjectName(QString::fromUtf8("LabelFontAction"));
    m_labelMenu->addAction(labelFontAction);
    connect(labelFontAction, SIGNAL(triggered()), this, SLOT(showLabelFontsSettings()));

    QAction* axesAction = new QAction("Cartesian Axes", viewMenu);
    axesAction->setObjectName(QString::fromUtf8("CartesianAction"));
    axesAction->setCheckable(true);
    axesAction->setChecked(true);
    toggleCartesian(true); // VD - Cartesian axes by default
    connect(axesAction, SIGNAL(toggled(bool)), this, SLOT(toggleCartesian(bool)));

    QAction* fitAction = new QAction("Square Axes", viewMenu);
    fitAction->setObjectName(QString::fromUtf8("FitAction"));
    fitAction->setCheckable(true);
    fitAction->setChecked(true);
    connect(fitAction, SIGNAL(triggered()), this, SLOT(toggleFit()));

    QAction* limitsAction = new QAction("Set Axes Limits", viewMenu);
    connect(limitsAction, SIGNAL(triggered()), this, SLOT(showAxesSettings()));

    QAction* xAxisLabelAction = new QAction("Set X-Axis label", viewMenu);
    connect(xAxisLabelAction, SIGNAL(triggered()), this, SLOT(changeXAxisLabel()));

    QAction* yAxisLabelAction = new QAction("Set Y-Axis label", viewMenu);
    connect(yAxisLabelAction, SIGNAL(triggered()), this, SLOT(changeYAxisLabel()));

    QAction* xAxisColorAction = new QAction("Set X-Axis colour", viewMenu);
    connect(xAxisColorAction, SIGNAL(triggered()), this, SLOT(changeXAxisColor()));

    QAction* yAxisColorAction = new QAction("Set Y-Axis colour", viewMenu);
    connect(yAxisColorAction, SIGNAL(triggered()), this, SLOT(changeYAxisColor()));

    QAction* axesFontAction = new QAction("Set Font", viewMenu);
    axesFontAction->setObjectName(QString::fromUtf8("AxesFontAction"));
    connect(axesFontAction, SIGNAL(triggered()), this, SLOT(showAxisFontsSettings()));

    QAction* changeTitleTextAction = new QAction("Set Text", viewMenu);
    connect(changeTitleTextAction, SIGNAL(triggered()), this, SLOT(changeTitleText()));

    QAction* changeTitleFontAction = new QAction("Set Font", viewMenu);
    connect(changeTitleFontAction, SIGNAL(triggered()), this, SLOT(changeTitleFont()));

    QAction* changeTitleColorAction = new QAction("Set Colour", viewMenu);
    connect(changeTitleColorAction, SIGNAL(triggered()), this, SLOT(changeTitleColor()));

    QAction* axisThicknessAction = new QAction("Set Thickness", viewMenu);
    connect(axisThicknessAction, SIGNAL(triggered()), this, SLOT(changeAxisThickness()));

    ///////////////////////////////////////////////////////

    QAction* bkgndAction = new QAction("Background Colour", viewMenu);
    connect(bkgndAction, SIGNAL(triggered()), this, SLOT(bkgndColor()));
    viewMenu->addAction(bkgndAction);

    QAction* gridAction = new QAction("Show Grid", viewMenu);
    gridAction->setObjectName(QString::fromUtf8("GridAction"));
    gridAction->setCheckable(true);
    //gridAction->setChecked(true);
    connect(gridAction, SIGNAL(toggled(bool)), this, SLOT(toggleGrid(bool)));

    QAction* gridColorAction = new QAction("Set Grid colour", viewMenu);
    connect(gridColorAction, SIGNAL(triggered()), this, SLOT(changeGridColor()));

    QAction* marginAction = new QAction("Set Plot Margin", viewMenu);
    connect(marginAction, SIGNAL(triggered()), this, SLOT(changeMargin()));

    QAction* copyPlot = new QAction("Copy to Clipboard", viewMenu);
    copyPlot->setShortcut(QKeySequence("Ctrl+C"));
    connect(copyPlot, SIGNAL(triggered()), this, SLOT(copyPlotToClipboard()));

    QAction* savePlot = new QAction("Export", viewMenu);
    savePlot->setShortcut(QKeySequence("Ctrl+S"));
    connect(savePlot, SIGNAL(triggered()), this, SLOT(exportPlot()));

    QAction* disableMouseCoordinates = new QAction("Disable mouse coordinates", viewMenu);
    connect(disableMouseCoordinates, SIGNAL(triggered()), this, SLOT(disableMouse()));
    disableMouseCoordinates->setObjectName(QString::fromUtf8("mouseCoord"));
    disableMouseCoordinates->setCheckable(true);
    disableMouseCoordinates->setChecked(false);
    m_mouseCoordinates=true;

    QAction* hideMouseCoordinates = new QAction("Hide mouse coordinates", viewMenu);
    connect(hideMouseCoordinates, SIGNAL(triggered()), this, SLOT(hideCoord()));
    hideMouseCoordinates->setObjectName(QString::fromUtf8("hideCoord"));
    hideMouseCoordinates->setCheckable(true);
    hideMouseCoordinates->setChecked(false);
    m_hideCoordinates=false;

    viewMenu->addAction(bkgndAction);
    viewMenu->addAction(gridAction);
    viewMenu->addAction(gridColorAction);
    viewMenu->addAction(marginAction);
    viewMenu->addSeparator();

    viewMenu->addMenu(m_titleMenu);
    m_titleMenu->addAction(changeTitleTextAction);
    m_titleMenu->addAction(changeTitleColorAction);
    m_titleMenu->addAction(changeTitleFontAction);

    viewMenu->addMenu(m_axesMenu);
    m_axesMenu->addAction(axesAction);
    m_axesMenu->addAction(fitAction);
    m_axesMenu->addAction(limitsAction);
    m_axesMenu->addAction(xAxisLabelAction);
    m_axesMenu->addAction(yAxisLabelAction);
    m_axesMenu->addAction(axesFontAction);
    m_axesMenu->addAction(xAxisColorAction);
    m_axesMenu->addAction(yAxisColorAction);
    m_axesMenu->addAction(axisThicknessAction);

    viewMenu->addMenu(m_labelMenu);
    viewMenu->addSeparator();
    viewMenu->addAction(disableMouseCoordinates);
    viewMenu->addAction(hideMouseCoordinates);
    viewMenu->addSeparator();
    viewMenu->addAction(copyPlot);
    viewMenu->addAction(savePlot);
    menubar->addMenu(viewMenu);
    setMenuBar(menubar);
    QWidget* centralWidget = new QWidget(this);
    centralWidget->setLayout(verticalLayout);
    setCentralWidget(centralWidget);
    setWindowTitle("Graph Plotter");

    QObject::connect(m_plotPicker, SIGNAL(selected(const QwtDoubleRect&)),
                    this, SLOT(markersSelected(const QwtDoubleRect&)));
    QObject::connect(m_plotPicker, SIGNAL(moved(const QwtDoublePoint&)),
                    this, SLOT(fillClickOutput(const QwtDoublePoint&)));
    QObject::connect(m_xAxisCombo, SIGNAL(activated(int)), this, SLOT(axisComboChanged()));
    QObject::connect(m_yAxisCombo, SIGNAL(activated(int)), this, SLOT(axisComboChanged()));

    QSplitter* page = new QSplitter();
    hLayout->addWidget(page);
    page->addWidget(m_plotFrame);
    page->addWidget(m_groupsTree);

    QList<int> sizes;
    sizes.append(500);
    sizes.append(100);
    page->setSizes(sizes);

    m_axesSettings = new AxesSettings();
    m_axesSettings->setPlotter(this);

    QwtSymbol leftSymbol;
    m_leftMark = new QwtPlotMarker();
    m_leftMark->setLineStyle(QwtPlotMarker::NoLine);
    m_leftMark->setValue(0.0, 0.0);
    leftSymbol.setSize(10);
    leftSymbol.setBrush(QBrush(Qt::red));
    leftSymbol.setPen(QPen(Qt::red));
    leftSymbol.setStyle(QwtSymbol::XCross);
    m_leftMark->setSymbol(leftSymbol);

    QwtSymbol rightSymbol;
    m_rightMark = new QwtPlotMarker();
    m_rightMark->setLineStyle(QwtPlotMarker::NoLine);
    m_rightMark->setValue(0.0, 0.0);
    rightSymbol.setSize(10);
    rightSymbol.setBrush(QBrush(Qt::blue));
    rightSymbol.setPen(QPen(Qt::blue));
    rightSymbol.setStyle(QwtSymbol::XCross);
    m_rightMark->setSymbol(rightSymbol);

    m_loadGroups = true;
}

PlotterNode::~PlotterNode()
{
    m_selectedMarkers.clear();
    clearLoadedGroups();
}

void PlotterNode::clearLoadedGroups()
{
    foreach(GroupTreeItem* groupItem, m_loadedGroupItems)
    {
        for(int i=0; i<groupItem->childCount(); ++i)
            delete groupItem->child(i);
        delete groupItem;
    }
    m_loadedGroupItems.clear();
}

bool PlotterNode::getBounds(double& xMin, double& xMax, double& yMin, double& yMax)
{
    if(m_axesSettings->isReady())
    {
        m_axesSettings->getBounds(xMin, xMax, yMin, yMax);
        return true;
    }
    m_xAxisInput = getInputPortData0();
	m_yAxisInput = getInputPortData1();

    if( m_xAxisInput == NULL || !m_xAxisInput->isValid() ||
        m_yAxisInput == NULL || !m_yAxisInput->isValid())
        return false;
    xMax = std::numeric_limits<double>::min();
    xMin = std::numeric_limits<double>::max();
    yMax = std::numeric_limits<double>::min();
    yMin = std::numeric_limits<double>::max();

    for(unsigned int i=0; i<m_xAxisInput->GetRows(); ++i)
    {
        double xValue = m_xAxisInput->get(i, m_xAxisCombo->currentIndex());
        if(xValue > xMax)
            xMax= xValue;
        if(xValue <  xMin)
            xMin = xValue;
    }

    for(unsigned int i=0; i<m_yAxisInput->GetRows(); ++i)
    {
        double yValue = m_yAxisInput->get(i, m_yAxisCombo->currentIndex());
        if(yValue > yMax)
            yMax= yValue;
        if(yValue <  yMin)
            yMin = yValue;
    }
    return true;
}

void PlotterNode::assignAxisValues()
{
    //Calculate the axes scale values
    double xMax, xMin, yMax, yMin;
    if(!getBounds(xMin, xMax, yMin, yMax))
        return;

    m_plotRescaler->setEnabled(!m_fitPlot);

    if(!m_fitPlot)
    {
        xMax = yMax = max(xMax,yMax);
        xMin = yMin = min(xMin, yMin);
    }
    m_plotArea->setAxisScale(QwtPlot::xBottom, xMin, xMax);
    m_plotArea->setAxisScale(QwtPlot::yLeft, yMin, yMax);
    m_plotArea->updateAxes();

    scaleAxes();
}

void PlotterNode::calculateLoadingsOutput()
{
    m_loadingsInput = getInputPortData2();
    if( m_loadingsInput == NULL || !m_loadingsInput->isValid())
        return;
    if ( m_xAxisCombo->currentIndex()<0 || m_yAxisCombo->currentIndex()<0 )
        return;
    if ( m_xAxisCombo->currentIndex() >= (int)m_loadingsInput->GetCols() )
    {
        Logger::getInstance()->log("[Plotter Node] X Variables do not match Loadings input! No Loadings output generated",
                                    Logger::WARNING);
        return;
    }
    if ( m_yAxisCombo->currentIndex() >= (int)m_loadingsInput->GetCols() )
    {
        Logger::getInstance()->log("[Plotter Node] Y Variables do not match Loadings input! No Loadings output generated",
                                    Logger::WARNING);
        return;
    }
    m_loadingsOutput->ReSize(m_loadingsInput->GetRows(),2);
    for(unsigned int i=0; i<m_loadingsInput->GetRows(); ++i)
    {
        double xVal = m_loadingsInput->get(i, m_xAxisCombo->currentIndex());
        double yVal = m_loadingsInput->get(i, m_yAxisCombo->currentIndex());
        m_loadingsOutput->set(i,0,xVal);
        m_loadingsOutput->set(i,1,yVal);
    }

    vector<double> minscores(2);
    vector<double> maxscores(2);

    // use axes as ranges for the sliders
    getBounds(minscores[0], maxscores[0], minscores[1], maxscores[1]);

    m_loadingsOutput->setMin(minscores);
    m_loadingsOutput->setMax(maxscores);
    m_loadingsOutput->setNumScores(2);

    setOutputPort0(m_loadingsOutput);
}

void PlotterNode::process()
{
    m_xAxisInput = getInputPortData0();
	m_yAxisInput = getInputPortData1();

    if( m_xAxisInput == NULL || !m_xAxisInput->isValid() ||
        m_yAxisInput == NULL || !m_yAxisInput->isValid())
        return;
    Logger::getInstance()->log("[Plotter Node] Processing started");

    m_xAxisCombo->clear();
    for(unsigned int i=0; i<m_xAxisInput->GetCols();++i)
        m_xAxisCombo->addItem(m_xAxisInput->getColumnLabel(i));
    m_xAxisCombo->setCurrentIndex(0);

    m_yAxisCombo->clear();
    for(unsigned int i=0; i<m_yAxisInput->GetCols();++i)
        m_yAxisCombo->addItem(m_yAxisInput->getColumnLabel(i));

    if (m_xAxisInput == m_yAxisInput)
        m_yAxisCombo->setCurrentIndex(1);
    else
        m_yAxisCombo->setCurrentIndex(0);

    plot();
    calculateLoadingsOutput();

    if (m_mouseCoordinates)
    {
        m_clickOutput->clear();
        m_clickOutput->addFloat(0.0);
        m_clickOutput->addFloat(0.0);
        setOutputPort1(m_clickOutput);
        m_translatedClickOutput->clear();
        m_translatedClickOutput->addFloat(0.0);
        m_translatedClickOutput->addFloat(0.0);
        setOutputPort2(m_translatedClickOutput);
    }

    m_loadGroups = false;
    Logger::getInstance()->log("[Plotter Node] Processing finished");
}

void PlotterNode::plot()
{
    if ( m_xAxisCombo->currentIndex()<0 || m_yAxisCombo->currentIndex()<0 )
        return;

    m_xAxisInput = getInputPortData0();
	m_yAxisInput = getInputPortData1();

    if( m_xAxisInput == NULL || !m_xAxisInput->isValid() ||
        m_yAxisInput == NULL || !m_yAxisInput->isValid())
        return;

    if(m_xAxisInput->GetRows() != m_yAxisInput->GetRows())
    {
        Logger::getInstance()->log("[Plotter Node] Data count mismatch, couldn't plot variables!", Logger::WARNING);
        return;
    }

    m_selectedMarkers.clear();
    m_plotArea->detachItems(QwtPlotItem::Rtti_PlotMarker, true);
    assignAxisValues();

    if(m_xAxisInput->getSpecimenGroupNumber() == 0 ) //Plot without group information
    {
        unsigned int n = m_xAxisInput->GetRows();

        for(unsigned int i=0; i<n; ++i)
        {
            double xVal = m_xAxisInput->get(i, m_xAxisCombo->currentIndex());
            double yVal = m_yAxisInput->get(i, m_yAxisCombo->currentIndex());

            QwtPlotMarker* mark = new QwtPlotMarker();
            mark->setLineStyle(QwtPlotMarker::NoLine);
            mark->setValue(xVal, yVal);
            mark->setLabel(QwtText(QString(m_showLabels ? "<b>"+m_xAxisInput->getRowLabel(i)+"</b>" : "")));
            mark->setLabelAlignment(Qt::AlignTop);
            mark->setSymbol(m_plotSymbol);
            mark->attach(m_plotArea);
        }
    }
    else
    {
        m_groupsTree->clear();
        m_markerItems.clear();
        m_allMarkers.clear();
        m_rowIndices.clear();

        QStringList groupsInTree;
        for(unsigned int i=0; i<m_xAxisInput->getSpecimenGroupNumber(); ++i)
        {
            QString groupName = m_xAxisInput->getSpecimenGroupName(i);
            if((m_xAxisInput->getSpecimenGroupNumber()>2) && groupName == "Ungrouped")
            {
                continue;
            }
            if(!groupsInTree.contains(m_xAxisInput->getSpecimenGroupName(i)))
            {
                //Fill groups tree
                SpecimenGroup* sg = m_xAxisInput->getSpecimenGroup(i);
                GroupTreeItem* groupItem = new GroupTreeItem(m_groupsTree);
                groupItem->setText(0, m_xAxisInput->getSpecimenGroupName(i));
                groupItem->setData(0, Qt::DecorationRole, sg->getGroupColor());
                groupItem->setText(1, QString().setNum(sg->getGroupLMKSize()));
                groupItem->setTextAlignment(1, Qt::AlignCenter);

                PlotSymbolCombo* groupPlotSym = new PlotSymbolCombo(m_groupsTree, groupItem);
                groupPlotSym->setCurrentIndex(sg->getGroupPlotSymbol());
                m_groupsTree->setItemWidget(groupItem, 2, groupPlotSym);
                connect(groupPlotSym, SIGNAL(symbolChanged(int,int,QTreeWidgetItem*)), m_groupsTree, SLOT(changeGroupPSymbol(int,int,QTreeWidgetItem*)));
                //

                for(unsigned int j=0; j<sg->getSize(); ++j)
                {
                    unsigned int rowIndex = sg->getMember(j)->getSpecimenIndex();
                    if(rowIndex <0 || rowIndex>=m_xAxisInput->GetRows())
                        continue;
                    double xVal = m_xAxisInput->get(rowIndex, m_xAxisCombo->currentIndex());
                    double yVal = m_yAxisInput->get(rowIndex, m_yAxisCombo->currentIndex());

                    QwtPlotMarker* mark = new QwtPlotMarker();
                    mark->setLineStyle(QwtPlotMarker::NoLine);
                    mark->setValue(xVal, yVal);
                    mark->setLabel(QwtText(QString(m_showLabels ? "<b>"+m_xAxisInput->getRowLabel(rowIndex)+"</b>" : "")));
                    mark->setLabelAlignment(Qt::AlignTop);
                    m_plotSymbol.setSize((int)(sg->getMemberViewerSize(j)*2.5));
                    QColor memberColor = sg->getMemberColor(j)==Qt::transparent? sg->getGroupColor():sg->getMemberColor(j);
                    m_plotSymbol.setBrush(QBrush(memberColor));
                    m_plotSymbol.setPen(QPen(memberColor));
                    m_plotSymbol.setStyle(getSymbolFromIndex(sg->getMemberPlotSymbol(j)));
                    mark->setSymbol(m_plotSymbol);
                    mark->attach(m_plotArea);

                    //Fill groups tree members
                    MemberTreeItem* memberItem = new MemberTreeItem(groupItem);
                    memberItem->setText(0, sg->getMemberID(j));
                    memberItem->setData(0, Qt::DecorationRole, memberColor);
                    memberItem->setText(1, QString().setNum(sg->getMemberViewerSize(j)));
                    memberItem->setTextAlignment(1, Qt::AlignCenter);
                    memberItem->setColor(sg->getMemberColor(j));
                    memberItem->setSpecimenIndex(rowIndex);
                    memberItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);

                    PlotSymbolCombo* memberPlotSym = new PlotSymbolCombo(m_groupsTree,memberItem);
                    memberPlotSym->setCurrentIndex(sg->getMemberPlotSymbol(j));
                    m_groupsTree->setItemWidget(memberItem, 2, memberPlotSym);
                    connect(memberPlotSym, SIGNAL(symbolChanged(int,int,QTreeWidgetItem*)), m_groupsTree, SLOT(changeGroupPSymbol(int,int,QTreeWidgetItem*)));
                    //
                    m_markerItems.insert(memberItem, mark);
                    m_allMarkers.push_back(mark);
                    m_rowIndices.push_back(rowIndex);
                }
                groupItem->setExpanded(true);
                groupsInTree.append(m_xAxisInput->getSpecimenGroupName(i));
            }
        }
        m_groupsTree->resizeColumnToContents(0);
        m_groupsTree->resizeColumnToContents(1);
        m_groupsTree->resizeColumnToContents(2);
        //m_groupsTree->adjustSize();
        m_groupsTree->show();
        checkLoadedGroups();
    }

    m_plotArea->setAxisTitle(QwtPlot::xBottom, m_xAxisCombo->currentText());
    m_plotArea->setAxisTitle(QwtPlot::yLeft, m_yAxisCombo->currentText());
    m_plotArea->replot();
}

void PlotterNode::updatePlot()
{
    m_selectedMarkers.clear();
    assignAxisValues();

    m_rightMark->detach();
    m_leftMark->detach();

    for(int i=0;i<m_allMarkers.size();i++)
    {
        m_allMarkers[i]->detach();
        int rowIndex = m_rowIndices[i];
        double xVal = m_xAxisInput->get(rowIndex, m_xAxisCombo->currentIndex());
        double yVal = m_yAxisInput->get(rowIndex, m_yAxisCombo->currentIndex());
        QwtPlotMarker* mark = m_allMarkers[i];
        mark->setValue(xVal, yVal);
        mark->attach(m_plotArea);
    }

    m_plotArea->setAxisTitle(QwtPlot::xBottom, m_xAxisCombo->currentText());
    m_plotArea->setAxisTitle(QwtPlot::yLeft, m_yAxisCombo->currentText());
    m_plotArea->replot();
}

void PlotterNode::axisComboChanged()
{
    updatePlot();
    calculateLoadingsOutput();
    emit dataReady();
}

void PlotterNode::markerInColor(QColor col)
{
    QColor result = m_plotSymbol.brush().color();
    result = col.isValid()? col : QColorDialog::getColor(result);
    if(result.isValid())
    {
        for (int i = 0; i<m_selectedMarkers.size(); ++i)
        {
            QwtSymbol oldSymbol = m_selectedMarkers[i]->symbol();
            if(m_cachedColor == oldSymbol.brush().color())
            {
                oldSymbol.setBrush(QBrush(result));
                m_selectedMarkers[i]->setSymbol(oldSymbol);
            }
        }
    }
    m_plotArea->replot();
}

void PlotterNode::setMarkersSymbol(int index)
{
    for (int i = 0; i<m_selectedMarkers.size(); ++i)
    {
        QwtSymbol oldSymbol = m_selectedMarkers[i]->symbol();
        if(m_cachedSymbol == oldSymbol.style())
        {
            oldSymbol.setStyle(getSymbolFromIndex(index));
            m_selectedMarkers[i]->setSymbol(oldSymbol);
        }
    }
    m_plotArea->replot();
}

void PlotterNode::setMarkersSize(float size)
{
    for (int i = 0; i<m_selectedMarkers.size(); ++i)
    {
        QwtSymbol oldSymbol = m_selectedMarkers[i]->symbol();
        if(m_cachedSize == oldSymbol.size().width())
        {
            oldSymbol.setSize((int)(size*2.5f));
            m_selectedMarkers[i]->setSymbol(oldSymbol);
        }
    }
    m_plotArea->replot();
}

void PlotterNode::clearSelectedMarkers()
{
    m_selectedMarkers.clear();
    for (QwtPlotItemIterator it=m_plotArea->itemList().begin(); it!=m_plotArea->itemList().end(); ++it )
    {
        if ( (*it)->rtti() == QwtPlotItem::Rtti_PlotMarker)
        {
            QwtPlotMarker* marker = (QwtPlotMarker*)(*it);
            QwtSymbol highlightSym = marker->symbol();
            highlightSym.setPen(QPen(highlightSym.brush().color()));
            marker->setSymbol(highlightSym);
        }
    }
}

void PlotterNode::selectMarkers()
{
    if(!m_groupsTree->selectedItems().count())
        return;
    //Clear Highlighted markes
    clearSelectedMarkers();

    QTreeWidgetItem* selected = m_groupsTree->selectedItems().front();
    m_cachedSize = (int)(selected->text(1).toFloat()*2.5f);
    m_cachedColor = selected->data(0, Qt::DecorationRole).value<QColor>();
    m_cachedSymbol = getSymbolFromIndex( dynamic_cast< PlotSymbolCombo* >( m_groupsTree->itemWidget(selected,2) )->currentIndex() );

    GroupTreeItem* groupItem = dynamic_cast<GroupTreeItem*>(selected);
    MemberTreeItem* memberItem = dynamic_cast<MemberTreeItem*>(selected);
    if(groupItem)
    {
        for(int i=0; i<groupItem->childCount(); ++i)
        {
            MemberTreeItem* mItem = (MemberTreeItem*)groupItem->child(i);
            highlightMember(mItem);
        }
    }
    else if(memberItem)
        highlightMember(memberItem);
    m_plotArea->replot();
}

void PlotterNode::highlightMember(MemberTreeItem* memberItem)
{
    if(m_markerItems.contains(memberItem))
    {
        QwtPlotMarker* marker = m_markerItems.value(memberItem);
        QwtSymbol highlightSym = marker->symbol();
        highlightSym.setPen(QPen((highlightSym.brush().color() == Qt::green) ? Qt::red : Qt::green));
        marker->setSymbol(highlightSym);
        m_selectedMarkers.push_back(marker);
    }
}

void PlotterNode::markersSelected(const QwtDoubleRect& rect)
{
//    QwtPlotItemIterator it;
//    QwtSymbol highlightSym = m_plotSymbol;
//
//    const QwtPlotItemList &itemList = m_plotArea->itemList();
//    m_selectedMarkers.clear();
//    for ( it = itemList.begin(); it != itemList.end(); ++it )
//    {
//        if ( (*it)->rtti() == QwtPlotItem::Rtti_PlotMarker)
//        {
//            QwtPlotMarker* marker = (QwtPlotMarker*)(*it);
//            QwtSymbol highlightSym = marker->symbol();
//            if(rect.contains(marker->value()))
//            {
//                highlightSym.setPen(QPen((highlightSym.brush().color() == Qt::green) ? Qt::red : Qt::green));
//                marker->setSymbol(highlightSym);
//                m_selectedMarkers.push_back(marker);
//            }
//            else
//            {
//                highlightSym.setPen(QPen(highlightSym.brush().color()));
//                marker->setSymbol(highlightSym);
//            }
//        }
//    }
//    m_plotArea->replot();
    if(rect.bottomRight() == rect.topLeft()) //Mouse click
    {
        fillClickOutput(rect.bottomRight());
        m_groupsTree->clearSelection();
        clearSelectedMarkers();
        m_plotArea->replot();
    }
}

void PlotterNode::bkgndColor()
{
    QColor result = m_plotArea->canvasBackground();
    result = m_cachedBGColor==Qt::transparent? QColorDialog::getColor(result):m_cachedBGColor;
    if(result.isValid())
    {
/*
        m_plotArea->setCanvasBackground(result);
        result.setRgb(255-result.red(), 255-result.green(), 255-result.blue());
        m_plotPicker->setTrackerPen(result);
        m_plotGrid->setMajPen(QPen(result, 0, Qt::DotLine));

        QPalette axisPalette;
        axisPalette.setColor(QPalette::WindowText, result);
        axisPalette.setColor(QPalette::Text, result);
        m_xAxisScale->setPalette(axisPalette);
        m_yAxisScale->setPalette(axisPalette);
*/

        QString r,g,b;
        r.setNum(result.red());
        g.setNum(result.green());
        b.setNum(result.blue());
        QString colorString = "background-color: rgb(" + r + ", " + g + ", " + b + ");";

        m_plotFrame->setStyleSheet(QString::fromUtf8(colorString.toStdString().c_str()));

        //result = (result.value()<127) ? result.lighter() : result.darker();
        //m_plotGrid->setMinPen(QPen(result, 0 , Qt::DotLine));
    }
    m_cachedBGColor = Qt::transparent;
    m_plotArea->replot();
}

void PlotterNode::toggleGrid(bool on)
{
    m_plotGrid->setVisible(on);
    m_plotArea->replot();
}

void PlotterNode::toggleLabels(bool on)
{
    m_showLabels = on;
    QwtPlotItemIterator it;
    const QwtPlotItemList &itemList = m_plotArea->itemList();
    unsigned int i=0;
    for (it = itemList.begin(); it != itemList.end(); ++it )
    {
        if ( (*it)->rtti() == QwtPlotItem::Rtti_PlotMarker)
        {
            QwtPlotMarker* marker = (QwtPlotMarker*)(*it);
            marker->setLabel(QwtText(QString(on ? "<b>"+m_xAxisInput->getRowLabel(i)+"</b>" : "")));
            ++i;
        }
    }
    m_plotArea->replot();
}

void PlotterNode::labelAlignAction(QAction* clickedItem)
{
    bool toggled = clickedItem->isChecked();
    Qt::Alignment alignment = Qt::AlignTop;
    if(toggled)
    {
        QMapIterator<QAction*, Qt::Alignment> i(m_markerAlignments);
        while (i.hasNext())
        {
            i.next();
            if( i.key() == clickedItem)
                alignment = i.value();
        }
    }

    QwtPlotItemIterator it;
    const QwtPlotItemList &itemList = m_plotArea->itemList();
    for (it = itemList.begin(); it != itemList.end(); ++it )
    {
        if ( (*it)->rtti() == QwtPlotItem::Rtti_PlotMarker)
        {
            QwtPlotMarker* marker = (QwtPlotMarker*)(*it);
            marker->setLabelAlignment(alignment);
        }
    }
    m_plotArea->replot();
}

void PlotterNode::labelOrientAction(QAction* clickedItem)
{
    bool toggled = clickedItem->isChecked();
    Qt::Orientation orientation = Qt::Horizontal;
    if(toggled)
    {
        QMapIterator<QAction*, Qt::Orientation> i(m_markerOrientations);
        while (i.hasNext())
        {
            i.next();
            if( i.key() == clickedItem)
                orientation = i.value();
        }
    }

    QwtPlotItemIterator it;
    const QwtPlotItemList &itemList = m_plotArea->itemList();
    for (it = itemList.begin(); it != itemList.end(); ++it )
    {
        if ( (*it)->rtti() == QwtPlotItem::Rtti_PlotMarker)
        {
            QwtPlotMarker* marker = (QwtPlotMarker*)(*it);
            marker->setLabelOrientation (orientation);
        }
    }
    m_plotArea->replot();
}

void PlotterNode::toggleCartesian(bool on)
{
    //Show/Hide Cartesian Axes
    m_plotArea->enableAxis(QwtPlot::xBottom, !on);
    m_plotArea->enableAxis(QwtPlot::yLeft, !on);
    m_xAxisScale->setVisible(on);
    m_yAxisScale->setVisible(on);

    m_plotArea->replot();
}

void PlotterNode::copyPlotToClipboard()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setPixmap(QPixmap::grabWidget(m_plotFrame));
}

void PlotterNode::scaleAxes()
{
    double xMin = m_plotArea->axisScaleDiv(QwtPlot::xBottom)->lowerBound() * m_axesScaleFactor;
    double xMax = m_plotArea->axisScaleDiv(QwtPlot::xBottom)->upperBound() * m_axesScaleFactor;
    double yMin = m_plotArea->axisScaleDiv(QwtPlot::yLeft)->lowerBound() * m_axesScaleFactor;
    double yMax = m_plotArea->axisScaleDiv(QwtPlot::yLeft)->upperBound() * m_axesScaleFactor;

    m_plotArea->setAxisScale(QwtPlot::xBottom, xMin, xMax);
    m_plotArea->setAxisScale(QwtPlot::yLeft, yMin, yMax);
    m_xAxisScale->setPosition(0);
    m_yAxisScale->setPosition(0);
    m_plotArea->updateAxes();
    if(m_plotRescaler->isEnabled())m_plotRescaler->rescale();
    m_plotArea->replot();
}

void PlotterNode::showAxesSettings()
{
    double xMin, xMax, yMin, yMax;
    getBounds(xMin, xMax, yMin, yMax);

    m_axesSettings->setMinX(xMin);
    m_axesSettings->setMaxX(xMax);
    m_axesSettings->setMinY(yMin);
    m_axesSettings->setMaxY(yMax);

    m_axesSettings->show();
}

void PlotterNode::setAxesLimits()
{
    double xMin, xMax, yMin, yMax;
    m_axesSettings->getBounds(xMin, xMax, yMin, yMax);

    m_plotArea->setAxisScale(QwtPlot::xBottom, xMin, xMax);
    m_plotArea->setAxisScale(QwtPlot::yLeft, yMin, yMax);
    m_xAxisScale->setPosition(0);
    m_yAxisScale->setPosition(0);
    m_plotArea->updateAxes();
    if(m_plotRescaler->isEnabled())m_plotRescaler->rescale();
    m_plotArea->replot();
}

void PlotterNode::fillClickOutput(const QwtDoublePoint& p)
{
    if (m_mouseCoordinates)
    {
        if (m_plotPicker->getMouseButton())
        {
            m_clickOutput->clear();
            m_clickOutput->addFloat(p.x());
            m_clickOutput->addFloat(p.y());
            setOutputPort1(m_clickOutput);
            lastPos=p;

            m_leftMark->detach();
            m_leftMark->setValue(p.x(), p.y());
            m_leftMark->attach(m_plotArea);
            m_plotArea->replot();
        }
        else
        {
            m_translatedClickOutput->clear();
            m_translatedClickOutput->addFloat(p.x());//-lastPos.x());
            m_translatedClickOutput->addFloat(p.y());//-lastPos.y());
            setOutputPort2(m_translatedClickOutput);

            m_rightMark->setValue(p.x(), p.y());
            m_rightMark->attach(m_plotArea);
            m_plotArea->replot();
        }
        emit dataReady();
    }
}

bool PlotterNode::exportPlot()
{
    QSettings settings("Evan");
    QStringList fileSettings = settings.value("lastDirectory").toStringList();
    QString fsLastDir = "";
    if( fileSettings.size() > 0 )
        fsLastDir = fileSettings[0];

    bool success = true;
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save plot"), fsLastDir,
                tr("Portable Network Graphics (*.png)\nJoint Photographic Experts Group (*.jpg)\nWindows Bitmap (*.bmp)\nScalable Vector Graphics (*.svg)\nPortable Document Format (*.pdf)\nPostScript (*.ps)"));

    if (fileName == "")
    {
        return false;
    }

    if(fileName.contains("/"))
    {
        QString dirName = fileName.left(fileName.lastIndexOf("/"));
        settings.setValue("lastDirectory",dirName);
    }
    else if(fileName.contains("\\"))
    {
        QString dirName = fileName.left(fileName.lastIndexOf("\\"));
        settings.setValue("lastDirectory",dirName);
    }

    QString type = fileName.right(3).toLower();

    if(type == "svg")
    {
        QSvgGenerator generator;
        generator.setFileName(fileName);

        QSize hint = m_plotArea->sizeHint();
        generator.setSize(m_plotArea->sizeHint());

        QPainter painter;
        painter.begin(&generator);
        m_plotArea->print(&painter,QRect(0, 0, hint.width(), hint.height()));
        painter.end();
    }
    else if (type == "pdf" || type == ".ps")
    {
        QPrinter pp;
        pp.setOutputFileName(fileName);
        if (type == ".ps")
        {
            pp.setOutputFormat(QPrinter::PostScriptFormat);
        }
        else
        {
            pp.setOutputFormat(QPrinter::PdfFormat);
        }

        QSize hint = m_plotArea->sizeHint();
        if(hint.width() > hint.height())
        {
            pp.setOrientation(QPrinter::Landscape);
        }
        else
        {
            pp.setOrientation(QPrinter::Portrait);
        }
        m_plotArea->print(pp);
    }
    else
    {
        QPixmap pix = QPixmap::grabWidget(m_plotFrame);
        success = pix.save(fileName, type.toAscii());
    }

    if (success)
    {
        QString msg = "[Plotter Node] Plot exported to file " + fileName;
        Logger::getInstance()->log(msg,Logger::INFO);
    }
    else
    {
        Logger::getInstance()->log("[Plotter Node] Failed to export plot",Logger::WARNING);
    }

    return success;
}

void PlotterNode::disableMouse()
{
    m_mouseCoordinates = !m_mouseCoordinates;
    QAction* mouse = viewMenu->findChild<QAction *>("mouseCoord");
    if (mouse)
    {
        mouse->setChecked(!m_mouseCoordinates);
    }
}

void PlotterNode::hideCoord()
{
    m_hideCoordinates = !m_hideCoordinates;
    if (m_hideCoordinates)
    {
        m_plotPicker->setTrackerMode(QwtPicker::AlwaysOff);
        m_leftMark->hide();
        m_rightMark->hide();
    }
    else
    {
        m_plotPicker->setTrackerMode(QwtPicker::AlwaysOn);
        m_leftMark->show();
        m_rightMark->show();
    }
    m_plotArea->replot();

    QAction* mouse = viewMenu->findChild<QAction *>("hideCoord");
    if (mouse)
    {
        mouse->setChecked(m_hideCoordinates);
    }
}

QString PlotterNode::toString() const
{
    QString result = "";

    result +=   QString().setNum(m_plotArea->canvasBackground().red())  +"|"+
                QString().setNum(m_plotArea->canvasBackground().green())+"|"+
                QString().setNum(m_plotArea->canvasBackground().blue())+"|";

    result +=   QString().setNum(m_plotGrid->isVisible())  +"|"+
                QString().setNum(m_xAxisScale->isVisible())  +"|"+
                QString().setNum(m_fitPlot)  +"|"+
                QString().setNum(m_showLabels)  +"|"+
                QString().setNum(m_hideCoordinates)  +"|"+
                QString().setNum(m_mouseCoordinates)  +"|"+
                QString().setNum(m_axesScaleFactor)  +"$";

    for(int i=0; i<m_groupsTree->topLevelItemCount(); ++i)
    {
        GroupTreeItem* groupItem = (GroupTreeItem*)m_groupsTree->topLevelItem(i);
        QColor groupColor = groupItem->data(0, Qt::DecorationRole).value<QColor>();
        result += groupItem->text(0)+":";
        result += groupItem->text(1)+":";
        result +=   QString().setNum(groupColor.red())  +"|"+
                    QString().setNum(groupColor.green())+"|"+
                    QString().setNum(groupColor.blue())+":";
        result += QString().setNum(((PlotSymbolCombo*)m_groupsTree->itemWidget(groupItem, 2))->currentIndex())+":";

        for(int j=0; j<groupItem->childCount(); ++j)
        {
            MemberTreeItem* groupMemberItem = ((MemberTreeItem*)groupItem->child(j));
            result +=   groupMemberItem->text(0) + "^";
            result +=   groupMemberItem->text(1) + "^";
            result +=   QString().setNum(groupMemberItem->getSpecimenIndex()) + "^";
            result +=   QString().setNum(groupMemberItem->getColor().red())  +"|"+
                        QString().setNum(groupMemberItem->getColor().green())+"|"+
                        QString().setNum(groupMemberItem->getColor().blue())+"|"+
                        QString().setNum(groupMemberItem->getColor().alpha())+"^";
            result += QString().setNum(((PlotSymbolCombo*)m_groupsTree->itemWidget(groupMemberItem, 2))->currentIndex())+"/";
        }

        result += ",";
    }
    return result;
}

void PlotterNode::fromString(const QString& params)
{
    QStringList plotterData = params.split("$");
    if(plotterData.count() != 2)
        return;
    QStringList plotterAttribs = plotterData[0].split("|");
    if(plotterAttribs.count() == 11)
    {
        m_cachedBGColor = QColor( plotterAttribs[0].toInt(),
                                plotterAttribs[1].toInt(),
                                plotterAttribs[2].toInt());
        QTimer::singleShot(200, this, SLOT(bkgndColor()));
        QAction* menuAction;

        toggleGrid(plotterAttribs[3].toInt());
        menuAction = viewMenu->findChild<QAction *>("GridAction");
        menuAction->setChecked(plotterAttribs[3].toInt());

        toggleCartesian(plotterAttribs[4].toInt());
        menuAction = viewMenu->findChild<QAction *>("CartesianAction");
        menuAction->setChecked(plotterAttribs[4].toInt());

        m_fitPlot = plotterAttribs[5].toInt();

        menuAction = viewMenu->findChild<QAction *>("FitAction");
        menuAction->setChecked(m_fitPlot);

        m_showLabels = plotterAttribs[6].toInt();
        menuAction = m_labelMenu->findChild<QAction *>("LabelAction");
        menuAction->setChecked(m_showLabels);

        m_hideCoordinates = !plotterAttribs[8].toInt();
        hideCoord();
        m_mouseCoordinates = !plotterAttribs[9].toInt();
        disableMouse();
        m_axesScaleFactor = plotterAttribs[10].toDouble();
    }

    QStringList groupsData = plotterData[1].split(",");
    foreach(QString groupData, groupsData)
    {
        QStringList groupParams = groupData.split(":");
        if(groupParams.count() != 5)
            continue;
        if(groupParams[4] == "" || groupParams[4].isEmpty())
            continue;
        QStringList groupColors = groupParams[2].split("|");
        if(groupColors.count() != 3)
            continue;

        GroupTreeItem* groupItem = new GroupTreeItem();
        groupItem->setText(0, groupParams[0]);
        groupItem->setText(1, groupParams[1]);
        groupItem->setTextAlignment(1, Qt::AlignCenter);
        QColor groupColor = QColor( groupColors[0].toInt(),
                                    groupColors[1].toInt(),
                                    groupColors[2].toInt());
        groupItem->setData(0, Qt::DecorationRole, groupColor);
        groupItem->setText(2, groupParams[3]);

        QStringList membersData = groupParams[4].split("/");
        foreach(QString memberData, membersData)
        {
            QStringList memberParams = memberData.split("^");
            if(memberParams.count() != 5)
                continue;
            QStringList memberColors = memberParams[3].split("|");
            if(memberColors.count() != 4)
                continue;

            MemberTreeItem* memberItem = new MemberTreeItem(groupItem);
            memberItem->setText(0, memberParams[0]);
            memberItem->setText(1, memberParams[1]);
            memberItem->setTextAlignment(1, Qt::AlignCenter);
            memberItem->setSpecimenIndex(memberParams[2].toInt());
            QColor memberColor = QColor(memberColors[0].toInt(),
                                         memberColors[1].toInt(),
                                         memberColors[2].toInt(),
                                         memberColors[3].toInt());
            memberItem->setData(0, Qt::DecorationRole, memberColor==Qt::transparent? groupColor: memberColor);
            memberItem->setColor(memberColor);

            memberItem->setText(2, memberParams[4]);
        }
        m_loadedGroupItems.push_back(groupItem);
    }
}

void PlotterNode::checkLoadedGroups()
{
    if(m_loadedGroupItems.count() ==0)
        return;

    for(int i=0; i<m_groupsTree->topLevelItemCount(); ++i)
    {
        if(i>= m_loadedGroupItems.count())
            break;
        GroupTreeItem* groupItem = (GroupTreeItem*)m_groupsTree->topLevelItem(i);
        if(groupItem->text(0) != m_loadedGroupItems[i]->text(0))
            continue;

        QColor groupColor = m_loadedGroupItems[i]->data(0, Qt::DecorationRole).value<QColor>();
        groupItem->setData(0, Qt::DecorationRole, groupColor);
        groupItem->setText(1, m_loadedGroupItems[i]->text(1));
        ((PlotSymbolCombo*)m_groupsTree->itemWidget(groupItem, 2))->setCurrentIndex(m_loadedGroupItems[i]->text(2).toInt());

        for(int j=0; j<groupItem->childCount(); ++j)
        {
            if(j>= m_loadedGroupItems[i]->childCount())
                break;
            MemberTreeItem* groupMemberItem = ((MemberTreeItem*)groupItem->child(j));
            MemberTreeItem* loadedMemberItem = ((MemberTreeItem*)m_loadedGroupItems[i]->child(j));
            if(groupMemberItem->text(0) != loadedMemberItem->text(0))
                continue;

            QColor memberColor = loadedMemberItem->getColor();
            groupMemberItem->setData(0, Qt::DecorationRole, memberColor==Qt::transparent? groupColor:memberColor);
            groupMemberItem->setText(1, loadedMemberItem->text(1));
            ((PlotSymbolCombo*)m_groupsTree->itemWidget(groupMemberItem, 2))->setCurrentIndex(loadedMemberItem->text(2).toInt());

            groupMemberItem->setColor(memberColor);
        }
    }

    refreshMarkers();
    clearLoadedGroups();
}

void PlotterNode::refreshMarkers()
{
    foreach(MemberTreeItem* markerItem, m_markerItems.keys())
    {
        QwtPlotMarker* marker = m_markerItems.value(markerItem);
        QwtSymbol oldSymbol = marker->symbol();
        oldSymbol.setSize((int)(markerItem->text(1).toDouble()*2.5f));
        oldSymbol.setBrush(QBrush(markerItem->data(0, Qt::DecorationRole).value<QColor>()));
        oldSymbol.setPen(QPen(markerItem->data(0, Qt::DecorationRole).value<QColor>()));
        oldSymbol.setStyle(getSymbolFromIndex(((PlotSymbolCombo*)m_groupsTree->itemWidget(markerItem, 2))->currentIndex()));
        marker->setSymbol(oldSymbol);
    }
    m_plotArea->replot();
}

void PlotterNode::showAxisFontsSettings()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, this );
    if ( ok )
    {
        QwtText xAxisText = m_plotArea->axisTitle(2);
        QwtText yAxisText = m_plotArea->axisTitle(0);
        xAxisText.setFont(font);
        yAxisText.setFont(font);
        m_plotArea->setAxisTitle(QwtPlot::xBottom, xAxisText);
        m_plotArea->setAxisTitle(QwtPlot::yLeft, yAxisText);

        m_xAxisScale->setFont(font);
        m_yAxisScale->setFont(font);
        m_plotArea->replot();
    }
}
void PlotterNode::showLabelFontsSettings()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, this );
    if ( ok )
    {
        m_showLabels = true;
        QwtPlotItemIterator it;
        const QwtPlotItemList &itemList = m_plotArea->itemList();
        unsigned int i=0;

        for (it = itemList.begin(); it != itemList.end(); ++it )
        {
            if ( (*it)->rtti() == QwtPlotItem::Rtti_PlotMarker)
            {
                QwtPlotMarker* marker = (QwtPlotMarker*)(*it);
                QwtText label = marker->label();
                label.setFont(font);
                marker->setLabel(label);
                ++i;
            }
        }
        m_plotArea->replot();
    }
}

void PlotterNode::changeXAxisLabel()
{
    bool ok;
    QString text = QInputDialog::getText( NULL, "X-axis label", "Enter value for X-axis label", QLineEdit::Normal, QString(), &ok);
    if (ok)
    {
        m_plotArea->setAxisTitle(QwtPlot::xBottom, text);
        m_plotArea->replot();
    }
}

void PlotterNode::changeYAxisLabel()
{
    bool ok;
    QString text = QInputDialog::getText( NULL, "Y-axis label", "Enter value for Y-axis label", QLineEdit::Normal, QString(), &ok);
    if (ok)
    {
        m_plotArea->setAxisTitle(QwtPlot::yLeft, text);
        m_plotArea->replot();
    }
}

void PlotterNode::changeTitleFont()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, this );
    if ( ok )
    {
        QwtText title = m_plotArea->title();
        title.setFont(font);
        m_plotArea->setTitle(title);
        m_plotArea->replot();
    }
}

void PlotterNode::changeTitleText()
{
    bool ok;
    QString title = QInputDialog::getText( NULL, "Set plot title", "Write a title for the plot", QLineEdit::Normal, QString(), &ok);
    if (ok)
    {
        m_plotArea->setTitle(title);
        m_plotArea->replot();
    }
}

void PlotterNode::changeTitleColor()
{
    QColor result;
    result = QColorDialog::getColor(result);
    if(result.isValid())
    {
        QwtText title = m_plotArea->title();
        title.setColor(result);
        m_plotArea->setTitle(title);
        m_plotArea->replot();
    }
}

void PlotterNode::changeLabel(QTreeWidgetItem* item, int num)
{
    MemberTreeItem* changedItem = ((MemberTreeItem*)item);
    if(m_selectedMarkers.count() > 0)
    {
        QwtPlotMarker* marker = m_selectedMarkers[0];
        QwtText oldLabel = marker->label();
        QwtText newLabel = QString("<b>"+changedItem->text(0)+"</b>");
        QFont font = oldLabel.font();
        newLabel.setFont(font);
        marker->setLabel(newLabel);
        m_plotArea->replot();
    }
}

void PlotterNode::changeXAxisColor()
{
    QwtScaleWidget *scale = (QwtScaleWidget *) m_plotArea->axisWidget(2);
    if (scale)
    {
        QColor result;
        result = QColorDialog::getColor(result);
        if(result.isValid())
        {
            QPalette pal = scale->palette();
            pal.setColor(QPalette::Foreground, result);
            scale->setPalette(pal);
            m_xAxisScale->setPalette(pal);
            m_plotArea->replot();
        }
    }
}

void PlotterNode::changeYAxisColor()
{
    QwtScaleWidget *scale = (QwtScaleWidget *) m_plotArea->axisWidget(0);
    if (scale)
    {
        QColor result;
        result = QColorDialog::getColor(result);
        if(result.isValid())
        {
            QPalette pal = scale->palette();
            pal.setColor(QPalette::Foreground, result);
            scale->setPalette(pal);
            m_yAxisScale->setPalette(pal);
            m_plotArea->replot();
        }
    }
}

void PlotterNode::changeAxisThickness()
{
    QwtScaleWidget *scaleY = (QwtScaleWidget *) m_plotArea->axisWidget(0);
    QwtScaleWidget *scaleX = (QwtScaleWidget *) m_plotArea->axisWidget(2);

    if (scaleY && scaleX)
    {
        int width = QInputDialog::getInteger(this, QString("Axis thickness"), QString("Set axis thickness"), scaleY->penWidth(), 1, 100);
        scaleY->setPenWidth(width);
        scaleX->setPenWidth(width);
    }
}

void PlotterNode::changeGridColor()
{
    QColor result;
    result = QColorDialog::getColor(result);
    if(result.isValid())
    {
        m_plotGrid->setPen(QPen(result, 0, Qt::DotLine));
        m_plotArea->replot();
    }
}

void PlotterNode::changeMargin()
{
    int margin = QInputDialog::getInteger(this, QString("Plot margin"), QString("Set plot margin"), m_plotArea->margin(), 0, 1000);
    m_plotArea->setMargin(margin);
}


void PlotterNode::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
    {
        clearSelectedMarkers();
        refreshMarkers();
    }
}


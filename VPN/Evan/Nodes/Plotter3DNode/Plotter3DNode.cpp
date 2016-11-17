// #include "Plotter3DNode.h"

// #include "../../DataTypes/PCScore.h"

// #include <QtSvg/QGraphicsSvgItem>
// #include <QtSvg/QSvgGenerator>
// #include <QtSvg/QSvgRenderer>
// #include <QtSvg/QSvgWidget>
// #include <QPrinter>
// #include <QSettings>

// Plotter3DNode::Plotter3DNode(QWidget* parent) : QMainWindow(parent),  INode()
// {
//     Logger::getInstance()->log( "Plotter3DNode::Plotter3DNode()" );

//     m_groupsColorTree = new GroupTreeWidget(this);
//     m_groupsColorTree->hide();

// 	m_xAxisInput = NULL;
// 	m_yAxisInput = NULL;
// 	m_zAxisInput = NULL;
//     m_majorlines = 5;

//     m_groupsTree = new GroupTreeWidget(this);
//     m_groupsTree->headerItem()->setText(0, QApplication::translate("Plotter3DNode", "Name", 0, QApplication::UnicodeUTF8));
//     m_groupsTree->headerItem()->setText(1, QApplication::translate("Plotter3DNode", "Size", 0, QApplication::UnicodeUTF8));
//     m_groupsTree->headerItem()->setText(2, QApplication::translate("Plotter3DNode", "Plot Symbol", 0, QApplication::UnicodeUTF8));
//     QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
//     m_groupsTree->setSizePolicy(sizePolicy);
//     m_groupsTree->hide();
//     connect(m_groupsTree, SIGNAL(itemSelectionChanged()), this, SLOT(selectMarkers()));
//     connect(m_groupsTree, SIGNAL(sizeChanged(float)), this, SLOT(setMarkersSize(float)));
//     connect(m_groupsTree, SIGNAL(colorChanged(QColor)), this, SLOT(markerInColor(QColor)));
//     //connect(m_groupsTree, SIGNAL(symbolChanged(int)), this, SLOT(setMarkersSymbol(int)));

// 	m_3dplotarea = new Qwt3D::SurfacePlot( this );

//     for (unsigned i=0; i!=m_3dplotarea->coordinates()->axes.size(); ++i)
//     {
//         m_3dplotarea->coordinates()->axes[i].setMajors(10);
//         m_3dplotarea->coordinates()->axes[i].setMinors(4);
//     }

//     int sides = /*Qwt3D::LEFT |*/ Qwt3D::RIGHT | /*Qwt3D::CEIL |*/ Qwt3D::FLOOR | Qwt3D::FRONT /*| Qwt3D::BACK*/;
//     m_3dplotarea->coordinates()->setGridLines(sides!=Qwt3D::NOSIDEGRID, false, sides);

//     m_3dplotarea->updateData();
//     m_3dplotarea->updateGL();

//     QVBoxLayout* verticalLayout = new QVBoxLayout(this);

//     QSplitter* page = new QSplitter( Qt::Horizontal );
//     page->addWidget( m_3dplotarea );
//     page->addWidget( m_groupsTree );
//     page->setStretchFactor(0,10);
//     page->setStretchFactor(1,1);

//     verticalLayout->addWidget( page );

//     //QLineEdit* titleLineEdit = new QLineEdit(this);
//     m_xAxisCombo = new QComboBox(this);
//     m_xAxisCombo->setSizeAdjustPolicy(QComboBox::AdjustToContents);
//     m_yAxisCombo = new QComboBox(this);
//     m_yAxisCombo->setSizeAdjustPolicy(QComboBox::AdjustToContents);
//     m_zAxisCombo = new QComboBox(this);
//     m_zAxisCombo->setSizeAdjustPolicy(QComboBox::AdjustToContents);

//     //QLabel* titleLabel = new QLabel("Plot Title:", this);
//     QLabel* xlabel = new QLabel("X Axis", this);
//     QLabel* ylabel = new QLabel("Y Axis", this);
//     QLabel* zlabel = new QLabel("Z Axis", this);

//     QHBoxLayout* horizontalLayout = new QHBoxLayout();
//     QSpacerItem* horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

//     //horizontalLayout->addWidget(titleLabel);
//     //horizontalLayout->addWidget(titleLineEdit);
//     horizontalLayout->addItem(horizontalSpacer);
//     horizontalLayout->addWidget(xlabel);
//     horizontalLayout->addWidget(m_xAxisCombo);
//     horizontalLayout->addWidget(ylabel);
//     horizontalLayout->addWidget(m_yAxisCombo);
//     horizontalLayout->addWidget(zlabel);
//     horizontalLayout->addWidget(m_zAxisCombo);

//     verticalLayout->addLayout(horizontalLayout);

//     ///////////////////////////////
//     // menu setup
//     // top menu item
//     QMenuBar* menubar = new QMenuBar();
//     viewMenu = new QMenu("Plot");
//     // background
//     QAction* bkgndAction = new QAction("Background Color", viewMenu);
//     connect(bkgndAction, SIGNAL(triggered()), this, SLOT(bkgndColor()));
//     // ortho view
//     QAction* orthoAction = new QAction("Ortho View", viewMenu);
//     orthoAction->setCheckable(true);
//     orthoAction->setChecked(true);
//     connect(orthoAction, SIGNAL(toggled(bool)), this, SLOT(toggleOrtho(bool)));
//     // title
//     QMenu* titleMenu = new QMenu("Title");
//     QAction* textAction = new QAction("Set Text", titleMenu);
//     titleMenu->addAction(textAction);
//     connect(textAction, SIGNAL(triggered()), this, SLOT(titleText()));
//     QAction* textColorAction = new QAction("Set Color", titleMenu);
//     titleMenu->addAction(textColorAction);
//     connect(textColorAction, SIGNAL(triggered()), this, SLOT(titleColor()));
//     QAction* textFontAction = new QAction("Set Font", titleMenu);
//     titleMenu->addAction(textFontAction);
//     connect(textFontAction, SIGNAL(triggered()), this, SLOT(titleFont()));
//     // labels
//     QMenu* labelMenu = new QMenu("Labels");
//     QAction* labelAction = new QAction("Show/Hide", labelMenu);
//     labelAction->setCheckable(true);
//     labelAction->setChecked(true);
//     labelMenu->addAction(labelAction);
//     connect(labelAction, SIGNAL(toggled(bool)), this, SLOT(toggleLabelShow(bool)));
//     QAction* labelColorAction = new QAction("Color", labelMenu);
//     labelMenu->addAction(labelColorAction);
//     connect(labelColorAction, SIGNAL(triggered()), this, SLOT(labelColor()));
//     // grids
//     QMenu* gridsMenu = new QMenu("Grids");
//     QAction* gridAction = new QAction("Show/Hide", gridsMenu);
//     gridAction->setCheckable(true);
//     gridAction->setChecked(false);
//     gridsMenu->addAction(gridAction);
//     connect(gridAction, SIGNAL(toggled(bool)), this, SLOT(toggleGridsShow(bool)));
//     QAction* linesAction = new QAction("Set grid lines", gridsMenu);
//     gridsMenu->addAction(linesAction);
//     connect(linesAction, SIGNAL(triggered()), this, SLOT(majorLineCount()));
//     gridsMenu->addSeparator();

//     gridLeftAction = new QAction("Show left grid", gridsMenu);
//     gridLeftAction->setCheckable(true);
//     gridLeftAction->setChecked(false);
//     gridsMenu->addAction(gridLeftAction);
//     connect(gridLeftAction, SIGNAL(toggled(bool)), this, SLOT(toggleLeftGridShow(bool)));

//     gridRightAction = new QAction("Show right grid", gridsMenu);
//     gridRightAction->setCheckable(true);
//     gridRightAction->setChecked(true);
//     gridsMenu->addAction(gridRightAction);
//     connect(gridRightAction, SIGNAL(toggled(bool)), this, SLOT(toggleRightGridShow(bool)));

//     gridTopAction = new QAction("Show top grid", gridsMenu);
//     gridTopAction->setCheckable(true);
//     gridTopAction->setChecked(false);
//     gridsMenu->addAction(gridTopAction);
//     connect(gridTopAction, SIGNAL(toggled(bool)), this, SLOT(toggleTopGridShow(bool)));

//     gridBottomAction = new QAction("Show bottom grid", gridsMenu);
//     gridBottomAction->setCheckable(true);
//     gridBottomAction->setChecked(true);
//     gridsMenu->addAction(gridBottomAction);
//     connect(gridBottomAction, SIGNAL(toggled(bool)), this, SLOT(toggleBottomGridShow(bool)));

//     gridFrontAction = new QAction("Show front grid", gridsMenu);
//     gridFrontAction->setCheckable(true);
//     gridFrontAction->setChecked(true);
//     gridsMenu->addAction(gridFrontAction);
//     connect(gridFrontAction, SIGNAL(toggled(bool)), this, SLOT(toggleFrontGridShow(bool)));

//     gridBackAction = new QAction("Show back grid", gridsMenu);
//     gridBackAction->setCheckable(true);
//     gridBackAction->setChecked(false);
//     gridsMenu->addAction(gridBackAction);
//     connect(gridBackAction, SIGNAL(toggled(bool)), this, SLOT(toggleBackGridShow(bool)));

//     // square axes
//     QAction* sqAction = new QAction("Square Axes", viewMenu);
//     sqAction->setCheckable(true);
//     sqAction->setChecked(false);
//     connect(sqAction, SIGNAL(toggled(bool)), this, SLOT(toggleSquareAxis(bool)));

//     // clip board
//     QAction* clipAction = new QAction("Copy to Clipboard", viewMenu);
//     connect(clipAction, SIGNAL(triggered()), this, SLOT(copyPlotToClipboard()));

//     // export
//     QAction* exportAction = new QAction("Export", viewMenu);
//     connect(exportAction, SIGNAL(triggered()), this, SLOT(exportPlot()));

//     // setup the final menu bar
//     menubar->addMenu(viewMenu);
//     viewMenu->addAction(bkgndAction);
//     viewMenu->addAction(orthoAction);
//     viewMenu->addSeparator();
//     viewMenu->addMenu(titleMenu);
//     viewMenu->addMenu(labelMenu);
//     viewMenu->addMenu(gridsMenu);
//     viewMenu->addAction(sqAction);
//     viewMenu->addSeparator();
//     viewMenu->addAction( clipAction );
//     viewMenu->addAction(exportAction);
//     setMenuBar(menubar);
//     //
//     ///////////////////////////////

//     QObject::connect(m_xAxisCombo, SIGNAL(activated(int)), this, SLOT(axisComboChanged()));
//     QObject::connect(m_yAxisCombo, SIGNAL(activated(int)), this, SLOT(axisComboChanged()));
//     QObject::connect(m_zAxisCombo, SIGNAL(activated(int)), this, SLOT(axisComboChanged()));

//     QWidget* centralWidget = new QWidget(this);
//     centralWidget->setLayout(verticalLayout);
//     setCentralWidget(centralWidget);
//     resize( QSize( 800, 600 ) );
//     setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
//     setWindowTitle("3D Plotter");

//     m_squareaxis = false;
// }

// Plotter3DNode::~Plotter3DNode()
// {
//     m_selectedMarkers.clear();
//     m_selectedMembers.clear();
//     m_selectedIndices.clear();

//     clearGroupsColorTree();
// }

// void Plotter3DNode::majorLineCount()
// {
//     bool ok;
//     int num = QInputDialog::getInteger( NULL, "Set major line count", "Specify number of major lines", m_majorlines, 1, 100, 1, &ok);
//     if (ok)
//     {
//         m_majorlines = num;
//         majorMinorLines();
//         m_3dplotarea->updateGL();
//     }
// }

// void Plotter3DNode::titleText()
// {
//     bool ok;
//     QString title = QInputDialog::getText( NULL, "Set plot title", "Write a title for the plot", QLineEdit::Normal, QString(), &ok);
//     if (ok)
//     {
//         m_3dplotarea->setTitle(title);
//         m_3dplotarea->updateGL();
//     }
// }

// void Plotter3DNode::titleColor()
// {
//     QColor result;
//     result = QColorDialog::getColor(result);
//     if(result.isValid())
//     {
//         m_3dplotarea->setTitleColor( Qwt3D::RGBA( result.redF(), result.greenF(), result.blueF() ) );
//         m_3dplotarea->updateGL();
//     }
// }

// void Plotter3DNode::titleFont()
// {
//     bool ok;
//     QFont font = QFontDialog::getFont(&ok, this );
//     if ( ok )
//     {
//         //m_3dplotarea->setTitleFont(title);
//         m_3dplotarea->updateGL();
//     }
// }

// void Plotter3DNode::toggleLabelShow( bool show )
// {
//     for( unsigned int i = 0; i < m_dots.size(); ++i )
//     {
//         Qwt3D::Enrichment* e = m_dots[i];
//         CustomDot* memberItem = dynamic_cast<CustomDot*>(e);
//         memberItem->renderLabel( show );
//     }

//     m_3dplotarea->updateData();
//     m_3dplotarea->updateGL();
// }

// void Plotter3DNode::labelColor()
// {
//     Logger::getInstance()->log( "Plotter3DNode::labelColor()" );

//     QColor result = QColorDialog::getColor( QColor( 0, 0, 0, 255 ) );
//     if(result.isValid())
//     {
//         for( unsigned int i = 0; i < m_dots.size(); ++i )
//         {
//             Qwt3D::Enrichment* e = m_dots[i];
//             CustomDot* memberItem = dynamic_cast<CustomDot*>(e);
//             memberItem->p.label.setColor( result.redF(), result.greenF(), result.blueF() );
//         }

//         m_3dplotarea->updateData();
//         m_3dplotarea->updateGL();
//     }
// }

// void Plotter3DNode::toggleGridsShow( bool show )
// {
//     int sides = Qwt3D::NOSIDEGRID;
//     if( show )
//         sides = Qwt3D::LEFT | Qwt3D::RIGHT | Qwt3D::CEIL | Qwt3D::FLOOR | Qwt3D::FRONT | Qwt3D::BACK;

//     gridLeftAction->setChecked(show);
//     gridRightAction->setChecked(show);
//     gridTopAction->setChecked(show);
//     gridBottomAction->setChecked(show);
//     gridFrontAction->setChecked(show);
//     gridBackAction->setChecked(show);

//     m_3dplotarea->coordinates()->setGridLines(sides!=Qwt3D::NOSIDEGRID, false, sides);
//     m_3dplotarea->updateGL();
// }

// void Plotter3DNode::toggleLeftGridShow( bool show )
// {
//     setGrid( Qwt3D::LEFT, show );
// }

// void Plotter3DNode::toggleRightGridShow( bool show )
// {
//     setGrid( Qwt3D::RIGHT, show );
// }

// void Plotter3DNode::toggleTopGridShow( bool show )
// {
//     setGrid( Qwt3D::CEIL, show );
// }

// void Plotter3DNode::toggleBottomGridShow( bool show )
// {
//     setGrid( Qwt3D::FLOOR, show );
// }

// void Plotter3DNode::toggleFrontGridShow( bool show )
// {
//     setGrid( Qwt3D::FRONT, show );
// }

// void Plotter3DNode::toggleBackGridShow( bool show )
// {
//     setGrid( Qwt3D::BACK, show );
// }

// void Plotter3DNode::setGrid( Qwt3D::SIDE side, bool show )
// {
//     if ( !m_3dplotarea )
//         return;

//     int sum = m_3dplotarea->coordinates()->grids();

//     if (show)
//         sum |= side;
//     else
//         sum &= ~side;

//     m_3dplotarea->coordinates()->setGridLines(sum!=Qwt3D::NOSIDEGRID, false, sum);
//     m_3dplotarea->updateGL();
// }

// void Plotter3DNode::highlightMember(MemberTreeItem* memberItem)
// {
//     if(m_markerItems.contains(memberItem))
//     {
//         Qwt3D::Enrichment* marker = m_markerItems.value(memberItem);
//         CustomDot* cd = dynamic_cast<CustomDot*>(marker);
//         cd->selected( true );
//         m_selectedMarkers.push_back(marker);
//         m_selectedMembers.push_back(memberItem);
//     }
// }

// void Plotter3DNode::clearSelectedMarkers()
// {
//     for( int i = 0; i < m_selectedMarkers.size(); ++i )
//     {
//         Qwt3D::Enrichment* marker = m_selectedMarkers[i];
//         CustomDot* cd = dynamic_cast<CustomDot*>(marker);
//         cd->selected( false );
//     }
//     m_selectedMarkers.clear();
//     m_selectedMembers.clear();
//     m_selectedIndices.clear();
// }

// void Plotter3DNode::selectMarkers()
// {
//     if( !m_groupsTree->selectedItems().count() )
//         return;
//     //Clear Highlighted markes
//     clearSelectedMarkers();

//     QTreeWidgetItem* selected = m_groupsTree->selectedItems().front();
//     m_cachedColor = selected->data(0, Qt::DecorationRole).value<QColor>();
//     //m_cachedSymbol = getSymbolFromIndex(((PlotSymbolCombo*)m_groupsTree->itemWidget(selected,2))->currentIndex());

//     GroupTreeItem* groupItem = dynamic_cast<GroupTreeItem*>(selected);
//     MemberTreeItem* memberItem = dynamic_cast<MemberTreeItem*>(selected);
//     if(groupItem)
//     {
//         int groupIdx = m_groupsTree->indexOfTopLevelItem( groupItem );

//         for(int i=0; i<groupItem->childCount(); ++i)
//         {
//             MemberTreeItem* mItem = dynamic_cast< MemberTreeItem* >( groupItem->child(i) );
//             highlightMember(mItem);

//             int childIdx = groupItem->indexOfChild( mItem );
//             m_selectedIndices.push_back( QPoint( groupIdx, childIdx ) );
//         }

//         m_selectedIndices.push_back( QPoint( groupIdx, -1 ) );
//         Logger::getInstance()->log( "Found group node!!!" );

//     }
//     else if(memberItem) {
//         highlightMember(memberItem);

//         int groupIdx = -1;
//         int childIdx = -1;
//         for( int i = 0; i < m_groupsTree->topLevelItemCount(); ++i ) {
//             QTreeWidgetItem *ti = m_groupsTree->topLevelItem( i );
//             for( int j = 0; j < ti->childCount(); ++j ) {
//                 if ( memberItem == ti->child( j ) ) {
//                     groupIdx = i;
//                     childIdx = j;
//                     Logger::getInstance()->log( "Found member item!!!" );
//                     // skip both loops
//                     i = m_groupsTree->topLevelItemCount();
//                     break;
//                 }
//             }
//         }
//         m_selectedIndices.push_back( QPoint( groupIdx, childIdx ) );
//     }

//     m_3dplotarea->updateData();
//     m_3dplotarea->updateGL();
// }


// void Plotter3DNode::markerInColor(QColor col)
// {
//     Logger::getInstance()->log( "Plotter3DNode::markerInColor()" );

//     if(col.isValid())
//     {
//         for (int i = 0; i<m_selectedMarkers.size(); ++i)
//         {
//             Logger::getInstance()->log( QString( "i=" ) + QString::number( i ) );

//             Qwt3D::Enrichment* selected = m_selectedMarkers[i];
//             CustomDot* memberItem = dynamic_cast<CustomDot*>(selected);
//             memberItem->p.color.r = col.redF();
//             memberItem->p.color.g = col.greenF();
//             memberItem->p.color.b = col.blueF();
//             memberItem->p.color.a = col.alphaF();

//             // markup in our duplicate tree which stores color permanently
//             {
//                 int groupIdx = m_selectedIndices[ i ].x();
//                 int childIdx = m_selectedIndices[ i ].y();
//                 Logger::getInstance()->log( QString( "current marker node = " ) +
//                                             QString::number( groupIdx ) +
//                                             QString( ", " ) +
//                                             QString::number( childIdx ) );

//                 if ( childIdx >= 0 )
//                 {
//                     MemberTreeItem* mItem = dynamic_cast< MemberTreeItem* >( m_groupsColorTree->topLevelItem( groupIdx )->child( childIdx ) );
//                     mItem->setColor( col );
//                 }
//             }


//         }

//         int diffElements = m_selectedIndices.size() - m_selectedMarkers.size();
//         while ( diffElements > 0 ) {
//                 int groupIdx = m_selectedIndices[ m_selectedMarkers.size() - 1 + diffElements ].x();
//             // specially encoded for group nodes!
//             Logger::getInstance()->log( "markerInColor for group node!!!" );
//             GroupTreeItem* groupColorItem = dynamic_cast< GroupTreeItem* >( m_groupsColorTree->topLevelItem( groupIdx ) );
//             groupColorItem->setData(0, Qt::DecorationRole, col );

//             --diffElements;
//         }
//     }
//     m_3dplotarea->updateData();
// }

// void Plotter3DNode::setMarkersSize( float size )
// {
//     if( size > 0.0f )
//     {
//         for (int i = 0; i<m_selectedMarkers.size(); ++i)
//         {
//             Qwt3D::Enrichment* selected = m_selectedMarkers[i];
//             CustomDot* memberItem = dynamic_cast<CustomDot*>(selected);
//             memberItem->pointSize( size * 2.5f );
//         }
//     }
//     m_3dplotarea->updateData();
// }

// void Plotter3DNode::bkgndColor()
// {
//     Qwt3D::RGBA rgba = m_3dplotarea->backgroundRGBAColor();
//     QColor result = QColorDialog::getColor( QColor( (int)(rgba.r * 255), (int)(rgba.g * 255), (int)(rgba.b * 255), (int)(rgba.a * 255) ) );
//     if(result.isValid())
//     {
//         Qwt3D::RGBA c;
//         result.getRgbF( &c.r, &c.g, &c.b );
//         m_3dplotarea->setBackgroundColor( c );

//         m_3dplotarea->coordinates()->setAxesColor( Qwt3D::RGBA( 1.0 - c.r, 1.0 - c.g, 1.0 - c.b ) );
//         m_3dplotarea->coordinates()->setNumberColor( Qwt3D::RGBA( 1.0 - c.r, 1.0 - c.g, 1.0 - c.b ) );
//         m_3dplotarea->coordinates()->setLabelColor( Qwt3D::RGBA( 1.0 - c.r, 1.0 - c.g, 1.0 - c.b ) );
//         m_3dplotarea->setTitleColor( Qwt3D::RGBA( 1.0 - c.r, 1.0 - c.g, 1.0 - c.b ) );
//     }
//     m_3dplotarea->updateGL();
// }

// void Plotter3DNode::getBounds( float &minx, float &maxx, float &miny, float &maxy, float &minz, float &maxz )
// {
//     Qwt3D::Enrichment* e = m_dots[0];
//     CustomDot* cd = dynamic_cast<CustomDot*>(e);
//     minx = maxx - cd->p.x;
//     miny = maxy - cd->p.y;
//     minz = maxz - cd->p.z;

//     for( unsigned int i = 1; i < m_dots.size(); ++i )
//     {
//         Qwt3D::Enrichment* e = m_dots[i];
//         CustomDot* cd = dynamic_cast<CustomDot*>(e);
//         if( cd->p.x < minx )
//             minx = cd->p.x;
//         if( cd->p.x > maxx )
//             maxx = cd->p.x;
//         if( cd->p.y < miny )
//             miny = cd->p.y;
//         if( cd->p.y > maxy )
//             maxy = cd->p.y;
//         if( cd->p.z < minz )
//             minz = cd->p.z;
//         if( cd->p.z > maxz )
//             maxz = cd->p.z;
//     }
// }

// void Plotter3DNode::getSquareAxisBounds( float &start, float &end )
// {
//     float minx = 0.0f, maxx = 0.0f, miny = 0.0f, maxy = 0.0f, minz = 0.0f, maxz = 0.0f;
//     getBounds( minx, maxx, miny, maxy, minz, maxz );

//     if( miny < minx )
//         minx = miny;
//     if( minz < minx )
//         minx = minz;

//     if( maxy > maxx )
//         maxx = maxy;
//     if( maxz > maxx )
//         maxx = maxz;

//     start = minx;
//     end = maxx;
// }

// void Plotter3DNode::toggleOrtho( bool ortho )
// {
//     m_3dplotarea->setOrtho( ortho );
// }

// void Plotter3DNode::toggleSquareAxis( bool square )
// {
//     m_squareaxis = square;
//     if( square )
//     {
//         float s = 0.0f, e = 0.0f;
//         getSquareAxisBounds( s, e );

//         Qwt3D::Triple ft = m_3dplotarea->coordinates()->first();
//         Qwt3D::Triple st = m_3dplotarea->coordinates()->second();

//         double min = ft.x, max = ft.x;
//         if( ft.y < min )
//             min = ft.y;
//         if( ft.z < min )
//             min = ft.z;
//         if( st.x < min )
//             min = st.x;
//         if( st.y < min )
//             min = st.x;
//         if( st.z < min )
//             min = st.z;

//         if( ft.y > max )
//             max = ft.y;
//         if( ft.z > max )
//             max = ft.z;
//         if( st.x > max )
//             max = st.x;
//         if( st.y > max )
//             max = st.x;
//         if( st.z > max )
//             max = st.z;

//         m_3dplotarea->coordinates()->setAutoScale( false );
//         m_3dplotarea->coordinates()->init( Qwt3D::Triple( min, min, min ), Qwt3D::Triple( max, max, max ) );
//         for( unsigned int i = 0; i < m_3dplotarea->coordinates()->axes.size(); ++i )
//         {
//             m_3dplotarea->coordinates()->axes[i].setLimits( s, e );
//             m_3dplotarea->coordinates()->axes[i].recalculateTics();
//         }
//         setAxisLabels();
//     }
//     else
//     {
//         m_3dplotarea->coordinates()->setAutoScale( true );
//         for( unsigned int i = 0; i < m_3dplotarea->coordinates()->axes.size(); ++i )
//         {
//             m_3dplotarea->coordinates()->axes[i].recalculateTics();
//         }
//         setAxisLabels();
//         plot();
//     }

//     m_3dplotarea->updateData();
//     m_3dplotarea->updateGL();
// }

// bool Plotter3DNode::exportPlot()
// {
//     QSettings settings("Evan");
//     QStringList fileSettings = settings.value("lastDirectory").toStringList();
//     QString fsLastDir = "";
//     if( fileSettings.size() > 0 )
//         fsLastDir = fileSettings[0];

//     bool success = true;
//     QString fileName = QFileDialog::getSaveFileName(this, tr("Save plot"), fsLastDir,
//                 tr("Portable Network Graphics (*.png)\nJoint Photographic Experts Group (*.jpg)\nWindows Bitmap (*.bmp)\nPortable Document Format (*.PDF)\nPostScript (*.PS)"));

//     if (fileName == "")
//     {
//         return false;
//     }

//     if(fileName.contains("/"))
//     {
//         QString dirName = fileName.left(fileName.lastIndexOf("/"));
//         settings.setValue("lastDirectory",dirName);
//     }
//     else if(fileName.contains("\\"))
//     {
//         QString dirName = fileName.left(fileName.lastIndexOf("\\"));
//         settings.setValue("lastDirectory",dirName);
//     }

//     QString type = fileName.right(3).toLower();

//     if( type == "pdf" )
//         type = "PDF";

//     if( type == ".ps" )
//         type = "EPS";

// //    Qwt3D::VectorWriter* handler = (Qwt3D::VectorWriter*)Qwt3D::IO::outputHandler("PDF");
// //    handler->setTextMode(Qwt3D::VectorWriter::TEX);
// //    handler = (Qwt3D::VectorWriter*)Qwt3D::IO::outputHandler("EPS");
// //    handler->setTextMode(Qwt3D::VectorWriter::TEX);

//     Qwt3D::VectorWriter* vw = (Qwt3D::VectorWriter*)Qwt3D::IO::outputHandler("PDF");
//     if (vw)
//         vw->setSortMode(Qwt3D::VectorWriter::BSPSORT);
//     success = Qwt3D::IO::save(m_3dplotarea, fileName.toLower(), type );

//     if (success)
//     {
//         QString msg = "[3D Plotter Node] Plot exported to file " + fileName;
//         Logger::getInstance()->log(msg,Logger::INFO);
//     }
//     else
//     {
//         Logger::getInstance()->log("[3D Plotter Node] Failed to export plot",Logger::WARNING);
//     }

//     return success;
// }

// void Plotter3DNode::copyPlotToClipboard()
// {
//     QClipboard *clipboard = QApplication::clipboard();
//     clipboard->setPixmap(QPixmap::grabWidget(m_3dplotarea));
// }

// void Plotter3DNode::clearPlot()
// {
//     if( !m_xAxisCombo || !m_3dplotarea )
//         return;

//     m_xAxisCombo->clear();
//     m_yAxisCombo->clear();
//     m_zAxisCombo->clear();

//     Qwt3D::TripleField tf;
//     Qwt3D::CellField cf;
//     Qwt3D::Cell c;
//     cf.push_back( c );
//     m_3dplotarea->loadFromData( tf, cf );
//     m_3dplotarea->updateData();
//     m_3dplotarea->updateGL();
// }

// void Plotter3DNode::process()
// {
//     m_processed = true;

//     m_xAxisInput = getInputPortData0();
//     m_yAxisInput = getInputPortData1();
//     m_zAxisInput = getInputPortData2();

//     if( m_xAxisInput == NULL || !m_xAxisInput->isValid() ||
//         m_yAxisInput == NULL || !m_yAxisInput->isValid() ||
//         m_zAxisInput == NULL || !m_zAxisInput->isValid())
//         return;

//     Logger::getInstance()->log("[3D Plotter Node] Processing started");

//     unsigned int xI = m_xAxisInput->GetRows();
//     unsigned int yI = m_yAxisInput->GetRows();
//     unsigned int zI = m_zAxisInput->GetRows();
//     if( xI != yI || xI != zI )
//     {
//         Logger::getInstance()->log("[3D Plotter Node] Input ports row count not the same");

//         clearPlot();
//     }
//     else
//     {
//         m_xAxisCombo->clear();
//         for( unsigned int i=0; i<m_xAxisInput->GetCols(); ++i )
//             m_xAxisCombo->addItem( m_xAxisInput->getColumnLabel( i ), i );
//         m_xAxisCombo->setCurrentIndex( 0 );

//         m_yAxisCombo->clear();
//         for( unsigned int i=0; i<m_yAxisInput->GetCols(); ++i )
//             m_yAxisCombo->addItem( m_yAxisInput->getColumnLabel( i ), i );

//         m_zAxisCombo->clear();
//         for( unsigned int i=0; i<m_zAxisInput->GetCols(); ++i )
//             m_zAxisCombo->addItem( m_zAxisInput->getColumnLabel( i ), i );

//         plot();
//     }

//     Logger::getInstance()->log("[3D Plotter Node] Processing finished");

// }

// bool Plotter3DNode::inputChanged( int port )
// {
//     if( m_processed )
//     {
//         plot();
//         return false;
//     }
//     else return false;
// }

// bool Plotter3DNode::plot()
// {
//     // Logger::getInstance()->log( "Plotter3DNode::plot()" );

//     m_xAxisInput = getInputPortData0();
//     m_yAxisInput = getInputPortData1();
//     m_zAxisInput = getInputPortData2();

//     if( m_xAxisInput == NULL || !m_xAxisInput->isValid() ||
//         m_yAxisInput == NULL || !m_yAxisInput->isValid() ||
//         m_zAxisInput == NULL || !m_zAxisInput->isValid())
//         return false;

//     unsigned int xI = m_xAxisInput->GetRows();
//     unsigned int yI = m_yAxisInput->GetRows();
//     unsigned int zI = m_zAxisInput->GetRows();
//     if( xI != yI || xI != zI )
//     {
//         Logger::getInstance()->log("[3D Plotter Node] Input ports row count not the same");

//         clearPlot();

//         return false;
//     }

//     for( unsigned int i = 0; i < m_dots.size(); ++i )
//     {
//         m_3dplotarea->degrade( m_dots[i] );
//     }
//     m_dots.clear();

//     Qwt3D::TripleField tf;

//     m_3dplotarea->setOrtho( true );
//     m_3dplotarea->setScale(1,1,1);
// 	m_3dplotarea->setPlotStyle( Qwt3D::USER );
//     m_3dplotarea->setCoordinateStyle( Qwt3D::FRAME );

//     m_labels.clear();

//     if(m_xAxisInput->getSpecimenGroupNumber() == 0) //Plot without group information
//     {
//         m_markerItems.clear();
//         m_selectedMarkers.clear();
//         m_selectedMembers.clear();
//         m_selectedIndices.clear();
//         m_groupsTree->clear();

//         //Fill groups tree
//         GroupTreeItem* groupItem = new GroupTreeItem(m_groupsTree);
//         groupItem->setText(0, "Untitled");
//         groupItem->setData(0, Qt::DecorationRole, QColor(0, 0, 0) );
//         groupItem->setText(1, QString().setNum(5) );
//         groupItem->setTextAlignment(1, Qt::AlignCenter);

//         PlotSymbolCombo* groupPlotSym = new PlotSymbolCombo(m_groupsTree, groupItem);
//         groupPlotSym->setCurrentIndex(0);
//         m_groupsTree->setItemWidget(groupItem, 2, groupPlotSym);

//         for(unsigned int i = 0; i < m_xAxisInput->GetRows(); i++ )
//         {

//             CustomTriple p;
//             p.x = m_xAxisInput->get( i, m_xAxisCombo->itemData( m_xAxisCombo->currentIndex() ).toUInt() );
//             p.y = m_yAxisInput->get( i, m_yAxisCombo->itemData( m_yAxisCombo->currentIndex() ).toUInt() );
//             p.z = m_zAxisInput->get( i, m_zAxisCombo->itemData( m_zAxisCombo->currentIndex() ).toUInt() );

//             p.color.r = 0.0; p.color.g = 0.0; p.color.b = 0.0; p.color.a = 1.0;

//             p.label.setPosition( Qwt3D::Triple( p.x, p.y, p.z ) );
//             p.label.setColor( 0, 0, 0 );
//             p.label.setString( m_xAxisInput->getRowLabel(i) );
//             m_labels.push_back( p.label );


//             CustomDot ct( 5, true );
//             ct.p = p;
//             Qwt3D::Enrichment* e = m_3dplotarea->addEnrichment( ct );
//             m_dots.push_back( e );
//             tf.push_back( p );


//             //Fill groups tree members
//             MemberTreeItem* memberItem = new MemberTreeItem(groupItem);
//             memberItem->setText(0, m_xAxisInput->getRowLabel(i) );
//             memberItem->setData(0, Qt::DecorationRole, QColor(0, 0, 0) );
//             memberItem->setText(1, QString().setNum(5) );
//             memberItem->setTextAlignment(1, Qt::AlignCenter);
//             memberItem->setColor(QColor(0, 0, 0));
//             memberItem->setSpecimenIndex(i);

//             PlotSymbolCombo* memberPlotSym = new PlotSymbolCombo(m_groupsTree,memberItem);
//             memberPlotSym->setCurrentIndex(0);
//             m_groupsTree->setItemWidget(memberItem, 2, memberPlotSym);

//             m_markerItems.insert(memberItem, e);
//         }
//         groupItem->setExpanded(true);

//         m_groupsTree->resizeColumnToContents(0);
//         m_groupsTree->resizeColumnToContents(1);
//         m_groupsTree->resizeColumnToContents(2);
//         m_groupsTree->show();
//     }
//     else
//     {
//         m_markerItems.clear();
//         m_selectedMarkers.clear();
//         m_selectedMembers.clear();
//         m_selectedIndices.clear();
//         m_groupsTree->clear();


//         bool ungroupedTreeLoaded = false;
//         for( unsigned int i = 0; i < m_xAxisInput->getSpecimenGroupNumber(); ++i )
//         {
//             QString groupName = m_xAxisInput->getSpecimenGroupName(i);
//             if(ungroupedTreeLoaded && groupName == "Ungrouped")
//             {
//                 continue;
//             }
//             else
//             {
//                 ungroupedTreeLoaded = true;
//             }

//             //Fill groups tree
//             SpecimenGroup* sg = m_xAxisInput->getSpecimenGroup(i);

//             bool resetThisGroupsColors = false;

//             GroupTreeItem* groupItem = new GroupTreeItem(m_groupsTree);
//             groupItem->setText(0, m_xAxisInput->getSpecimenGroupName(i));
//             groupItem->setData(0, Qt::DecorationRole, sg->getGroupColor());
//             groupItem->setText(1, QString().setNum(sg->getGroupLMKSize()));
//             groupItem->setTextAlignment(1, Qt::AlignCenter);

//             PlotSymbolCombo* groupPlotSym = new PlotSymbolCombo(m_groupsTree, groupItem);
//             groupPlotSym->setCurrentIndex(sg->getGroupPlotSymbol());
//             m_groupsTree->setItemWidget(groupItem, 2, groupPlotSym);

//                 if ( m_groupsColorTree->topLevelItemCount() <= i ) {
//                     GroupTreeItem* groupColorItem = new GroupTreeItem( );
//                     groupColorItem->setText(0, m_xAxisInput->getSpecimenGroupName(i));
//                     groupColorItem->setData(0, Qt::DecorationRole, sg->getGroupColor());
//                     groupColorItem->setText(1, QString().setNum(sg->getGroupLMKSize()));
//                     groupColorItem->setTextAlignment(1, Qt::AlignCenter);
//                     m_groupsColorTree->addTopLevelItem( groupColorItem );
//                     resetThisGroupsColors = true;
//                 } else {
//                     GroupTreeItem* groupColorItem = dynamic_cast< GroupTreeItem* >( m_groupsColorTree->topLevelItem( i ) );
//                     QColor groupColor = groupColorItem->data(0, Qt::DecorationRole).value<QColor>();
//                     groupItem->setData( 0, Qt::DecorationRole, groupColor );
// //                    Logger::getInstance()->log( QString( "Set group color! " ) +
// //                                               QString::number( groupColor.red() ) + QString( " " ) +
// //                                               QString::number( groupColor.green() ) + QString( " " ) +
// //                                               QString::number( groupColor.blue() ) + QString( " " ) );
//                 }

//             for( unsigned int j = 0; j < sg->getSize(); ++j )
//             {
//                 unsigned int rowIndex = sg->getMember(j)->getSpecimenIndex();
//                 if( ( static_cast<int>(rowIndex) < 0 ) || rowIndex>=m_xAxisInput->GetRows() )
//                     continue;
//                 double xVal = m_xAxisInput->get(rowIndex, m_xAxisCombo->currentIndex());
//                 double yVal = m_yAxisInput->get(rowIndex, m_yAxisCombo->currentIndex());
//                 double zVal = m_zAxisInput->get(rowIndex, m_zAxisCombo->currentIndex());

//                 //!!! QColor memberColor = sg->getMemberColor(j)==Qt::transparent? sg->getGroupColor():sg->getMemberColor(j);
//                 QColor memberColor;
//                 if ( resetThisGroupsColors ) {
//                     memberColor = sg->getMemberColor(j)==Qt::transparent? sg->getGroupColor():sg->getMemberColor(j);

//                     GroupTreeItem* groupColorItem = dynamic_cast< GroupTreeItem* >( m_groupsColorTree->topLevelItem( i ) );

//                     MemberTreeItem* memberColorItem = dynamic_cast< MemberTreeItem* >( groupColorItem->child( j ) );
//                     if ( memberColorItem != NULL ) {
//                         // assign member color
//                         memberColorItem->setData( 0, Qt::DecorationRole, sg->getMemberColor( j ) );
//                     } else {
//                         if ( groupColorItem->childCount() <= j ) {
//                             // filled the first time
//                             MemberTreeItem* newMemberItem = new MemberTreeItem( groupColorItem );
//                             newMemberItem->setData( 0, Qt::DecorationRole, sg->getMemberColor( j ) );
//                         } else {
//                             // assign group color
//                             groupColorItem->setData( 0, Qt::DecorationRole, sg->getGroupColor() );
//                         }
//                     }

//                 } else {
//                     GroupTreeItem* groupColorItem = dynamic_cast< GroupTreeItem* >( m_groupsColorTree->topLevelItem( i ) );
//                     QColor groupColor = groupColorItem->data(0, Qt::DecorationRole).value<QColor>();

//                     MemberTreeItem* groupMemberColorItem = dynamic_cast< MemberTreeItem* >( groupColorItem->child( j ) );

//                     memberColor = ( groupMemberColorItem->getColor() == Qt::transparent ) ?
//                                     groupColor :
//                                     groupMemberColorItem->getColor();
//                 }

//                 CustomDot ct( sg->getMemberViewerSize(j)*2.5, true );

//                 ct.p.x = xVal;
//                 ct.p.y = yVal;
//                 ct.p.z = zVal;

//                 // color of actually plotted data points in the plot area
//                 ct.p.color.r = memberColor.redF();
//                 ct.p.color.g = memberColor.greenF();
//                 ct.p.color.b = memberColor.blueF();
//                 ct.p.color.a = 1.0;

//                 ct.p.label.setPosition( Qwt3D::Triple( ct.p.x, ct.p.y, ct.p.z ) );

//                 // text/label color in the plot area, not the data-dot's color
//                 ct.p.label.setColor( 0.0, 0.0, 0.0 );

//                 ct.p.label.setString( m_xAxisInput->getRowLabel(rowIndex) );
//                 m_labels.push_back( ct.p.label );

//                 Qwt3D::Enrichment* e = m_3dplotarea->addEnrichment( ct );
//                 m_dots.push_back( e );
//                 tf.push_back( ct.p );

//                 //Fill groups tree members
//                 MemberTreeItem* memberItem = new MemberTreeItem(groupItem);
//                 memberItem->setText(0, sg->getMemberID(j));

//                 // color that is displayed in the group widget on the right
//                 memberItem->setData(0, Qt::DecorationRole, memberColor);

//                 memberItem->setText(1, QString().setNum(sg->getMemberViewerSize(j)));
//                 memberItem->setTextAlignment(1, Qt::AlignCenter);
//                 memberItem->setColor(sg->getMemberColor(j));
//                 memberItem->setSpecimenIndex(rowIndex);

//                 PlotSymbolCombo* memberPlotSym = new PlotSymbolCombo(m_groupsTree,memberItem);
//                 memberPlotSym->setCurrentIndex(sg->getMemberPlotSymbol(j));
//                 m_groupsTree->setItemWidget(memberItem, 2, memberPlotSym);

//                 m_markerItems.insert(memberItem, e);
//             }
//             groupItem->setExpanded(true);

//         }

//         m_groupsTree->resizeColumnToContents(0);
//         m_groupsTree->resizeColumnToContents(1);
//         m_groupsTree->resizeColumnToContents(2);
//         m_groupsTree->show();
//     }

//     Qwt3D::CellField cf;
//     Qwt3D::Cell c;
//     cf.push_back( c );

//     m_3dplotarea->loadFromData( tf, cf );

//     // set the data color to black
//     Qwt3D::ColorVector cv;
//     cv.push_back( Qwt3D::RGBA( 0, 0, 0 ) );
//     Qwt3D::StandardColor* sc = new Qwt3D::StandardColor( m_3dplotarea );
//     sc->setColorVector( cv );
//     m_3dplotarea->setDataColor( sc );

//     m_3dplotarea->assignMouse(Qt::LeftButton,
// 							Qwt3D::MouseState(Qt::LeftButton, Qt::ShiftModifier),
// 							Qt::LeftButton,
// 							Qt::NoButton, // Ignore the scale for now
// 							Qt::NoButton,
// 							Qt::NoButton,
// 							Qwt3D::MouseState(Qt::RightButton),
// 							Qwt3D::MouseState(Qt::MidButton),
// 							Qwt3D::MouseState(Qt::MidButton)
//               );

//     m_3dplotarea->assignKeyboard(   Qt::Key_Down, Qt::Key_Up,
//                                     Qwt3D::KeyboardState(Qt::Key_Right, Qt::ShiftModifier),
//                                     Qwt3D::KeyboardState(Qt::Key_Left, Qt::ShiftModifier),
//                                     Qt::Key_Right, Qt::Key_Left,
//                                     Qt::NoButton, // Ignore the scale for now
//                                     Qt::NoButton,
//                                     Qt::NoButton,
//                                     Qt::NoButton,
//                                     Qt::NoButton,
//                                     Qt::NoButton,
//                                     Qwt3D::KeyboardState(Qt::Key_Down, Qt::AltModifier|Qt::ControlModifier),
//                                     Qwt3D::KeyboardState(Qt::Key_Up, Qt::AltModifier|Qt::ControlModifier),
//                                     Qwt3D::KeyboardState(Qt::Key_Right, Qt::ControlModifier),
//                                     Qwt3D::KeyboardState(Qt::Key_Left, Qt::ControlModifier),
//                                     Qwt3D::KeyboardState(Qt::Key_Down, Qt::ControlModifier),
//                                     Qwt3D::KeyboardState(Qt::Key_Up, Qt::ControlModifier)
//                                 );

//     setAxisLabels();

//     if( m_squareaxis )
//         toggleSquareAxis( true );

//     majorMinorLines();

//     m_3dplotarea->updateData();
//     m_3dplotarea->updateGL();

//     return true;
// }

// void Plotter3DNode::majorMinorLines()
// {
//     for( unsigned int i = 0; i < m_3dplotarea->coordinates()->axes.size(); ++i )
//     {
//         m_3dplotarea->coordinates()->axes[i].setMajors( m_majorlines );
//     }
// }

// void Plotter3DNode::axisComboChanged()
// {
//     plot();
// }

// bool Plotter3DNode::isUpdated() const
// {
// 	//TODO: Code to determine whether the node parameters have been changed or not
// 	return false;
// }


// void Plotter3DNode::setAxisLabels()
// {
//     QString xstr = m_xAxisCombo->currentText();
//     QString ystr = m_yAxisCombo->currentText();
//     QString zstr = m_zAxisCombo->currentText();

//     m_3dplotarea->coordinates()->adjustNumbers( 10 );
//     m_3dplotarea->coordinates()->adjustLabels( 10 );

//     m_3dplotarea->coordinates()->axes[Qwt3D::X1].setLabelString(xstr);
//     m_3dplotarea->coordinates()->axes[Qwt3D::X2].setLabelString(xstr);
//     m_3dplotarea->coordinates()->axes[Qwt3D::X3].setLabelString(xstr);
//     m_3dplotarea->coordinates()->axes[Qwt3D::X4].setLabelString(xstr);

//     m_3dplotarea->coordinates()->axes[Qwt3D::Y1].setLabelString(ystr);
//     m_3dplotarea->coordinates()->axes[Qwt3D::Y2].setLabelString(ystr);
//     m_3dplotarea->coordinates()->axes[Qwt3D::Y3].setLabelString(ystr);
//     m_3dplotarea->coordinates()->axes[Qwt3D::Y4].setLabelString(ystr);

//     m_3dplotarea->coordinates()->axes[Qwt3D::Z1].setLabelString(zstr);
//     m_3dplotarea->coordinates()->axes[Qwt3D::Z2].setLabelString(zstr);
//     m_3dplotarea->coordinates()->axes[Qwt3D::Z3].setLabelString(zstr);
//     m_3dplotarea->coordinates()->axes[Qwt3D::Z4].setLabelString(zstr);
// }

// void Plotter3DNode::keyPressEvent(QKeyEvent *event)
// {
//     if (event->key() == Qt::Key_Escape)
//     {
//         clearSelectedMarkers();
//         refreshMarkers();
//     }
// }

// void Plotter3DNode::refreshMarkers()
// {
// }

// QString Plotter3DNode::toString() const
// {
//     //Logger::getInstance()->log( "Plotter3DNode::toString" );

//     QString result = "";

//     result +=   QString().setNum(m_3dplotarea->backgroundRGBAColor().r)  +"|"+
//                 QString().setNum(m_3dplotarea->backgroundRGBAColor().g)+"|"+
//                 QString().setNum(m_3dplotarea->backgroundRGBAColor().b) ;//+"|";
// /*
//     result +=   QString().setNum(m_plotGrid->isVisible())  +"|"+
//                 QString().setNum(m_xAxisScale->isVisible())  +"|"+
//                 QString().setNum(m_fitPlot)  +"|"+
//                 QString().setNum(m_showLabels)  +"|"+
//                 QString().setNum(m_hideCoordinates)  +"|"+
//                 QString().setNum(m_mouseCoordinates)  +"|"+
//                 QString().setNum(m_axesScaleFactor)  +"$";
//                 */
//     result +=  "$";

// Logger::getInstance()->log( QString( "m_groupsColorTree->topLevelItemCount() = " ) +
//                             QString::number( m_groupsColorTree->topLevelItemCount() ) );
// Logger::getInstance()->log( QString( "m_groupsTree->topLevelItemCount() = " ) +
//                             QString::number( m_groupsTree->topLevelItemCount() ) );

//     for(int i=0; i<m_groupsColorTree->topLevelItemCount(); ++i)
//     {
//         GroupTreeItem* groupColorItem = dynamic_cast< GroupTreeItem* >( m_groupsColorTree->topLevelItem(i) );
//         QColor groupColor = groupColorItem->data(0, Qt::DecorationRole).value<QColor>();
//         result += groupColorItem->text(0)+":";
//         result += groupColorItem->text(1)+":";
//         result +=   QString().setNum(groupColor.red())  +"|"+
//                     QString().setNum(groupColor.green())+"|"+
//                     QString().setNum(groupColor.blue())+":";
//         // item widgets have been added to the 'real' m_groupsTree only, not to the mirrored color-only m_groupsColorTree
//         // [ see Plotter3DNode::plot() ==> m_groupsTree->setItemWidget(groupItem, 2, groupPlotSym); ]
//         // so look up these values in the correct tree
//         GroupTreeItem* groupItem = dynamic_cast< GroupTreeItem* >( m_groupsTree->topLevelItem(i) );
//         PlotSymbolCombo *pCombo = dynamic_cast< PlotSymbolCombo* >( m_groupsTree->itemWidget( groupItem, 2 ) );
//         if ( pCombo == NULL ) {
// //                if ( m_groupsTree->itemWidget( groupItem, 2 ) == NULL ) {
// //                    Logger::getInstance()->log( "pCombo cast failed - not a type problem, but ptr itself is NULL!" );
// //                }
// //                Logger::getInstance()->log( "pCombo cast failed!" );
//             continue;
//         }
//         result += QString().setNum( pCombo->currentIndex() )+":";

//         for(int j=0; j<groupColorItem->childCount(); ++j)
//         {
//             MemberTreeItem* groupColorMemberItem = dynamic_cast< MemberTreeItem* >( groupColorItem->child(j) );
//             result +=   groupColorMemberItem->text(0) + "^";
//             result +=   groupColorMemberItem->text(1) + "^";
//             result +=   QString().setNum(groupColorMemberItem->getSpecimenIndex()) + "^";
//             result +=   QString().setNum(groupColorMemberItem->getColor().red())  +"|"+
//                         QString().setNum(groupColorMemberItem->getColor().green())+"|"+
//                         QString().setNum(groupColorMemberItem->getColor().blue())+"|"+
//                         QString().setNum(groupColorMemberItem->getColor().alpha())+"^";
// 			//result += QString().setNum( dynamic_cast< PlotSymbolCombo* >( m_groupsColorTree->itemWidget(groupMemberItem, 2) )->currentIndex() )+"/";
// 			MemberTreeItem* groupMemberItem = dynamic_cast< MemberTreeItem* >( groupItem->child(j) );
//             result += QString().setNum( dynamic_cast< PlotSymbolCombo* >( m_groupsTree->itemWidget(groupMemberItem, 2) )->currentIndex() )+"/";
//         }

//         result += ",";
//     }

//     return result;
// }

// void Plotter3DNode::fromString(const QString& params)
// {
//     Logger::getInstance()->log( "Plotter3DNode::fromString()" );

//     QStringList plotterData = params.split("$");

//     if ( plotterData.isEmpty() || ( plotterData.size() != 2 ) ) {
//         Logger::getInstance()->log( "one of many bailouts fromString" );
//         return;
//     }

//     QStringList plotterAttribs = plotterData[0].split("|");

//     if ( plotterAttribs.isEmpty() || ( plotterAttribs.size() != 3 ) ) {
//         return;
//     }


//     // plotterAttribs contains background color
//     double red = plotterAttribs[ 0 ].toDouble();
//     double green = plotterAttribs[ 1 ].toDouble();
//     double blue = plotterAttribs[ 2 ].toDouble();
//     Qwt3D::RGBA bgColor( red, green, blue );
//     m_3dplotarea->setBackgroundColor( bgColor );


//     /*
//     QStringList plotterData = params.split("$");
//     if(plotterData.count() != 2)
//         return;
//     QStringList plotterAttribs = plotterData[0].split("|");
//     if(plotterAttribs.count() == 11)
//     {
//         m_cachedBGColor = QColor( plotterAttribs[0].toInt(),
//                                 plotterAttribs[1].toInt(),
//                                 plotterAttribs[2].toInt());
//         QTimer::singleShot(200, this, SLOT(bkgndColor()));
//         QAction* menuAction;

//         toggleGrid(plotterAttribs[3].toInt());
//         menuAction = viewMenu->findChild<QAction *>("GridAction");
//         menuAction->setChecked(plotterAttribs[3].toInt());

//         toggleCartesian(plotterAttribs[4].toInt());
//         menuAction = viewMenu->findChild<QAction *>("CartesianAction");
//         menuAction->setChecked(plotterAttribs[4].toInt());

//         m_fitPlot = plotterAttribs[5].toInt();

//         menuAction = viewMenu->findChild<QAction *>("FitAction");
//         menuAction->setChecked(m_fitPlot);

//         m_showLabels = plotterAttribs[6].toInt();
//         menuAction = m_labelMenu->findChild<QAction *>("LabelAction");
//         menuAction->setChecked(m_showLabels);

//         m_hideCoordinates = !plotterAttribs[8].toInt();
//         hideCoord();
//         m_mouseCoordinates = !plotterAttribs[9].toInt();
//         disableMouse();
//         m_axesScaleFactor = plotterAttribs[10].toDouble();
//     }
// */
//     QStringList groupsData = plotterData[1].split(",");
//     foreach(QString groupData, groupsData)
//     {
//         QStringList groupParams = groupData.split(":");

//         if(groupParams.count() != 5)
//             continue;

//         if(groupParams[4] == "" || groupParams[4].isEmpty())
//             continue;

//         QStringList groupColors = groupParams[2].split("|");
//         if(groupColors.count() != 3)
//             continue;

//         GroupTreeItem* groupItem = new GroupTreeItem();
//         groupItem->setText(0, groupParams[0]);
//         groupItem->setText(1, groupParams[1]);
//         groupItem->setTextAlignment(1, Qt::AlignCenter);
//         QColor groupColor = QColor( groupColors[0].toInt(),
//                                     groupColors[1].toInt(),
//                                     groupColors[2].toInt());
//         groupItem->setData(0, Qt::DecorationRole, groupColor);
//         groupItem->setText(2, groupParams[3]);


//         // persistent color hack
//         GroupTreeItem* groupColorItem = new GroupTreeItem();
//         groupColorItem->setText(0, groupParams[0]);
//         groupColorItem->setText(1, groupParams[1]);
//         groupColorItem->setTextAlignment(1, Qt::AlignCenter);

//         groupColorItem->setData(0, Qt::DecorationRole, groupColor);
//         groupColorItem->setText(2, groupParams[3]);

//         m_groupsColorTree->addTopLevelItem( groupColorItem );



//         QStringList membersData = groupParams[4].split("/");

//         foreach(QString memberData, membersData)
//         {
//             QStringList memberParams = memberData.split("^");
//             if(memberParams.count() != 5)
//                 continue;
//             QStringList memberColors = memberParams[3].split("|");
//             if(memberColors.count() != 4)
//                 continue;

//             MemberTreeItem* memberItem = new MemberTreeItem(groupItem);
//             memberItem->setText(0, memberParams[0]);
//             memberItem->setText(1, memberParams[1]);
//             memberItem->setTextAlignment(1, Qt::AlignCenter);
//             memberItem->setSpecimenIndex(memberParams[2].toInt());
//             QColor memberColor = QColor( memberColors[0].toInt(),
//                                          memberColors[1].toInt(),
//                                          memberColors[2].toInt(),
//                                          memberColors[3].toInt() );
// //            if( memberColor == Qt::transparent ) {
// //                memberColor = groupColor;
// //            }
//             memberItem->setData(0, Qt::DecorationRole, memberColor==Qt::transparent? groupColor: memberColor);
//             memberItem->setColor(memberColor);
//             memberItem->setText(2, memberParams[4]);


//             // persistent color hack
//             MemberTreeItem* memberColorItem = new MemberTreeItem( groupColorItem );
//             memberColorItem->setText(0, memberParams[0]);
//             memberColorItem->setText(1, memberParams[1]);
//             memberColorItem->setTextAlignment(1, Qt::AlignCenter);
//             memberColorItem->setSpecimenIndex(memberParams[2].toInt());

//             memberColorItem->setData(0, Qt::DecorationRole, memberColor==Qt::transparent? groupColor: memberColor);
//             memberColorItem->setColor(memberColor);
//             memberColorItem->setText(2, memberParams[4]);

//             Logger::getInstance()->log( "member color = " + memberParams[3] );

//         }
// //        Logger::getInstance()->log( QString( "group item childcount = " ) +
// //                                    QString::number( groupItem->childCount() ) );

//     }

// }


// void Plotter3DNode::clearGroupsColorTree()
// {
//     m_groupsColorTree->clear();
//     delete m_groupsColorTree;
// }

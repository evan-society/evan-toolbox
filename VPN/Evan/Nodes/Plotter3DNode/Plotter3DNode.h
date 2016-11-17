// #ifndef PLOTTER3DNODE_H
// #define PLOTTER3DNODE_H

// #include "../INode.h"
// #include <QMainWindow>
// #include <QtGui>
// #include "../../DataTypes/Variables.h"
// #include "../../Utilities/Logger.h"

// #include "../GroupNode/GroupTreeWidget.h"
// #include "../GroupNode/PlotSymbolCombo.h"

// #include <qwt3d_surfaceplot.h>
// //#include <qwt3d_io_gl2ps.h>

// class GroupTreeWidget;

// struct CustomTriple : public Qwt3D::Triple
// {
// public:
//     CustomTriple(double xv = 0,double yv = 0,double zv = 0) : Qwt3D::Triple( xv, yv, zv )
//     {
//     }

//     Qwt3D::RGBA color;
//     Qwt3D::Label label;
// };

// // The Point Style
// class CustomDot : public Qwt3D::VertexEnrichment
// {
//     GLint setDevicePointSize(GLfloat val)
//     {
//         if (val<0)
//             val=0;

//         //GLint ret = gl2psPointSize(val);

//         //glPassThrough(GL_POINT_SIZE);//GL2PS_POINT_SIZE_TOKEN);
//         //glPassThrough(val);

//         GLfloat lw[2];
//         glGetFloatv(GL_POINT_SIZE_RANGE, lw);

//         if (val < lw[0])
//             val = lw[0];
//         else if (val > lw[1])
//             val = lw[1];

//         glPointSize(val);
//         return 1;
//     }

// public:
//     CustomDot() { configure(1, false); renderLabel_ = true; selected_ = false; }
//     CustomDot(double pointsize, bool smooth) { configure(pointsize, smooth); renderLabel_ = true; selected_ = false; }

//     Qwt3D::Enrichment* clone() const { return new CustomDot(*this); }

//     void configure(double pointsize, bool smooth) { plot = 0; pointsize_ = pointsize; smooth_ = smooth; }
//     void drawBegin()
//     {
//         setDevicePointSize( pointsize_ );
//         oldstate_ = glIsEnabled(GL_POINT_SMOOTH);
//         if (smooth_)
//             glEnable(GL_POINT_SMOOTH);
//         else
//             glDisable(GL_POINT_SMOOTH);
//         glBegin( GL_POINTS );
//     }

//     void drawEnd()
//     {
//         glEnd();
//         if (oldstate_)
//             glEnable(GL_POINT_SMOOTH);
//         else
//             glDisable(GL_POINT_SMOOTH);

//         if( renderLabel_ )
//             p.label.draw();
//     }

//     void draw( Qwt3D::Triple const& pp)
//     {
//         CustomTriple &pos = p;
//         if( selected_ )
//             glColor4d( 0.0, 1.0, 0.0, 1.0 );
//         else
//             glColor4d( pos.color.r, pos.color.g, pos.color.b, pos.color.a );
//         glVertex3d( pos.x, pos.y, pos.z );
//     }

//     void renderLabel( bool render ) { renderLabel_ = render; }
//     bool renderLabel() { return renderLabel_; }

//     void pointSize( float size ) { pointsize_ = size; }
//     float pointSize() { return pointsize_; }

//     void selected( bool select ) { selected_ = select; }
//     bool selected() { return selected_; }

//     CustomTriple p;

// private:
//     bool renderLabel_;
//     bool smooth_;
//     bool selected_;
//     double pointsize_;
//     GLboolean oldstate_;
// };

// class Plotter3DNode : public QMainWindow , public INode
// {
// Q_OBJECT
// private:
// 	Variables* m_xAxisInput;
// 	Variables* m_yAxisInput;
// 	Variables* m_zAxisInput;
// 	bool m_squareaxis;
//     bool m_processed;
//     int m_majorlines;

//     QFrame* m_plotFrame;
//     GroupTreeWidget* m_groupsTree;
//     GroupTreeWidget* m_groupsColorTree;
// 	Qwt3D::SurfacePlot* m_3dplotarea;
//     QComboBox* m_xAxisCombo;
//     QComboBox* m_yAxisCombo;
//     QComboBox* m_zAxisCombo;
//     QColor m_cachedColor;

//     QMenu* viewMenu;

//     QAction* gridLeftAction;
//     QAction* gridRightAction;
//     QAction* gridTopAction;
//     QAction* gridBottomAction;
//     QAction* gridFrontAction;
//     QAction* gridBackAction;

//     std::vector< Qwt3D::Label > m_labels;
//     std::vector< Qwt3D::Enrichment* > m_dots;
//     QMap<MemberTreeItem*, Qwt3D::Enrichment*> m_markerItems;
//     QVector<Qwt3D::Enrichment*> m_selectedMarkers;

//     QVector< MemberTreeItem* > m_selectedMembers;
//     QVector< QPoint > m_selectedIndices;
//     void clearGroupsColorTree();


//     void clearPlot();
//     void clearSelectedMarkers();
//     void highlightMember(MemberTreeItem* memberItem);
//     void setGrid(Qwt3D::SIDE side, bool show);
//     void getBounds( float &minx, float &maxx, float &miny, float &maxy, float &minz, float &maxz );
//     void getSquareAxisBounds( float &start, float &end );
//     void majorMinorLines();
//     void setAxisLabels();
//     void refreshMarkers();

// private slots:
//     void axisComboChanged();
//     bool plot();
//     void bkgndColor();
//     void selectMarkers();
//     void markerInColor(QColor col);
//     void toggleLabelShow( bool show );
//     void toggleGridsShow( bool show );
//     void toggleLeftGridShow( bool show );
//     void toggleRightGridShow( bool show );
//     void toggleTopGridShow( bool show );
//     void toggleBottomGridShow( bool show );
//     void toggleFrontGridShow( bool show );
//     void toggleBackGridShow( bool show );
//     void labelColor();
//     void setMarkersSize( float size );
//     void toggleSquareAxis( bool square );
//     void toggleOrtho( bool ortho );
//     bool exportPlot();
//     void copyPlotToClipboard();
//     void titleText();
//     void titleColor();
//     void titleFont();
//     void majorLineCount();
//     void keyPressEvent(QKeyEvent *event);


// protected:
// 	Variables* getInputPortData0()const { return dynamic_cast<Variables *>( getInputData( 0 ) ); }
// 	Variables* getInputPortData1()const { return dynamic_cast<Variables *>( getInputData( 1 ) ); }
// 	Variables* getInputPortData2()const { return dynamic_cast<Variables *>( getInputData( 2 ) ); }
// 	bool inputChanged(int);

// public:
// 	Plotter3DNode(QWidget* parent=0);
// 	~Plotter3DNode();
// 	void process();
// 	void showGUI()
//     {
//         QWidget::activateWindow();
//         QWidget::show();
//     }
//     const QObject* getDialog() const {return this;}
// 	bool isUpdated() const;

// 	QString toString() const;
// 	void fromString(const QString& params);

// signals:
// 	void dataReady();
// };
// #endif

#ifndef WARPGRID_H_INCLUDED
#define WARPGRID_H_INCLUDED

#include "Surface.h"
#include "LandmarkSet.h"
#include "FloatVector.h"
#include "Warpings.h"
#include <tps.h>
#include <QObject>
#include <QTimer>
#include <QMap>
#include <osg/Plane>
#include <osg/ClipPlane>

#include <osg/MatrixTransform>
class GroupWarpGrids;
class WarpGrid;
class MyClipPlane;

class AbstractWarpGrid : public QObject, public Surface
{
protected:
    osg::Matrixd m_previousMatrixTransform;
    osg::Matrixd m_firstSelection;
    GroupWarpGrids* parent;
    osg::Matrixd m_startPosition;
    osg::Matrixd m_endPosition;
    osg::Quat m_startPositionRot;
    osg::Quat m_endPositionRot;
    osg::Vec3d m_startPositionTrans;
    osg::Vec3d m_endPositionTrans;
    osg::Vec3d m_startPositionScale;
    osg::Vec3d m_endPositionScale;

public:
    AbstractWarpGrid():QObject(), Surface()
    {
        m_startPosition=osg::Matrixd::identity();
        m_endPosition=osg::Matrixd::identity();
        addType(GRID_T);
        parent=NULL;
    }

    void update() {updateGrid();}

    osg::Matrixd computeWorldtoLocalMatrix();

    void setParent(GroupWarpGrids* p) {parent=p;}
    GroupWarpGrids* getParent() const {return parent;}

    void resetTransform()  {m_osgTransform->setMatrix(m_firstSelection);}
    void setGridTransform(const osg::Matrixd& mat)
    {
         m_firstSelection=m_previousMatrixTransform=mat;
         getOsgTransform()->setMatrix(mat);
    }

    virtual bool initializeGrid(const QString& n)=0;

    MyClipPlane* asClipPlane();
    WarpGrid* asWarpGrid();
    virtual void updateGrid() = 0;
    //animation
    void setStartPosition();
    void setEndPosition();
    bool hasToBeAnimated() {return m_startPosition==m_endPosition;}
    void setInterpolatedTransfMatrix(float i);
};

////////////////////////////////////////////////////////////////
class MyClipPlane : public AbstractWarpGrid
{
    Q_OBJECT
private:
    int m_clipPlaneIndex;
    osg::ClipPlane* m_clipPlane;
    osg::Plane* m_plane;
    float m_size;
    bool m_isFlipped;
    QTimer m_timer;

public:
    MyClipPlane();

    void setSize(float size)    {m_size=size;}

    void setClipPlaneIndex(int index);
    int getClipPlaneIndex() const{return m_clipPlaneIndex;}

    void setClipPlane(osg::ClipPlane* clip) {m_clipPlane=clip;}
    osg::Plane* getPlane()            const {return m_plane;}
    void setPlane(osg::Plane* plane)        {m_plane=plane;}
    osg::ClipPlane* getClipPlane()    const {return m_clipPlane;}

    void flip();
    bool isFlipped() const{return m_isFlipped;}
    void setFlipped(bool b) {m_isFlipped=b;}

    bool initializeGrid(const QString& n);
    void toBeDeleted(){emit deleted();}

public slots:
    void updateGrid();

signals:
    void deleted();
};

////////////////////////////////////////////////////
class WarpGrid : public AbstractWarpGrid
{
    Q_OBJECT
private:
    bool m_useShader;
    Matrix<double>* m_originalVertices;
    LandmarkSet* m_referenceLMarks;
    LandmarkSet* m_warpedLMarks;
    QVector<osg::ref_ptr<osg::Vec3Array> > m_warpVector;
    QList<QString> m_coefArray;

    unsigned int m_rows;
    unsigned int m_cols;
    float m_size;

    QStringList m_bindNames;
    QList<unsigned int> m_bindLocations;
    MyClipPlane* m_clipPlane;

public:
    WarpGrid();
    ~WarpGrid();

    MyClipPlane* makeBestFitPlane();

    void applyShader();
    void removeShader();
    void setUsedShader(bool b);

    void setRowsAndCols(int row,int cols) {m_rows=row;m_cols=cols;}
    int getRows() const {return m_rows;}
    int getCols() const {return m_cols;}
    void setSize(float ratio){m_size=ratio;}
    float getSize() const {return m_size;}

    Matrix<double>* getOriginalVertices() const {return m_originalVertices;}

    void setOriginalVertices()                  {m_originalVertices=new Matrix<double>(*(getVertices()));}
    void setReferenceLMarks(LandmarkSet* ref)   {m_referenceLMarks=ref;}
    void setWarpedLMarks(LandmarkSet* ref)      {m_warpedLMarks=ref;}


    LandmarkSet* getReferenceLMarks() const {return m_referenceLMarks;}
    LandmarkSet* getWarpedLMarks() const {return m_warpedLMarks;}

    bool initializeGrid(const QString&);
    bool reInitializeGrid();
    void generateShape();

    void updateGrid();
    QString toString() const;
    static WarpGrid* loadFromString(const QString& params) ;

public slots:
    void removeClipPlane();
};

////////////////////////////////////////
class GroupWarpGrids : public QObject, public IRenderable
{
    Q_OBJECT
private:
    QList<WarpGrid*> m_listGrid;
    QList<GroupWarpGrids*> m_listGroupGrid;
    bool m_useShader;
    QVector<TPS*> m_tps;
//    osg::Matrixd m_previousMatrixTransform;
    osg::Matrixd m_parentMatrixTransform;
    bool m_isMainGroup;
    ////Animation///
    osg::Matrixd m_startPosition;
    osg::Matrixd m_endPosition;
    osg::Quat m_startPositionRot;
    osg::Quat m_endPositionRot;
    osg::Vec3d m_startPositionTrans;
    osg::Vec3d m_endPositionTrans;
    osg::Vec3d m_startPositionScale;
    osg::Vec3d m_endPositionScale;
    ///////
protected:
    QTimer m_timer;

public:
    GroupWarpGrids(): QObject(), IRenderable(),
        m_useShader(false)
    {

        m_isMainGroup=false;
//        m_previousMatrixTransform=osg::Matrixd::identity();
        m_parentMatrixTransform=osg::Matrixd::identity();
        //connect(&m_timer, SIGNAL(timeout()), this, SLOT(update()));
    }

    ~GroupWarpGrids()
    {if(m_timer.isActive()) m_timer.stop();}

    void addGrid(WarpGrid* wgrid);
    void addGroupGrid(GroupWarpGrids* groupWarpGrid);

    WarpGrid*       getGrid(int i)      const {return m_listGrid[i];}
    GroupWarpGrids* getGroupGrid(int i) const {return m_listGroupGrid[i];}

    int getNumGrid()        const {return m_listGrid.size();}
    int getNumGroupGrid()   const {return m_listGroupGrid.size();}

    IRenderable* getChild(unsigned int i) const;
    unsigned int getChildrenNum()         const{return 10;} // 10 grids maximum

    void clear(){};

    void setTps(QVector<TPS*> tps);
    QVector<TPS*> getTps()           {return m_tps;}

    bool isValid()           const   {return true;}
    bool initialize(const QString&);
    void updateGrids();

    void deleteGrid(WarpGrid* warpg);
    void deleteGrid(int i);
    void deleteGroupGrid(GroupWarpGrids* groupWarpg);
    void removeGrid(WarpGrid* warpg) {warpg->removeShader();m_listGrid.removeAll(warpg);initialize(getRenderableName());}

    void setParentMatrixTransform(osg::Matrixd mat) {m_parentMatrixTransform=mat;}
    osg::Matrixd getParentMatrixTransform() const   {return m_parentMatrixTransform*getOsgTransform()->getMatrix();}

    bool isShaderUsed() {return m_useShader;}
    void setUsedShader(bool b);

    void setMainGroup(){m_timer.start(50);connect(&m_timer, SIGNAL(timeout()), this, SLOT(update()));m_isMainGroup=true;}
    bool isMainGroup() const {return m_isMainGroup;}

    QString toString() const;
    static GroupWarpGrids* loadFromString(const QString& params, QVector<TPS*> tps);

    ////Animation///
    void setStartPosition();
    void setEndPosition();
    bool hasToBeAnimated() {return m_startPosition==m_endPosition;}
    void setInterpolatedTransfMatrix(float i);
    void stopTimer() {m_timer.stop();}
    void startTimer() {m_timer.start(50);}
    ///////

public slots:
    void update();
};
#endif // WARPGRID_H_INCLUDED

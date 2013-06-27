#include "WarpGrid.h"
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>
#include "../Utilities/Logger.h"
#include <osg/LineWidth>
#include <Matrix.h>
#include <osgManipulator/Dragger>
#include <osgManipulator/CommandManager>
#include <osg/Switch>
#include <osg/Plane>
#include <pca.h>
#include "Loadings.h"
#include <osgUtil/Optimizer>
extern ofstream dbgfile;// debug

//AbstractwarpGrid

MyClipPlane* AbstractWarpGrid::asClipPlane()
{
    return dynamic_cast<MyClipPlane*>(this);
}

WarpGrid* AbstractWarpGrid::asWarpGrid(){
    return dynamic_cast<WarpGrid*>(this);
}

osg::Matrixd AbstractWarpGrid::computeWorldtoLocalMatrix()
{
    if (!parent || !parent->getOsgTransform() || !parent->getOsgTransform().valid())
        return osg::Matrixd::identity();
    osg::Matrixd mat = getOsgTransform()->getMatrix();
    mat *= parent->getParentMatrixTransform();
    return mat;
}

void AbstractWarpGrid::setInterpolatedTransfMatrix(float i)
{
    if (m_startPosition==m_endPosition)
        return;

    osg::Quat interpolatedRot;
    osg::Vec3d interpolatedScale;
    osg::Vec3d interpolatedTrans;
    interpolatedRot.slerp(i,m_startPositionRot,m_endPositionRot);

    osg::Vec3d tmp = m_endPositionScale - m_startPositionScale;
    tmp *= i;
    interpolatedScale = m_startPositionScale + tmp;

    tmp = m_endPositionTrans - m_startPositionTrans;
    tmp *= i;
    interpolatedTrans = m_startPositionTrans + tmp;

    osg::Matrix interpolatedMat = osg::Matrix::scale(interpolatedScale)
                                * osg::Matrix::rotate(interpolatedRot)
                                * osg::Matrix::translate(interpolatedTrans);

    getOsgTransform()->setMatrix(interpolatedMat);
}

void AbstractWarpGrid::setStartPosition() {
    m_startPosition=getOsgTransform()->getMatrix();
    m_startPositionRot=m_startPosition.getRotate();
    m_startPositionTrans=m_startPosition.getTrans();
    m_startPositionScale=m_startPosition.getScale();
}

void AbstractWarpGrid::setEndPosition() {
    m_endPosition=getOsgTransform()->getMatrix();
    m_endPositionRot=m_endPosition.getRotate();
    m_endPositionTrans=m_endPosition.getTrans();
    m_endPositionScale=m_endPosition.getScale();
}

//MyClipPlane/////////////////////////////////////////////////////////////////////////////

MyClipPlane::MyClipPlane():AbstractWarpGrid()
{
    m_clipPlane = NULL;
    m_plane=NULL;
    m_clipPlaneIndex=-1;
    m_size=1;
    m_isFlipped=false;
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(updateGrid()));
    m_timer.start(50);
}

void MyClipPlane::flip()
{
    m_isFlipped=!m_isFlipped;
    m_plane->flip();
    m_clipPlane->setClipPlane(*m_plane);
}

void MyClipPlane::updateGrid()
{
    if (getClipPlaneIndex()<0 || m_osgTransform->getMatrix() == m_previousMatrixTransform)
        return;

    m_plane->set(0.0,0.0,1.0,0.0);
    m_plane->transform(m_osgTransform->getMatrix()); // transform the plane to have the same orientation as the grid
    if (m_isFlipped)
        m_plane->flip();
    m_clipPlane->setClipPlane(*m_plane);
    getOsgNode()->getOrCreateStateSet()->setAttributeAndModes(m_clipPlane, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF);
    m_previousMatrixTransform=m_osgTransform->getMatrix();
}

bool MyClipPlane::initializeGrid(const QString& n)
{
    Matrix<double>* gridVertices = new Matrix<double>(4,3);
    gridVertices->set(0,0,m_size * -0.5);gridVertices->set(0,1,m_size * 0.5);gridVertices->set(0,2,0);
    gridVertices->set(1,0,m_size * -0.5);gridVertices->set(1,1,m_size * -0.5);gridVertices->set(1,2,0);
    gridVertices->set(2,0,m_size * 0.5);gridVertices->set(2,1,m_size * -0.5);gridVertices->set(2,2,0);
    gridVertices->set(3,0,m_size * 0.5);gridVertices->set(3,1,m_size * 0.5);gridVertices->set(3,2,0);
    Matrix<unsigned int>* gridFaces = new Matrix<unsigned int>(2,3);
    gridFaces->set(0,0,0);
    gridFaces->set(0,1,1);
    gridFaces->set(0,2,2);
    gridFaces->set(1,0,2);
    gridFaces->set(1,1,3);
    gridFaces->set(1,2,0);
    setVertices(gridVertices);
    setIndices(gridFaces);
    initialize(n);
    getOsgGeometry()->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
    QColor result;
    result.setRgbF(1.0,1.0,1.0,0.2);
    setFrontMaterial(result, true);
    return true;
}

void MyClipPlane::setClipPlaneIndex(int index)
{
    m_clipPlaneIndex=index;
}

//WarpGrid////////////////////////////////////////////////////////////////////////
WarpGrid::WarpGrid():AbstractWarpGrid()
{
    m_originalVertices=NULL;
    m_coefArray.clear();
    m_coefArray.append("coefArray1");
    m_coefArray.append("coefArray2");
    m_coefArray.append("coefArray3");
    m_coefArray.append("coefArray4");
    m_coefArray.append("coefArray5");
    m_bindNames << "morph1" << "morph2" << "morph3" << "morph4" << "morph5";
    m_bindLocations << 1 << 4 << 5 << 6 << 7;
    m_clipPlane=NULL;
}

WarpGrid::~WarpGrid()
{
    AbstractWarpGrid::clear();
    if (m_originalVertices!=NULL)
        delete m_originalVertices;
    m_warpVector.clear();
}

void WarpGrid::updateGrid()
{
    osg::Matrixd compositeMatrixTransform = computeWorldtoLocalMatrix();
    if (compositeMatrixTransform == m_previousMatrixTransform)
        return;

    if (m_useShader)
    {
        generateShape();

        getShaderUniform("P")->set(compositeMatrixTransform);
        getShaderUniform("P_Inverted")->set(osg::Matrixd::inverse(compositeMatrixTransform));

        QList<osg::ref_ptr<osg::Vec3Array> > bindValues;
        for (int j=0;j<qMin(5,getParent()->getTps().size());j++)
            bindValues.push_back(m_warpVector[j]);

        bindVertexAttributes(m_bindNames, m_bindLocations, bindValues);
    }
    else
    {
        Matrix<double>* evanMat = osgMatrixToEvanMatrix(compositeMatrixTransform);
        Matrix<double>* points = new Matrix<double>(*m_originalVertices);
        points->transform(*evanMat);

        TPS tps;

        tps.LoadData(getReferenceLMarks()->GetMatrix(),getWarpedLMarks()->GetMatrix(),getReferenceLMarks()->GetRows(),getReferenceLMarks()->GetCols());
        tps.PerformTPS();

        Matrix<double>* wpoints = new Matrix<double>(points->GetRows(),points->GetCols());
        tps.WarpPoints(points->GetMatrix(),wpoints,points->GetRows());
        evanMat->Inv();
        wpoints->transform(*evanMat);
        setVertices(wpoints);
        refreshGeometry();
        delete evanMat;
    }
    m_previousMatrixTransform=compositeMatrixTransform;
    if (m_clipPlane)
    {
        m_clipPlane->getOsgTransform()->setMatrix(compositeMatrixTransform);
//        m_clipPlane->getPlane()->set(0.0,0.0,1.0,0.0);
//        m_clipPlane->getPlane()->transform(m_osgTransform->getMatrix()); // transform the plane to have the same orientation as the grid
//        if (m_clipPlane->isFlipped())
//            m_clipPlane->getPlane()->flip();
//        m_clipPlane->getClipPlane()->setClipPlane(*(m_clipPlane->getPlane())); //?
//        m_clipPlane->getOsgNode()->getOrCreateStateSet()->setAttributeAndModes(m_clipPlane->getClipPlane(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF);
//        m_clipPlane->setGridAttached(false, compositeMatrixTransform);
//        //m_clipPlane->setFlipped(true);
//        //m_clipPlane->updateGrid();
    }
}

void WarpGrid::generateShape()
{
    if (!getParent() || getParent()->getTps().isEmpty())
        return;

    m_warpVector.clear();

    Matrix<double>* pointInput = new Matrix<double>(*m_originalVertices);
    pointInput->transform(*osgMatrixToEvanMatrix(computeWorldtoLocalMatrix()));
    QVector<osg::Vec3Array*> warpGridVector;
    Matrix<double>* pointOutMat;

    for (int i=0; i<qMin(5,getParent()->getTps().size()); i++)
    {
        pointOutMat = new Matrix<double>((int) pointInput->GetRows(),3);
        getParent()->getTps().value(i)->WarpPoints(pointInput->GetMatrix(),pointOutMat,pointInput->GetRows());
        osg::Vec3Array* morphVector = new osg::Vec3Array;
        for (unsigned int i=0;i< pointOutMat->GetRows();i++)
            morphVector->push_back(osg::Vec3(pointOutMat->get(i,0),pointOutMat->get(i,1),pointOutMat->get(i,2)));
        m_warpVector.push_back(morphVector);
    }
}

bool WarpGrid::initializeGrid(const QString& n)
{
    Matrix<double>* gridVertices = new Matrix<double>(m_rows * m_cols,3);
    for (unsigned int i=0 ; i < m_rows ; i++)
        for (unsigned int j=0 ; j< m_cols ; j++)
        {
            gridVertices->set(i*m_cols+j,0,m_size * (float) ((i-((float)(m_rows-1)/2.0f))/m_rows));
            gridVertices->set(i*m_cols+j,1,m_size * (float) ((j-((float)(m_cols-1)/2.0f))/m_cols));
            gridVertices->set(i*m_cols+j,2,0);
        }

    Matrix<unsigned int>* gridFaces = new Matrix<unsigned int>((m_rows - 1)*(m_cols - 1) ,4);
    for (unsigned int i=0 ; i< m_rows-1; i++)
        for (unsigned int j=0 ; j<m_cols-1 ; j++)
        {
            gridFaces->set(i*(m_cols-1)+j,0,i*m_cols + j);
            gridFaces->set(i*(m_cols-1)+j,1,i*m_cols + j + 1);
            gridFaces->set(i*(m_cols-1)+j,2,(i+1)*m_cols + j + 1);
            gridFaces->set(i*(m_cols-1)+j,3,(i+1)*m_cols + j);
        }
    setVertices(gridVertices);
    setIndices(gridFaces);
    initialize(n);

    osgUtil::Optimizer optimizer;
    optimizer.setPermissibleOptimizationsForObject(m_osgNode,osgUtil::Optimizer::ALL_OPTIMIZATIONS);
    optimizer.optimize(m_osgNode);

    setOriginalVertices();
    return true;
}

bool WarpGrid::reInitializeGrid()
{
    Matrix<double>* gridVertices = new Matrix<double>(m_rows * m_cols,3);
    for (unsigned int i=0 ; i < m_rows ; i++)
        for (unsigned int j=0 ; j< m_cols ; j++)
        {
            gridVertices->set(i*m_cols+j,0,m_size * (float) ((i-((float)(m_rows-1)/2.0f))/m_rows));
            gridVertices->set(i*m_cols+j,1,m_size * (float) ((j-((float)(m_cols-1)/2.0f))/m_cols));
            gridVertices->set(i*m_cols+j,2,0);
        }

    Matrix<unsigned int>* gridFaces = new Matrix<unsigned int>((m_rows - 1)*(m_cols - 1) ,4);
    for (unsigned int i=0 ; i< m_rows-1; i++)
        for (unsigned int j=0 ; j<m_cols-1 ; j++)
        {
            gridFaces->set(i*(m_cols-1)+j,0,i*m_cols + j);
            gridFaces->set(i*(m_cols-1)+j,1,i*m_cols + j + 1);
            gridFaces->set(i*(m_cols-1)+j,2,(i+1)*m_cols + j + 1);
            gridFaces->set(i*(m_cols-1)+j,3,(i+1)*m_cols + j);
        }
    setVertices(gridVertices);
    setIndices(gridFaces);
    refreshGeometry();
    setRenderableName(getRenderableName());
    setOriginalVertices();
    generateShape();
    applyShader();
    osgUtil::Optimizer optimizer;
    optimizer.setPermissibleOptimizationsForObject(m_osgNode,osgUtil::Optimizer::ALL_OPTIMIZATIONS);
    optimizer.optimize(m_osgNode);
    return true;
}

MyClipPlane* WarpGrid::makeBestFitPlane()
{
    MyClipPlane* result=new MyClipPlane;
    result->setSize(1);
    QString name("Best Fit Plane for ");
    name.append(getRenderableName());
    result->initializeGrid(name);

    // get points
    osg::Matrixd mat = computeWorldtoLocalMatrix();
    Matrix<double>* evanMat = osgMatrixToEvanMatrix(mat);
    Matrix<double>* points = new Matrix<double>(*m_originalVertices);
    points->transform(*evanMat);
    TPS tps;
    tps.LoadData(getReferenceLMarks()->GetMatrix(),getWarpedLMarks()->GetMatrix(),getReferenceLMarks()->GetRows(),getReferenceLMarks()->GetCols());
    tps.PerformTPS();
    Matrix<double>* wpoints = new Matrix<double>(points->GetRows(),points->GetCols());
    tps.WarpPoints(points->GetMatrix(),wpoints,points->GetRows());

    // centroid
    float x0=0,y0=0,z0=0;
    for (unsigned int i=0;i<wpoints->GetRows();i++)
    {
        x0+=wpoints->get(i,0);
        y0+=wpoints->get(i,1);
        z0+=wpoints->get(i,2);
    }
    x0/=wpoints->GetRows();
    y0/=wpoints->GetRows();
    z0/=wpoints->GetRows();

    // PCA
    PCA m_pca;

    m_pca.LoadData(wpoints->GetMatrix(),wpoints->GetRows(),wpoints->GetCols() );

    m_pca.PerformPCA(true, false);

    Matrix<double>* mat3x3 = new Matrix<double>();
    m_pca.GetPCLoadings(mat3x3);

    osg::Matrixd mat4x4 = osg::Matrixd::identity();
     for (unsigned int i=0;i<mat3x3->GetRows();i++){
        for (unsigned int j=0;j<mat3x3->GetCols();j++){
            mat4x4(i,j) = mat3x3->get(j,i);
        }
    }

    mat4x4(3,0)=x0;
    mat4x4(3,1)=y0;
    mat4x4(3,2)=z0;
    osg::Quat rot;
    rot.makeRotate(osg::Vec3(0.0,0.0,1.0),osg::Vec3(0.0,0.0,-1.0));
    mat4x4.preMultRotate(rot);
    result->setGridTransform(mat4x4);

    delete evanMat;
    delete points;
    delete wpoints;
    delete mat3x3;
    connect(result,SIGNAL(deleted()),this,SLOT(removeClipPlane()));
    m_clipPlane=result;
    return result;
}

void WarpGrid::setUsedShader(bool b)
{
    m_useShader=b;
    Matrix<double>* points = new Matrix<double>(*m_originalVertices);
    setVertices(points);
    refreshGeometry();
}

void WarpGrid::applyShader()
{
    for (int k=0;k<m_coefArray.size();k++)
    {
        addShaderUniform(osg::Uniform::FLOAT, m_coefArray.at(k));
        getShaderUniform(m_coefArray.at(k))->set(0.0f);
    }
    addShaderUniform(osg::Uniform::INT, "numElements");
    getOsgGeometry()->getOrCreateStateSet()->addUniform(new osg::Uniform("P",computeWorldtoLocalMatrix()));
    getOsgGeometry()->getOrCreateStateSet()->addUniform(new osg::Uniform("P_Inverted",osg::Matrixd::inverse(computeWorldtoLocalMatrix())));
    getShaderUniform("numElements")->set(getParent()->getTps().size());
    if(!attachShaders("grid"))
        return;
    QList<osg::ref_ptr<osg::Vec3Array> > bindValues;
    int i;
    for (i=0;i<qMin(5,getParent()->getTps().size());i++)
    {
        bindValues.push_back(m_warpVector[i]);
    }
    if(i<5)
        for( ;i<5;++i){
            osg::ref_ptr<osg::Vec3Array> morphVector = new osg::Vec3Array;
            for (unsigned int j=0; j<getVertices()->GetRows();j++)
                morphVector->push_back(osg::Vec3(0,0,0));
            bindValues.push_back(morphVector);
        }
    bindVertexAttributes(m_bindNames, m_bindLocations, bindValues);
    toggleShaders(m_useShader);
}

void WarpGrid::removeShader()
{
    for (int k=0;k<m_coefArray.size();k++)
        m_osgGeometry->getOrCreateStateSet()->removeUniform(m_coefArray.at(k).toStdString());

    m_osgGeometry->getOrCreateStateSet()->removeUniform("numElements");
    m_osgGeometry->getOrCreateStateSet()->removeUniform("P");
    m_osgGeometry->getOrCreateStateSet()->removeUniform("P_Inverted");
    for (int i=0;i<m_bindNames.size();++i)
        m_osgShader->removeBindAttribLocation(m_bindNames[i].toStdString());

    toggleShaders(false);
}

QString WarpGrid::toString() const
{
    if(!m_osgNode.valid())
        return "";
    QString result = "";

    result +=   getRenderableName()  +"|";

    if (m_osgNode->getStateSet())
        result +=   QString().setNum(m_fMatColor.red())  +"|"+
                    QString().setNum(m_fMatColor.green())+"|"+
                    QString().setNum(m_fMatColor.blue())+"|"+
                    QString().setNum(m_fMatColor.alpha())+"|";
    else
        result += "0|0|0|0|"; // VD - bug fix

    osg::Matrixd rTransform = m_osgTransform->getMatrix();
    result +=   QString().setNum(rTransform.getScale().x())+"|"+
                QString().setNum(rTransform.getScale().y())+"|"+
                QString().setNum(rTransform.getScale().z())+"|"+

                QString().setNum(rTransform.getRotate().x())+"|"+
                QString().setNum(rTransform.getRotate().y())+"|"+
                QString().setNum(rTransform.getRotate().z())+"|"+
                QString().setNum(rTransform.getRotate().w())+"|"+

                QString().setNum(rTransform.getTrans().x())+"|"+
                QString().setNum(rTransform.getTrans().y())+"|"+
                QString().setNum(rTransform.getTrans().z())+"|";

    result +=   QString().setNum(m_bMatColor.red())  +"|"+
                QString().setNum(m_bMatColor.green())+"|"+
                QString().setNum(m_bMatColor.blue())+"|";

    result +=   QString().setNum(m_rows) +"|"+
                QString().setNum(m_cols) +"|"+
                QString().setNum(m_size);

    return result;
}

WarpGrid* WarpGrid::loadFromString(const QString& params)
{
    QStringList gridParams = params.split("|");
    if(gridParams.count() == 21)
    {
        WarpGrid* newGrid = new WarpGrid;
        newGrid->setRowsAndCols(gridParams[18].toInt(),gridParams[19].toInt());
        newGrid->setSize(gridParams[20].toDouble());
        newGrid->initializeGrid(gridParams[0]);
        QColor matColor = QColor(   gridParams[1].toInt(),
                            gridParams[2].toInt(),
                            gridParams[3].toInt(),
                            gridParams[4].toInt());
        newGrid->setFrontMaterial(matColor==Qt::transparent ? Qt::lightGray : matColor,  matColor!=Qt::transparent);

        newGrid->setGridTransform(osg::Matrix::scale(gridParams[5].toDouble(), gridParams[6].toDouble(), gridParams[7].toDouble()) *
                    osg::Matrix::rotate(osg::Quat(gridParams[8].toDouble(), gridParams[9].toDouble(), gridParams[10].toDouble(), gridParams[11].toDouble())) *
                    osg::Matrix::translate(gridParams[12].toDouble(), gridParams[13].toDouble(), gridParams[14].toDouble()));
        return newGrid;
    }
    else return NULL;
}

void WarpGrid::removeClipPlane()
{
    m_clipPlane=NULL;
}

// GroupWarpGrids /////////////////////////////////////////////////////////////////
IRenderable* GroupWarpGrids::getChild(unsigned int i) const
{
    if ((int) i<m_listGrid.size())
        return getGrid(i);
    if ((int) i<m_listGroupGrid.size()+m_listGrid.size())
        return getGroupGrid(i-m_listGrid.size());
    return NULL;
}

bool GroupWarpGrids::initialize(const QString& n)
{
    m_osgNode = new osg::Geode;
    setRenderableName(n);
    return true;
}

void GroupWarpGrids::addGrid(WarpGrid* wgrid)
{
    wgrid->generateShape();
    m_listGrid.append(wgrid);
    wgrid->applyShader();
}

void GroupWarpGrids::deleteGroupGrid(GroupWarpGrids* groupWarpg){
    if (m_listGroupGrid.indexOf(groupWarpg)>=0)
    {
        for (int i=0;i<groupWarpg->getNumGrid();++i)
            groupWarpg->deleteGrid(i);
        for (int i=0;i<groupWarpg->getNumGroupGrid();++i)
            groupWarpg->deleteGroupGrid(getGroupGrid(i));
        m_listGroupGrid.removeAll(groupWarpg);
        initialize(getRenderableName());
        delete groupWarpg;
    }
}

void GroupWarpGrids::addGroupGrid(GroupWarpGrids* groupWarpGrid)
{
    m_listGroupGrid.append(groupWarpGrid);
    groupWarpGrid->setTps(getTps());
}

void GroupWarpGrids::setUsedShader(bool b)
{
    m_useShader=b;
    for (int i=0;i<m_listGrid.size();i++)
        getGrid(i)->setUsedShader(b);
    for (int i=0;i<getNumGroupGrid();i++)
        m_listGroupGrid[i]->setUsedShader(b);
    updateGrids();
}

void GroupWarpGrids::update()
{
     if(m_isMainGroup)
     {
//        if (!m_timer.isActive())
//            m_timer.start(50);

            for (int i=0;i<getNumGroupGrid();i++)
            {
                m_listGroupGrid[i]->setParentMatrixTransform(m_listGroupGrid[i]->getOsgTransform()->getMatrix()*getOsgTransform()->getMatrix());
                m_listGroupGrid[i]->update();
            }
     }

    for (int i=0;i<m_listGrid.size();i++)
        getGrid(i)->update();
}

void GroupWarpGrids::deleteGrid(WarpGrid* warpg)
{
    if (m_listGrid.indexOf(warpg)>=0)
        deleteGrid(m_listGrid.indexOf(warpg));
    else {
        for (int i=0;i<getNumGroupGrid();i++)
            m_listGroupGrid[i]->deleteGrid(warpg);
    }
}

void GroupWarpGrids::deleteGrid(int i)
{
    if (i<m_listGrid.size())
    {
        WarpGrid* grid = m_listGrid.at(i);
        m_listGrid.removeAt(i);
        initialize(getRenderableName());
        delete grid;
    }
}

void GroupWarpGrids::updateGrids()
{
    for (int i=0;i<m_listGrid.size();i++)
    {
        WarpGrid* warpg = getGrid(i);
        warpg->generateShape();
        warpg->applyShader();
    }
    for (int i=0;i<getNumGroupGrid();i++)
        m_listGroupGrid[i]->updateGrids();
}

QString GroupWarpGrids::toString() const
{
    if(!m_osgNode.valid())
        return "";
    QString result = "";

    result +=   getRenderableName()  +"|";

    if (m_osgNode->getStateSet())
        result +=   QString().setNum(m_fMatColor.red())  +"|"+
                    QString().setNum(m_fMatColor.green())+"|"+
                    QString().setNum(m_fMatColor.blue())+"|"+
                    QString().setNum(m_fMatColor.alpha())+"|";
    else
        result += "0|0|0|0|"; // VD - bug fix

    osg::Matrixd rTransform = m_osgTransform->getMatrix();
    result +=   QString().setNum(rTransform.getScale().x())+"|"+
                QString().setNum(rTransform.getScale().y())+"|"+
                QString().setNum(rTransform.getScale().z())+"|"+

                QString().setNum(rTransform.getRotate().x())+"|"+
                QString().setNum(rTransform.getRotate().y())+"|"+
                QString().setNum(rTransform.getRotate().z())+"|"+
                QString().setNum(rTransform.getRotate().w())+"|"+

                QString().setNum(rTransform.getTrans().x())+"|"+
                QString().setNum(rTransform.getTrans().y())+"|"+
                QString().setNum(rTransform.getTrans().z())+"|";

    result +=   QString().setNum(m_bMatColor.red())  +"|"+
                QString().setNum(m_bMatColor.green())+"|"+
                QString().setNum(m_bMatColor.blue())+"|";


    result += "(";
    for(int i=0; i<getNumGroupGrid(); ++i)
    {
        result += getGroupGrid(i)->toString();
        result += "/";
    }
    result += ")";

    result += "[";

    for(int i=0; i<getNumGrid(); ++i)
    {
        result += getGrid(i)->toString();
        result += ":";
    }

    result += "]";

    return result;
}

GroupWarpGrids* GroupWarpGrids::loadFromString(const QString& params, QVector<TPS*> tps)
{

    QStringList paramList = params.split("(");
    QStringList rInts = paramList[0].split("|");

    GroupWarpGrids* groupWarpGrids = new GroupWarpGrids;
    if(rInts.count() != 19)
        return groupWarpGrids;

    groupWarpGrids->setTps(tps);

    groupWarpGrids->initialize(rInts[0]);;


    QColor matColor = QColor(   rInts[1].toInt(),
                                rInts[2].toInt(),
                                rInts[3].toInt(),
                                rInts[4].toInt());
    groupWarpGrids->setFrontMaterial(matColor==Qt::transparent ? Qt::lightGray : matColor ,matColor!=Qt::transparent);

    groupWarpGrids->getOsgTransform()->setMatrix(osg::Matrix::scale(rInts[5].toDouble(), rInts[6].toDouble(), rInts[7].toDouble()) *
                        osg::Matrix::rotate(osg::Quat(rInts[8].toDouble(), rInts[9].toDouble(), rInts[10].toDouble(), rInts[11].toDouble())) *
                        osg::Matrix::translate(rInts[12].toDouble(), rInts[13].toDouble(), rInts[14].toDouble()));


    if(params.contains("(") && params.contains(")"))
    {
        QString childrenData = params.mid(params.indexOf("(")+1, params.lastIndexOf(")") - params.indexOf("(")-1);
        QStringList childrenParams = childrenData.split("/");
        foreach(QString childParam, childrenParams)
        {
            if(childParam == "" || childParam.isEmpty())
                continue;

            GroupWarpGrids* groupChild = GroupWarpGrids::loadFromString(childParam, tps);
            groupWarpGrids->addGroupGrid(groupChild);
        }
    }

    if(params.contains("[") && params.contains("]"))
    {
        QString childrenData0 = params.mid(params.lastIndexOf(")")+1);
        QString childrenData = childrenData0.mid(childrenData0.indexOf("[")+1, childrenData0.lastIndexOf("]")-childrenData0.indexOf("[")-1);
        QStringList childrenParams = childrenData.split(":");
        foreach(QString childParam, childrenParams)
        {
            if(childParam == "" || childParam.isEmpty())
                continue;

            WarpGrid* gridChild = WarpGrid::loadFromString(childParam);
            if (!gridChild)
                continue;

            gridChild->setParent(groupWarpGrids);
            groupWarpGrids->addGrid(gridChild);
        }
    }
    groupWarpGrids->setUsedShader(true);
    return groupWarpGrids;
}

void GroupWarpGrids::setTps(QVector<TPS*> tps)
{
    m_tps=tps;
    for (int i=0;i<getNumGroupGrid();++i)
    {
        getGroupGrid(i)->setTps(tps);
    }
}

void GroupWarpGrids::setInterpolatedTransfMatrix(float i)
{
    if (m_startPosition==m_endPosition)
        return;

    osg::Quat interpolatedRot;
    osg::Vec3d interpolatedScale;
    osg::Vec3d interpolatedTrans;
    interpolatedRot.slerp(i,m_startPositionRot,m_endPositionRot);

    osg::Vec3d tmp = m_endPositionScale - m_startPositionScale;
    tmp *= i;
    interpolatedScale = m_startPositionScale + tmp;

    tmp = m_endPositionTrans - m_startPositionTrans;
    tmp *= i;
    interpolatedTrans = m_startPositionTrans + tmp;

    osg::Matrix interpolatedMat = osg::Matrix::scale(interpolatedScale)
                                * osg::Matrix::rotate(interpolatedRot)
                                * osg::Matrix::translate(interpolatedTrans);

    getOsgTransform()->setMatrix(interpolatedMat);
}

void GroupWarpGrids::setStartPosition() {
    m_startPosition=getOsgTransform()->getMatrix();
    m_startPositionRot=m_startPosition.getRotate();
    m_startPositionTrans=m_startPosition.getTrans();
    m_startPositionScale=m_startPosition.getScale();
}

void GroupWarpGrids::setEndPosition() {
    m_endPosition=getOsgTransform()->getMatrix();
    m_endPositionRot=m_endPosition.getRotate();
    m_endPositionTrans=m_endPosition.getTrans();
    m_endPositionScale=m_endPosition.getScale();
}

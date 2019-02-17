#ifndef SURFACE_H
#define SURFACE_H

#include "IRenderable.h"
#include "Matrix.h"
#include <osg/Texture2D>
#include <fstream>
using namespace std;

class Surface : public IRenderable
{
private:
    Matrix<double>* m_vertices;
    Matrix<unsigned int>* m_indeces;
    osg::ref_ptr<osg::Texture2D> m_texture;

    bool m_bRaw;
    bool m_bTextured;
    bool m_hasTexture;
    int m_lineWidth;
    int m_specimenNumber;

public:
    Surface() : IRenderable(),
        m_vertices(NULL),
        m_indeces(NULL),
        m_texture(),
        m_bRaw(true),
        m_bTextured(false),
        m_hasTexture(false),
        m_lineWidth(0),
        m_specimenNumber(1)
    {
        addType(SURFACE_T);
        addPartOfRelation(SURFACE_VECTOR_T);
    }

    ~Surface()
    {clear();}

    void clear();

    void setSpecimenNumber(int number)      {m_specimenNumber = number;}
    int getSpecimenNumber()                 {return m_specimenNumber;}
    void setVertices(Matrix<double>* v)     {if(m_vertices)delete m_vertices; m_vertices=v;}
    void setIndices(Matrix<unsigned int>* i){if(m_indeces) delete m_indeces;  m_indeces=i;}
    Matrix<double>* getVertices()     const {return m_vertices;}
    Matrix<unsigned int>* getIndeces()const {return m_indeces;}
    unsigned int getVertexCount()     const {return m_vertices->GetRows();}
    unsigned int getIndexCount()      const {return m_indeces->GetRows();}
    bool isRaw()                      const {return m_bRaw;}
    bool isTextured()                 const {return m_bTextured;}
    bool hasTexture()                 const {return m_hasTexture;}
    osg::ref_ptr<osg::Texture2D> getTexture() const {return m_texture;}
    void setLineWidth(int width)            {m_lineWidth=width;refreshGeometry();}
    int getLineWidth()                const {return m_lineWidth;}
    void setRaw(bool raw) {m_bRaw = raw;}

    void applyTransform(const Matrix<double>& mat)
    {m_vertices->transform(mat);}

    bool loadTexture(const QString&);
    bool initialize(const QString&);
    bool initialize(const QString&, const QString&);    //Initialize from a .obj file
    bool initialize(const Surface*, const QString&);    //Initialize from another surface
    void refreshGeometry(bool flip=false);              //To use after changing the vertex data

    void update(){}

    bool setFromContainer(DataTypePtr&, unsigned int);
    bool isValid() const
    {
        if(m_bRaw)
            return (m_vertices && m_indeces)?
                            (!m_vertices->IsEmpty() && !m_indeces->IsEmpty()) : false;
        else
            return (m_vertices && m_osgNode.get())?
                            (!m_vertices->IsEmpty() && m_osgNode.valid()) : false;
    }
    bool saveToFile(const QString&);

    void assignTexture();
    void removeTexture();
};

Matrix<double>* osgMatrixToEvanMatrix(const osg::Matrixd& mosg);

#endif



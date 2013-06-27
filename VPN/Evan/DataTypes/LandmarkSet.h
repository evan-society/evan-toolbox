#ifndef LANDMARKSET_H
#define LANDMARKSET_H

#include "LMKSetRenderable.h"
#include "MatrixD.h"
#include "DataTypePtr.h"
#include <QSet>

class LandmarkSet : public MatrixD
{
private:
    unsigned int m_specimenIndex;
    QSet<unsigned int> m_missing;
    DataTypePtr m_renderablePtr;
    double m_centroidSize;

public:
    LandmarkSet(size_t row = 0, size_t col = 0) : MatrixD(row,col)
    {
        addType(LANDMARKSET_T);
        addPartOfRelation(SPECIMENS_T);
        m_renderablePtr = DataTypePtr(new LMKSetRenderable);
        m_centroidSize = 0.0;
    }
    LandmarkSet(const LandmarkSet& copy) : MatrixD(copy)
    {
        m_renderablePtr = DataTypePtr(new LMKSetRenderable);
    }
    ~LandmarkSet()
    {}

    LMKSetRenderable* getRenderable() const {return (LMKSetRenderable*)m_renderablePtr.getPtr();}
    void setSpecimenIndex(unsigned int i) {m_specimenIndex = i;}
    bool isComplete() const {return m_missing.empty();}
    void addMissing(unsigned int i) {m_missing.insert(i);}
    bool isMissing(unsigned int i) const {return m_missing.contains(i);}
    const QSet<unsigned int>& getMissingLandmarks() const {return m_missing;}
    unsigned int getSpecimenIndex() const {return m_specimenIndex;}

    bool setFromContainer(DataTypePtr&, unsigned int);
    bool initialize(const QString& n)
    {
        getRenderable()->setData(GetRows(), GetCols(), GetMatrix());
        return getRenderable()->initialize(n);
    }
    void update() {getRenderable()->update();}
    double getCentroidSize() {return m_centroidSize;}
    void setCentroidSize(double centroidSize) {m_centroidSize = centroidSize;}
};

#endif

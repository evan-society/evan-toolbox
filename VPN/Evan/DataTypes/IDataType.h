#ifndef IDATATYPE_H
#define IDATATYPE_H

#include <QVector>

class DataTypePtr;

class IDataType
{
public:
    enum Type{  IDATATYPE_T, LANDMARKSET_T, SURFACE_T, SPECIMENS_T, LOADINGS_T, PCSCORE_T,
                PROCRUSTESDISTANCES_T, TRANSFORMMATRIX_T, FLOAT_T, IRENDERABLE_T, RENDERABLES_T,
                CENTROIDSIZES_T, INFO_T, VARIABLES_T, MATRIXD_T, VECTORD_T, TANGENTCOORDINATES_T,
                 WARPINGS_T, FLOATVECTOR_T, RLIST_T, SURFACE_VECTOR_T, GRID_T, VOLUME_T, IPRINTABLE_T, PRINTABLES_T};
    enum RelationType {DIRECT, CONTAINS, PART_OF, INVALID};
private:
    unsigned int m_refCount;
    friend class DataTypePtr;

    QVector<Type> m_types;
    QVector<Type> m_containList;
    QVector<Type> m_partOfList;

protected:
    virtual ~IDataType() {}

public:
    explicit IDataType() :  m_refCount(0),  m_types(), m_containList(), m_partOfList()
    {}
    IDataType(const IDataType& copy) : m_refCount(0),
                                       m_types(copy.getTypes()),
                                       m_containList(copy.getCTypes()),
                                       m_partOfList(copy.getPTypes())
    {}//We don't want to call the default copy constructor

    IDataType & operator = (const IDataType& copy)
    {
        if (this != &copy)
        {
            m_refCount = 0;
            m_types = copy.getTypes();
            m_containList = copy.getCTypes();
            m_partOfList= copy.getPTypes();
        }
        return *this;
    }


    bool directMatch(const IDataType* dt) const
    {
        foreach(Type type, m_types)
            if (!dt->isType(type))
                return false;
        return true;
    }
    bool matches(const IDataType* dt) const
    {
        foreach(Type type, dt->getTypes())
            if( m_containList.contains(type) || m_partOfList.contains(type))
                return true;
        return directMatch(dt);
    }
    RelationType getRelationType(const IDataType* dt) const
    {
        foreach(Type type, dt->getTypes())
        {
            if (m_containList.contains(type))       return CONTAINS;
            else if (m_partOfList.contains(type))   return PART_OF;
        }
        if(directMatch(dt))                         return DIRECT;
        return INVALID;
    }

    void addPartOfRelation(Type t)  {m_partOfList.push_back(t);}
    void addContainsRelation(Type t){m_containList.push_back(t);}
    void addType(Type t) {m_types.push_back(t);}
    bool isType(Type t) const {return m_types.contains(t);}
    const QVector<Type>& getTypes() const {return m_types;}
    const QVector<Type>& getCTypes()const {return m_containList;}
    const QVector<Type>& getPTypes()const {return m_partOfList;}

    virtual void addPart(DataTypePtr&, const QString&) {}
    virtual void removePart(DataTypePtr&) {}
    virtual bool setFromContainer(DataTypePtr&, unsigned int) {return true;}
    virtual bool isValid() const = 0;
};

#endif

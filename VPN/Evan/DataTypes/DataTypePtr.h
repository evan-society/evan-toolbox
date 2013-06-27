#ifndef DATATYPEPTR_H
#define DATATYPEPTR_H

#include "IDataType.h"

class DataTypePtr
{
private:
    IDataType* m_ptr;
public:
    explicit DataTypePtr(IDataType* p = 0)
    {acquire(p);}

    DataTypePtr(const DataTypePtr& dtPtr)
    {acquire(dtPtr.getPtr());}

    ~DataTypePtr()
    {release();}

    DataTypePtr& operator=(const DataTypePtr& dtPtr);
    bool operator==(const DataTypePtr& dtPtr) {return m_ptr == dtPtr.getPtr();}

    IDataType* getPtr()         const {return m_ptr;}
    IDataType& operator*()      const {return *m_ptr;}
    IDataType* operator->()     const {return m_ptr;}
    bool isValid()              const {return (m_ptr ? m_ptr->isValid() : false);}

    void acquire(IDataType* ptr);
    void release();
};

#endif

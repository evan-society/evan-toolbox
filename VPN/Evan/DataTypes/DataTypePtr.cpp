#include "DataTypePtr.h"

void DataTypePtr::acquire(IDataType* ptr)
{
    m_ptr = ptr;
    if (m_ptr) ++m_ptr->m_refCount;
}

void DataTypePtr::release()
{
    if (m_ptr)
    {
        if (--m_ptr->m_refCount == 0)
            delete m_ptr;
        m_ptr = 0;
    }
}

DataTypePtr& DataTypePtr::operator=(const DataTypePtr& dtPtr)
{
    if (m_ptr != dtPtr.getPtr())
    {
        release();
        acquire(dtPtr.getPtr());
    }
    return *this;
}

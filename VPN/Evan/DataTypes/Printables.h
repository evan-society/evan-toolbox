#ifndef PRINTABLES_H
#define PRINTABLES_H

#include "IPrintable.h"
#include "DataTypePtr.h"

class Printables : public IDataType
{
private:
    QList<DataTypePtr> m_printables;

public:
    Printables() : IDataType()
    {
        addType(PRINTABLES_T);
        addContainsRelation(IPRINTABLE_T);
    }

    void addPrintable(IPrintable* p)
    {
		if(p)
        {
            int i = m_printables.indexOf(DataTypePtr(p));
            if(i >= 0 ) setPrintable(i, p);
            else        m_printables.push_back(DataTypePtr(p));
        }
    }
    void setPrintable(int i, IPrintable* p)     {m_printables[i] = DataTypePtr(p);}
    void removePrintable(IPrintable* p)
    {
        if(p)
        {
            int i = m_printables.indexOf(DataTypePtr(p));
            if(i >= 0 )
                m_printables.removeAt(i);
        }
    }
    IPrintable* getPrintable(int i)       const {return (IPrintable*)m_printables[i].getPtr();}
    unsigned int getSize()                  const {return m_printables.size();}
    bool containsPrintable(IPrintable* p) const {return m_printables.contains(DataTypePtr(p));}

    void clear();

    void addPart(DataTypePtr& part, const QString& desc);

    void removePart(DataTypePtr& part);

    bool isValid() const { return m_printables.size() > 0;}
};

#endif

#ifndef RLIST_H
#define RLIST_H

// include R headers, but set R_NO_REMAP and access everything via Rf_ prefixes
#define R_NO_REMAP

#include <R.h>
#include <Rinternals.h>
#include <QStringList>
#include "DataTypePtr.h"
// On some R implementations, PROTECT and UNPROTECT are defined as protect
// and unprotect, even in the presence of R_NO_REMAP.
#undef PROTECT
#undef UNPROTECT
#define PROTECT Rf_protect
#define UNPROTECT Rf_unprotect

class RList : public IDataType
{
private:
    QVector<DataTypePtr> m_parts;
    QVector<SEXP> m_values;
    QVector<std::string> m_names;
    QStringList m_descriptions;
    unsigned int m_numProtected;

    double getSEXPValue(SEXP var, int index)
    {
        if (Rf_isInteger(var))
            return (double)INTEGER(var)[index];
        else if (Rf_isReal(var))
            return REAL(var)[index];
        else return 0.0;
    }
public:
    RList() : IDataType()
    {
        addType(RLIST_T);
        addContainsRelation(SPECIMENS_T);
        addContainsRelation(MATRIXD_T);
        addContainsRelation(FLOAT_T);
        addContainsRelation(VARIABLES_T);
        addContainsRelation(LOADINGS_T);
        m_numProtected = 0;
    }

    ~RList()
    {
        clear();
        //UNPROTECT(numProtected);
    }

    void clear()
    {
        UNPROTECT(m_numProtected);
        m_values.clear();
        m_names.clear();
        for(int i=0; i<m_parts.size(); ++i)
            m_parts[i].release();
        m_parts.clear();
        m_descriptions.clear();
    }

    void setName(unsigned int i, std::string n){m_names[i] = n;}
    const char* getName(unsigned int i) const {return m_names[i].c_str();}
    unsigned int getSize()              const {return m_values.size();}
    SEXP getListMember(unsigned int i)  const {return m_values[i];}
    QString getDescription(unsigned int i)const {return m_descriptions[i];}
    DataTypePtr getDataType(unsigned int i)const {return m_parts[i];}
    SEXP getReturnList() const;

    int appendList(SEXP);
    void addVar(const std::string& varName, SEXP value, bool isProtected=false)
    {
        int i = m_values.indexOf(value);
        if(i >= 0 )
        {
            m_values[i] = value;
            m_names[i] = varName;
        }
        else
        {
            m_values.push_back(value);
            m_names.push_back(varName);
        }
        if(isProtected)
            m_numProtected++;
    }

    void addPart(DataTypePtr& part, const QString&);

    void removePart(DataTypePtr& part)
    {
        int i = m_parts.indexOf(part);
        if(i >= 0 )
        {
            m_values.remove(i);
            m_names.remove(i);
            m_parts.remove(i);
            //part.release();
            m_descriptions.removeAt(i);
        }
    }
    bool isValid() const {return getSize()>0;}
};

#endif

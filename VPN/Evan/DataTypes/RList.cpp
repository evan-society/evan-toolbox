#ifdef USE_R_

#include "Specimens.h"
#include "RList.h"
#include <QString>
#include "Float.h"

#include "../../Labels/StringLabelValue.h"
#include "../../Labels/ScalarLabelValue.h"


SEXP RList::getReturnList() const
{
    unsigned int nret = getSize();
    SEXP rl = PROTECT(Rf_allocVector(VECSXP,nret));
    SEXP nm = PROTECT(Rf_allocVector(STRSXP,nret));

    for (unsigned int i = 0; i<nret; ++i)
    {
        SET_VECTOR_ELT(rl, i, m_values[i]);
        SET_STRING_ELT(nm, i, Rf_mkChar(m_names[i].c_str()));
    }

    Rf_setAttrib(rl, R_NamesSymbol, nm);
    UNPROTECT(2);
    return rl;
}

int RList::appendList(SEXP listPtr)
{
    if (!Rf_isNewList(listPtr))
        return 1;
    int len = Rf_length(listPtr);
    SEXP names = Rf_getAttrib(listPtr, R_NamesSymbol);
    if (names == R_NilValue)
        return 2;

    m_parts.clear();

    for (int i=0; i<len; ++i)
    {
        std::string varName = std::string(CHAR(STRING_ELT(names,i)));
        SEXP varValue = VECTOR_ELT(listPtr, i);
        if (!Rf_isNumeric(varValue) || Rf_length(varValue) <= 0)
            return 3;
        //Single Scalar
        else if(Rf_length(varValue) == 1)
        {
            double value = getSEXPValue(varValue, 0);
            m_parts.push_back(DataTypePtr(new Float(value)));
        }
        else if(Rf_length(varValue) > 1)
        {
            //Matrix
            if (Rf_isMatrix(varValue))
            {
                // Get matrix dimensions
                SEXP dimAttr = Rf_getAttrib(varValue, R_DimSymbol);
                int rows = INTEGER(dimAttr)[0];
                int cols = INTEGER(dimAttr)[1];
                MatrixD* m = new MatrixD(rows,cols);
                for (int x=0; x<rows; ++x)
                    for (int y=0; y<cols; ++y)
                    {
                        double value = getSEXPValue(varValue, (x+rows*y));
                        m->set(x,y, value);
                    }
                m_parts.push_back(DataTypePtr(m));
            }
            //Specimens
            else if (Rf_isArray(varValue))
            {
                // Get array dimensions
                SEXP dimAttr = Rf_getAttrib(varValue, R_DimSymbol);
                int landmarks = INTEGER(dimAttr)[0];
                int dimensions = INTEGER(dimAttr)[1];
                int individuals = INTEGER(dimAttr)[2];

                Specimens* specimens = new Specimens();
                int l=0;
                for (int i=0; i<individuals; i++)
                {
                    LandmarkSet* ls = new LandmarkSet(landmarks,dimensions);
                    for (int k=0; k<dimensions; k++)
                    {
                        for (int j=0; j<landmarks; j++)
                        {
                            double value = getSEXPValue(varValue, l++);
                            (*ls)[j][k] = value;
                        }
                    }
                    specimens->addLandmarkSet(ls);
                }
                m_parts.push_back(DataTypePtr(specimens));
            }
            //Vector
            else if (Rf_isVector(varValue))
            {
                // Get matrix dimensions
                int rows = Rf_length(varValue);
                MatrixD* m = new MatrixD(rows,1);
                for (int x=0; x<rows; ++x)
                {
                    double value = getSEXPValue(varValue, x);
                    m->set(x,0, value);
                }
                m_parts.push_back(DataTypePtr(m));
            }
            //Undefined structure
            else
            {
                 continue;
            }
        }
        else    return 4;    //Undefined Output

        addVar(varName, varValue);
    }

    return 0;
}

void RList::addPart(DataTypePtr& part, const QString& desc)
{
    if(!part.isValid())
        return;

    SEXP value = NULL;
    std::string varName = "";
    int i = m_parts.indexOf(part);
    if(i >= 0 ) m_parts[i] = part;
    else
    {
        m_parts.push_back(part);
        m_numProtected++;
    }

    if(part->isType(SPECIMENS_T))
    {
        Specimens* arr = dynamic_cast<Specimens *>( part.getPtr() );

        int n = arr->getSize();
        int k = arr->getLandmarkCount();
        int m = arr->getLandmarkDimensions();

        SEXP dim;
        //Set dimensions of array
        PROTECT(dim = Rf_allocVector(INTSXP, 3));
        INTEGER(dim)[0] = k;
        INTEGER(dim)[1] = m;
        INTEGER(dim)[2] = n;

        //Allocate memory
        PROTECT(value = Rf_allocArray(REALSXP, dim));

        value = (i>=0) ? m_values[i] : PROTECT(Rf_allocArray(REALSXP, dim));
        varName = (i>=0) ? m_names[i] : QString("arr%1").arg(getSize()).toStdString();

        int x=0;
        for (int i = 0;i<n;i++)
        {
            LandmarkSet* mat = arr->getLandmarkSet(i);
            for (int j = 0;j<m;j++)
            {
                for (int l = 0;l<k;l++)
                {
                    REAL(value)[x++] = (*mat)[l][j];
                }
            }
        }
    }
    else if(part->isType(MATRIXD_T))
    {
        MatrixD* mat = dynamic_cast<MatrixD *>( part.getPtr() );
        unsigned int nx = mat->GetRows();
        unsigned int ny = mat->GetCols();
        double** matPtr = mat->GetMatrix();
        if(nx > 1 && ny > 1)
        {
            value = (i>=0) ? m_values[i] : PROTECT(Rf_allocMatrix(REALSXP, nx, ny));
            varName = (i>=0) ? m_names[i] : QString("mat%1").arg(getSize()).toStdString();
            for(unsigned int x=0; x<nx; ++x)
                for(unsigned int y=0; y<ny; ++y)
                    REAL(value)[x + nx*y] = matPtr[x][y];
        }
        else if(nx == 1)
        {
            value = (i>=0) ? m_values[i] : PROTECT(Rf_allocVector(REALSXP, ny));
            varName = (i>=0) ? m_names[i] : QString("vec%1").arg(getSize()).toStdString();
            for(unsigned int x=0; x<ny; ++x)
                REAL(value)[x] = matPtr[0][x];
        }
        else if(ny == 1)
        {
            value = (i>=0) ? m_values[i] : PROTECT(Rf_allocVector(REALSXP, nx));
            varName = (i>=0) ? m_names[i] : QString("vec%1").arg(getSize()).toStdString();
            for(unsigned int x=0; x<nx; ++x)
                REAL(value)[x] = matPtr[x][0];
        }
    }
    else if(part->isType(FLOAT_T))
    {
        Float* real = dynamic_cast<Float *>( part.getPtr() );
        value = (i>=0) ? m_values[i] : PROTECT(Rf_allocVector(REALSXP, 1));
        varName = (i>=0) ? m_names[i] : QString("float%1").arg(getSize()).toStdString();
        REAL(value)[0] = real->getData();
    }
    addVar( varName, value, false);
    m_descriptions.push_back(desc);

    // add specimen labels
    if(part->isType(SPECIMENS_T))
    {
        Specimens* arr = dynamic_cast<Specimens *>( part.getPtr() );
        unsigned int nx = arr->getSize();

        if (!arr->getSpecimenLabels(0)->empty())
        {
            Labels *lbl = arr->getSpecimenLabels(0);
            QStringList labelNames = lbl->keys();

            if (labelNames.size() > 0)
            {
                for (int i=0;i<labelNames.size();i++)
                {
                    QList<ILabelValue*> labelValues = lbl->values();
                    varName = QString("label%1").arg(getSize()).toStdString();
                    if (labelValues[i]->getType()==ILabelValue::STRING_VALUE)
                    {
                        value = PROTECT(Rf_allocVector(STRSXP, nx));
                        for (unsigned int j=0;j<arr->getSize();j++)
                        {
                            Labels *lbl2 = arr->getSpecimenLabels(j);
                            QStringList labelNames2 = lbl2->keys();
                            QList<ILabelValue*> labelValues2 = lbl2->values();

                            StringLabelValue * val = (StringLabelValue*)(labelValues2[i]);
                            SET_STRING_ELT(value,j,Rf_mkChar(val->getValue().toStdString().c_str()));
                        }
                    }
                    else if (labelValues[i]->getType()==ILabelValue::SCALAR_VALUE)
                    {
                        value = PROTECT(Rf_allocVector(REALSXP, nx));
                        for (unsigned int j=0;j<arr->getSize();j++)
                        {
                            Labels *lbl2 = arr->getSpecimenLabels(j);
                            QStringList labelNames2 = lbl2->keys();
                            QList<ILabelValue*> labelValues2 = lbl2->values();

                            ScalarLabelValue * val = (ScalarLabelValue*)(labelValues2[i]);
                            REAL(value)[j] = val->getValue();
                        }
                    }
                    addVar( varName, value, false);
                    m_descriptions.push_back(labelNames[i]);
                }
            }
        }
    }
}

#endif

#include "MMRNode.h"
#include "../../Utilities/Logger.h"
#include <sstream>
#include <QFileDialog>
#include <QDateTime>
#include <algorithm>
#include <QSettings>


#include "../../Labels/StringLabelValue.h"
#include "../../Labels/ScalarLabelValue.h"

#include "../../Utilities/asa063.hpp"   // incomplete beta function

#define RADIX 2.0
void _balanc(double **a, int n)
{
int last,j,i;
	double s,r,g,f,c,sqrdx;

	sqrdx=RADIX*RADIX;
	last=0;
	while (last == 0) {
		last=1;
		for (i=1;i<=n;i++) {
			r=c=0.0;
			for (j=1;j<=n;j++)
				if (j != i) {
					//c += fabs(a[j][i]);r += fabs(a[i][j]);
					c += abs(a[j][i]);r += abs(a[i][j]);
				}
			if (c && r) {
				g=r/RADIX;
				f=1.0;
				s=c+r;
				while (c<g) {
					f *= RADIX;
					c *= sqrdx;
				}
				g=r*RADIX;
				while (c>g) {
					f /= RADIX;
					c /= sqrdx;
				}
				if ((c+r)/f < 0.95*s) {
					last=0;
					g=1.0/f;
					for (j=1;j<=n;j++) a[i][j] *= g;
					for (j=1;j<=n;j++) a[j][i] *= f;
				}
			}
		}
	}

}


#define SWAP(g,h) {y=(g);(g)=(h);(h)=y;}

void _elmhes(double **a, int n)
/*Reduction to Hessenberg form by the elimination method. The real, nonsymmetric matrix
a[1..n][1..n] is replaced by an upper Hessenberg matrix with identical eigenvalues. Recommended,
but not required, is that this routine be preceded by balanc. On output, the
Hessenberg matrix is in elements a[i][j] with i ¡Ü j+1. Elements with i > j+1 are to be
thought of as zero, but are returned with random values.*/
{
    int m,j;
	double  y;

	for (m=2;m<n;m++) {
		int i=m;
		double x=0.0;
		for (j=m;j<=n;j++) {
			if (fabs(a[j][m-1]) > fabs(x)) {
				x=a[j][m-1];
				i=j;
			}
		}
		if (i != m) {
			for (j=m-1;j<=n;j++) SWAP(a[i][j],a[m][j])
			for (j=1;j<=n;j++) SWAP(a[j][i],a[j][m])
		}
		if (x) {
			for (i=m+1;i<=n;i++) {
				if (y=a[i][m-1]) {
					y /= x;
					a[i][m-1]=y;
					for (j=m;j<=n;j++)
						a[i][j] -= y*a[m][j];
					for (j=1;j<=n;j++)
						a[j][m] += y*a[j][i];
				}
			}
		}
	}

}

#define SIGN(a,b) ((b) > 0 ? fabs(a) : -fabs(a))

void _hqr(double **a, int n, double wr[], double wi[])

{
	int nn,m,l,k,j,i,mmin;
	double z,y,x,w,v,u,t,s,r,q,p,anorm;

anorm=fabs(a[1][1]);
	for (i=2;i<=n;i++)
		for (j=(i-1);j<=n;j++)
			anorm += fabs(a[i][j]);
    nn=n;
	t=0.0;

	while (nn >= 1) {
		int its=0;
		do {
			for (l=nn;l>=2;l--) {
				s=fabs(a[l-1][l-1])+fabs(a[l][l]);
				if (s == 0.0) s=anorm;
				if (fabs(a[l][l-1]) + s == s) break;
			}
			x=a[nn][nn];
			if (l == nn) {
				wr[nn]=x+t;
				wi[nn--]=0.0;
			} else {
				y=a[nn-1][nn-1];
				w=a[nn][nn-1]*a[nn-1][nn];
				if (l == (nn-1)) {
					p=0.5*(y-x);
					q=p*p+w;
					z=sqrt(fabs(q));
					x += t;
					if (q >= 0.0) {
						z=p+SIGN(z,p);
						wr[nn-1]=wr[nn]=x+z;
						if (z) wr[nn]=x-w/z;
						wi[nn-1]=wi[nn]=0.0;
					} else {
						wr[nn-1]=wr[nn]=x+p;
						wi[nn-1]= -(wi[nn]=z);
					}
					nn -= 2;
				} else {
					if (its == 50) break;
					if (its == 10 || its == 20) {
						t += x;
						for (i=1;i<=nn;i++) a[i][i] -= x;
						s=fabs(a[nn][nn-1])+fabs(a[nn-1][nn-2]);
						y=x=0.75*s;
						w = -0.4375*s*s;
					}
					++its;
					for (m=(nn-2);m>=l;m--) {
						z=a[m][m];
						r=x-z;
						s=y-z;
						p=(r*s-w)/a[m+1][m]+a[m][m+1];
						q=a[m+1][m+1]-z-r-s;
						r=a[m+2][m+1];
						s=fabs(p)+fabs(q)+fabs(r);
						p /= s;
						q /= s;
						r /= s;
						if (m == l) break;
						u=fabs(a[m][m-1])*(fabs(q)+fabs(r));
						v=fabs(p)*(fabs(a[m-1][m-1])+fabs(z)+fabs(a[m+1][m+1]));
						if (u+v == v) break;
					}
					for (i=m+2;i<=nn;i++) {
						a[i][i-2]=0.0;
						if  (i != (m+2)) a[i][i-3]=0.0;
					}
					for (k=m;k<=nn-1;k++) {
						if (k != m) {
							p=a[k][k-1];
							q=a[k+1][k-1];
							r=0.0;
							if (k != (nn-1)) r=a[k+2][k-1];
							if (x=fabs(p)+fabs(q)+fabs(r)) {
								p /= x;
								q /= x;
								r /= x;
							}
						}
						if (s=SIGN(sqrt(p*p+q*q+r*r),p)) {
							if (k == m) {
								if (l != m)
								a[k][k-1] = -a[k][k-1];
							} else
								a[k][k-1] = -s*x;
							p += s;
							x=p/s;
							y=q/s;
							z=r/s;
							q /= p;
							r /= p;
							for (j=k;j<=nn;j++) {
								p=a[k][j]+q*a[k+1][j];
								if (k != (nn-1)) {
									p += r*a[k+2][j];
									a[k+2][j] -= p*z;
								}
								a[k+1][j] -= p*y;
								a[k][j] -= p*x;
							}
							mmin = nn<k+3 ? nn : k+3;
							for (i=l;i<=mmin;i++) {
								p=x*a[i][k]+y*a[i][k+1];
								if (k != (nn-1)) {
									p += z*a[i][k+2];
									a[i][k+2] -= p*r;
								}
								a[i][k+1] -= p*q;
								a[i][k] -= p;
							}
						}
					}
				}
			}
		} while (l < nn-1);
	}
}



MMRNode::MMRNode(QWidget* parent) : QDialog(parent),  INode()
{
    m_specimens = NULL;
    m_additionalVariables = NULL;

    m_regressionCoefficients = new Loadings();
    m_predictedValues = new Specimens();
    m_residuals = new Specimens();
    m_RSqr = new Variables();
    m_standardisedSpecimens = new Specimens();

    setupUi(this);

    listWidget->setSelectionMode(QAbstractItemView::MultiSelection);
    listWidget_2->setSelectionMode(QAbstractItemView::MultiSelection);

    QObject::connect(radioButton, SIGNAL(clicked()), this, SLOT(leftRadioChanged()));
    QObject::connect(radioButton_2, SIGNAL(clicked()), this, SLOT(leftRadioChanged()));
    QObject::connect(radioButton_3, SIGNAL(clicked()), this, SLOT(leftRadioChanged()));
    QObject::connect(radioButton_4, SIGNAL(clicked()), this, SLOT(rightRadioChanged()));
    QObject::connect(radioButton_5, SIGNAL(clicked()), this, SLOT(rightRadioChanged()));
    QObject::connect(radioButton_6, SIGNAL(clicked()), this, SLOT(rightRadioChanged()));
    QObject::connect(listWidget, SIGNAL(itemSelectionChanged()), this, SLOT(controlsChanged()));
    QObject::connect(listWidget_2, SIGNAL(itemSelectionChanged()), this, SLOT(controlsChanged()));
    QObject::connect(lineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(controlsChanged()));
    QObject::connect(lineEdit_2, SIGNAL(textChanged(const QString&)), this, SLOT(standardValueChanged()));

    QObject::connect(comboBox, SIGNAL(activated(int)), this, SLOT(comboChanged()));
    QObject::connect(pushButton, SIGNAL(clicked()), this, SLOT(browseOutputFile()));

    QObject::connect(listWidget, SIGNAL(itemSelectionChanged()), this, SLOT(updateComboBox()));
    QObject::connect(listWidget_2, SIGNAL(itemSelectionChanged()), this, SLOT(updateComboBox()));


    leftRadioChanged();
    rightRadioChanged();

    lineEdit_2->setText("0.0");
    m_updated = true;
    m_standardised = false;
}

MMRNode::~MMRNode()
{
}

void MMRNode::Regress()
{
    if ( radioButton_3->isChecked() &&  radioButton_6->isChecked() )
    {
        if ( listWidget->selectedItems().empty() && listWidget_2->selectedItems().empty() )
        {
            Logger::getInstance()->log("[Regression Node] No variables selected for independent or dependent variables.", Logger::WARNING);
            m_updated = false;
            return;
        }
    }
    if (radioButton_3->isChecked() && listWidget->selectedItems().empty())
    {
        Logger::getInstance()->log("[Regression Node] No variables selected for dependent variables.", Logger::WARNING);
        m_updated = false;
        return;
    }
    if (radioButton_6->isChecked() && listWidget_2->selectedItems().empty())
    {
        Logger::getInstance()->log("[Regression Node] No variables selected for independent variables.", Logger::WARNING);
        m_updated = false;
        return;
    }

    if (m_Y.GetRows() != m_X.GetRows())
    {
        Logger::getInstance()->log("[Regression Node] Number of observations for independent and dependent data must be the same.", Logger::WARNING);
        m_updated = false;
        return;
    }

    int numindependent = m_X.GetCols();
    int numdependent = m_Y.GetCols();
    int rows = m_Y.GetRows();

    m_mmr.LoadData(m_X.GetMatrix(),m_Y.GetMatrix(),numindependent,numdependent,rows);

    bool constantIsZero = false;
    m_mmr.PerformRegression(constantIsZero);

    m_mmr.GetBeta(m_regressionCoefficients);

    Matrix<double> predictions;
    Matrix<double> residuals;
    m_mmr.GetPredictions(&predictions);
    m_mmr.GetResiduals(&residuals);

    m_predictedValues->clear();
    m_residuals->clear();

    int landmarks = m_specimens->getLandmarkCount();
    int dimensions = m_specimens->getLandmarkDimensions();
    int index = 0;
    //! double x0 = lineEdit_2->text().toDouble();

    if (comboBox->currentIndex() > 0)
    {
        index = comboBox->currentIndex();
    }

    for (unsigned int i=0;i<m_specimens->getSize();i++)
    {
        LandmarkSet pred(landmarks,dimensions);
        LandmarkSet res(landmarks,dimensions);
        //! LandmarkSet *lmks = m_specimens->getLandmarkSet(i);

        for (int j=0;j<landmarks;j++)
        {
            for (int k=0;k<dimensions;k++)
            {
                pred[j][k] = predictions[i][j*dimensions+k];
                res[j][k] = residuals[i][j*dimensions+k];
            }
        }
        m_predictedValues->addLandmarkSet(new LandmarkSet(pred));
        m_residuals->addLandmarkSet(new LandmarkSet(res));
    }

    m_predictedValues->setSpecimenLabels(m_specimens->getSpecimenLabels());
    m_predictedValues->setLandmarkLabels(m_specimens->getLandmarkLabels());
    m_residuals->setSpecimenLabels(m_specimens->getSpecimenLabels());
    m_residuals->setLandmarkLabels(m_specimens->getLandmarkLabels());

    for (unsigned int i=0; i<m_specimens->getGroupNumber(); ++i)    //Add Groups
    {
        SpecimenGroup* sg = m_specimens->getGroup(i);
        SpecimenGroup* newGroup = new SpecimenGroup;
        SpecimenGroup* newGroup2 = new SpecimenGroup;
        if (sg!=0)
        {
//            m_independent->addSpecimenGroup(m_specimens->getGroupName(i), m_specimens->getGroup(i));
//            m_dependent->addSpecimenGroup(m_specimens->getGroupName(i), m_specimens->getGroup(i));
        }
        for (unsigned int j=0; j<sg->getSize(); ++j)
        {
            unsigned int specimenIndex = sg->getMember(j)->getSpecimenIndex();
            if (specimenIndex == m_specimens->getSize())
            {
                newGroup->addMean(m_specimens->getSize(),
                                  sg->getMemberID(j),
                                  sg->getMemberColor(j),
                                  sg->getMemberViewerSize(j),
                                  sg->getMemberPlotSymbol(j));
                newGroup2->addMean(m_specimens->getSize(),
                                   sg->getMemberID(j),
                                   sg->getMemberColor(j),
                                   sg->getMemberViewerSize(j),
                                   sg->getMemberPlotSymbol(j));
            }
            else
            {
                newGroup->addMember(  m_predictedValues->getLandmarkSet(specimenIndex),
                                      sg->getMemberID(j),
                                      sg->getMemberColor(j),
                                      sg->getMemberViewerSize(j),
                                      sg->getMemberPlotSymbol(j));
                newGroup2->addMember( m_residuals->getLandmarkSet(specimenIndex),
                                      sg->getMemberID(j),
                                      sg->getMemberColor(j),
                                      sg->getMemberViewerSize(j),
                                      sg->getMemberPlotSymbol(j));
            }
        }
        newGroup->setGroupColor(sg->getGroupColor());
        newGroup->setGroupLMKSize(sg->getGroupLMKSize());
        newGroup->setGroupPlotSymbol(sg->getGroupPlotSymbol());
        newGroup2->setGroupColor(sg->getGroupColor());
        newGroup2->setGroupLMKSize(sg->getGroupLMKSize());
        newGroup2->setGroupPlotSymbol(sg->getGroupPlotSymbol());

        m_predictedValues->addGroup(m_specimens->getGroupName(i), newGroup);
        m_residuals->addGroup(m_specimens->getGroupName(i), newGroup2);
    }
    m_predictedValues->initialize("Regressed Specimens");
    m_residuals->initialize("Regression Residuals");

    vector<double> rsqr;
    m_mmr.GetRSqr(&rsqr);
    m_RSqr->ReSize(rsqr.size(),1);
    for (unsigned int i=0;i<rsqr.size();i++)
    {
        (*m_RSqr)[i][0] = rsqr[i];
    }

    // ignore first column of regression coefficients
    Matrix<double> Z(m_regressionCoefficients->GetRows()-1,m_regressionCoefficients->GetCols());
    for (unsigned int i=0;i<m_regressionCoefficients->GetCols();i++)
    {
        for (unsigned int j=0;j<m_regressionCoefficients->GetRows()-1;j++)
        {
            Z[j][i] = (*m_regressionCoefficients)[j+1][i];
        }
    }
    // transpose for warper
    Z = Transpose(Z);

    m_regressionCoefficients->ReSize(Z.GetRows(),Z.GetCols());

    for (unsigned int i=0;i<Z.GetRows();i++)
    {
        for (unsigned int j=0;j<Z.GetCols();j++)
        {
            (*m_regressionCoefficients)[i][j] = Z[i][j];
        }
    }

}

void MMRNode::process()
{
    m_specimens = getInputPortData0();

    if (m_specimens == NULL || !m_specimens->isValid())
    {
        m_updated = false;
        return;
    }

    Logger::getInstance()->log("[Regression Node] Processing started");


    if (listWidget->count() == 0)
    {
        FillLeftList();
    }

    if (listWidget_2->count() == 0)
    {
        FillRightList();
    }

    if (comboBox->count() == 0)
    {
        FillComboBox();
    }

    if (m_updated)
    {
        LoadData();
        Regress();

        setOutputPort0(m_regressionCoefficients);
        setOutputPort1(m_predictedValues);
        setOutputPort2(m_residuals);
        setOutputPort3(m_standardisedSpecimens);
        setOutputPort4(m_RSqr);
    }

    if (lineEdit->text() != "")
    {
        PrintOutput();
    }

    m_updated = false;

    if(!m_standardised)
    {
        standardise();
    }

    Logger::getInstance()->log("[Regression Node] Processing finished");
}

void MMRNode::LoadData()
{
    if (radioButton->isChecked())   // shape variables
    {
        int dimensions = m_specimens->getLandmarkDimensions();
        int landmarks = m_specimens->getLandmarkCount();
        int individuals = m_specimens->getSize();
        m_Y.ReSize(individuals,dimensions*landmarks);

        for (int i=0;i<individuals;i++)
        {
            LandmarkSet *llm = m_specimens->getLandmarkSet(i);
            for (int j=0;j<landmarks;j++)
            {
                for (int k=0;k<dimensions;k++)
                {
                    m_Y[i][j*dimensions+k] = (*llm)[j][k];
                }
            }
        }
    }
    else if (radioButton_2->isChecked())   // form variables
    {
        int dimensions = m_specimens->getLandmarkDimensions();
        int landmarks = m_specimens->getLandmarkCount();
        int individuals = m_specimens->getSize();
        m_Y.ReSize(individuals,dimensions*landmarks+1);

        for (int i=0;i<individuals;i++)
        {
            LandmarkSet *llm = m_specimens->getLandmarkSet(i);
            for (int j=0;j<landmarks;j++)
            {
                for (int k=0;k<dimensions;k++)
                {
                    m_Y[i][j*dimensions+k] = (*llm)[j][k];
                }
            }
            m_Y[i][landmarks*dimensions] = log(m_specimens->getCentroidSize(i));
        }
    }
    else    // other variables
    {
        int individuals = m_specimens->getSize();
        int lcols = listWidget->selectedItems().count();

        if (individuals > 0 && lcols > 0)
        {
            m_Y.ReSize(individuals,lcols);

            // label values
            Labels *lbls = m_specimens->getSpecimenLabels(0);
            QStringList labelNames = lbls->keys();

            if (!labelNames.empty())
            {
                // first create a matrix of label values
                Matrix<double> labelMatrix(individuals,labelNames.size());

                QVector<QStringList> strvalues(m_specimens->getSpecimenLabels(0)->size());
                for (unsigned int i=0;i<m_specimens->getSize();i++)
                {
                    Labels *lbl = m_specimens->getSpecimenLabels(i);
                    QList<ILabelValue*> labelValues = lbl->values();

                    for (int j=0;j<labelValues.size();j++)
                    {
                        if (labelValues[j]->getType()==ILabelValue::STRING_VALUE)
                        {
                            StringLabelValue * val = dynamic_cast< StringLabelValue* >( labelValues[j] );
                            if (!strvalues[j].contains(val->getValue()))
                            {
                                labelMatrix[i][j] = strvalues[j].size();
                                strvalues[j].push_back(val->getValue());
                            }
                            else
                            {
                                labelMatrix[i][j] = strvalues[j].indexOf(val->getValue());
                            }
                        }
                        else if (labelValues[j]->getType()==ILabelValue::SCALAR_VALUE)
                        {
                            ScalarLabelValue * val = dynamic_cast< ScalarLabelValue* >( labelValues[j] );
                            labelMatrix[i][j] = val->getValue();
                        }
                    }
                }

                QList<QListWidgetItem *> leftSelected = listWidget->selectedItems();

                vector<int> leftIndexes;
                for (int i=0;i<leftSelected.count();i++)
                {
                    leftIndexes.push_back(listWidget->row(leftSelected.at(i)));
                }
                sort(leftIndexes.begin(),leftIndexes.end());

                for (unsigned int j=0;j<leftIndexes.size();j++)
                {
                    if (leftIndexes[j]==0)
                    {
                        m_Y.setColumnLabel(j,"Centroid sizes");
                        for (int i=0;i<individuals;i++)
                        {
                            m_Y[i][j] = m_specimens->getCentroidSize(i);
                        }
                    }
                    else if (leftIndexes[j]==1)
                    {
                        m_Y.setColumnLabel(j,"Ln of centroid sizes");
                        for (int i=0;i<individuals;i++)
                        {
                            m_Y[i][j] = log(m_specimens->getCentroidSize(i));
                        }
                    }
                    else if (leftIndexes[j] < (int)labelMatrix.GetCols()+2)
                    {
                        m_Y.setColumnLabel(j,labelNames[leftIndexes[j]-2]);
                        for (int i=0;i<individuals;i++)
                        {
                            m_Y[i][j] = labelMatrix[i][leftIndexes[j]-2];
                        }
                    }
                    else
                    {
                        m_Y.setColumnLabel(j,m_additionalVariables->getColumnLabel(leftIndexes[j]-2-labelMatrix.GetCols()));
                        for (int i=0;i<individuals;i++)
                        {
                            m_Y[i][j] = (*m_additionalVariables)[i][leftIndexes[j]-2-labelMatrix.GetCols()];
                        }
                    }
                }
            }
        }
    }

    if (radioButton_4->isChecked())   // shape variables
    {
        int dimensions = m_specimens->getLandmarkDimensions();
        int landmarks = m_specimens->getLandmarkCount();
        int individuals = m_specimens->getSize();

        m_X.ReSize(individuals,dimensions*landmarks);

        for (int i=0;i<individuals;i++)
        {
            LandmarkSet *rlm = m_specimens->getLandmarkSet(i);
            for (int j=0;j<landmarks;j++)
            {
                for (int k=0;k<dimensions;k++)
                {
                    m_X[i][j*dimensions+k] = (*rlm)[j][k];
                }
            }
        }
    }
    else if (radioButton_5->isChecked())   // form variables
    {
        int dimensions = m_specimens->getLandmarkDimensions();
        int landmarks = m_specimens->getLandmarkCount();
        int individuals = m_specimens->getSize();

        m_X.ReSize(individuals,dimensions*landmarks+1);

        for (int i=0;i<individuals;i++)
        {
            LandmarkSet *rlm = m_specimens->getLandmarkSet(i);
            for (int j=0;j<landmarks;j++)
            {
                for (int k=0;k<dimensions;k++)
                {
                    m_X[i][j*dimensions+k] = (*rlm)[j][k];
                }
            }
            m_X[i][landmarks*dimensions] = log(m_specimens->getCentroidSize(i));
        }
    }
    else    // other variables
    {
        int individuals = m_specimens->getSize();
        int rcols = listWidget_2->selectedItems().count();

        if (individuals > 0 && rcols > 0)
        {
            m_X.ReSize(individuals,rcols);

            // label values
            Labels *lbls = m_specimens->getSpecimenLabels(0);
            QStringList labelNames = lbls->keys();

            if (!labelNames.empty())
            {
                // first create a matrix of label values
                Matrix<double> labelMatrix(individuals,labelNames.size());

                QVector<QStringList> strvalues(m_specimens->getSpecimenLabels(0)->size());
                for (unsigned int i=0;i<m_specimens->getSize();i++)
                {
                    Labels *lbl = m_specimens->getSpecimenLabels(i);
                    QList<ILabelValue*> labelValues = lbl->values();

                    for (int j=0;j<labelValues.size();j++)
                    {
                        if (labelValues[j]->getType()==ILabelValue::STRING_VALUE)
                        {
                            StringLabelValue * val = dynamic_cast< StringLabelValue* >( labelValues[j] );
                            if (!strvalues[j].contains(val->getValue()))
                            {
                                labelMatrix[i][j] = strvalues[j].size();
                                strvalues[j].push_back(val->getValue());
                            }
                            else
                            {
                                labelMatrix[i][j] = strvalues[j].indexOf(val->getValue());
                            }
                        }
                        else if (labelValues[j]->getType()==ILabelValue::SCALAR_VALUE)
                        {
                            ScalarLabelValue * val = dynamic_cast< ScalarLabelValue* >( labelValues[j] );
                            labelMatrix[i][j] = val->getValue();
                        }
                    }
                }

                QList<QListWidgetItem *> rightSelected = listWidget_2->selectedItems();

                vector<int> rightIndexes;
                for (int i=0;i<rightSelected.count();i++)
                {
                    rightIndexes.push_back(listWidget_2->row(rightSelected.at(i)));
                }
                sort(rightIndexes.begin(),rightIndexes.end());

                for (unsigned int j=0;j<rightIndexes.size();j++)
                {
                    if (rightIndexes[j]==0)
                    {
                        m_X.setColumnLabel(j,"Centroid sizes");
                        for (int i=0;i<individuals;i++)
                        {
                            m_X[i][j] = m_specimens->getCentroidSize(i);
                        }
                    }
                    else if (rightIndexes[j]==1)
                    {
                        m_X.setColumnLabel(j,"Ln of centroid sizes");
                        for (int i=0;i<individuals;i++)
                        {
                            m_X[i][j] = log(m_specimens->getCentroidSize(i));
                        }
                    }
                    else if (rightIndexes[j] < (int)labelMatrix.GetCols()+2)
                    {
                        m_X.setColumnLabel(j,labelNames[rightIndexes[j]-2]);
                        for (int i=0;i<individuals;i++)
                        {
                            m_X[i][j] = labelMatrix[i][rightIndexes[j]-2];
                        }
                    }
                    else
                    {
                        m_X.setColumnLabel(j,m_additionalVariables->getColumnLabel(rightIndexes[j]-2-labelMatrix.GetCols()));
                        for (int i=0;i<individuals;i++)
                        {
                            m_X[i][j] = (*m_additionalVariables)[i][rightIndexes[j]-2-labelMatrix.GetCols()];
                        }
                    }
                }
            }
        }
    }
}

void MMRNode::FillComboBox()
{
    comboBox->clear();

    for(int i =0 ;i<listWidget_2->count();i++)
    {
        comboBox->addItem(listWidget_2->item(i)->text());
    }

    double average = 0;
    int individuals = m_specimens->getSize();
    if(m_specimens!=0)
    {
        for (int i=0;i<individuals;i++)
        {
            average += m_specimens->getCentroidSize(i);
        }
        average /= (double)individuals;
    }

    QString txt;
    txt.setNum(average);
    lineEdit_2->setText(txt);

}

void MMRNode::accept()
{
    if (m_updated)
        emit dataReady();
    QDialog::accept();
}

bool MMRNode::isUpdated() const
{
    return m_updated;
}

QString MMRNode::toString() const
{
    QString nodeData = "";

    if (radioButton->isChecked())
    {
        nodeData += "1";
    }
    else
    {
        nodeData += "0";
    }
    nodeData += "$$";
    if (radioButton_2->isChecked())
    {
        nodeData += "1";
    }
    else
    {
        nodeData += "0";
    }
    nodeData += "$$";
    if (radioButton_3->isChecked())
    {
        nodeData += "1";
    }
    else
    {
        nodeData += "0";
    }
    nodeData += "$$";
    if (radioButton_4->isChecked())
    {
        nodeData += "1";
    }
    else
    {
        nodeData += "0";
    }
    nodeData += "$$";
    if (radioButton_5->isChecked())
    {
        nodeData += "1";
    }
    else
    {
        nodeData += "0";
    }
    nodeData += "$$";
    if (radioButton_6->isChecked())
    {
        nodeData += "1";
    }
    else
    {
        nodeData += "0";
    }

    nodeData += "$$";
    nodeData += lineEdit->text();

    nodeData += "$$";
    nodeData += lineEdit_2->text();


    nodeData += "$$";
    QString idx;
    idx.setNum(comboBox->currentIndex());
    nodeData += idx;
    nodeData += "$$";

    Labels *lbl;
    QStringList labelNames;

    if(m_specimens != 0)
    {
    	lbl = m_specimens->getSpecimenLabels(0);
    	labelNames = lbl->keys();

    	nodeData += "Centroid sizes";
		nodeData += "|";
		nodeData += "Ln of centroid sizes";
		nodeData += "|";

		for (int i=0;i<labelNames.size();i++)
		{
			nodeData += labelNames[i];
			nodeData += "|";
		}

		if (m_additionalVariables != NULL && m_additionalVariables->isValid())
		{
			if (m_additionalVariables->GetRows() == m_specimens->getSize())
			{
				for (unsigned int i=0;i<m_additionalVariables->GetCols();i++)
				{
					nodeData += m_additionalVariables->getColumnLabel(i);
					nodeData += "|";
				}
			}
		}
		nodeData += "$$";
		for (int i=0;i<listWidget->count();i++)
		{
			if (listWidget->item(i)->isSelected())
			{
				nodeData += "1|";
			}
			else
			{
				nodeData += "0|";
			}
		}
		nodeData += "$$";
		for (int i=0;i<listWidget_2->count();i++)
		{
			if (listWidget_2->item(i)->isSelected())
			{
				nodeData += "1|";
			}
			else
			{
				nodeData += "0|";
			}
		}
    }

    return  nodeData;
}

void MMRNode::fromString(const QString& params)
{
/*
    QStringList values = params.split("$$");
    if (values.size() == 6)
    {
        radioButton->setChecked(values[0]=="1");
        radioButton_2->setChecked(values[1]=="1");
        radioButton_3->setChecked(values[2]=="1");
        radioButton_4->setChecked(values[3]=="1");
        radioButton_5->setChecked(values[4]=="1");
        radioButton_6->setChecked(values[5]=="1");
    }
    else if (values.size() > 6)
    {
        radioButton->setChecked(values[0]=="1");
        radioButton_2->setChecked(values[1]=="1");
        radioButton_3->setChecked(values[2]=="1");
        radioButton_4->setChecked(values[3]=="1");
        radioButton_5->setChecked(values[4]=="1");
        radioButton_6->setChecked(values[5]=="1");

        lineEdit->setText(values[6]);
        lineEdit_2->setText(values[7]);

        QStringList items = values[9].split("|");
        QStringList leftSelected = values[10].split("|");
        QStringList rightSelected = values[11].split("|");

        for (int i=0;i<leftSelected.count();i++)
        {
            listWidget->addItem(items[i]);
            listWidget_2->addItem(items[i]);
        }

        for (int i=0;i<listWidget->count();i++)
        {
            if (leftSelected[i] == "1")
                listWidget->setItemSelected ( listWidget->item(i), true );
        }

        for (int i=0;i<listWidget_2->count();i++)
        {
            if (rightSelected[i] == "1")
            {
                listWidget_2->setItemSelected ( listWidget_2->item(i), true );
                comboBox->addItem(items[i]);
            }
        }
        comboBox->setCurrentIndex(values[8].toInt());

        leftRadioChanged();
        rightRadioChanged();
    }
    m_updated = true;
*/
}

bool MMRNode::inputChanged(int inputIndex)
{
    switch (inputIndex)
    {
    case 0: // Specimens
        FillLeftList();
        FillRightList();
        FillComboBox();
        break;
    case 1: // Additional variables
        FillLeftList();
        FillRightList();
        FillComboBox();
        break;
    default:
        break;
    }
    return true;
}

void MMRNode::FillRightList()
{
    m_specimens = getInputPortData0();
    if (m_specimens == NULL || !m_specimens->isValid())
    {
        return;
    }

    QVector<int> sel;
    for (int i=0;i<listWidget_2->count();i++)
    {
        if (listWidget_2->item(i)->isSelected())
            sel.push_back(i);
    }

    listWidget_2->clear();
    listWidget_2->addItem("Centroid sizes");
    listWidget_2->addItem("Ln of centroid sizes");

    Labels *lbl = m_specimens->getSpecimenLabels(0);
    QStringList labelNames = lbl->keys();

    for (int i=0;i<labelNames.size();i++)
    {
        listWidget_2->addItem(labelNames[i]);
    }

    m_additionalVariables = getInputPortData1();
    if (m_additionalVariables == NULL || !m_additionalVariables->isValid())
    {
        // do nothing
    }
    else
    {
        if (m_additionalVariables->GetRows() == m_specimens->getSize())
        {
            for (unsigned int i=0;i<m_additionalVariables->GetCols();i++)
            {
                listWidget_2->addItem(m_additionalVariables->getColumnLabel(i));
            }
        }
    }

    for (int i=0;i<sel.size();i++)
    {
        listWidget_2->setItemSelected ( listWidget_2->item(sel[i]), true );
    }
}

void MMRNode::FillLeftList()
{
    m_specimens = getInputPortData0();

    if (m_specimens == NULL || !m_specimens->isValid())
    {
        return;
    }

    QVector<int> sel;
    for (int i=0;i<listWidget->count();i++)
    {
        if (listWidget->item(i)->isSelected())
            sel.push_back(i);
    }

    listWidget->clear();
    listWidget->addItem("Centroid sizes");
    listWidget->addItem("Ln of centroid sizes");

    Labels *lbl = m_specimens->getSpecimenLabels(0);
    QStringList labelNames = lbl->keys();

    for (int i=0;i<labelNames.size();i++)
    {
        listWidget->addItem(labelNames[i]);
    }

    m_additionalVariables = getInputPortData1();

    if (m_additionalVariables == NULL || !m_additionalVariables->isValid())
    {
        // do nothing
    }
    else
    {
        if (m_additionalVariables->GetRows() == m_specimens->getSize())
        {
            for (unsigned int i=0;i<m_additionalVariables->GetCols();i++)
            {
                listWidget->addItem(m_additionalVariables->getColumnLabel(i));
            }
        }
    }
    for (int i=0;i<sel.size();i++)
    {
        listWidget->setItemSelected ( listWidget->item(sel[i]), true );
    }
}


void MMRNode::PrintOutput()
{
    QString fileName = lineEdit->text();
    QFile file(fileName);

    ofstream outfile(fileName.toStdString().c_str());

    if (outfile.fail())
    {
        Logger::getInstance()->log("[Regression Node] Error opening output file: " + fileName, Logger::RUN_ERROR);
        return;
    }

    //get current date
    QDate date = QDate::currentDate();
    QString dateString = date.toString();

    //get current time
    QTime time = QTime::currentTime();
    QString timeString = time.toString();

    //get current date and time
    QDateTime dateTime = QDateTime::currentDateTime();
    QString dateTimeString = dateTime.toString();

    outfile << "EVAN Toolbox" << endl;
    outfile << "Multiple Multivariate Regression" << endl;

    outfile << endl;

    outfile << "Date:\t" << dateString.toStdString() << endl;
    outfile << "Time:\t" << timeString.toStdString() << endl;

    outfile << endl;

    outfile << "Dependent variables (Y)" << endl;
    outfile << m_Y << endl;

    outfile << endl;

    outfile << "Independent variables (X)" << endl;
    outfile << m_X << endl;

    outfile << endl;

//    outfile << "R squared" << endl;
//    outfile << (*m_RSqr) << endl;

    outfile << endl;





    Matrix<double> y = m_Y;
    Matrix<double> x = m_X;

    int m_numIndependent = x.GetCols();
    int m_numDependent = y.GetCols();
    int m_rows = y.GetRows();
    bool constantIsZero = false;

    // Degrees of Freedom
    int df = m_rows - m_numIndependent - 1;

    if (constantIsZero)
    {
        x = m_X;
        df++;
    }
    else
    {
        x.ReSize(m_rows,m_numIndependent+1);
        for (int i=0;i<m_rows;i++)
        {
            x[i][0] = 1;
            for (int j=0;j<m_numIndependent;j++)
            {
                x[i][j+1] = m_X[i][j];
            }
        }
    }


    Matrix<double> xtx = Transpose(x) * x;
    Matrix<double> xty = Transpose(x) * y;
    Matrix<double> yty = Transpose(y) * y;
    Matrix<double> invxtx = Inverse(xtx);

    Matrix<double> beta = invxtx * xty;

    Matrix<double> btxty = Transpose(beta) * Transpose(x) * y;

    Matrix<double> rss = yty-btxty;
    Matrix<double> rms = rss/(double)(m_rows-m_numIndependent);

    Matrix<double> FStat(m_numDependent,m_numDependent);
    Matrix<double> Rsqr(m_numDependent,m_numDependent);
    Matrix<double> AdjRsqr(m_numDependent,m_numDependent);
    Matrix<double> tss(m_numDependent,m_numDependent);
    Matrix<double> ess(m_numDependent,m_numDependent);

    Row<double> meany = Mean(y);

    if (constantIsZero)
    {
        for ( int i = 0; i < m_numDependent; i++)
        {
            //diagonals store relevant values
            for (int j = 0; j < m_numDependent; j++)
            {
                tss[i][j] = yty[i][j];
                ess[i][j] = btxty[i][j]; // explained variance (regression sum of squares)
                if (fabs(tss[i][j]) > 1e-8)
                {
                    Rsqr[i][j] = ess[i][j] / tss[i][j];
                }
                else
                {
                    Rsqr[i][j] = 0.0;
                }
                AdjRsqr[i][j] = 1.0 - (1.0 - Rsqr[i][j]) * ((m_rows) / (double)(m_rows - m_numIndependent));
                FStat[i][j] = (Rsqr[i][j] / (double)m_numIndependent) / ((1.0 - Rsqr[i][j]) / (double)(m_rows - m_numIndependent));
            }
        }
    }
    else
    {
        for ( int i = 0; i < m_numDependent; i++)
        {
            //diagonals store relevant values
            for (int j = 0; j < m_numDependent; j++)
            {
                tss[i][j] = yty[i][j] - (m_rows * (meany[j] * meany[j]));
                ess[i][j] = btxty[i][j] - (m_rows* (meany[j] * meany[j])); // explained variance (regression sum of squares)
                if (fabs(tss[i][j]) > 1e-8)
                {
                    Rsqr[i][j] = ess[i][j] / tss[i][j];
                }
                else
                {
                    Rsqr[i][j] = 0.0;
                }
                AdjRsqr[i][j] = 1.0 - (1.0 - Rsqr[i][j]) * ((m_rows - 1.0) / (m_rows- m_numIndependent - 1.0));
                FStat[i][j] = (Rsqr[i][j] / m_numIndependent) / ((1.0 - Rsqr[i][j]) / (m_rows - m_numIndependent - 1.0));
            }
        }
    }

    // Compute Standard Error for the betas - values held in diagonal
    int nr = invxtx.GetRows();
    int nc = rms.GetCols();
    Matrix<double> seB(nr,nc);
    Matrix<double> tStatB(nr,nc);

    for ( int i = 0; i < nr; i++)
    {
        for ( int j = 0; j < nc; j++)
        {
            if (fabs(rms[j][j]*invxtx[i][i]) > 1e-8)
            {
                if (constantIsZero)
                {
                    seB[i][j] = sqrt(rms[j][j]*invxtx[i][i]);
                }
                else
                {
                    seB[i][j] = sqrt(rms[j][j]*invxtx[i][i]*(m_rows-m_numIndependent)/(m_rows-m_numIndependent-1));
                }
                tStatB[i][j] = beta[i][j] / seB[i][j]; //element-wise division
            }
            else
            {
                seB[i][j] = 0.0;
                tStatB[i][j] = 0.0;
            }
        }
    }

    // Compute Standard Error for the y estimate

    Row<double> seY(m_numDependent);

    Matrix<double> xb = x*beta;
    Row<double> meanxb = Mean(xb);

    if (constantIsZero)
    {
        for (int j = 0; j < m_numDependent; j++)
        {
            double sumxx = 0.0;
            double sumyy = 0.0;
            double sumxy = 0.0;
            for (int i = 0; i < m_rows; i++)
            {
                sumxx += (xb[i][j]-meanxb[j]) * (xb[i][j]-meanxb[j]);
                sumyy += y[i][j] * y[i][j];
                sumxy += y[i][j] * (xb[i][j]-meanxb[j]);
            }
            seY[j] = sqrt( ( sumyy - (sumxy*sumxy/sumxx) ) /(m_rows - m_numIndependent));
        }
    }
    else
    {
        for (int j = 0; j < m_numDependent; j++)
        {
            double sumxx = 0.0;
            double sumyy = 0.0;
            double sumxy = 0.0;
            for (int i = 0; i < m_rows; i++)
            {
                sumxx += (xb[i][j]-meanxb[j]) * (xb[i][j]-meanxb[j]);
                sumyy += (y[i][j]-meany[j]) * (y[i][j]-meany[j]);
                sumxy += (y[i][j]-meany[j]) * (xb[i][j]-meanxb[j]);
            }
            seY[j] = sqrt( ( sumyy - (sumxy*sumxy/sumxx) ) /(m_rows - m_numIndependent - 1));
        }
    }

    Matrix<double> tProbB(nr,nc);

    for (int i = 0; i < nr; i++)
    {
        for (int j = 0; j < nc; j++)
        {
            double xval = df / (df + tStatB[i][j] * tStatB[i][j]);
            double a = df / 2.0;
            double b = 0.5;
            // incomplete beta: calc integrals in gaussian curves
            // check out: http://mathworld.wolfram.com/BetaFunction.html

            int ifault = 0;
            double beta_log = alogam ( a, &ifault )
             + alogam ( b, &ifault )
             - alogam ( a + b, &ifault );
            tProbB[i][j] = betain ( xval, a, b, beta_log, &ifault );
        }
    }

    // F-test probs
    Matrix<double> FProb(m_numDependent,m_numDependent);
    Matrix<double> FProb2(m_numDependent,m_numDependent);

    for (int i = 0; i < m_numDependent; i++)
    {
        double a = df / 2.0;
        double b = m_numIndependent / 2.0;
        double xval = df / (df + m_numIndependent * FStat[i][i]);

        int ifault = 0;
        double beta_log = alogam ( a, &ifault )
             + alogam ( b, &ifault )
             - alogam ( a + b, &ifault );
        FProb[i][i] = betain ( xval, a, b, beta_log, &ifault );
    }

    Matrix<double> WilksLambda = ess*Inverse(rss);

    Row<double> D(m_numDependent);

    Matrix<double> Hi(m_numDependent+1,m_numDependent+1);
    for (int i = 0; i < m_numDependent; i++)
    {
        for (int j = 0; j < m_numDependent; j++)
        {
            Hi[i+1][j+1] = WilksLambda[i][j];
        }
    }

    double Er[m_numDependent+1];
    double Ei[m_numDependent+1];

    // balance matrix, convert to Hessenburg form then use QR decomposition

    _balanc(Hi.GetMatrix(), m_numDependent);
    _elmhes(Hi.GetMatrix(), m_numDependent);
    _hqr(Hi.GetMatrix(), m_numDependent, Er, Ei);

    for (int i = 0; i < m_numDependent; i++)
    {
        D[i] = Er[i+1];
    }

    double test1 = 0.0;
    double test2 = 0.0;
    double test3 = 0.0;
    double test4 = 0.0;
    double tmp1 = 0.0;
    double tmp2 = 0.0;

    //calculate wilks lambda and pillai trace etc
    for (size_t i = 0u; i < D.GetCols(); i++)
    {
        tmp1 = 1.0 / (1.0 + D[i]);
        tmp2 = D[i] / (1.0 + D[i]);
        if (fabs(test1) < 1e-8)
        {
            test1 = tmp1;
        }
        else
        {
            test1 *= tmp1;
        }

        test2 += tmp2;      //Pillai's trace
        test3 += D[i];      //Hotelling-Lawley trace
        if (D[i] > test4)
        {
            test4 = D[i];   //Roy's largest root
        }
    }

    // Now for the F-stat of Wilks Lambda
    tmp1 = 0.0;
    tmp2 = 0.0;
    double FTestWilks = 0.0;
    double s = 0.0;

    /*
    Rao's F-test for Wilk's

    Ra = [ (1 -  L^1/s) /  L^1/s ] x [ (1 + t s - p q / 2) / p q ]

    where	t=tmp1 = (n-1) - (p + q + 1) / 2,
            s = 1 if p^2 + q^2 <= 5; otherwise s = [(p^2 q^2 -4)/(p^2 + q^2 - 5)]^0.5 ,
            n = the number of observations,
            p = the number of X variables (i.e., the rank of X), and
            q = the number of Y variables (i.e., the rank of Y).

        Ra has an approximate F-distribution with p q degrees of freedom in the numerator and 1 + t s - p q / 2 degrees of freedom in the denominator.  When p = 1 or 2 or when q = 1 or 2, then Ra has an exact F-distribution.
    */

    if ((m_numIndependent * m_numIndependent) + (m_numDependent * m_numDependent) > 5 )
    {
        s = sqrt(((m_numIndependent * m_numIndependent)*(m_numDependent * m_numDependent) - 4)/((m_numIndependent * m_numIndependent) + (m_numDependent * m_numDependent) - 5));
    }
    else
    {
        s = 1.0;
    }

    tmp2 = ((m_rows - 2) -(m_numDependent - m_numIndependent + 2) / 2)*s - (m_numIndependent*m_numDependent - 2) / 2;
    tmp1 = m_numDependent * m_numIndependent;
    FTestWilks = ((1.0 - pow(test1, 1.0/s)) / pow(test1, 1.0/s))*(tmp2/tmp1);

    double a = tmp2/2.0;
    double b = tmp1/2.0;
    double xval = tmp2 / (tmp2 + tmp1 * FTestWilks);

    int ifault = 0;
    double beta_log = alogam ( a, &ifault )
             + alogam ( b, &ifault )
             - alogam ( a + b, &ifault );
    double FProbWilks = betain ( xval, a, b, beta_log, &ifault );


    double m_WilksLambda = test1;
    double m_PillaisTrace = test2;
    double m_HotellingLawleyTrace = test3;
    double m_RoysLargestRoot = test4;


    int numIndependent = m_numIndependent;

    if (!constantIsZero)
    {
        numIndependent++;
    }


    for (int i=0;i<m_numDependent;i++)
    {
        outfile<<endl;
        outfile<<endl;
        outfile<<"Dependent Variable "<< i+1 <<": "<< m_Y.getColumnLabel(i).toStdString() <<endl;

        outfile<<"Coefficients"<<endl;

        for (int j =0; j<numIndependent;j++)
        {
            outfile<<"\tBeta["<<j<<"]:\t"<< beta[j][i]<<endl;
            outfile<<"\t\tStandard error:\t"<< seB[j][i] <<endl;
            outfile<<"\t\tt-stat:\t"<< tStatB[j][i] <<endl;
            outfile<<"\t\tp-value:\t"<< tProbB[j][i] <<endl;
        }

        outfile<<"R-Square:\t"<<Rsqr[i][i] <<endl;
        outfile<<"Adjusted R-Square:\t"<<AdjRsqr[i][i] <<endl;
        outfile<<"F-test:\t"<<FStat[i][i] <<endl;
        outfile<<"p-value for F-test:\t"<<FProb[i][i] <<endl;
        outfile<<"Residual Sum of Squares:\t"<<rss[i][i] <<endl;
        outfile<<"Error Sum of Squares:\t"<<ess[i][i] <<endl;
        outfile<<"Residual Mean Squares:\t"<<rms[i][i] <<endl;
    }
    outfile<<endl;
//    for (int j =0; j<m_numIndependent;j++)
//    {
//        outfile<<"Standard error for Y["<<j<<"]:\t"<< m_standardErrorY[j][j]<<endl;
//    }
//    outfile<<endl;

//    outfile<<"Multivariate Statistics:"<<endl;

//    outfile<< "Wilk's Lambda:\t" << m_WilksLambda << "\t" <<endl;
//    outfile<< "\tF-test\t" << FTestWilks <<endl;
//    outfile<< "\tThe incomplete Beta function ratio\t" << FProbWilks <<endl;

//    s = 0.0;
//    if ((m_numIndependent * m_numIndependent) + (m_numDependent * m_numDependent) > 5 )
//    {
//        s = sqrt(((m_numIndependent * m_numIndependent)*(m_numDependent * m_numDependent) - 4)/((m_numIndependent * m_numIndependent) + (m_numDependent * m_numDependent) - 5));
//    }
//    else
//    {
//        s = 1.0;
//    }

//    int df1 = m_numDependent * m_numIndependent;
//    int df2 = ((m_rows - 2) -(m_numDependent - m_numIndependent + 2) / 2)*s - (m_numIndependent*m_numDependent - 2) / 2;

//    outfile<< "\tDF1:\t" << df1 <<endl;
//    outfile<< "\tDF2:\t" << df2 <<endl;

//    outfile<< "Pillai's Trace:\t" << m_PillaisTrace <<endl;
//    outfile<< "Hotelling Lawley's Trace:\t" << m_HotellingLawleyTrace <<endl;
//    outfile<< "Roy's Largest Root:\t" << m_RoysLargestRoot <<endl;



    double SS_err = 0.0;
    double SS_tot = 0.0;

    for (int j = 0; j < m_numDependent; j++)
        {
            for (int i = 0; i < m_rows; i++)
            {
                SS_err += (y[i][j]-xb[i][j]) * (y[i][j]-xb[i][j]);
                SS_tot += (y[i][j]-meany[j]) * (y[i][j]-meany[j]);
            }
       }

    double explained_var = (1.0 - SS_err/SS_tot)*100;

    outfile<< "Total sum of squares "<<SS_tot<<endl;
    outfile<< explained_var <<" % of the total variance is accounted for by the selected independent variable(s)."<<endl;

//    outfile<< "Total sample variance explained by this regression:\t" << bla <<" %"<<endl;

    outfile.close();

    Logger::getInstance()->log("[Regression Node] Output written to file: " + fileName);
}

void MMRNode::standardise()
{
    Matrix<double> X;
    Matrix<double> Y;
    int dimensions = m_specimens->getLandmarkDimensions();
    int landmarks = m_specimens->getLandmarkCount();
    int individuals = m_specimens->getSize();
    Y.ReSize(individuals,dimensions*landmarks);
    X.ReSize(individuals,1);

    int index = comboBox->currentIndex();

    if(index==0)
    {
        for (int i=0;i<individuals;i++)
        {
            X[0][i] =  m_specimens->getCentroidSize(i);
        }
    }
    else if(index==1)
    {
        for (int i=0;i<individuals;i++)
        {
            X[0][i] =  log(m_specimens->getCentroidSize(i));
        }
    }
    else
    {
        // label values
        Labels *lbls = m_specimens->getSpecimenLabels(0);
        QStringList labelNames = lbls->keys();

        if (!labelNames.empty())
        {
            // first create a matrix of label values
            Matrix<double> labelMatrix(individuals,labelNames.size());

            QVector<QStringList> strvalues(m_specimens->getSpecimenLabels(0)->size());
            for (unsigned int i=0;i<m_specimens->getSize();i++)
            {
                Labels *lbl = m_specimens->getSpecimenLabels(i);
                QList<ILabelValue*> labelValues = lbl->values();

                for (int j=0;j<labelValues.size();j++)
                {
                    if (labelValues[j]->getType()==ILabelValue::STRING_VALUE)
                    {
                        StringLabelValue * val = dynamic_cast< StringLabelValue* >( labelValues[j] );
                        if (!strvalues[j].contains(val->getValue()))
                        {
                            labelMatrix[i][j] = strvalues[j].size();
                            strvalues[j].push_back(val->getValue());
                        }
                        else
                        {
                            labelMatrix[i][j] = strvalues[j].indexOf(val->getValue());
                        }
                    }
                    else if (labelValues[j]->getType()==ILabelValue::SCALAR_VALUE)
                    {
                        ScalarLabelValue * val = dynamic_cast< ScalarLabelValue* >( labelValues[j] );
                        labelMatrix[i][j] = val->getValue();
                    }
                }
            }

            if (index < (int)labelMatrix.GetCols()+2)
            {
                for (int i=0;i<individuals;i++)
                {
                    X[0][i] = labelMatrix[i][comboBox->currentIndex()-2];
                }
            }
            else
            {
                for (int i=0;i<individuals;i++)
                {
                    X[0][i] = (*m_additionalVariables)[i][index - 2 - labelMatrix.GetCols()];
                }
            }
        }
    }

    bool ok;
    double x0 = lineEdit_2->text().toDouble(&ok);
    if(!ok)
    {
        Logger::getInstance()->log("Cannot standardise with " + comboBox->currentText() + " as: " + lineEdit_2->text(),Logger::WARNING);
        return;
    }

    for (int i=0;i<individuals;i++)
    {
        LandmarkSet *llm = m_specimens->getLandmarkSet(i);
        for (int j=0;j<landmarks;j++)
        {
            for (int k=0;k<dimensions;k++)
            {
                Y[i][j*dimensions+k] = (*llm)[j][k];
            }
        }
    }

    MMR mmr;
    mmr.LoadData(X.GetMatrix(),Y.GetMatrix(),1,dimensions*landmarks,individuals);
    mmr.PerformRegression(false);
    Matrix<double> regressionCoefficients;
    mmr.GetBeta(&regressionCoefficients);

    m_standardisedSpecimens->clear();

    for (unsigned int i=0;i<m_specimens->getSize();i++)
    {
        LandmarkSet stand(landmarks,dimensions);
        LandmarkSet *lmks = m_specimens->getLandmarkSet(i);

        for (int j=0;j<landmarks;j++)
        {
            for (int k=0;k<dimensions;k++)
            {
                double beta = (regressionCoefficients)[1][j*dimensions+k];
                stand[j][k] = (*lmks)[j][k] - beta * (X[0][i] - x0);
            }
        }
        m_standardisedSpecimens->addLandmarkSet(new LandmarkSet(stand));
    }

    m_standardisedSpecimens->setSpecimenLabels(m_specimens->getSpecimenLabels());
    m_standardisedSpecimens->setLandmarkLabels(m_specimens->getLandmarkLabels());

    for (unsigned int i=0; i<m_specimens->getGroupNumber(); ++i)    //Add Groups
    {
        SpecimenGroup* sg = m_specimens->getGroup(i);
        SpecimenGroup* newGroup = new SpecimenGroup;

        for (unsigned int j=0; j<sg->getSize(); ++j)
        {
            unsigned int specimenIndex = sg->getMember(j)->getSpecimenIndex();
            if (specimenIndex == m_specimens->getSize())
            {
                newGroup->addMean(m_specimens->getSize(),
                                   sg->getMemberID(j),
                                   sg->getMemberColor(j),
                                   sg->getMemberViewerSize(j),
                                   sg->getMemberPlotSymbol(j));
            }
            else
            {
                newGroup->addMember( m_standardisedSpecimens->getLandmarkSet(specimenIndex),
                                      sg->getMemberID(j),
                                      sg->getMemberColor(j),
                                      sg->getMemberViewerSize(j),
                                      sg->getMemberPlotSymbol(j));
            }
        }
        newGroup->setGroupColor(sg->getGroupColor());
        newGroup->setGroupLMKSize(sg->getGroupLMKSize());
        newGroup->setGroupPlotSymbol(sg->getGroupPlotSymbol());

        m_standardisedSpecimens->addGroup(m_specimens->getGroupName(i), newGroup);
    }
    m_standardisedSpecimens->initialize("Standardised Specimens");

    m_standardised = true;

    setOutputPort3(m_standardisedSpecimens);
    emit dataReady();

}

void MMRNode::comboChanged()
{
    Matrix<double> X;
    int individuals = m_specimens->getSize();
    X.ReSize(individuals,1);

    int index = comboBox->currentIndex();

    if(index==0)
    {
        for (int i=0;i<individuals;i++)
        {
            X[0][i] =  m_specimens->getCentroidSize(i);
        }
    }
    else if(index==1)
    {
        for (int i=0;i<individuals;i++)
        {
            X[0][i] =  log(m_specimens->getCentroidSize(i));
        }
    }
    else
    {
        // label values
        Labels *lbls = m_specimens->getSpecimenLabels(0);
        QStringList labelNames = lbls->keys();

        if (!labelNames.empty())
        {
            // first create a matrix of label values
            Matrix<double> labelMatrix(individuals,labelNames.size());

            QVector<QStringList> strvalues(m_specimens->getSpecimenLabels(0)->size());
            for (unsigned int i=0;i<m_specimens->getSize();i++)
            {
                Labels *lbl = m_specimens->getSpecimenLabels(i);
                QList<ILabelValue*> labelValues = lbl->values();

                for (int j=0;j<labelValues.size();j++)
                {
                    if (labelValues[j]->getType()==ILabelValue::STRING_VALUE)
                    {
                        StringLabelValue * val = dynamic_cast< StringLabelValue* >( labelValues[j] );
                        if (!strvalues[j].contains(val->getValue()))
                        {
                            labelMatrix[i][j] = strvalues[j].size();
                            strvalues[j].push_back(val->getValue());
                        }
                        else
                        {
                            labelMatrix[i][j] = strvalues[j].indexOf(val->getValue());
                        }
                    }
                    else if (labelValues[j]->getType()==ILabelValue::SCALAR_VALUE)
                    {
                        ScalarLabelValue * val = dynamic_cast< ScalarLabelValue* >( labelValues[j] );
                        labelMatrix[i][j] = val->getValue();
                    }
                }
            }

            if (index < (int)labelMatrix.GetCols()+2)
            {
                for (int i=0;i<individuals;i++)
                {
                    X[0][i] = labelMatrix[i][comboBox->currentIndex()-2];
                }
            }
            else
            {
                for (int i=0;i<individuals;i++)
                {
                    X[0][i] = (*m_additionalVariables)[i][index - 2 - labelMatrix.GetCols()];
                }
            }
        }
    }

    double average = 0;
    for(int i=0;i<individuals;i++)
    {
        average += X[0][i];
    }
    average /= (double)individuals;

    QString txt;
    txt.setNum(average);
    lineEdit_2->setText(txt);

    m_standardised = false;
}

void MMRNode::browseOutputFile()
{
    QSettings settings("Evan");
    QStringList fileSettings = settings.value("lastDirectory").toStringList();
    QString fsLastDir = "";
    if( fileSettings.size() > 0 )
        fsLastDir = fileSettings[0];

    QString fileName = QFileDialog::getSaveFileName(this, tr("Select a file for Regression output"),
                       fsLastDir,
                       tr("All files (*)\nText files (*.txt)"));

    if (fileName != "" && !fileName.isEmpty())
    {
        lineEdit->setText(fileName);

        if(fileName.contains("/"))
        {
            QString dirName = fileName.left(fileName.lastIndexOf("/"));
            settings.setValue("lastDirectory",dirName);
        }
        else if(fileName.contains("\\"))
        {
            QString dirName = fileName.left(fileName.lastIndexOf("\\"));
            settings.setValue("lastDirectory",dirName);
        }
    }
}


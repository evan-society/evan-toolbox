#ifndef GPANODE_H
#define GPANODE_H

#include "../INode.h"
#include <QDialog>
#include "../../DataTypes/Specimens.h"
#include "../../DataTypes/TransformMatrix.h"
#include "../../DataTypes/CentroidSizes.h"
#include "../../DataTypes/Loadings.h"
#include "../../DataTypes/Surface.h"
#include "Designer/ui_gpa.h"
#include <gpa.h>



class GPANode : public QDialog , public INode, public Ui::GPADialog
{
    Q_OBJECT

private:
    Specimens* m_specimensInput;
    Surface *m_surfaceInput;
    Specimens* m_procCoordinatesOutput;
    TransformMatrix* m_transformsOutput;
    Specimens* m_residualsOutput;
    Variables* m_procDistancesOutput;
    CentroidSizes* m_centroidSizes;
    LandmarkSet* m_meanSpecimen;
    Specimens* m_uniformBasis;
    Surface *m_transformedSurface;
    Surface *m_registeredSurface;
    Variables* m_shapeVectors;
    Variables* m_formVectors;

    GPA m_gpa;
//    bool m_updated;
    int m_method;
    bool m_pairwise;
    int m_maxiter;
    double m_mintol;
    QString m_settings;

    void CalculateGPA();
    void PrintOutput();

protected:
    Specimens* getInputPortData0()  const { return dynamic_cast<Specimens *>( getInputData( 0 ) ); }
    Surface* getInputPortData1()    const { return dynamic_cast<Surface *>( getInputData( 1 ) ); }
    void setOutputPort0(Specimens* p)           {setOutputData(0,p);}
    void setOutputPort1(TransformMatrix* p)     {setOutputData(1,p);}
    void setOutputPort2(Specimens* p)           {setOutputData(2,p);}
    void setOutputPort3(Variables* p)           {setOutputData(3,p);}
    void setOutputPort4(CentroidSizes* p)       {setOutputData(4,p);}
    void setOutputPort5(LandmarkSet* p)         {setOutputData(5,p);}
    void setOutputPort6(Specimens* p)           {setOutputData(6,p);}
    void setOutputPort7(Surface* p)             {setOutputData(7,p);}
    void setOutputPort8(Surface* p)             {setOutputData(8,p);}
    void setOutputPort9(Variables* p)           {setOutputData(9,p);}
    void setOutputPort10(Variables* p)          {setOutputData(10,p);}

public:
    GPANode(QWidget* parent=0);

    void showGUI()
    {
        m_settings = toString();
        QWidget::activateWindow();
        QWidget::show();
    }
    const QObject* getDialog() const
    {
        return this;
    }
    void process();
    bool isUpdated() const;

	QString toString() const;
	void fromString(const QString&);


public slots:
    void accept();
    void reject();
    void controlsChanged()
    {
        m_updated = true;
    }

    void togglePairwise()
    {
        if(m_pairwise)
        {
            checkBox_6->setChecked(false);
            checkBox_7->setChecked(true);
            radioButton_5->setDisabled(true);
            radioButton_8->setDisabled(true);
            radioButton_4->setDisabled(false);
            radioButton_6->setDisabled(false);
            radioButton_7->setDisabled(false);
            m_pairwise = false;
        }
        else
        {
            checkBox_6->setChecked(true);
            checkBox_7->setChecked(false);
            radioButton_5->setDisabled(false);
            radioButton_8->setDisabled(false);
            radioButton_4->setDisabled(true);
            radioButton_6->setDisabled(true);
            radioButton_7->setDisabled(true);
            m_pairwise = true;
        }

        m_updated = true;
    }

    void browseOutputFile();

signals:
    void dataReady();
};

#endif

#include "WarperNode.h"
#include <QtGui>
#include <tps.h>
#include "../../Utilities/Logger.h"
#include "../../DataTypes/Loadings.h"
#include <QButtonGroup>
#include <QPushButton>

#include <cmath>

extern ofstream dbgfile; //RP 3/1/09 Toolbox debug file

WarperNode::WarperNode(QWidget* parent) : QTabWidget(parent), INode()
{
    setupUi(this);

	m_lmk1Input = NULL;
    m_warpVectorInput = NULL;
    m_surfaceInput = NULL;
    m_createDialog = NULL;
    m_warpFactors = NULL;
    m_convertedWarpings = new Warpings();

    m_warpGrids = new GroupWarpGrids();
    m_warpGrids->setMainGroup();
    m_surfaceOutput = new Surface();
    m_warpedLmksOutput = new LandmarkSet();
    m_range = 261;
    m_rangeValue = 5.0;

    morphSliders.clear();

    QObject::connect(rangePushButton, SIGNAL(clicked()), this, SLOT(setRange()));

    checkBox->setChecked(true);
    QObject::connect(checkBox , SIGNAL(clicked(bool)), this, SLOT(setDependency(bool)));

    m_useShadersCheck->setChecked(true);
    QObject::connect(m_useShadersCheck , SIGNAL(clicked(bool)), this, SLOT(useShader(bool)));

    //VD - switch warped space / unwarped space
    checkbox4->setChecked(false);
    QObject::connect(checkbox4, SIGNAL(clicked(bool)), this, SLOT(setIsUnwarped(bool)));

    treeWidget->setGroupWarpGrids(m_warpGrids);
//    connect(treeWidget, SIGNAL(membersUpdated()), this, SLOT(updateGrid()));
    connect(treeWidget, SIGNAL(showCreateGridDialog()), this, SLOT(setTobeCreated()));
    connect(treeWidget, SIGNAL(signalCloneGrid(WarpGrid*)), this, SLOT(cloneGrid(WarpGrid*)));
    connect(treeWidget, SIGNAL(parallelGrid(WarpGrid*)), this, SLOT(parallelGrid(WarpGrid*)));
    connect(treeWidget, SIGNAL(perpendicularGrid(WarpGrid*)), this, SLOT(perpendicularGrid(WarpGrid*)));
    connect(treeWidget, SIGNAL(propertyGrid(GridTreeItem*)), this, SLOT(propertyGrid(GridTreeItem*)));
    connect(treeWidget, SIGNAL(membersUpdated()), this, SLOT(membersUpdated()));

     // VD - grid
    setWindowTitle("Warper tool");
    setCurrentIndex(0);
    rangeLineEdit->setText("5.0");

    QScrollArea* scrollbar = new QScrollArea(this);
	QWidget *subWidget = new QWidget(this);
	layoutSliders = new QGridLayout(subWidget);
	scrollbar->setWidget(subWidget);
	layoutSliders->setColumnStretch(0, 2);
	layoutSliders->setColumnStretch(1, 20);
	layoutSliders->setColumnStretch(2, 2);
	layoutSliders->setColumnStretch(3, 2);
	scrollbar->setWidgetResizable(true);

	laySliders->addWidget(scrollbar);

	//VD - grid animation
	connect(pushButton_2, SIGNAL(clicked()), this, SLOT(setGridStartPosition()));
	connect(pushButton_3, SIGNAL(clicked()), this, SLOT(setGridStopPosition()));
	connect(pushButton_4, SIGNAL(clicked(bool)), this, SLOT(gridPlay(bool)));

    m_scaleAmounts.clear();

    m_dependent = false;
    m_isUnwarped = false;

    m_plotterWarp = false;
    m_formSpace = false;

    formSpaceCheckBox = new QCheckBox("Scale in Form Space");
    connect(formSpaceCheckBox, SIGNAL(clicked()), this, SLOT(toggleShadersCheck()));
    QObject::connect(pushButton, SIGNAL(released()), this, SLOT(playAll()));
}


WarperNode::~WarperNode()
{clearTPS();}

void WarperNode::setDependency(bool checked)
{
    m_dependent = !checked;
}

void WarperNode::enterEvent(QEvent* event)
{
    QWidget::enterEvent(event);
    treeWidget->updateName();
}

void WarperNode::processBaseLmks()
{
    m_lmk1Input = getInputPortData0();
    if(m_lmk1Input == NULL)
        return;
    if(!m_lmk1Input->isValid())
        return;
    m_warpedLmksOutput->ReSize(m_lmk1Input->GetRows(), m_lmk1Input->GetCols());
    for(unsigned int i=0; i<m_lmk1Input->GetRows(); ++i)
        for(unsigned int j=0; j<m_lmk1Input->GetCols(); ++j)
            m_warpedLmksOutput->set(i,j, m_lmk1Input->get(i,j));
    m_warpedLmksOutput->initialize("Warped Landmarks");
    setOutputPort0(m_warpedLmksOutput);

    // VD - grid
    if (m_warpGrids->getNumGrid()>0)
    {
        for (int i=0;i<m_warpGrids->getNumGrid();i++){
            if (m_warpGrids->getGrid(i))
            {
                m_warpGrids->getGrid(i)->setReferenceLMarks(m_lmk1Input);
                m_warpGrids->getGrid(i)->setWarpedLMarks(m_warpedLmksOutput);
            }
        }
    }
}

void WarperNode::processWarps()
{
    m_warpVectorInput = getInputPortData1();
    m_lmk1Input = getInputPortData0();

    if(m_warpVectorInput == NULL || m_lmk1Input == NULL)
        return;
    if(!m_warpVectorInput->isValid() || !m_lmk1Input->isValid())
        return;

    m_convertedWarpings->clear();
    m_morphAmount.clear();
    for (int i=0;i<morphSliders.count();i++)
        delete morphSliders[i];
    morphSliders.clear();
    for (int i=0;i<morphAmountLineEdit.count();i++)
        delete morphAmountLineEdit[i];
    morphAmountLineEdit.clear();
    for (int i=0;i<morphLabel.count();i++)
        delete morphLabel[i];
    morphLabel.clear();
    for (int i=0;i<m_animationWidget.count();i++)
        delete m_animationWidget[i];
    m_animationWidget.clear();
    for (int i=0;i<m_playButton.count();i++)
        delete m_playButton[i];
    m_playButton.clear();

    //Conversion from Loadings to Warp vectors (difference vectors)
    for (unsigned int i=0; i<m_warpVectorInput->getSize(); ++i)
    {
        MatrixD* loadings = m_warpVectorInput->getWarp(i);
        if(loadings->isType(IDataType::LOADINGS_T))
        {
            if (m_lmk1Input->GetRows()*m_lmk1Input->GetCols() == loadings->GetRows() )
            {
                m_formSpace = false;
                if(formSpaceCheckBox->isVisible())
                {
                    horizontalLayout_3->removeWidget(formSpaceCheckBox);
                    formSpaceCheckBox->setVisible(false);
                    m_useShadersCheck->setDisabled(false);
                }
            }
            else if (m_lmk1Input->GetRows()*m_lmk1Input->GetCols() + 1 == loadings->GetRows() )
            {
                // assume we have form vectors
                m_formSpace = true;
                if(!formSpaceCheckBox->isVisible())
                {
                    horizontalLayout_3->addWidget(formSpaceCheckBox);
                    formSpaceCheckBox->setVisible(true);
                    m_useShadersCheck->setDisabled(true);
                }
            }
            else
            {
                Logger::getInstance()->log("[Warper Node] Loadings and Source landmark configurations do not match!", Logger::RUN_ERROR);
                return;
            }

            int numscores = ( dynamic_cast< Loadings* >( loadings) )->getNumScores();

            if(numscores == 0)
            {
                numscores = ( dynamic_cast< Loadings* >( loadings) )->GetCols();
            }
            for(int j=0; j<numscores; ++j)
            {
                MatrixD* warpVector = new MatrixD(m_lmk1Input->GetRows(), m_lmk1Input->GetCols());
                for(unsigned int x=0; x<m_lmk1Input->GetRows(); ++x)
                    for(unsigned int y=0; y<m_lmk1Input->GetCols(); ++y)
                        warpVector->set(x, y, loadings->get((x*m_lmk1Input->GetCols())+y,j));
                        //warpVector->set(x, y, loadings->get((x*m_lmk1Input->GetCols())+y,j));
                m_convertedWarpings->addWarp(warpVector);

                if(m_formSpace)
                {
                    m_convertedWarpings->addSizeLoadings((*loadings)[loadings->GetRows()-1][j]);
                }
            }

            // 26.06.09 hpg - get scale values from min and max scores
            vector<double> minrange;
            vector<double> maxrange;
            minrange = *( dynamic_cast< Loadings* >( loadings) )->getMin();
            maxrange = *( dynamic_cast< Loadings* >( loadings) )->getMax();
            m_scaleAmounts.resize(maxrange.size());
            for(int k=0;k<m_scaleAmounts.size();k++)
            {
                if(abs(maxrange[k])>abs(minrange[k]))
                    m_scaleAmounts[k] = abs(maxrange[k]);
                else
                    m_scaleAmounts[k] = abs(minrange[k]);
            }
        }
        else m_convertedWarpings->addWarp(loadings);
    }
    //////////////////////Conversion End/////////////////////////////////////

    for (unsigned int i=0;i<m_convertedWarpings->getSize();i++)
        if (m_lmk1Input->GetRows() != m_convertedWarpings->getWarp(i)->GetRows())
        {
            Logger::getInstance()->log("[Warper Node] Warp Vectors and base landmark configurations do not match!", Logger::RUN_ERROR);
            return;
        }

    QStringList animParamList;
    if (m_animParam != "")
        animParamList = m_animParam.split("_");

    for (unsigned int i=0;i<m_convertedWarpings->getSize();i++)
    {
        // sliders
        QSlider* sl = createSlider();
        sl->setMinimumWidth(200);
        QLabel* la = new QLabel(QString::number(m_morphAmount.size()+1));
        QLineEdit* am = new QLineEdit("0.0");
        morphSliders.push_back(sl);
        morphAmountLineEdit.push_back(am);
        morphLabel.push_back(la);
        QPushButton* button = new QPushButton("+");
        button->setCheckable(true);
        button->setChecked(false);
        layoutSliders->addWidget(la,2*m_morphAmount.size(),0);
        layoutSliders->addWidget(sl,2*m_morphAmount.size(),1);
        layoutSliders->addWidget(am,2*m_morphAmount.size(),2);
        layoutSliders->addWidget(button,2*m_morphAmount.size(),3);
        QObject::connect(sl, SIGNAL(valueChanged(int)), this, SLOT(setMorph(int)));
        QObject::connect(am, SIGNAL(editingFinished()), this, SLOT(setSlider()));
        m_morphAmount.push_back(0);

        // animation
        AnimationWidget* animWidget = new AnimationWidget(this);
        if (i<(unsigned int)animParamList.count())
            animWidget->fromString(animParamList[i]);
        m_animationWidget.push_back(animWidget);
        animWidget->setGeometry(QRect(0,0,layoutSliders->geometry().width(),40));
        layoutSliders->addWidget(animWidget,2*m_morphAmount.size()-1,0,1,5);
        animWidget->hide();
        QObject::connect(button, SIGNAL(clicked(bool)), animWidget, SLOT(setVisible(bool)));
        QObject::connect(animWidget->getMinValueButton(), SIGNAL(clicked()), this, SLOT(setMinValue()));
        QObject::connect(animWidget->getMaxValueButton(), SIGNAL(clicked()), this, SLOT(setMaxValue()));
        QObject::connect(animWidget->getStartValueButton(), SIGNAL(clicked()), this, SLOT(setStartValue()));
        QObject::connect(animWidget->getPlayButton(), SIGNAL(clicked(bool)), this, SLOT(playAnimation(bool)));
    }
    layoutSliders->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding),2*m_morphAmount.size()+2,1);
    setMinimumSize(600,400);
    setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Preferred);
}

void WarperNode::playAll()
{
    for (int i=0;i<m_animationWidget.size();++i)
        if (m_animationWidget[i]->isActive())
            m_animationWidget[i]->getPlayButton()->click();
}

void WarperNode::playAnimation(bool play)
{
    m_timer.start(50);
    QPushButton* buttonSender = dynamic_cast<QPushButton*>(sender());
    if (buttonSender)
    {
        int i=-1;
        bool notFound=true;
        while (notFound)
        {
            i++;
            if (m_animationWidget[i]->getPlayButton() == buttonSender)
                notFound=false;
        }
        if (play)
        {
            m_animationWidget[i]->setCurrent(m_animationWidget[i]->getStartValue());
            morphAmountLineEdit[i]->setText(QString::number(m_animationWidget[i]->getStartValue()));
            setSlider(i);

            switch(m_animationWidget[i]->getPolicy1())
            {
                case 0:
                    m_animationWidget[i]->setCurrentMovement(0);
                    m_animationWidget[i]->setNextMovement(0);
                break;
                case 1:
                    m_animationWidget[i]->setCurrentMovement(1);
                    m_animationWidget[i]->setNextMovement(1);
                break;
                case 2:
                    m_animationWidget[i]->setCurrentMovement(0);
                    m_animationWidget[i]->setNextMovement(1);
                break;
                case 3:
                    m_animationWidget[i]->setCurrentMovement(1);
                    m_animationWidget[i]->setNextMovement(0);
                break;
                default:break;
            }
            m_animationWidget[i]->setNbCycle(0);
            m_animation.push_back(i);
            if (m_animation.size()==1)
                connect(&m_timer,SIGNAL(timeout()),this,SLOT(moveAnimation()));
        }
        else
        {
            if (m_animation.indexOf(i)>=0)
                m_animation.remove(m_animation.indexOf(i));
             if (m_animation.size()==0)
                disconnect(&m_timer,SIGNAL(timeout()),this,SLOT(moveAnimation()));
        }
    }
}

void WarperNode::moveAnimation()
{
    for (int j=0;j<m_animation.size();j++)
    {
        int i=m_animation[j];
        if (m_animationWidget[i]->getPauseButton()->isChecked())
            continue;
        double current = m_animationWidget[i]->getCurrent();
        double delta = m_animationWidget[i]->getMaxValue() - m_animationWidget[i]->getMinValue();
        double delta_t = 20 * m_animationWidget[i]->getLengthValue();
        int policy2 = m_animationWidget[i]->getPolicy2();

        if (current>m_animationWidget[i]->getMaxValue())
        {
            bool next=m_animationWidget[i]->getNextMovement();
            m_animationWidget[i]->setNextMovement(m_animationWidget[i]->getCurrentMovement());
            m_animationWidget[i]->setCurrentMovement(next);
            if (!next)
                current=m_animationWidget[i]->getMinValue();
        }
        else if (current<m_animationWidget[i]->getMinValue())
        {
            bool next=m_animationWidget[i]->getNextMovement();
            m_animationWidget[i]->setNextMovement(m_animationWidget[i]->getCurrentMovement());
            m_animationWidget[i]->setCurrentMovement(next);
            if (next)
                current=m_animationWidget[i]->getMaxValue();
        }

        if (policy2==0)
        {
            int policy1 = m_animationWidget[i]->getPolicy1();
            if (abs(current - m_animationWidget[i]->getStartValue())<0.5*delta/delta_t)
                m_animationWidget[i]->setNbCycle(m_animationWidget[i]->getNbCycle()+1);
            if ((policy1 == 0 || policy1 == 1) && m_animationWidget[i]->getNbCycle()==2)
                m_animationWidget[i]->getPlayButton()->click();
            else if (m_animationWidget[i]->getNbCycle()==3)
                m_animationWidget[i]->getPlayButton()->click();
        }
        if (m_animationWidget[i]->getCurrentMovement())
            m_animationWidget[i]->setCurrent(current-delta/delta_t);
        else
            m_animationWidget[i]->setCurrent(current+delta/delta_t);
        morphAmountLineEdit[i]->setText(QString::number(m_animationWidget[i]->getCurrent()));
        setSlider(i);
    }
}

void WarperNode::setMinValue()
{
    QPushButton* buttonSender = dynamic_cast<QPushButton*>(sender());
    if (buttonSender)
    {
        int i=-1;
        bool notFound=true;
        while (notFound)
        {
            i++;
            if (m_animationWidget[i]->getMinValueButton() == buttonSender)
                notFound=false;
        }
        m_animationWidget[i]->setMinValue(QString::number(0.01*m_morphAmount[i]));
        m_animationWidget[i]->setStartValue(QString::number(0.01*m_morphAmount[i]));
        m_animationWidget[i]->setActive(true);
    }
}

void WarperNode::setMaxValue()
{
    QPushButton* buttonSender = dynamic_cast<QPushButton*>(sender());
    if (buttonSender)
    {
        int i=-1;
        bool notFound=true;
        while (notFound)
        {
            i++;
            if (m_animationWidget[i]->getMaxValueButton() == buttonSender)
                notFound=false;
        }
        m_animationWidget[i]->setMaxValue(QString::number(0.01*m_morphAmount[i]));
        m_animationWidget[i]->setActive(true);
    }
}

void WarperNode::setStartValue()
{
    QPushButton* buttonSender = dynamic_cast<QPushButton*>(sender());
    if (buttonSender)
    {
        int i=-1;
        bool notFound=true;
        while (notFound)
        {
            i++;
            if (m_animationWidget[i]->getStartValueButton() == buttonSender)
                notFound=false;
        }
        if (m_morphAmount[i] <= (int) (100*m_animationWidget[i]->getMaxValue()) &&
            m_morphAmount[i] >= (int) (100*m_animationWidget[i]->getMinValue()))
        m_animationWidget[i]->setStartValue(QString::number(0.01*m_morphAmount[i]));
    }
}

void WarperNode::setGridStartPosition()
{
    m_warpGrids->setStartPosition();
    for (int i=0;i<m_warpGrids->getNumGrid();++i)
        m_warpGrids->getGrid(i)->setStartPosition();
    for (int g=0;g<m_warpGrids->getNumGroupGrid();++g)
        for (int i=0;i<m_warpGrids->getGroupGrid(g)->getNumGrid();++i)
        {
            m_warpGrids->getGroupGrid(g)->setStartPosition();
            m_warpGrids->getGroupGrid(g)->getGrid(i)->setStartPosition();
        }
}

void WarperNode::setGridStopPosition()
{
    m_warpGrids->setEndPosition();
    for (int i=0;i<m_warpGrids->getNumGrid();++i)
        m_warpGrids->getGrid(i)->setEndPosition();
    for (int g=0;g<m_warpGrids->getNumGroupGrid();++g)
        for (int i=0;i<m_warpGrids->getGroupGrid(g)->getNumGrid();++i)
         {
            m_warpGrids->getGroupGrid(g)->setEndPosition();
            m_warpGrids->getGroupGrid(g)->getGrid(i)->setEndPosition();
        }
}

void WarperNode::gridPlay(bool b)
{
    m_timer.start(50);
    if (b)
    {
        switch(comboBox->currentIndex())
        {
            case 0:
                m_currentMovement=0;
                m_nextMovement=0;
                m_interpolationParameter=0;
            break;
            case 1:
                m_currentMovement=1;
                m_nextMovement=1;
                m_interpolationParameter=1;
            break;
            case 2:
                m_currentMovement=0;
                m_nextMovement=1;
                m_interpolationParameter=0;
            break;
            case 3:
                m_currentMovement=1;
                m_nextMovement=0;
                m_interpolationParameter=1;
            break;
            default:break;
        }
        connect(&m_timer,SIGNAL(timeout()),this,SLOT(playGridAnimation()));
        m_warpGrids->stopTimer();
    }
    else
    {
        disconnect(&m_timer,SIGNAL(timeout()),this,SLOT(playGridAnimation()));
        m_warpGrids->startTimer();
    }
}

void WarperNode::playGridAnimation()
{
    if (pushButton_5->isChecked())
        return;
    m_warpGrids->setInterpolatedTransfMatrix(m_interpolationParameter);
    for (int i=0;i<m_warpGrids->getNumGrid();++i)
            m_warpGrids->getGrid(i)->setInterpolatedTransfMatrix(m_interpolationParameter);
    for (int g=0;g<m_warpGrids->getNumGroupGrid();++g)
        for (int i=0;i<m_warpGrids->getGroupGrid(g)->getNumGrid();++i)
        {
            m_warpGrids->getGroupGrid(g)->setInterpolatedTransfMatrix(m_interpolationParameter);
            m_warpGrids->getGroupGrid(g)->getGrid(i)->setInterpolatedTransfMatrix(m_interpolationParameter);
        }
    m_warpGrids->update();

    m_interpolationParameter+= (m_currentMovement?-0.05:0.05)/lineEdit->text().toFloat();

    if (m_interpolationParameter<=0 || m_interpolationParameter>=1)
    {
        bool next=m_nextMovement;
        m_nextMovement=m_currentMovement;
        m_currentMovement=next;

        if (m_interpolationParameter<=0 && next)
            m_interpolationParameter=1;
        else if (m_interpolationParameter>=1 && !next)
            m_interpolationParameter=0;

        if (!comboBox_2->currentIndex())
            if (((comboBox->currentIndex()==0 || comboBox->currentIndex()==2) && m_interpolationParameter<=0)
                || ((comboBox->currentIndex()==1 || comboBox->currentIndex()==3) && m_interpolationParameter>=1))
                {
                    disconnect(&m_timer,SIGNAL(timeout()),this,SLOT(playGridAnimation()));
                    pushButton_4->click();
                }
    }
}

void WarperNode::processSurface()
{
    m_surfaceInput = getInputPortData2();
    if (m_surfaceInput && m_surfaceInput->isValid())
    {
        m_surfaceOutput->clear();
        m_surfaceOutput->initialize(m_surfaceInput, "Warped Surface");
        setOutputPort1(m_surfaceOutput);
    }
}

void WarperNode::process()
{
    Logger::getInstance()->log("[Warper Node] Processing started");

    processBaseLmks();
    processWarps();
    doTPS();
    processSurface();
    bindFullWarp();
    setOutputPort2(m_warpGrids);

    Logger::getInstance()->log("[Warper Node] Processing finished");
}

void WarperNode::setMorph(int n=0)
{
    // hpg: 28.06.09
    if (m_dependent && !m_isUnwarped)
    {
        for (int k=0;k<morphSliders.size();k++)
        {
            morphSliders.value(k)->blockSignals(true);
            morphSliders.value(k)->setValue(n);
            morphSliders.value(k)->blockSignals(false);
            if (abs(n) <= m_range)
            {
                float newValue=convertMorphSlider(n);

                if (k < m_scaleAmounts.size())
                    newValue *= m_scaleAmounts[k];

                morphAmountLineEdit[k]->setText(QString().setNum(newValue,'g',2));
                m_morphAmount.replace(k,(int) (100.0*newValue));
            }
        }
        doWarp();
        return;
    }

    if(m_plotterWarp)
        // do not calculate new values when using plotter
        return;

    QSlider *slider = qobject_cast<QSlider *>(sender());
    if (!slider)
        return;

    int rank = morphSliders.indexOf(slider);

    if (abs(n) <= m_range)
    {
        float warpAmounts = convertMorphSlider(n);

        if (rank < m_scaleAmounts.size())
            warpAmounts *= m_scaleAmounts[rank];

        morphAmountLineEdit[rank]->setText(QString().setNum(warpAmounts,'g',2));
        m_morphAmount.replace(rank,(int) (100.0*warpAmounts));
    }
    doWarp();
}

inline float WarperNode::convertMorphSlider(int n)
{
    return (float) ((abs(n)<=100) ? n/100.0 : ((n>0) ? 1 : -1) * exp(((n>0) ? 1 : -1) * n/100.0 - 1.0));
}

void WarperNode::setSlider()
{
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(sender());
    if (!lineEdit)
        return;
    int i=morphAmountLineEdit.indexOf(lineEdit);
    if (i<0)
        return;
    setSlider(i);
}

void WarperNode::setSlider(int i)
{
    bool ok;
    float value = morphAmountLineEdit[i]->text().toFloat(&ok);
    if (ok)
    {
        if (i < m_scaleAmounts.size())
            value /= m_scaleAmounts[i];
        if (abs(value) <= 1.0)
            morphSliders[i]->setValue((int) (value*100));
        else
        {
            int value2 = (int) roundf(100.0 * (log(abs(value))+1.0));
            if ( value2 <= m_range)
                morphSliders[i]->setValue((int)(abs(value)/value) * value2);
            else
                morphAmountLineEdit[i]->setText("Value outside range!");
        }
    }
}

void WarperNode::setRange()
{
    float value;
    bool ok;
    value = rangeLineEdit->text().toFloat(&ok);
    if (ok)
    {
        if (value<=1.00)
        {
            m_range = 100;
            rangeLineEdit->setText("1.00");
        }
        else
            m_range = (int) (100.0 * log((float) value)+100.0);
        for (int i=0;i<morphSliders.size();i++)
            morphSliders.value(i)->setRange(-m_range,+m_range);

        m_rangeValue = value;
    }
}

//!Create a basic Slider
/*!
Create a slider with pre-configured options
*/
QSlider* WarperNode::createSlider()
{
    QSlider *slider = new QSlider(Qt::Horizontal);
    slider->setRange(-261, +261);// range: -5 to 5
    slider->setSingleStep(100);
    slider->setPageStep(100);
    slider->setTickInterval(100);
    slider->setTickPosition(QSlider::TicksBelow);
    slider->setMinimumWidth(50);
    return slider;
}

void WarperNode::computeOutput()
{
    m_warpFactors = getInputPortData3();

    if (m_warpFactors == NULL || !m_warpFactors->isValid() )
        return;

    m_plotterWarp = true;
    for (int i=0; i<morphSliders.size(); ++i)
    {
        double factor = m_warpFactors->getFloat(i);
        morphAmountLineEdit[i]->setText(QString().setNum(factor,'g',2));
        m_morphAmount.replace(i,(int) (100.0*factor));

        // change slider position
        if (i < m_scaleAmounts.size())
        {
            if (m_scaleAmounts[i] > 1.0e-10)
            {
                int sliderPosition = (int) (factor / m_scaleAmounts[i] * m_range / m_rangeValue);
                morphSliders.value(i)->setValue(sliderPosition);
            }
            else
            {
                morphSliders.value(i)->setValue(0);
            }
        }
        else if (abs(factor) <= 1.0)
        {
            morphSliders.value(i)->setValue((int) (factor*100));
        }
        else
        {
            int value2 = (int) roundf(100.0 * (log(abs(factor))+1.0));
            if (value2 <= m_range)
                morphSliders.value(i)->setValue((int)(abs(factor)/factor) * value2);
            else
                morphAmountLineEdit.value(i)->setText("Value outside range!");
        }
    }
    m_plotterWarp = false;
    doWarp();
}

// VD - grid

void WarperNode::setTobeCreated()
{
    if(m_convertedWarpings == NULL || m_warpedLmksOutput == NULL || m_lmk1Input == NULL)
        return;
    if(!m_convertedWarpings->isValid() || !m_warpedLmksOutput->isValid() || !m_lmk1Input->isValid())
        return;

    if (!m_createDialog)
        m_createDialog = new CreateGridDialog(this);

    m_createDialog->setMaxLmkIndex(m_lmk1Input->GetRows());

    QString name("Grid ");
    int total=m_warpGrids->getNumGrid();
    for (int i=0;i<m_warpGrids->getNumGroupGrid();++i)
        total+=m_warpGrids->getGroupGrid(i)->getNumGrid();
    name.append(QString::number(total+1));
    m_createDialog->lineEdit->setText(name);

    m_createDialog->exec();
    if (!m_createDialog->result())
        return;
    bool ok;
    float valueSizeRatio = m_createDialog->lineEdit_2->text().toFloat(&ok);
    if (!ok)
        valueSizeRatio=1.0;

    if (m_createDialog->radioButton->isChecked())
    {
        osg::Vec3 rot;
        if (m_createDialog->radioButton_4->isChecked())
            rot = osg::Vec3(0.0,0.0,1.0);
        else if (m_createDialog->radioButton_5->isChecked())
            rot = osg::Vec3(0.0,1.0,0.0);
        else // radioButton_6->isChecked() == true
            rot = osg::Vec3(1.0,0.0,0.0);
        initializeGrid(m_createDialog->spinBox->value()-1, rot, valueSizeRatio);
    }
    else if (m_createDialog->radioButton_2->isChecked())
    {
        osg::Vec3 rot;
       if (m_createDialog->radioButton_4->isChecked())
            rot = osg::Vec3(1.0,0.0,0.0);
        else if (m_createDialog->radioButton_5->isChecked())
            rot = osg::Vec3(0.0,1.0,0.0);
        else // radioButton_6->isChecked() == true
            rot = osg::Vec3(0.0,0.0,1.0);
        if (m_createDialog->spinBox->value() != m_createDialog->spinBox_2->value())
            initializeGrid(m_createDialog->spinBox->value()-1, m_createDialog->spinBox_2->value()-1, rot , valueSizeRatio);
        else {
            label_4->setText("Warning: indices of landmarks need to be different");
        }
    }
    else if (m_createDialog->radioButton_3->isChecked())
    {
        if (m_createDialog->spinBox->value() != m_createDialog->spinBox_2->value()
        && m_createDialog->spinBox->value() != m_createDialog->spinBox_3->value()
        && m_createDialog->spinBox_3->value() != m_createDialog->spinBox_2->value())
            initializeGrid(m_createDialog->spinBox->value()-1,m_createDialog->spinBox_2->value()-1,m_createDialog->spinBox_3->value()-1,valueSizeRatio);
        else {
            label_4->setText("Warning: indices of landmarks need to be different");
        }
    }
    else if (m_createDialog->radioButton_9->isChecked())
    {
        osg::Vec3 rot;
        if (m_createDialog->radioButton_4->isChecked())
            rot = osg::Vec3(0.0,0.0,1.0);
        else if (m_createDialog->radioButton_5->isChecked())
            rot = osg::Vec3(0.0,1.0,0.0);
        else // radioButton_6->isChecked() == true
            rot = osg::Vec3(1.0,0.0,0.0);
        if (!m_warpedLmksOutput->getRenderable()->getOsgNode())
            return;
        osg::Matrix transfo = osg::Matrix::rotate(osg::Vec3(0.0,0.0,1.0),rot)*osg::Matrix::translate(m_warpedLmksOutput->getRenderable()->getOsgNode()->getBoundingBox().center());
        float ratio = 2.2 * m_warpedLmksOutput->getRenderable()->getOsgNode()->getBoundingBox().radius();
        initializeGrid(transfo,ratio);
    }
    else {
        label_4->setText("Warning: indices of landmarks has to be set up before");
    }
}

QString WarperNode::toString() const
{
    QString result = "";

    if(m_warpGrids->getNumGroupGrid() != 0 || m_warpGrids->getNumGrid() != 0)
        result += m_warpGrids->toString();
    result += "*";
    for (int i=0;i<m_animationWidget.count();i++)
        result += m_animationWidget[i]->toString() + "_";

    return result;
}

void WarperNode::useShader(bool checked)
{
    if(m_surfaceOutput && m_surfaceOutput->isValid())
    {
        m_useShadersCheck->setChecked(!checked);
        for (int k=0;k<morphSliders.size();k++){
            morphSliders.value(k)->setValue(0);
            morphSliders.value(k)->setTracking(checked);
        }
        m_useShadersCheck->setChecked(checked);
        m_surfaceOutput->toggleShaders(checked);
        m_warpGrids->setUsedShader(checked);
    }
}

void WarperNode::setIsUnwarped(bool checked)
{
    m_isUnwarped = checked;

    if (m_isUnwarped)
    {
        m_oldMorphAmount.clear();
        for (int k=0;k<morphSliders.size();k++)
        {
            m_oldMorphAmount.append(morphSliders.value(k)->value());

        }
        for (int k=0;k<morphSliders.size();k++){
            morphSliders.value(k)->blockSignals(true);
            morphSliders.value(k)->setValue(0);
            morphSliders.value(k)->setEnabled(false);
            morphAmountLineEdit[k]->setText(QString().setNum(0.0,'g',2));
            m_morphAmount.replace(k,0);
        }
    }
    else
    {
        for (int k=0;k<morphSliders.size();k++)
        {
            morphSliders.value(k)->blockSignals(false);
            morphSliders.value(k)->setValue(m_oldMorphAmount.at(k));
            morphSliders.value(k)->setEnabled(true);
        }
    }
    doWarp();
}


bool WarperNode::inputChanged(int inputIndex)
{
    switch(inputIndex)
    {
        case 0: //Landmarks of base specimen
            processBaseLmks();
            doTPS();
            bindFullWarp();
        break;
        case 1: //Warps Vector
            processWarps();
            doTPS();
            bindFullWarp();
        break;
        case 2: //Base Surface
            processSurface();
            bindFullWarp();
        break;
        case 3: //Warp Values
            computeOutput();
        break;
        default:
        return false;
    }

    //VD - load grids
    if (m_gridParam != "" && m_warpGrids->getNumGrid() == 0  && m_warpGrids->getNumGroupGrid() == 0)
    {
        m_warpGrids = GroupWarpGrids::loadFromString(m_gridParam, m_tps);
        m_warpGrids->setMainGroup();

        for (int i=0;i<m_warpGrids->getNumGroupGrid();i++)
        {
            GroupGridTreeItem* groupGrids = new GroupGridTreeItem();
            groupGrids->setGroupGrid(m_warpGrids->getGroupGrid(i));
            groupGrids->setText(0, m_warpGrids->getGroupGrid(i)->getRenderableName());
            treeWidget->insertTopLevelItem(treeWidget->topLevelItemCount(),groupGrids);

            for (int j=0;j<m_warpGrids->getGroupGrid(i)->getNumGrid();j++)
            {
                GridTreeItem* grids = new GridTreeItem();
                grids->setGrid(m_warpGrids->getGroupGrid(i)->getGrid(j));
                grids->setText(0, m_warpGrids->getGroupGrid(i)->getGrid(j)->getRenderableName());
                m_warpGrids->getGroupGrid(i)->getGrid(j)->setReferenceLMarks(m_lmk1Input);
                m_warpGrids->getGroupGrid(i)->getGrid(j)->setWarpedLMarks(m_warpedLmksOutput);
                groupGrids->addChild(grids);
            }
        }
        for (int i=0;i<m_warpGrids->getNumGrid();i++)
        {
            GridTreeItem* grids = new GridTreeItem();
            grids->setGrid(m_warpGrids->getGrid(i));
            grids->setText(0, m_warpGrids->getGrid(i)->getRenderableName());
            m_warpGrids->getGrid(i)->setReferenceLMarks(m_lmk1Input);
            m_warpGrids->getGrid(i)->setWarpedLMarks(m_warpedLmksOutput);
            treeWidget->insertTopLevelItem(treeWidget->topLevelItemCount(),grids);
        }
        m_warpGrids->initialize("Warped Grids");
        treeWidget->setGroupWarpGrids(m_warpGrids);
    }

    doWarp();
    return true;
}

void WarperNode::doTPS()
{
    if(m_convertedWarpings == NULL || m_lmk1Input == NULL)
        return;
    if(!m_convertedWarpings->isValid() || !m_lmk1Input->isValid())
        return;

    if (m_lmk1Input->GetRows() != m_convertedWarpings->getWarp(0)->GetRows() )
    {
        Logger::getInstance()->log("[Warper Node] Source and Target landmark configurations do not match!", Logger::RUN_ERROR);
        return;
    }

    clearTPS();

    for (unsigned int i=0; i<m_convertedWarpings->getSize(); i++)
    {
        MatrixD* mat = m_convertedWarpings->getWarp(i);
        TPS *tps = new TPS;
        tps->LoadData(m_lmk1Input->GetMatrix(),mat->GetMatrix(),m_lmk1Input->GetRows(),3);
        tps->PerformTPS();
        m_tps.push_back(tps);
    }
}

void WarperNode::bindFullWarp()
{
    if(m_surfaceOutput->isValid() && m_tps.size())
    {
        if(!m_surfaceOutput->attachShaders("warp"))
        {
            Logger::getInstance()->log("[Warper Node] Failed to load shaders!", Logger::SYS_ERROR);
            return;
        }
        //Generate Full Warp
        QList< osg::ref_ptr<osg::Vec3Array> > bindValues;
        Matrix<double>* pointInput = m_surfaceOutput->getVertices();
        for (int i=0; i<5; i++)
        {
            Matrix<double> pointOutMat((int) pointInput->GetRows(),3);
            if(i<m_tps.size())
                m_tps[i]->WarpPoints(pointInput->GetMatrix(),&pointOutMat,pointInput->GetRows());

            osg::ref_ptr<osg::Vec3Array> morphVector = new osg::Vec3Array;
            for (unsigned int j=0; j<pointInput->GetRows(); ++j)
                morphVector->push_back(osg::Vec3( pointOutMat[j][0], pointOutMat[j][1], pointOutMat[j][2]));
            bindValues.push_back(morphVector);
        }

        QStringList bindNames;
        bindNames << "morph1" << "morph2" << "morph3" << "morph4" << "morph5";
        QList<unsigned int> bindLocations;
        bindLocations << 1 << 4 << 5 << 6 << 7;

        m_surfaceOutput->bindVertexAttributes(bindNames, bindLocations, bindValues);
        // Uniform m_warpUniform
        int arraySize =  m_tps.size();
        m_coefArray.clear();
        m_coefArray.append("coefArray1");
        m_coefArray.append("coefArray2");
        m_coefArray.append("coefArray3");
        m_coefArray.append("coefArray4");
        m_coefArray.append("coefArray5");
        for (int k=0;k<m_coefArray.size();k++)
        {
            if(m_surfaceOutput->getShaderUniform(m_coefArray.at(k))==NULL)
                m_surfaceOutput->addShaderUniform(osg::Uniform::FLOAT, m_coefArray.at(k));
            m_surfaceOutput->getShaderUniform(m_coefArray.at(k))->set(0.0f);
        }

        //Uniform m_warpNumElements
        if(m_surfaceOutput->getShaderUniform("numElements")==NULL)
            m_surfaceOutput->addShaderUniform(osg::Uniform::INT, "numElements");
        m_surfaceOutput->getShaderUniform("numElements")->set(arraySize);

        //Texture Uniform
        if(m_surfaceOutput->getShaderUniform("textureUsed")==NULL)
            m_surfaceOutput->addShaderUniform(osg::Uniform::INT, "textureUsed");
        m_surfaceOutput->getShaderUniform("textureUsed")->set(m_surfaceOutput->isTextured() ? 1 : 0);

        m_surfaceOutput->toggleShaders(m_useShadersCheck->isChecked());
        setOutputPort1(m_surfaceOutput);

        //Set sliders to 0
        for (int k=0;k<morphSliders.size();k++)
        {
            morphSliders.value(k)->setValue(0);
            morphAmountLineEdit[k]->setText(QString().setNum(0.0,'g',2));
            m_morphAmount.replace(k,0);
        }
    }

    if (m_tps.size())
    {
        m_warpGrids->setTps(m_tps);
        m_warpGrids->updateGrids();
    }
}

void WarperNode::clearTPS()
{
    for(int i=0; i<m_tps.size(); ++i)
        delete m_tps[i];
    m_tps.clear();
}

void WarperNode::doWarp()
{
    if(m_convertedWarpings == NULL || m_warpedLmksOutput == NULL || m_lmk1Input == NULL)
        return;
    if(!m_convertedWarpings->isValid() || !m_warpedLmksOutput->isValid() || !m_lmk1Input->isValid())
        return;

    if (m_lmk1Input->GetRows() != m_warpedLmksOutput->GetRows() )
    {
        Logger::getInstance()->log("[Warper Node] Source and Target landmark configurations do not match!", Logger::RUN_ERROR);
        return;
    }


    for (unsigned int i=0; i<m_warpedLmksOutput->GetRows();++i)
    {
	    float x=m_lmk1Input->get(i,0);
        float y=m_lmk1Input->get(i,1);
        float z=m_lmk1Input->get(i,2);
        for (unsigned int n=0;n<m_convertedWarpings->getSize();n++)
        {
            x += m_morphAmount[n] * m_convertedWarpings->getWarp(n)->get(i,0) * 0.01f;
            y += m_morphAmount[n] * m_convertedWarpings->getWarp(n)->get(i,1) * 0.01f;
            z += m_morphAmount[n] * m_convertedWarpings->getWarp(n)->get(i,2) * 0.01f;
        }
        m_warpedLmksOutput->set(i,0,x);
        m_warpedLmksOutput->set(i,1,y);
        m_warpedLmksOutput->set(i,2,z);
    }

    bool scaleInFormSpace = (m_formSpace && formSpaceCheckBox->isChecked());

    if(scaleInFormSpace)
    {
        double scale = 1.0;
        double csize = 1.0;
        for (unsigned int n=0;n<m_convertedWarpings->getSize();n++)
        {
            // scale with loadings for ln cs
            double sizeloadings = m_convertedWarpings->getSizeLoadings(n);
            scale *= exp(log(csize) + sizeloadings * m_morphAmount[n] * 0.01) / csize;
        }
        (*m_warpedLmksOutput) *= scale;
    }
    setOutputPort0(m_warpedLmksOutput);

    if (m_surfaceOutput && m_surfaceOutput->isValid())
    {
        if(m_useShadersCheck->isChecked() && !scaleInFormSpace)
        {
            for (int k=0;k<qMin(m_morphAmount.size(),5);k++)
            {
                m_surfaceOutput->getShaderUniform(m_coefArray.at(k))->set((float)m_morphAmount[k]*0.01f);
            }
        }
        else
        {
            Matrix<double>* points = m_surfaceInput->getVertices();
            Matrix<double>* wpoints = new Matrix<double>(points->GetRows(),points->GetCols());

            TPS tps;

            tps.LoadData(m_lmk1Input->GetMatrix(),m_warpedLmksOutput->GetMatrix(),m_lmk1Input->GetRows(),m_lmk1Input->GetCols());
            tps.PerformTPS();

            tps.WarpPoints(points->GetMatrix(),wpoints,points->GetRows());

            m_surfaceOutput->setVertices(wpoints);
            m_surfaceOutput->refreshGeometry();
        }
        setOutputPort1(m_surfaceOutput);
    }

    if (m_warpGrids->getNumGrid()>0 || m_warpGrids->getNumGroupGrid()>0)
    {
        // VD - grids
        if(m_useShadersCheck->isChecked() && !scaleInFormSpace)
        {
            for (int j=0;j<m_warpGrids->getNumGrid();j++) // grid
                for (int k=0;k<qMin(m_morphAmount.size(),5);k++)
                {
                    osg::StateSet* ss = m_warpGrids->getGrid(j)->getOsgGeometry()->getOrCreateStateSet();
                    if (!ss->getUniform(m_coefArray.at(k).toStdString()))
                        ss->addUniform(new osg::Uniform(osg::Uniform::FLOAT, m_coefArray.at(k).toStdString()));
                    ss->getUniform(m_coefArray.at(k).toStdString())->set(((float)m_morphAmount[k])*0.01f);
                }
            for (int i=0;i<m_warpGrids->getNumGroupGrid();i++) // groupGrid
                for (int j=0;j<m_warpGrids->getGroupGrid(i)->getNumGrid();j++)
                    for (int k=0;k<qMin(m_morphAmount.size(),5);k++)
                    {
                        osg::StateSet* ss = m_warpGrids->getGroupGrid(i)->getGrid(j)->getOsgGeometry()->getOrCreateStateSet();
                        if (!ss->getUniform(m_coefArray.at(k).toStdString()))
                            ss->addUniform(new osg::Uniform(osg::Uniform::FLOAT, m_coefArray.at(k).toStdString()));
                        ss->getUniform(m_coefArray.at(k).toStdString())->set(((float)m_morphAmount[k])*0.01f);
                    }
        }
        else
        {
            Matrix<double>* points, *wpoints;
            TPS tps;

            tps.LoadData(m_lmk1Input->GetMatrix(),m_warpedLmksOutput->GetMatrix(),m_lmk1Input->GetRows(),m_lmk1Input->GetCols());
            tps.PerformTPS();

            for (int i=0;i<m_warpGrids->getNumGrid();i++)
            {
                points = new Matrix<double>(*(m_warpGrids->getGrid(i)->getOriginalVertices()));
                Matrix<double>* mat = osgMatrixToEvanMatrix(m_warpGrids->getGrid(i)->computeWorldtoLocalMatrix());
                points->transform(*mat);

                wpoints = new Matrix<double>(points->GetRows(),points->GetCols());

                tps.WarpPoints(points->GetMatrix(),wpoints,points->GetRows());
                mat->Inv();
                wpoints->transform(*mat);
                m_warpGrids->getGrid(i)->setVertices(wpoints);
                delete mat;

                m_warpGrids->getGrid(i)->refreshGeometry();
                m_warpGrids->getGrid(i)->setWarpedLMarks(m_warpedLmksOutput);
            }

            for (int i=0;i<m_warpGrids->getNumGroupGrid();i++)
            {
                for (int j=0;j<m_warpGrids->getGroupGrid(i)->getNumGrid();j++)
                {
                    points = new Matrix<double>(*(m_warpGrids->getGroupGrid(i)->getGrid(j)->getOriginalVertices()));
                    Matrix<double>* mat = osgMatrixToEvanMatrix(m_warpGrids->getGroupGrid(i)->getGrid(j)->computeWorldtoLocalMatrix());
                    points->transform(*mat);

                    wpoints = new Matrix<double>(points->GetRows(),points->GetCols());

                    tps.WarpPoints(points->GetMatrix(),wpoints,points->GetRows());
                    mat->Inv();
                    wpoints->transform(*mat);
                    m_warpGrids->getGroupGrid(i)->getGrid(j)->setVertices(wpoints);
                    delete mat;

                    m_warpGrids->getGroupGrid(i)->getGrid(j)->refreshGeometry();
                    m_warpGrids->getGroupGrid(i)->getGrid(j)->setWarpedLMarks(m_warpedLmksOutput);
                }
            }
        }
        setOutputPort2(m_warpGrids);
    }
    emit dataReady();
}

void WarperNode::fromString(const QString& param)
{
      QStringList listParam = param.split("*");
      if (listParam.count() >0 )
          m_gridParam = listParam[0];
      if (listParam.count() >1 )
          m_animParam = listParam[1];
}

/**
 *   Initialize a new grid
 *   @param rows number of rows
 *   @param cols number of cols
 *   @param transfo 4x4 matrix with the initial transformation matrix
*/
void WarperNode::initializeGrid(const osg::Matrixd& transfo, float ratio)
{
    unsigned int rows,cols;
    bool ok;
    rows = m_createDialog->line1->text().toInt(&ok);
    if (!ok)
        rows=15;
    cols = m_createDialog->line2->text().toInt(&ok);
    if (!ok)
        cols=15;
    if (rows < 3 || cols < 3)
    {
        label_4->setText("Warning: 'Rows' and 'Colums' have to equal or greater than 4");
        return;
    }

    WarpGrid* newGrid = new WarpGrid;
    newGrid->setRowsAndCols(rows,cols);
    newGrid->setSize(ratio);
    newGrid->initializeGrid(m_createDialog->lineEdit->text());
    newGrid->setParent(m_warpGrids);
    newGrid->setGridTransform(transfo);

    newGrid->setReferenceLMarks(m_lmk1Input);
    newGrid->setWarpedLMarks(m_warpedLmksOutput);

    m_warpGrids->addGrid(newGrid);
    m_warpGrids->setUsedShader(m_useShadersCheck->isChecked());

    m_warpGrids->initialize("Warped grids");

    GridTreeItem* grids = new GridTreeItem();
    grids->setGrid(newGrid);
    grids->setText(0, newGrid->getRenderableName());
    treeWidget->insertTopLevelItem(treeWidget->topLevelItemCount(),grids);

    setOutputPort2(m_warpGrids);
    doWarp();
}

void WarperNode::initializeGrid(unsigned int i, osg::Vec3 rot, float ratio)
{
    LandmarkSet& lmk = *m_lmk1Input;
    if (lmk.GetRows() < i)
        return;

    osg::Matrixd mat = osg::Matrix::identity();

    mat(3,0) = lmk.get(i,0);
    mat(3,1) = lmk.get(i,1);
    mat(3,2) = lmk.get(i,2);

    mat.preMult(osg::Matrix::rotate(osg::Vec3(0.0,0.0,1.0),rot));

    initializeGrid(mat, ratio);
}

void WarperNode::initializeGrid(unsigned int i, unsigned int j, osg::Vec3 rot, float ratio)
{
    LandmarkSet& lmk = *m_lmk1Input;
    if (lmk.GetRows() < i && lmk.GetRows() < j)
        return;

    float vx,vy,vz;
    vx=lmk.get(i,0)-lmk.get(j,0);
    vy=lmk.get(i,1)-lmk.get(j,1);
    vz=lmk.get(i,2)-lmk.get(j,2);

    osg::Vec3 to = osg::Vec3(vx,vy,vz);

    osg::Matrixd mat;
    if (to==rot) {
        mat = osg::Matrix::identity();
    }
    else {
        mat = osg::Matrix::rotate(osg::Vec3(0.0,0.0,1.0) , rot^to);
    }

    mat(3,0) = 0.5*lmk.get(i,0)+0.5*lmk.get(j,0);
    mat(3,1) = 0.5*lmk.get(i,1)+0.5*lmk.get(j,1);
    mat(3,2) = 0.5*lmk.get(i,2)+0.5*lmk.get(j,2);
    mat(3,3) = 1;

   initializeGrid(mat, ratio);
}

void WarperNode::initializeGrid(unsigned int i, unsigned int j, unsigned int k, float ratio)
{
    LandmarkSet& lmk = *m_lmk1Input;
    if (lmk.GetRows() < i && lmk.GetRows() < j && lmk.GetRows() < k)
        return;

    float vx1,vy1,vz1;
    float vx2,vy2,vz2;
    vx1=lmk.get(i,0)-lmk.get(j,0);
    vy1=lmk.get(i,1)-lmk.get(j,1);
    vz1=lmk.get(i,2)-lmk.get(j,2);
    vx2=lmk.get(k,0)-lmk.get(j,0);
    vy2=lmk.get(k,1)-lmk.get(j,1);
    vz2=lmk.get(k,2)-lmk.get(j,2);

    osg::Vec3f from=osg::Vec3f(0.0f,0.0f,1.0f);
    osg::Vec3f v1=osg::Vec3f(vx1,vy1,vz1);
    osg::Vec3f v2=osg::Vec3f(vx2,vy2,vz2);

    osg::Matrixd mat = osg::Matrix::rotate(from,v1^v2);

    mat(3,0) = 0.33*lmk.get(i,0)+0.33*lmk.get(j,0)+0.33*lmk.get(k,0);
    mat(3,1) = 0.33*lmk.get(i,1)+0.33*lmk.get(j,1)+0.33*lmk.get(k,1);
    mat(3,2) = 0.33*lmk.get(i,2)+0.33*lmk.get(j,2)+0.33*lmk.get(k,2);
    mat(3,3) = 1;

   initializeGrid(mat, ratio);
}

void WarperNode::cloneGrid(WarpGrid* warpg)
{
    WarpGrid* newGrid = new WarpGrid;
    newGrid->setRowsAndCols(warpg->getRows(),warpg->getCols());
    newGrid->setSize(warpg->getSize());
    newGrid->initializeGrid(QString("Copy of ").append(warpg->getRenderableName()));
    newGrid->setParent(m_warpGrids);

    newGrid->setGridTransform(warpg->getOsgTransform()->getMatrix());

    newGrid->setReferenceLMarks(m_lmk1Input);
    newGrid->setWarpedLMarks(m_warpedLmksOutput);

    m_warpGrids->addGrid(newGrid);
    m_warpGrids->setUsedShader(m_useShadersCheck->isChecked());
    m_warpGrids->initialize("Warped grids");

    GridTreeItem* grids = new GridTreeItem();
    grids->setGrid(newGrid);
    grids->setText(0, newGrid->getRenderableName());
//    grids->setData(0, Qt::DecorationRole, Qt::white);
    treeWidget->insertTopLevelItem(treeWidget->topLevelItemCount(),grids);

    setOutputPort2(m_warpGrids);
    doWarp();
}

void WarperNode::parallelGrid(WarpGrid* warpg)
{
    //@todo dialog to choose between spacing or lmk
//    double spacing = QInputDialog::getDouble(this,QString("Enter the spacing"),QString("Spacing:"),0.0);
    ParallelGridDialog* paraDialog = new ParallelGridDialog(this, (int)  m_lmk1Input->GetRows());
    if (!paraDialog->exec())
    {
        delete paraDialog;
        return;
    }


    WarpGrid* newGrid = new WarpGrid;
    newGrid->setRowsAndCols(warpg->getRows(),warpg->getCols());
    newGrid->setSize(warpg->getSize());

    QString name("Grid ");
    int total=m_warpGrids->getNumGrid();
    for (int i=0;i<m_warpGrids->getNumGroupGrid();++i)
        total+=m_warpGrids->getGroupGrid(i)->getNumGrid();
    name.append(QString::number(total+1));

    newGrid->initializeGrid(name);
    newGrid->setParent(m_warpGrids);

    if (paraDialog->isSpacing())
    {
        osg::Matrixd mat = osg::Matrix::translate(osg::Vec3(0.0,0.0,paraDialog->getSpacing()))*osg::Matrix(warpg->getOsgTransform()->getMatrix());
        newGrid->setGridTransform(mat);
    }
    else // isLmk
    {
        osg::Matrixd mat = osg::Matrix(warpg->getOsgTransform()->getMatrix());

        int i = paraDialog->getLmk();

        mat(3,0) = m_lmk1Input->get(i,0);
        mat(3,1) = m_lmk1Input->get(i,1);
        mat(3,2) = m_lmk1Input->get(i,2);

        newGrid->setGridTransform(mat);
    }

    newGrid->setReferenceLMarks(m_lmk1Input);
    newGrid->setWarpedLMarks(m_warpedLmksOutput);

    m_warpGrids->addGrid(newGrid);
    m_warpGrids->setUsedShader(m_useShadersCheck->isChecked());
    m_warpGrids->initialize("Warped grids");

    GridTreeItem* grids = new GridTreeItem();
    grids->setGrid(newGrid);
    grids->setText(0, name);
//    grids->setData(0, Qt::DecorationRole, Qt::white);
    treeWidget->insertTopLevelItem(treeWidget->topLevelItemCount(),grids);

    doWarp();
    delete paraDialog;
}

void WarperNode::perpendicularGrid(WarpGrid* warpg)
{
    PerpendicularGridDialog* perpDialog = new PerpendicularGridDialog(this);
    if (!perpDialog->exec())
    {
        delete perpDialog;
        return;
    }
    bool orientation=perpDialog->getOrientation();
    delete perpDialog;
    WarpGrid* newGrid = new WarpGrid;
    newGrid->setRowsAndCols(warpg->getRows(),warpg->getCols());
    newGrid->setSize(warpg->getSize());

    QString name("Grid ");
    int total=m_warpGrids->getNumGrid();
    for (int i=0;i<m_warpGrids->getNumGroupGrid();++i)
        total+=m_warpGrids->getGroupGrid(i)->getNumGrid();
    name.append(QString::number(total+1));

    newGrid->initializeGrid(name);
    newGrid->setParent(m_warpGrids);

    osg::Matrix mat=warpg->getOsgTransform()->getMatrix();
    if (orientation)
    {
        mat.preMult(osg::Matrix::rotate(osg::Vec3(1.0,0.0,0.0),osg::Vec3(0.0,0.0,1.0)));
    }
    else
    {
        mat.preMult(osg::Matrix::rotate(osg::Vec3(0.0,1.0,0.0),osg::Vec3(0.0,0.0,1.0)));
    }
    newGrid->setGridTransform(mat);

    newGrid->setReferenceLMarks(m_lmk1Input);
    newGrid->setWarpedLMarks(m_warpedLmksOutput);

    m_warpGrids->addGrid(newGrid);
    m_warpGrids->setUsedShader(m_useShadersCheck->isChecked());
    m_warpGrids->initialize("Warped grids");

    GridTreeItem* grids = new GridTreeItem();
    grids->setGrid(newGrid);
    grids->setText(0, name);
//    grids->setData(0, Qt::DecorationRole, Qt::white);
    treeWidget->insertTopLevelItem(treeWidget->topLevelItemCount(),grids);

    setOutputPort2(m_warpGrids);
    doWarp();
}

void WarperNode::propertyGrid(GridTreeItem* gridItem)
{
    WarpGrid* warpg = gridItem->getGrid();
    GridPropertyDialog* propertyDialog = new GridPropertyDialog(this,warpg);
    propertyDialog->exec();


    bool modified=false;
    if (propertyDialog->lineEdit->isModified()){
        warpg->setRenderableName(propertyDialog->lineEdit->text());
        modified=true;
        gridItem->setText(0, propertyDialog->lineEdit->text());
    }
    if (propertyDialog->lineEdit_2->isModified())
    {
        bool ok;
        float res=propertyDialog->lineEdit_2->text().toFloat(&ok);
        if (ok){
            warpg->setSize(res);
            modified=true;
        }
    }
    if (propertyDialog->line1->isModified() || propertyDialog->line2->isModified())
    {
        bool ok1,ok2;
        int row=propertyDialog->line1->text().toInt(&ok1);
        int col=propertyDialog->line2->text().toInt(&ok2);
        if (ok1 && ok2){
            warpg->setRowsAndCols(row,col);
            modified=true;
        }
    }
    if (propertyDialog->isTransformationMatrixModified())
    {
        warpg->getOsgTransform()->setMatrix(propertyDialog->getTransformationMatrix());
    }

    if (modified)
        warpg->reInitializeGrid();
    delete propertyDialog;
    doWarp();
}

void WarperNode::membersUpdated()
{
    m_warpGrids->setUsedShader(m_useShadersCheck->isChecked());
    m_warpGrids->initialize("Warped grids");
    doWarp();
}

///////AnimationWidget/////////////////////////////////////////////////////////////
AnimationWidget::AnimationWidget(QWidget* parent):
	QWidget(parent)
{
    QGridLayout* animLayout = new QGridLayout();
    m_minValueButton = new QPushButton("Minimum");
    m_maxValueButton = new QPushButton("Maximum");
    m_startValueButton = new QPushButton("Start");
    m_minValue = new QLineEdit("0");
    m_maxValue = new QLineEdit("0");
    m_startValue = new QLineEdit("0");
    m_playButton = new QPushButton("Play");
    m_pauseButton = new QPushButton("Pause");
    m_playButton->setCheckable(true);
    m_playButton->setChecked(false);
    m_pauseButton->setCheckable(true);
    m_pauseButton->setChecked(false);
    QLabel* length = new QLabel("Length (sec)");
    m_lengthValue = new QLineEdit("10.0");
    m_policy1 = new QComboBox(this);
    m_policy1->addItem("Min to Max");
    m_policy1->addItem("Max to Min");
    m_policy1->addItem("Min to Max then Max to Min");
    m_policy1->addItem("Max to Min then Min to Max");
    m_policy2 = new QComboBox(this);
    m_policy2->addItem("One cycle");
    m_policy2->addItem("Always");
    m_isActive = new QCheckBox("Active");
    m_currentMovement=0;
    m_nextMovement=0;
    m_current = 0.0;
    m_nbCycle = 0;

    animLayout->addWidget(m_minValueButton,0,0);
    animLayout->addWidget(m_minValue,0,1);
    animLayout->addWidget(m_maxValueButton,0,2);
    animLayout->addWidget(m_maxValue,0,3);
    animLayout->addWidget(m_startValueButton,0,4);
    animLayout->addWidget(m_startValue,0,5);
    animLayout->addWidget(m_isActive,0,6);
    animLayout->addWidget(length,1,0);
    animLayout->addWidget(m_lengthValue,1,1);
    animLayout->addWidget(m_policy1,1,2,1,2);
    animLayout->addWidget(m_policy2,1,4);
    animLayout->addWidget(m_playButton,1,5);
    animLayout->addWidget(m_pauseButton,1,6);

    setLayout(animLayout);
}

QString AnimationWidget::toString()
{
    QString res;

    res +=  m_minValue->text() + "~" +
            m_maxValue->text() + "~" +
            m_startValue->text() + "~" +
            m_lengthValue->text() + "~" +
            QString::number(m_policy1->currentIndex()) + "~" +
            QString::number(m_policy2->currentIndex());

    return res;

}

void AnimationWidget::fromString(const QString& params)
{
    QStringList paramlist = params.split("~");
    if(paramlist.count() == 6)
    {
        m_minValue->setText(paramlist[0]);
        m_maxValue->setText(paramlist[1]);
        m_startValue->setText(paramlist[2]);
        m_lengthValue->setText(paramlist[3]);
        m_policy1->setCurrentIndex(paramlist[4].toInt());
        m_policy2->setCurrentIndex(paramlist[5].toInt());
    }
}

///////CreateGridDialog/////////////////////////////////////////////////////////////
CreateGridDialog::CreateGridDialog(QWidget* parent):QDialog(parent)
{
    setupUi(this);

    QButtonGroup* groupButton1 = new QButtonGroup();
    QButtonGroup* groupButton2 = new QButtonGroup();

    groupButton1->addButton(radioButton);
    groupButton1->addButton(radioButton_9);
    groupButton1->addButton(radioButton_2);
    groupButton1->addButton(radioButton_3);
    groupButton2->addButton(radioButton_4);
    groupButton2->addButton(radioButton_5);
    groupButton2->addButton(radioButton_6);

    radioButton_4->setChecked(true);

    connect(radioButton, SIGNAL(clicked()), this, SLOT(setLmkIndex()));
    connect(radioButton_2, SIGNAL(clicked()), this, SLOT(setLmkIndex()));
    connect(radioButton_3, SIGNAL(clicked()), this, SLOT(setLmkIndex()));
    connect(radioButton_9, SIGNAL(clicked()), this, SLOT(setLmkIndex()));
}

void CreateGridDialog::setMaxLmkIndex(int maxIndex)
{
    spinBox->setMinimum(1);
    spinBox_2->setMinimum(1);
    spinBox_3->setMinimum(1);
    spinBox->setMaximum(maxIndex);
    spinBox_2->setMaximum(maxIndex);
    spinBox_3->setMaximum(maxIndex);
}

void CreateGridDialog::setLmkIndex()
{
    if (radioButton->isChecked())
    {
        spinBox->setEnabled (true);
        spinBox_2->setEnabled (false);
        spinBox_3->setEnabled (false);
        radioButton_4->setEnabled(true);
        radioButton_4->setText("X-Y plane");
        radioButton_5->setText("X-Z plane");
        radioButton_6->setText("Y-Z plane");
        radioButton_4->setChecked(true);
        radioButton_5->setEnabled(true);
        radioButton_6->setEnabled(true);
    }
    if (radioButton_2->isChecked())
    {
        spinBox->setEnabled (true);
        spinBox_2->setEnabled (true);
        spinBox_3->setEnabled (false);
        radioButton_4->setEnabled(true);
        radioButton_4->setText("X axis");
        radioButton_5->setText("Y axis");
        radioButton_6->setText("Z axis");
        radioButton_4->setChecked(true);
        radioButton_5->setEnabled(true);
        radioButton_6->setEnabled(true);
    }
    if (radioButton_3->isChecked())
    {
        spinBox->setEnabled (true);
        spinBox_2->setEnabled (true);
        spinBox_3->setEnabled (true);
        radioButton_4->setEnabled(false);
        radioButton_5->setEnabled(false);
        radioButton_6->setEnabled(false);
    }
    if (radioButton_9->isChecked())
    {
        spinBox->setEnabled (false);
        spinBox_2->setEnabled (false);
        spinBox_3->setEnabled (false);
        radioButton_4->setEnabled(true);
        radioButton_4->setText("X-Y plane");
        radioButton_5->setText("X-Z plane");
        radioButton_6->setText("Y-Z plane");
        radioButton_4->setEnabled(true);
        radioButton_4->setChecked(true);
        radioButton_5->setEnabled(true);
        radioButton_6->setEnabled(true);
    }
}

////////////////////////////////////////////////////////////
GridPropertyDialog::GridPropertyDialog(QWidget* parent,WarpGrid* warpg):QDialog(parent),m_warpGrid(warpg)
{
    setupUi(this);

    if (!m_warpGrid)
        return;

    // matrix
    osg::Matrix m =warpg->getOsgTransform()->getMatrix();
    mat00->setText(QString::number(m(0,0)));mat01->setText(QString::number(m(0,1)));mat02->setText(QString::number(m(0,2)));mat03->setText(QString::number(m(0,3)));
    mat10->setText(QString::number(m(1,0)));mat11->setText(QString::number(m(1,1)));mat12->setText(QString::number(m(1,2)));mat13->setText(QString::number(m(1,3)));
    mat20->setText(QString::number(m(2,0)));mat21->setText(QString::number(m(2,1)));mat22->setText(QString::number(m(2,2)));mat23->setText(QString::number(m(2,3)));
    mat30->setText(QString::number(m(3,0)));mat31->setText(QString::number(m(3,1)));mat32->setText(QString::number(m(3,2)));mat33->setText(QString::number(m(3,3)));

    lineEdit->setText(warpg->getRenderableName());
    lineEdit_2->setText(QString::number(warpg->getSize()));
    line1->setText(QString::number(warpg->getRows()));
    line2->setText(QString::number(warpg->getCols()));
}

bool GridPropertyDialog::isTransformationMatrixModified()
{
    return    (mat00->isModified() || mat01->isModified() || mat02->isModified() || mat03->isModified()
            || mat10->isModified() || mat11->isModified() || mat12->isModified() || mat13->isModified()
            || mat20->isModified() || mat21->isModified() || mat22->isModified() || mat23->isModified()
            || mat30->isModified() || mat31->isModified() || mat32->isModified() || mat33->isModified());
}

osg::Matrix GridPropertyDialog::getTransformationMatrix()
{
    osg::Matrix m = osg::Matrix::identity();

    m(0,0)=mat00->text().toFloat();m(0,1)=mat01->text().toFloat();m(0,2)=mat02->text().toFloat();
    m(1,0)=mat10->text().toFloat();m(1,1)=mat11->text().toFloat();m(1,2)=mat12->text().toFloat();
    m(2,0)=mat20->text().toFloat();m(2,1)=mat21->text().toFloat();m(2,2)=mat22->text().toFloat();
    m(3,0)=mat30->text().toFloat();m(3,1)=mat31->text().toFloat();m(3,2)=mat32->text().toFloat();

    if (!m.valid())
        return m_warpGrid->getOsgTransform()->getMatrix();
    else
        return m;
}

//////////////////////////////////////////////////////////////////
PerpendicularGridDialog::PerpendicularGridDialog(QWidget* parent)
{
        QVBoxLayout* layout = new QVBoxLayout;

        button1 = new QRadioButton("X-Z perpendicular plane");
        button1->setChecked(true);

        button2 = new QRadioButton("Y-Z perpendicular plane");

        QDialogButtonBox* buttonBox = new QDialogButtonBox(this);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(30, 240, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

        layout->addWidget(button1);
        layout->addWidget(button2);
        layout->addWidget(buttonBox);
        setLayout(layout);
}

bool PerpendicularGridDialog::getOrientation()
{
    return button1->isChecked();
}

/////////////////////////////////////////////////////////////////////
ParallelGridDialog::ParallelGridDialog(QWidget* parent,int max)
{
    resize(327, 93);
    gridLayout = new QGridLayout(this);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    radioButton = new QRadioButton("Spacing");
    radioButton->setChecked(true);
    radioButton->setObjectName(QString::fromUtf8("radioButton"));

    gridLayout->addWidget(radioButton, 0, 0, 1, 1);

    radioButton_2 = new QRadioButton("Select a landmark");
    radioButton_2->setObjectName(QString::fromUtf8("radioButton_2"));

    gridLayout->addWidget(radioButton_2, 0, 1, 1, 1);

    lineEdit = new QLineEdit(this);
    lineEdit->setObjectName(QString::fromUtf8("lineEdit"));

    gridLayout->addWidget(lineEdit, 1, 0, 1, 1);

    spinBox = new QSpinBox(this);
    spinBox->setObjectName(QString::fromUtf8("spinBox"));
    spinBox->setEnabled(false);
    spinBox->setMinimum(1);
    spinBox->setMaximum(max);

    gridLayout->addWidget(spinBox, 1, 1, 1, 1);

    buttonBox = new QDialogButtonBox(this);
    buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
    buttonBox->setGeometry(QRect(30, 240, 341, 32));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
    gridLayout->addWidget(buttonBox, 2, 1, 1, 1);

    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    QObject::connect(radioButton, SIGNAL(clicked()), this, SLOT(swapEnabled()));
    QObject::connect(radioButton_2, SIGNAL(clicked()), this, SLOT(swapEnabled()));
}

void ParallelGridDialog::swapEnabled()
{
    if (radioButton->isChecked())
    {
        lineEdit->setEnabled(true);
        spinBox->setEnabled(false);
    }
    else
    {
        lineEdit->setEnabled(false);
        spinBox->setEnabled(true);
    }
}


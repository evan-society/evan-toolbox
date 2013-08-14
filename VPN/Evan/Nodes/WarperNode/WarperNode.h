#ifndef WARPERNODE_H
#define WARPERNODE_H

#include "../INode.h"
#include "../../DataTypes/LandmarkSet.h"
#include "../../DataTypes/Surface.h"
#include "../../DataTypes/FloatVector.h"
#include "../../DataTypes/Warpings.h"
#include <QTabWidget>
#include <QtGui>
#include "Designer/ui_warper.h"
#include "Designer/ui_createGrid.h"
#include "Designer/ui_gridProperty.h"

class TPS;
class CreateGridDialog;
class AnimationWidget;

class WarperNode : public QTabWidget, public INode, public Ui::WarperDialog
{
    Q_OBJECT

private:
    QGridLayout *layoutSliders;
    QVector<TPS*> m_tps;

    QList<QString> m_coefArray;

    QList<QSlider*> morphSliders;
    QVector<QLineEdit*> morphAmountLineEdit;
    QVector<QLabel*> morphLabel;
    QVector<QPushButton*> morphAmountPushButton;
    QVector<AnimationWidget*> m_animationWidget;
    QVector<QPushButton*> m_playButton;
    QVector<int> m_animation;
    ////Grid animation
    float m_interpolationParameter;
    bool m_currentMovement; // 0 left to right 1 right to left
    bool m_nextMovement;    // 0 left to right 1 right to left
    /////

    LandmarkSet* m_lmk1Input;
    Warpings* m_warpVectorInput;
    Warpings* m_convertedWarpings;
    Surface* m_surfaceInput;
    FloatVector* m_warpFactors;
    GroupWarpGrids* m_warpGrids;

    Surface* m_surfaceOutput;
    LandmarkSet* m_warpedLmksOutput;

    QTimer m_timer;

    QColor mygreen;
    bool m_dependent;
    bool m_isUnwarped; // switch to warped/unwarped space
    QVector<int> m_oldMorphAmount; // used to back up morph amount values when warping/unwarping
    QVector<int> m_morphAmount;
    QVector<double> m_scaleAmounts; // hpg: scale sliders
    int m_range;
    bool m_plotterWarp;
    double m_rangeValue;
    bool m_formSpace;
    QCheckBox *formSpaceCheckBox;

    CreateGridDialog* m_createDialog;

    QSlider* createSlider();
    void doWarp();
    void processBaseLmks();
    void processWarps();
    void bindFullWarp();
    void processSurface();
    void doTPS();
    void clearTPS();

    QString m_gridParam;
    QString m_animParam;

protected:
    LandmarkSet* getInputPortData0()const { return dynamic_cast<LandmarkSet *>( getInputData( 0 ) ); }
    Warpings* getInputPortData1()const { return dynamic_cast<Warpings *>( getInputData( 1 ) ); }
    Surface* getInputPortData2()const { return dynamic_cast<Surface *>( getInputData( 2 ) ); }
    FloatVector* getInputPortData3()const { return dynamic_cast<FloatVector *>(getInputData( 3 ) ); }

    void setOutputPort0(LandmarkSet* p)   {setOutputData(0,p);}
    void setOutputPort1(Surface* p)       {setOutputData(1,p);}
    void setOutputPort2(GroupWarpGrids* p){setOutputData(2,p);}

    virtual void enterEvent(QEvent* event);

public:
	WarperNode(QWidget* parent=0);
	~WarperNode();

	void showGUI() {QTabWidget::activateWindow();QTabWidget::show();}
	const QObject* getDialog() const {return this;}
    void process();
    bool isUpdated() const {return false;}
    bool inputChanged(int);
    inline float convertMorphSlider(int n);
    void setSlider(int i);
    QString toString() const;
    void fromString(const QString&) ;

public slots:
	void computeOutput();
    void setMorph(int);
    void setSlider();
    void setRange();
    void setDependency(bool);
    void useShader(bool);
    void setIsUnwarped(bool);
    void setMinValue();
    void setMaxValue();
    void setGridStartPosition();
    void setGridStopPosition();
    void gridPlay(bool);
    void setStartValue();
    void playAnimation(bool);
    void playAll();
    void moveAnimation();
    void playGridAnimation();

signals:
    void dataReady();

//VD - grid
public:
    void initializeGrid(const osg::Matrixd& transfo, float ratio);
    void initializeGrid(unsigned int i, osg::Vec3 rot, float ratio);
    void initializeGrid(unsigned int i,unsigned int j,osg::Vec3 rot, float ratio);
    void initializeGrid(unsigned int i,unsigned int j,unsigned int k, float ratio);

public slots:
    void cloneGrid(WarpGrid*);
    void setTobeCreated();
    void parallelGrid(WarpGrid*);
    void perpendicularGrid(WarpGrid*);
    void propertyGrid(GridTreeItem*);
    void membersUpdated();

 //   void updateGrid();
    void toggleShadersCheck(){ m_useShadersCheck->setDisabled(formSpaceCheckBox->isChecked());}
};

class AnimationWidget : public QWidget
{
    Q_OBJECT

    private:
    QPushButton* m_minValueButton;
    QPushButton* m_maxValueButton;
    QPushButton* m_startValueButton;
    QCheckBox* m_isActive;
    QPushButton* m_playButton;
    QPushButton* m_pauseButton;
    QLineEdit* m_minValue;
    QLineEdit* m_maxValue;
    QLineEdit* m_startValue;
    QLineEdit* m_lengthValue;
    QComboBox* m_policy1;
    QComboBox* m_policy2;

    bool m_currentMovement; // 0 left to right 1 right to left
    bool m_nextMovement;    // 0 left to right 1 right to left
    double m_current;
    int m_nbCycle;

    public:
    AnimationWidget(QWidget* parent);
    ~AnimationWidget() {} //@todo

    double getMinValue()    {return m_minValue->text().toDouble();}
    double getMaxValue()    {return m_maxValue->text().toDouble();}
    double getStartValue()  {return m_startValue->text().toDouble();}
    double getLengthValue() {return m_lengthValue->text().toDouble();}
    void setMinValue(QString value)     {m_minValue->setText(value);}
    void setMaxValue(QString value)     {m_maxValue->setText(value);}
    void setStartValue(QString value)     {m_startValue->setText(value);}
    QPushButton* getMinValueButton()    {return m_minValueButton;}
    QPushButton* getMaxValueButton()    {return m_maxValueButton;}
    QPushButton* getStartValueButton()  {return m_startValueButton;}
    QPushButton* getPlayButton()        {return m_playButton;}
    QPushButton* getPauseButton()        {return m_pauseButton;}
    int getPolicy1()    {return m_policy1->currentIndex();}
    int getPolicy2()    {return m_policy2->currentIndex();}
    bool getCurrentMovement  () const{return m_currentMovement;}
    bool getNextMovement     () const{return m_nextMovement;}
    void setCurrentMovement(bool movement) {m_currentMovement=movement;}
    void setNextMovement(bool movement) {m_nextMovement=movement;}
    double getCurrent()     const {return m_current;}
    void setCurrent(double current) {m_current=current;}
    void setNbCycle(int nbCycle) {m_nbCycle=nbCycle;}
    int getNbCycle() const {return m_nbCycle;}
    int isActive() const {return m_isActive->isChecked();}
    void setActive(bool b){return m_isActive->setChecked(b);}
    QString toString();
    void fromString(const QString& params);
};

class CreateGridDialog : public QDialog, public Ui::CreateGridDialog
{
    Q_OBJECT
public:
    CreateGridDialog(QWidget* parent=0);
    void setMaxLmkIndex(int maxIndex);

public slots:
    void setLmkIndex();
};

class GridPropertyDialog : public QDialog, public Ui::GridProperty
{
    Q_OBJECT
private:
    WarpGrid* m_warpGrid;
public:
    GridPropertyDialog(QWidget* parent=0,WarpGrid* warpg=NULL);
    bool isTransformationMatrixModified();
    osg::Matrixd getTransformationMatrix();
};

class PerpendicularGridDialog : public QDialog
{
    Q_OBJECT
private:
    QRadioButton* button1;
    QRadioButton* button2;

public:
    PerpendicularGridDialog(QWidget* parent=0);
    bool getOrientation();
};

class ParallelGridDialog : public QDialog
{
    Q_OBJECT
private:
    QGridLayout *gridLayout;
    QRadioButton *radioButton;
    QRadioButton *radioButton_2;
    QLineEdit *lineEdit;
    QSpinBox *spinBox;
    QDialogButtonBox *buttonBox;

public:
    ParallelGridDialog(QWidget* parent, int max);
    bool isSpacing(){return radioButton->isChecked();}
    bool isLmk(){return radioButton_2->isChecked();}
    float getSpacing(){return lineEdit->text().toFloat();}
    int getLmk(){return spinBox->value();}

public slots:
    void swapEnabled();
};
#endif

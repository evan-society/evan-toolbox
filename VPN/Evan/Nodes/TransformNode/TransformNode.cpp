#include "TransformNode.h"
#include "../../Utilities/Logger.h"

TransformNode::TransformNode() :  INode()
{
	m_landmarkInput = NULL;
	m_surfaceInput = NULL;
	m_transformInput = NULL;
	m_landmarkOutput = new LandmarkSet();
	m_surfaceOutput = new Surface();

	m_mainLayout =  new QVBoxLayout();
    m_mainLayout->setGeometry(QRect(0, 0, 100, 100));
    QHBoxLayout *layout1= new QHBoxLayout;
    QHBoxLayout *layout2= new QHBoxLayout;

    m_selectedIndex = new QSpinBox();
    m_selectedIndex->setMinimum(0);
    m_selectedIndex->setMaximum(0);
    m_indexLabel = new QLabel("Selected Transform :");
    m_indexLabel->setBuddy(m_selectedIndex);

    //layout
    layout1->addWidget(m_indexLabel);
    layout1->addWidget(m_selectedIndex);

    m_buttonBox = new QDialogButtonBox();
    m_buttonBox->setOrientation(Qt::Horizontal);
    m_buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
    connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    layout2->addWidget(m_buttonBox);

    m_mainLayout->addLayout(layout1);
    m_mainLayout->addLayout(layout2);
    setLayout(m_mainLayout);

    m_indexChanged = false;

    setWindowTitle("Transform");
    setModal(true);
}

TransformNode::~TransformNode()
{}

void TransformNode::process()
{
    m_landmarkInput = getInputPortData0();
    m_surfaceInput = getInputPortData1();
	m_transformInput = getInputPortData2();

    if  ( m_transformInput != NULL && m_transformInput->isValid() )
    {
        m_selectedIndex->setMinimum(1);
        m_selectedIndex->setMaximum(m_transformInput->getSize());
        if(m_indexChanged)
        {
            Logger::getInstance()->log("[Transform Node] Processing started");
            int transformIndex = m_selectedIndex->value()-1;
            Matrix<double>* transform = m_transformInput->getTransformMatrix(transformIndex);
            if(m_landmarkInput!= NULL && m_landmarkInput->isValid())    //Transform Landmarks
            {
                m_landmarkOutput->FreeMatrix();
                m_landmarkOutput->transform(*transform);
                m_landmarkOutput->initialize("Transformed LandmarkSet");
                setOutputPort0(m_landmarkOutput);
            }

            if(m_surfaceInput!= NULL && m_surfaceInput->isValid())      //Transform surface
            {
                m_surfaceOutput->clear();
                m_surfaceOutput->initialize(m_surfaceInput, "Transformed Surface");
                m_surfaceOutput->applyTransform(*transform);
                m_surfaceOutput->refreshGeometry();
                setOutputPort1(m_surfaceOutput);
            }
            m_indexChanged = false;
            Logger::getInstance()->log("[Transform Node] Processing finished");
        }
    }
}

void TransformNode::accept()
{
    if(m_transformInput != NULL && m_transformInput->isValid())
    {
        m_indexChanged = m_selectedIndex->value()!= 0 &&
                        (m_selectedIndex->value() <= (int)m_transformInput->getSize());
        if(m_indexChanged)
            emit dataReady();
    }
    QDialog::accept();
}

bool TransformNode::isUpdated() const
{
	return m_indexChanged;
}


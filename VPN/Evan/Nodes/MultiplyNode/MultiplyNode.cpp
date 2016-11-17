#include "MultiplyNode.h"
#include "../../Utilities/Logger.h"

MultiplyNode::MultiplyNode(QWidget* parent) : QDialog(parent),  INode()
{
	m_input = NULL;
	m_output = new Loadings();
	setupUi(this);

    m_mult = 1.0;
    doubleSpinBox->setValue(m_mult);

    QObject::connect(doubleSpinBox, SIGNAL(valueChanged()), this, SLOT(controlsChanged()));

    m_updated = true;
}

MultiplyNode::~MultiplyNode()
{}

void MultiplyNode::process()
{
    Logger::getInstance()->log("[Multiply Node] Processing started");

    m_input = getInputPortData0();

    if (m_input == NULL || !m_input->isValid())
        return;

    m_output->ReSize(m_input->GetRows(),m_input->GetCols());

    double mult = doubleSpinBox->value();

    for(unsigned int i=0;i<m_input->GetRows();i++)
        for(unsigned int j=0;j<m_input->GetCols();j++)
            (*m_output)[i][j] = mult * (*m_input)[i][j];

    for(size_t i=0u;i<m_input->GetCols();i++)
        m_output->setRowLabel(i,m_input->getRowLabel(i));

    for(size_t j=0u;j<m_input->GetCols();j++)
        m_output->setColumnLabel(j,m_input->getColumnLabel(j));

    m_output->setNumScores(m_input->getNumScores());
    setOutputPort0(m_output);

    m_updated = false;

    Logger::getInstance()->log("[Multiply Node] Processing finished");
}

bool MultiplyNode::isUpdated() const
{
    return m_updated;
}

QString MultiplyNode::toString() const
{
    QString nodeData;
    nodeData.setNum(doubleSpinBox->value());
    return nodeData;
}

void MultiplyNode::fromString(const QString& params)
{
    if(params != "")
    {
        bool ok;
        m_mult = params.toDouble(&ok);
        if (!ok)
        {
            m_mult = 1.0;
        }
        doubleSpinBox->setValue(m_mult);
    }
}

void MultiplyNode::accept()
{
    if (m_updated)
    {
        emit dataReady();
        m_mult = doubleSpinBox->value();
    }
    QDialog::accept();
}

void MultiplyNode::reject()
{
    m_updated = false;
    doubleSpinBox->setValue(m_mult);
    QDialog::reject();
}

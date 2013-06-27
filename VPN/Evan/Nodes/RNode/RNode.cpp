#ifdef USE_R_

#include "RNode.h"
#include "RInside.h"
#include "../../DataTypes/RList.h"
#include "../../Utilities/Logger.h"
#include <QLineEdit>
#include <QHeaderView>
#include <QSettings>

#ifdef WIN32
bool getRegistryEntryForR()
{
    bool foundR = false;

    QString RVersion = "";
    QString RPath = "";

    // Try to locate R
    QSettings Rentry("HKEY_LOCAL_MACHINE\\Software\\R-core\\R", QSettings::NativeFormat);
    QStringList Rgroups = Rentry.childGroups();
    QStringList Rkeys = Rentry.childKeys();

    if(Rentry.allKeys().size()>0)
    {
        RVersion = Rentry.value("Current Version","").toString();
        RPath = Rentry.value("InstallPath","").toString();
        Logger::getInstance()->log("R version: " + RVersion);
        Logger::getInstance()->log("R install path: " + RPath);
        foundR = true;
    }
    else
    {
        Logger::getInstance()->log("R version not found");
        Logger::getInstance()->log("R can be downloaded from: http://www.r-project.org/");
    }

    return foundR;
}

#endif

RNode::RNode(QWidget* parent) : QDialog(parent),  INode()
{

#ifdef WIN32

    m_foundR = getRegistryEntryForR();
//    m_foundR  = (system("R --version") == 0);
#endif

#ifdef linux

    char * pPath;
    pPath = getenv("R_HOME");
    if(pPath != NULL)
    {
        QString qPath(pPath);
        Logger::getInstance()->log("R home directory: " + qPath);
        m_foundR = true;
    }
    else
    {
        Logger::getInstance()->log("R home directory not found",Logger::WARNING);
        Logger::getInstance()->log("R_HOME environment variable needs to be set(e.g. R_HOME=/usr/lib/R)",Logger::WARNING);
        m_foundR = false;
    }

#endif

    if (m_foundR)
    {
        m_REnv = new RInside();
        m_rOutputs = new RList();
        m_rInputs = NULL;
        m_updated = false;
    }
	setupUi(this);


//	#include <RInside.h>                    // for the embedded R via RInside
//    RInside R();              // create an embedded R instance
//    R["txt"] = "Hello, world!\n";	// assign a char* (string) to 'txt'
//    R.parseEval("cat(txt)");           // eval the init string, ignoring any returns
//Logger::getInstance()->log("done");

	connect(m_rCommandArea, SIGNAL(textChanged()), this, SLOT(controlsChanged()));
}

RNode::~RNode()
{
    delete m_REnv;
}

void RNode::process()
{
    if(!m_foundR)
    {
        Logger::getInstance()->log("[R Node] R not installed or R_HOME environment variable not set. R node will not be processed",Logger::RUN_ERROR);
        m_updated = false;
        return;
    }

    m_rInputs = getInputPortData0();

    if( m_rInputs == NULL || !m_rInputs->isValid() )
        return;

    clearInputsTable();
    for(unsigned int i=0; i<m_rInputs->getSize(); ++i)
    {
        int row = m_inputsTable->rowCount();
        m_inputsTable->insertRow(row);

        QLineEdit *item2 = new QLineEdit;

        m_inputsTable->setCellWidget(row, 0, item2);
        item2->setValidator(new QRegExpValidator(QRegExp("[a-zA-Z_][a-zA-Z0-9_]*$"),0));
        item2->setText(m_rInputs->getName(i));

        QTableWidgetItem *newItem = new QTableWidgetItem(m_rInputs->getDescription(i));
        newItem->setFlags(Qt::ItemIsEnabled);
        m_inputsTable->setItem(row, 1, newItem);
    }
    m_inputsTable->resizeColumnsToContents();
    m_inputsTable->horizontalHeader()->setStretchLastSection(true);

    QString textArea = m_rCommandArea->toPlainText();
    if(textArea.isEmpty() || textArea == "" || !m_updated)
        return;

    Logger::getInstance()->log("[R Node] Processing started");
    std::string evalstr = textArea.toStdString();
    SEXP ans = R_NilValue;
//    delete m_REnv;
//    m_REnv = new RInside();
    m_REnv->assign( m_rInputs->getReturnList(), "inputs", false);
    switch(m_REnv->parseEval(evalstr, ans))
    {
        case 0: //Success
            m_rOutputs->clear();
            switch(m_rOutputs->appendList(ans)) //Check the outputs
            {
                case 0: //Correct format of outputs
                    setOutputPort0(m_rOutputs);
                    Logger::getInstance()->log("[R Node] Processing finished");
                break;
                case 1:
                    if(isOutputConnected(0))
                    {
                        Logger::getInstance()->log("[R Node] Invalid output found, R code must return a list!",Logger::RUN_ERROR);
                    }
                    else
                    {
                        Logger::getInstance()->log("[R Node] Processing finished");
                    }
                break;
                case 2:
                    Logger::getInstance()->log("[R Node] No list element names found!",Logger::WARNING);
                break;
                case 3:
                    Logger::getInstance()->log("[R Node] Non-numeric or invalid output found!",Logger::RUN_ERROR);
                break;
                case 4:
                    Logger::getInstance()->log("[R Node] Undefined Output!",Logger::RUN_ERROR);
                break;
                default: break;
            }
        break;
        case 1:
            Logger::getInstance()->log("[R Node] Parse error, incorrect R syntax!",Logger::RUN_ERROR);
        break;
        case 2:
            Logger::getInstance()->log("[R Node] Invalid R code, check used variables!",Logger::RUN_ERROR);
        break;
        case 3:
            Logger::getInstance()->log("[R Node] Parse NULL error!",Logger::RUN_ERROR);
        break;
        case 4:
            Logger::getInstance()->log("[R Node] Parse error, incomplete R syntax!",Logger::RUN_ERROR);
        break;
        case 5:
            Logger::getInstance()->log("[R Node] Unknown Parse error, incorrect R syntax!",Logger::RUN_ERROR);
        break;
        default: break;
    }
	m_updated = false;
}

void RNode::clearInputsTable()
{
    m_inputsTable->clearContents();
    for(int i=m_inputsTable->rowCount()-1; i>=0; --i)
       m_inputsTable->removeRow(i);
}

void RNode::renameInputs()
{
    if( m_rInputs == NULL || !m_rInputs->isValid() || m_rInputs->getSize()!=(unsigned int)m_inputsTable->rowCount())
        return;
    for(int i=0; i<m_inputsTable->rowCount(); ++i)
    {
        QLineEdit* varName = (QLineEdit*)m_inputsTable->cellWidget(i,0);
        m_rInputs->setName(i, varName->text().toStdString());
    }
}

void RNode::accept()
{
    if(m_updated)
    {
        renameInputs();
        emit dataReady();
    }
    QDialog::accept();
}

bool RNode::isUpdated() const
{
	return m_updated;
}

QString RNode::toString() const
{
    QString nodeData = "";
    nodeData += m_rCommandArea->toPlainText();
    return  nodeData;
}

void RNode::fromString(const QString& params)
{
    if (params  != "")
    {
        m_rCommandArea->setPlainText(params);
    }
}

#endif

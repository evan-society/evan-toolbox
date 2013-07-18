#include "SelectNode.h"
#include "../../Utilities/Logger.h"
#include "../../Utilities/ExpressionParser.h"
#include <QHeaderView>
#include <QMessageBox>

SelectNode::SelectNode(QWidget* parent) : QDialog(parent),  INode() //, m_rangeMin(0) , m_rangeMax(0)
{
    m_updated = false;
	m_specimenInput = NULL;
	m_specimenOutput = new Specimens();
	m_specimensModel = new SpecimensModel(m_specimenInput, this);
	m_lmkSetModel = new LandmarkSetModel(0, this);

	setupUi(this);
	m_specimensView->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_landmarksView->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_specimensView->setModel(m_specimensModel);
	m_landmarksView->setModel(m_lmkSetModel);

    connect(m_specimensView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(specimenChanged(const QModelIndex&)));
    connect(m_specimensView, SIGNAL(selectedAll()),this, SLOT(controlsChanged()));
    connect(m_lmkSetModel, SIGNAL(toggleLandmark(unsigned int, bool)),
            m_specimensModel, SLOT(checkLandmark(unsigned int, bool)));
    connect(m_lmkSetModel, SIGNAL(toggleLandmark(unsigned int, bool)),this, SLOT(controlsChanged()));
    connect(m_specimensModel, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),this, SLOT(controlsChanged()));
    connect(m_exprButton, SIGNAL(pressed()), this, SLOT(evaluateExpression()));
}

SelectNode::~SelectNode()
{
    delete m_specimensModel;
    delete m_lmkSetModel;
}

void SelectNode::process()
{
	m_specimenInput = getInputPortData0();
	if(m_specimenInput == NULL || !m_specimenInput->isValid())
        return;

    m_specimensView->clearSelection();
    m_landmarksView->clearSelection();
    m_specimensModel->setDataSource(m_specimenInput);
    m_lmkSetModel->setDataSource(NULL, m_specimensModel->getUnselectedLMKs());
    m_specimensView->resizeColumnsToContents();
    m_specimensView->horizontalHeader()->setStretchLastSection(true);

    if(m_loadedSelection.count() > 0)
    {
        m_specimensModel->addSelection(m_loadedSelection);
        if(m_loadedLmksSelection.count() > 0)
        {
            foreach (unsigned int value, m_loadedLmksSelection)
            {
                m_specimensModel->checkLandmark(value, false);
            }
            m_loadedLmksSelection.clear();
        }
        doSelection();
//        m_specimensView->setFocus();
//        m_updated = true;
        m_loadedSelection.clear();
    }
}

void SelectNode::doSelection()
{
    if(m_specimensModel->getSelectedSpecimens().count() == 0)
        return;
    m_specimenOutput->clear();
    int selectedCount = 0;
    Logger::getInstance()->log("[Select Node] Processing started");

    if(m_specimenInput->getGroupNumber() > 1)
    {
        for (unsigned int i=0; i<m_specimenInput->getGroupNumber(); ++i)    //Add Groups
        {
            SpecimenGroup* sg = m_specimenInput->getGroup(i);
            SpecimenGroup* newGroup = new SpecimenGroup;
            for (unsigned int j=0; j<sg->getSize(); ++j)
            {
                unsigned int specimenIndex = sg->getMember(j)->getSpecimenIndex();

                if (specimenIndex == m_specimenInput->getSize())
                {
                    newGroup->addMean(m_specimenInput->getSize(),
                                    sg->getMemberID(j),
                                    sg->getMemberColor(j),
                                    sg->getMemberViewerSize(j),
                                    sg->getMemberPlotSymbol(j));
                }
                else
                {
                    newGroup->addMember(  m_specimenInput->getLandmarkSet(specimenIndex),
                                    sg->getMemberID(j),
                                    sg->getMemberColor(j),
                                    sg->getMemberViewerSize(j),
                                    sg->getMemberPlotSymbol(j));
                }
            }
            newGroup->setGroupColor(sg->getGroupColor());
            newGroup->setGroupLMKSize(sg->getGroupLMKSize());
            newGroup->setGroupPlotSymbol(sg->getGroupPlotSymbol());

            m_specimenOutput->addGroup(m_specimenInput->getGroupName(i), newGroup);
        }
        //Add the selected specimens to the output
        foreach(unsigned int i, m_specimensModel->getSelectedSpecimens())
        {
            if( ( static_cast<int>(i) < 0 ) || ( i >= m_specimenInput->getSize() ) )
                continue;
            unsigned int unselectedLMKs = m_specimensModel->getUnselectedLMKs().count();
            LandmarkSet* oldSet = m_specimenInput->getLandmarkSet(i);
            if(unselectedLMKs == oldSet->GetRows())
                continue;
            LandmarkSet* newSet = new LandmarkSet(oldSet->GetRows()-unselectedLMKs, oldSet->GetCols());
            for(unsigned int x=0, j=0; x<oldSet->GetRows(); ++x)
            {
                if(m_specimensModel->getUnselectedLMKs().contains(x))
                    continue;
                for(unsigned int y=0; y<oldSet->GetCols(); ++y)
                    newSet->set(j, y, oldSet->get(x,y));
                j++;
            }
            m_specimenOutput->addLandmarkSet(newSet);
            m_specimenOutput->addSpecimenLabels(m_specimenInput->getSpecimenLabels(i));
            m_specimenOutput->setCentroidSize(i,m_specimenInput->getCentroidSize(i));
            selectedCount++;
        }
    }
    else
    {
        //Add the selected specimens to the output
        SpecimenGroup* defaultGroup = new SpecimenGroup;
        foreach(unsigned int i, m_specimensModel->getSelectedSpecimens())
        {
            if( ( static_cast<int>(i) < 0 ) || ( i >= m_specimenInput->getSize() ) )
                continue;
            unsigned int unselectedLMKs = m_specimensModel->getUnselectedLMKs().count();
            LandmarkSet* oldSet = m_specimenInput->getLandmarkSet(i);
            if(unselectedLMKs == oldSet->GetRows())
                continue;
            LandmarkSet* newSet = new LandmarkSet(oldSet->GetRows()-unselectedLMKs, oldSet->GetCols());
            for(unsigned int x=0, j=0; x<oldSet->GetRows(); ++x)
            {
                if(m_specimensModel->getUnselectedLMKs().contains(x))
                    continue;
                for(unsigned int y=0; y<oldSet->GetCols(); ++y)
                    newSet->set(j, y, oldSet->get(x,y));
                j++;
            }
            defaultGroup->addMember(newSet, m_specimenInput->getSpecimenLabelValue(i, "ID"));
            m_specimenOutput->addLandmarkSet(newSet);
            m_specimenOutput->addSpecimenLabels(m_specimenInput->getSpecimenLabels(i));
            m_specimenOutput->setCentroidSize(i,m_specimenInput->getCentroidSize(i));
            selectedCount++;
        }
        m_specimenOutput->addGroup("Ungrouped", defaultGroup);
    }
    m_specimenOutput->setLandmarkLabels(m_specimenInput->getLandmarkLabels());
    m_specimenOutput->initialize("Selection");
    setOutputPort0(m_specimenOutput);

    Logger::getInstance()->log(QString("[Select Node] Selected (%1) specimens").arg(selectedCount));
    Logger::getInstance()->log("[Select Node] Processing finished");
}

void SelectNode::accept()
{
    if(m_updated)
    {
        doSelection();
        emit dataReady();
        m_updated = false;
    }
    else
    {
        Logger::getInstance()->log(QString("[Select Node] No specimens selected."));
    }
    QDialog::accept();
}

void SelectNode::specimenChanged(const QModelIndex& index)
{
    m_specimenInput = getInputPortData0();
	if(m_specimenInput == NULL || !m_specimenInput->isValid())
        return;
    m_lmkSetModel->setDataSource(m_specimenInput->getLandmarkSet(index.row()), m_specimensModel->getUnselectedLMKs());
    groupBox_2->setTitle(QString("Landmarks for '%1'").arg(m_specimenInput->getSpecimenLabelValue(index.row(), "ID")));
//    m_landmarksView->resizeColumnsToContents();
//    m_landmarksView->horizontalHeader()->setStretchLastSection(true);
}

bool containsLetter(QString s)
{
    for(int i=0;i<s.size();i++)
    {
        if(s[i].isLetter())
            return true;
    }
    return false;
}

void SelectNode::evaluateExpression()
{
    m_specimenInput = getInputPortData0();

    if(!containsLetter(m_exprLineEdit->text()))
    {
        parseIndices();
        return;
    }
    ExpressionParser selectParser(m_specimenInput);
    selectParser.parseExpression(m_exprLineEdit->text()+" ");
    if(selectParser.isParsed())
    {
        m_specimensModel->addSelection(selectParser.getSelected());
        m_specimensView->setFocus();
        m_updated = true;
    }
    else
        QMessageBox::warning(0, "Parse Error", selectParser.getErrors().front());
}

void SelectNode::parseIndices()
{
    QString str = m_exprLineEdit->text().trimmed();

    if (str.startsWith("[") && str.endsWith("]"))
    {
        str = str.remove(0,1);
        str = str.remove(str.size()-1,1);
    }

    QStringList strlist = str.split(",");
    QSet<unsigned int> selected;

    for (int i = 0;i<strlist.size();i++)
    {
        if (strlist[i].contains("-"))
        {
            QStringList temp = strlist[i].split("-");
            if(temp.size()==2)
            {
                bool success1;
                bool success2;

                int start = temp[0].toInt(&success1);
                int stop = temp[1].toInt(&success2);

                if(success1 && success2)
                {
                    if( (int)m_specimenInput->getSize() < stop)
                    {
                        Logger::getInstance()->log("[Select Node] Index out of range: " + temp[1], Logger::WARNING);
                        stop = -1;
                    }
                    if( (int)m_specimenInput->getSize() < start)
                    {
                        Logger::getInstance()->log("[Select Node] Index out of range: " + temp[0], Logger::WARNING);
                    }
                    if(start > stop)
                    {
                        Logger::getInstance()->log("[Select Node] Cannot parse: " + strlist[i], Logger::WARNING);
                    }
                    else
                    {
                        for (int j = start;j<=stop;j++)
                        {
                            selected.insert(j-1);
                        }
                    }
                }
                else
                {
                    Logger::getInstance()->log("[Select Node] Cannot parse: " + strlist[i], Logger::WARNING);
                }
            }
            else
            {
                Logger::getInstance()->log("[Select Node] Cannot parse: " + strlist[i], Logger::WARNING);
            }
        }
        else
        {
            bool success;
            int j = strlist[i].toInt(&success);
            if(success)
            {
                if(j > 0 && j < (int)m_specimenInput->getSize())
                {
                    selected.insert(j-1);
                }
                else
                {
                    Logger::getInstance()->log("[Select Node] Index out of range: " + strlist[i], Logger::WARNING);
                }
            }
            else
            {
                Logger::getInstance()->log("[Select Node] Cannot parse: " + strlist[i], Logger::WARNING);
            }
        }
    }

    if(selected.size()>0)
    {
        m_specimensModel->addSelection(selected);
        m_specimensView->setFocus();
        m_updated = true;
    }
}


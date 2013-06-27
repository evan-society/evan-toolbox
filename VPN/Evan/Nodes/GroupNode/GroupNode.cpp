#include "GroupNode.h"
#include "PlotSymbolCombo.h"
#include "../../Utilities/Logger.h"
#include "../../Utilities/ExpressionParser.h"
#include <QHeaderView>
#include <QDialog>
#include <QMessageBox>

GroupNode::GroupNode(QWidget* parent) : QDialog(parent),  INode()
{
    m_updated = false;
	m_specimensInput = NULL;
	m_specimensOutput = new Specimens();
	m_groupMeans = new Specimens();
	m_specimensModel = new SpecimensModel(m_specimensInput, this);
	setupUi(this);
	m_specimensView->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_specimensView->setModel(m_specimensModel);

    connect(m_specimensModel, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),this, SLOT(checkSelection()));
	connect(m_exprButton, SIGNAL(pressed()), this, SLOT(evaluateExpression()));
	connect(m_groupsView, SIGNAL(itemSelectionChanged()), this, SLOT(checkSelection()));
	connect(m_newGrpBtn, SIGNAL(pressed()), m_groupsView, SLOT(addNewGroup()));
	connect(m_delGroupBtn, SIGNAL(pressed()), this, SLOT(deleteGroup()));
	connect(m_addButton, SIGNAL(pressed()), this, SLOT(addToGroup()));
	connect(m_removeButton, SIGNAL(pressed()), this, SLOT(removeFromGroup()));
	connect(m_groupsView, SIGNAL(membersUpdated()), this, SLOT(controlsChanged()));
	connect(m_addButton, SIGNAL(pressed()),this, SLOT(controlsChanged()));
	connect(m_delGroupBtn, SIGNAL(pressed()),this, SLOT(controlsChanged()));
	connect(m_removeButton, SIGNAL(pressed()),this, SLOT(controlsChanged()));

	m_autoMeanCheck->hide();
}

GroupNode::~GroupNode()
{
    delete m_specimensModel;
    clearLoadedGroups();
}

void GroupNode::clearLoadedGroups()
{
    foreach(GroupTreeItem* groupItem, m_loadedGroupItems)
    {
        for(int i=0; i<groupItem->childCount(); ++i)
            delete groupItem->child(i);
        delete groupItem;
    }
    m_loadedGroupItems.clear();
}

void GroupNode::process()
{
	m_specimensInput = getInputPortData0();
	if(m_specimensInput == NULL || !m_specimensInput->isValid())
        return;

    m_specimensView->clearSelection();
    m_specimensModel->setDataSource(m_specimensInput);
    m_specimensView->resizeColumnsToContents();
    m_specimensView->horizontalHeader()->setStretchLastSection(true);

    m_groupsView->clear();
    for(unsigned int i=0; i<m_specimensInput->getGroupNumber(); ++i)
    {
        SpecimenGroup* group = m_specimensInput->getGroup(i);
        GroupTreeItem* groupItem = new GroupTreeItem(m_groupsView);
        groupItem->setText(0, m_specimensInput->getGroupName(i));
        groupItem->setData(0, Qt::DecorationRole, group->getGroupColor());
        groupItem->setText(1, QString().setNum(group->getGroupLMKSize()));
        groupItem->setTextAlignment(1, Qt::AlignCenter);

        PlotSymbolCombo* groupPlotSym = new PlotSymbolCombo(m_groupsView, groupItem);
        groupPlotSym->setCurrentIndex(group->getGroupPlotSymbol());
        m_groupsView->setItemWidget(groupItem, 2, groupPlotSym);
        connect(groupPlotSym, SIGNAL(symbolChanged(int,int,QTreeWidgetItem*)), m_groupsView, SLOT(changeGroupPSymbol(int,int,QTreeWidgetItem*)));

        if(!i)
        {
            groupItem->setFlags(Qt::NoItemFlags);
            groupPlotSym->setDisabled(true);
        }

        for(unsigned int j=0; j<group->getSize(); ++j)
        {
            MemberTreeItem* memberItem = new MemberTreeItem(groupItem);
            memberItem->setText(0, group->getMemberID(j));
            memberItem->setData(0, Qt::DecorationRole,
                                group->getMemberColor(j)==Qt::transparent ? group->getGroupColor() : group->getMemberColor(j));
            memberItem->setText(1, QString().setNum(group->getMemberViewerSize(j)));
            memberItem->setTextAlignment(1, Qt::AlignCenter);
            memberItem->setColor(group->getMemberColor(j));
            memberItem->setSpecimenIndex(group->getMember(j)->getSpecimenIndex());

            PlotSymbolCombo* memberPlotSym = new PlotSymbolCombo(m_groupsView,memberItem);
            memberPlotSym->setCurrentIndex(group->getMemberPlotSymbol(j));
            m_groupsView->setItemWidget(memberItem, 2, memberPlotSym);
            connect(memberPlotSym, SIGNAL(symbolChanged(int,int,QTreeWidgetItem*)), m_groupsView, SLOT(changeGroupPSymbol(int,int,QTreeWidgetItem*)));

            if(!i)
            {
                memberItem->setFlags(Qt::NoItemFlags);
                memberPlotSym->setDisabled(true);
            }
            if(memberItem->getSpecimenIndex() == m_specimensInput->getSize())
                groupItem->setHasMean(true);
        }
    }
    fillLoadedGroups();
    m_groupsView->resizeColumnToContents(0);
    m_groupsView->resizeColumnToContents(1);
    m_groupsView->resizeColumnToContents(2);
}

void GroupNode::doGrouping()
{
    m_specimensInput = getInputPortData0();
	if(m_specimensInput == NULL || !m_specimensInput->isValid())
        return;

    m_specimensOutput->clear();
    m_groupMeans->clear();
    Logger::getInstance()->log("[Group Node] Processing started");

    //Fill the output with the input specimens
    for(unsigned int i=0; i<m_specimensInput->getSize(); ++i)
    {
        LandmarkSet* newSet = new LandmarkSet(*m_specimensInput->getLandmarkSet(i));
        m_specimensOutput->addLandmarkSet(newSet);
        m_specimensOutput->addSpecimenLabels(m_specimensInput->getSpecimenLabels(i));
    }

    for(unsigned int i=0; i<m_specimensInput->getSize(); ++i)
    {
        double csize = m_specimensInput->getCentroidSize(i);
        m_specimensOutput->setCentroidSize(i,csize);
    }

    // Add default group for means
	SpecimenGroup* defaultGroup = new SpecimenGroup;

    //Add the grouping info
    for(int i=0; i<m_groupsView->topLevelItemCount(); ++i)
    {
        GroupTreeItem* groupItem = (GroupTreeItem*)m_groupsView->topLevelItem(i);
        if(groupItem->childCount() == 0)
            continue;
        SpecimenGroup* sg = new SpecimenGroup;
        for(int j=0; j<groupItem->childCount(); ++j)
        {
            MemberTreeItem* groupMemberItem = ((MemberTreeItem*)groupItem->child(j));
            unsigned int specimenIndex = groupMemberItem->getSpecimenIndex();
            if(specimenIndex == m_specimensInput->getSize())
                sg->addMean(m_specimensInput->getSize(), groupMemberItem->text(0),
                            groupMemberItem->getColor(), groupMemberItem->text(1).toFloat(),
                            ((PlotSymbolCombo*)m_groupsView->itemWidget(groupMemberItem,2))->currentIndex());
            else
                sg->addMember(  m_specimensOutput->getLandmarkSet(specimenIndex),
                                groupMemberItem->text(0),
                                groupMemberItem->getColor(),
                                groupMemberItem->text(1).toFloat(),
                                ((PlotSymbolCombo*)m_groupsView->itemWidget(groupMemberItem,2))->currentIndex());
        }
        sg->setGroupLMKSize(groupItem->text(1).toFloat());
        sg->setGroupColor(groupItem->data(0, Qt::DecorationRole).value<QColor>());
        sg->setGroupPlotSymbol(((PlotSymbolCombo*)m_groupsView->itemWidget(groupItem,2))->currentIndex());
        m_specimensOutput->addGroup(groupItem->text(0), sg);

        if( groupItem->text(0) != "Ungrouped" && sg->getSize()>0)
        {
            LandmarkSet* meanSpecimen = new LandmarkSet(*sg->getMember(0)); //initialize mean to first specimen
            for(unsigned int k=1; k<sg->getSize(); ++k)
            {
                *meanSpecimen += *sg->getMember(k);
            }
            *meanSpecimen /= (double)sg->getSize();
            m_groupMeans->addLandmarkSet(meanSpecimen);
            QString groupName = groupItem->text(0) + " Mean";
            m_groupMeans->setLabelValue(i, "ID", groupName);
            defaultGroup->addMember(meanSpecimen, groupName,
                                    groupItem->data(0, Qt::DecorationRole).value<QColor>(), groupItem->text(1).toFloat(),
                                    ((PlotSymbolCombo*)m_groupsView->itemWidget(groupItem,2))->currentIndex());
        }
    }

    m_groupMeans->addGroup("Group means", defaultGroup);

    m_groupMeans->initialize("Group means");
    setOutputPort1(m_groupMeans);

    m_specimensOutput->initialize("Grouped Specimens");
    setOutputPort0(m_specimensOutput);

    Logger::getInstance()->log("[Group Node] Processing finished");
}

void GroupNode::accept()
{
    if(m_updated)
    {
        doGrouping();
        emit dataReady();
        m_updated = false;
    }
    QDialog::accept();
}

void GroupNode::evaluateExpression()
{
    m_specimensInput = getInputPortData0();
    ExpressionParser selectParser(m_specimensInput);
    selectParser.parseExpression(m_exprLineEdit->text()+" ");
    if(selectParser.isParsed())
    {
        m_specimensModel->addSelection(selectParser.getSelected());
        m_specimensView->setFocus();
        checkSelection();
    }
    else
        QMessageBox::warning(0, "Parse Error", selectParser.getErrors().front());
}

void GroupNode::checkSelection()
{
    if(m_groupsView->selectedItems().count())
    {
        QTreeWidgetItem* selected = m_groupsView->selectedItems().front();
        m_addButton->setDisabled(! (m_groupsView->isTopLevel(selected) &&
                                    m_specimensModel->getSelectedSpecimens().count()));
        m_delGroupBtn->setDisabled(! (m_groupsView->isTopLevel(selected) ));
        m_removeButton->setDisabled(m_groupsView->isTopLevel(selected));
    }
    else
    {
        m_addButton->setDisabled(true);
        m_delGroupBtn->setDisabled(true);
        m_removeButton->setDisabled(true);
    }
}

void GroupNode::addToGroup()
{
    m_specimensInput = getInputPortData0();
    if(m_groupsView->selectedItems().count())
    {
        GroupTreeItem* selected = (GroupTreeItem*)m_groupsView->selectedItems().front();
        if(m_groupsView->isTopLevel(selected) && m_specimensModel->getSelectedSpecimens().count())
        {
            foreach(unsigned int i, m_specimensModel->getSelectedSpecimens())
            {
                if( m_groupsView->isAChildOf(selected, i) )
                    continue;
                int j = selected->childCount()-1;
                for( ; j>=0; --j)
                {
                    MemberTreeItem* groupMemberItem = ((MemberTreeItem*)selected->child(j));
                    if(groupMemberItem->getSpecimenIndex() < i)
                        break;
                }
                MemberTreeItem* memberItem = new MemberTreeItem();
                memberItem->setText(0, m_specimensInput->getSpecimenLabelValue(i, "ID"));
                memberItem->setData(0, Qt::DecorationRole,  selected->data(0, Qt::DecorationRole));
                memberItem->setText(1, selected->text(1));
                memberItem->setTextAlignment(1, Qt::AlignCenter);
                memberItem->setSpecimenIndex(i);
                selected->insertChild(j+1, memberItem);

                PlotSymbolCombo* memberPlotSym = new PlotSymbolCombo(m_groupsView,memberItem);
                PlotSymbolCombo* groupPlotSym = (PlotSymbolCombo*)m_groupsView->itemWidget(selected, 2);
                memberPlotSym->setCurrentIndex(groupPlotSym->currentIndex());
                m_groupsView->setItemWidget(memberItem, 2, memberPlotSym);
                connect(memberPlotSym, SIGNAL(symbolChanged(int,int,QTreeWidgetItem*)), m_groupsView, SLOT(changeGroupPSymbol(int,int,QTreeWidgetItem*)));

                removeFromUngrouped(i);
            }
            if(m_autoMeanCheck->isChecked() && !selected->hasMean())
            {
                MemberTreeItem* memberItem = new MemberTreeItem(selected);
                memberItem->setText(0, "MEAN");
                memberItem->setData(0, Qt::DecorationRole,  selected->data(0, Qt::DecorationRole));
                memberItem->setSpecimenIndex(m_specimensInput->getSize());
                memberItem->setText(1, selected->text(1));
                memberItem->setTextAlignment(1, Qt::AlignCenter);

                PlotSymbolCombo* memberPlotSym = new PlotSymbolCombo(m_groupsView,memberItem);
                PlotSymbolCombo* groupPlotSym = (PlotSymbolCombo*)m_groupsView->itemWidget(selected, 2);
                memberPlotSym->setCurrentIndex(groupPlotSym->currentIndex());
                m_groupsView->setItemWidget(memberItem, 2, memberPlotSym);
                connect(memberPlotSym, SIGNAL(symbolChanged(int,int,QTreeWidgetItem*)), m_groupsView, SLOT(changeGroupPSymbol(int,int,QTreeWidgetItem*)));

                selected->setHasMean(true);
            }
            selected->setExpanded(true);
        }
    }
    m_groupsView->resizeColumnToContents(0);
    m_specimensModel->unSelectAllSpecimens();
    m_specimensView->setFocus();
    checkSelection();
}

void GroupNode::removeFromGroup()
{
    if(m_groupsView->selectedItems().count())
    {
        MemberTreeItem* selected = ((MemberTreeItem*)m_groupsView->selectedItems().front());
        unsigned int removedIndex = selected->getSpecimenIndex();
        GroupTreeItem* groupItem = (GroupTreeItem*)selected->parent();
        groupItem->removeChild(selected);
        if(removedIndex == m_specimensInput->getSize())
            groupItem->setHasMean(false);
        else
            addToUngrouped(removedIndex);
    }
}

void GroupNode::removeFromUngrouped(unsigned int index)
{
    if(!m_groupsView->topLevelItemCount())
        return;
    QTreeWidgetItem* ungroupedItem = m_groupsView->topLevelItem(0);
    for(int i=0; i<ungroupedItem->childCount(); ++i)
    {
        MemberTreeItem* groupMemberItem = ((MemberTreeItem*)ungroupedItem->child(i));
        if(groupMemberItem->getSpecimenIndex() == index)
        {
            ungroupedItem->removeChild(groupMemberItem);
            break;
        }
    }
}

void GroupNode::addToUngrouped(unsigned int index)
{
    if(!m_groupsView->topLevelItemCount())
        return;
    bool found = false;
    for(int i=1; i<m_groupsView->topLevelItemCount(); ++i)
    {
        QTreeWidgetItem* groupItem = m_groupsView->topLevelItem(i);
        for(int j=0; j<groupItem->childCount(); ++j)
        {
            MemberTreeItem* groupMemberItem = ((MemberTreeItem*)groupItem->child(j));
            if(groupMemberItem->getSpecimenIndex() == index)
            {
                found = true;
                break;
            }
        }
        if(found) break;
    }

    if(!found)
    {
        QTreeWidgetItem* ungroupedItem = m_groupsView->topLevelItem(0);
        int i = ungroupedItem->childCount()-1;
        for( ; i>=0; --i)
        {
            MemberTreeItem* groupMemberItem = ((MemberTreeItem*)ungroupedItem->child(i));
            if(groupMemberItem->getSpecimenIndex() < index)
                break;
        }
        MemberTreeItem* memberItem = new MemberTreeItem();
        memberItem->setText(0, m_specimensInput->getSpecimenLabelValue(index, "ID"));
        memberItem->setData(0, Qt::DecorationRole,  ungroupedItem->data(0, Qt::DecorationRole));
        memberItem->setText(1, ungroupedItem->text(1));
        memberItem->setTextAlignment(1, Qt::AlignCenter);
        memberItem->setSpecimenIndex(index);
        ungroupedItem->insertChild(i+1, memberItem);

        PlotSymbolCombo* memberPlotSym = new PlotSymbolCombo(m_groupsView,memberItem);
        PlotSymbolCombo* groupPlotSym = (PlotSymbolCombo*)m_groupsView->itemWidget(ungroupedItem, 2);
        memberPlotSym->setCurrentIndex(groupPlotSym->currentIndex());
        m_groupsView->setItemWidget(memberItem, 2, memberPlotSym);
        memberPlotSym->setDisabled(true);
    }
}

QString GroupNode::toString() const
{
    QString result = "";
    for(int i=0; i<m_groupsView->topLevelItemCount(); ++i)
    {
        GroupTreeItem* groupItem = (GroupTreeItem*)m_groupsView->topLevelItem(i);
        QColor groupColor = groupItem->data(0, Qt::DecorationRole).value<QColor>();
        result += groupItem->text(0)+":";
        result += groupItem->text(1)+":";
        result +=   QString().setNum(groupColor.red())  +"|"+
                    QString().setNum(groupColor.green())+"|"+
                    QString().setNum(groupColor.blue())+":";
        result += QString().setNum(((PlotSymbolCombo*)m_groupsView->itemWidget(groupItem, 2))->currentIndex())+":";

        for(int j=0; j<groupItem->childCount(); ++j)
        {
            MemberTreeItem* groupMemberItem = ((MemberTreeItem*)groupItem->child(j));
            result +=   groupMemberItem->text(0) + "^";
            result +=   groupMemberItem->text(1) + "^";
            result +=   QString().setNum(groupMemberItem->getSpecimenIndex()) + "^";
            result +=   QString().setNum(groupMemberItem->getColor().red())  +"|"+
                        QString().setNum(groupMemberItem->getColor().green())+"|"+
                        QString().setNum(groupMemberItem->getColor().blue())+"|"+
                        QString().setNum(groupMemberItem->getColor().alpha())+"^";
            result += QString().setNum(((PlotSymbolCombo*)m_groupsView->itemWidget(groupMemberItem, 2))->currentIndex())+"/";
        }

        result += ",";
    }
    return result;
}

void GroupNode::fromString(const QString& params)
{
    QStringList groupsData = params.split(",");
    foreach(QString groupData, groupsData)
    {
        QStringList groupParams = groupData.split(":");
        if(groupParams.count() != 5)
            continue;
        if(groupParams[4] == "" || groupParams[4].isEmpty())
            continue;
        QStringList groupColors = groupParams[2].split("|");
        if(groupColors.count() != 3)
            continue;

        GroupTreeItem* groupItem = new GroupTreeItem();
        groupItem->setText(0, groupParams[0]);
        groupItem->setText(1, groupParams[1]);
        groupItem->setTextAlignment(1, Qt::AlignCenter);
        QColor groupColor = QColor( groupColors[0].toInt(),
                                    groupColors[1].toInt(),
                                    groupColors[2].toInt());
        groupItem->setData(0, Qt::DecorationRole, groupColor);
        groupItem->setText(2, groupParams[3]);

        QStringList membersData = groupParams[4].split("/");
        foreach(QString memberData, membersData)
        {
            QStringList memberParams = memberData.split("^");
            if(memberParams.count() != 5)
                continue;
            QStringList memberColors = memberParams[3].split("|");
            if(memberColors.count() != 4)
                continue;

            MemberTreeItem* memberItem = new MemberTreeItem(groupItem);
            memberItem->setText(0, memberParams[0]);
            memberItem->setText(1, memberParams[1]);
            memberItem->setTextAlignment(1, Qt::AlignCenter);
            memberItem->setSpecimenIndex(memberParams[2].toInt());
            QColor memberColor = QColor(memberColors[0].toInt(),
                                         memberColors[1].toInt(),
                                         memberColors[2].toInt(),
                                         memberColors[3].toInt());
            memberItem->setData(0, Qt::DecorationRole, memberColor==Qt::transparent? groupColor: memberColor);
            memberItem->setColor(memberColor);

            memberItem->setText(2, memberParams[4]);
        }
        m_loadedGroupItems.push_back(groupItem);
    }
}

void GroupNode::fillLoadedGroups()
{
    if(m_loadedGroupItems.count() ==0)
        return;
    QStringList existingGroups;
    for(int i=0; i<m_groupsView->topLevelItemCount(); ++i)
        existingGroups.push_back(m_groupsView->topLevelItem(i)->text(0));

    QList<GroupTreeItem*>::iterator x = m_loadedGroupItems.begin();
    loopBegin:
    while(x != m_loadedGroupItems.end())
    {
        GroupTreeItem* groupItem = *x;
        if(existingGroups.contains(groupItem->text(0)))
        {
            ++x;
            continue;
        }
        for(int i=0; i<groupItem->childCount(); ++i)
        {
            MemberTreeItem* memberItem = (MemberTreeItem*)groupItem->child(i);
            if(memberItem->getSpecimenIndex() > m_specimensInput->getSize() || memberItem->getSpecimenIndex()<0)
            {
                ++x;
                goto loopBegin;
            }
            else if(memberItem->getSpecimenIndex() == m_specimensInput->getSize())
                groupItem->setHasMean(true);
            else
            {
                memberItem->setText(0, m_specimensInput->getSpecimenLabelValue(memberItem->getSpecimenIndex(), "ID"));
                removeFromUngrouped(memberItem->getSpecimenIndex());
            }
        }

        m_groupsView->addTopLevelItem(groupItem);

        //Update the symbol combos
        PlotSymbolCombo* groupPlotSym = new PlotSymbolCombo(m_groupsView, groupItem);
        groupPlotSym->setCurrentIndex(groupItem->text(2).toInt());
        m_groupsView->setItemWidget(groupItem, 2, groupPlotSym);
        connect(groupPlotSym, SIGNAL(symbolChanged(int,int,QTreeWidgetItem*)), m_groupsView, SLOT(changeGroupPSymbol(int,int,QTreeWidgetItem*)));

        for(int i=0; i<groupItem->childCount(); ++i)
        {
            MemberTreeItem* memberItem = (MemberTreeItem*)groupItem->child(i);
            PlotSymbolCombo* memberPlotSym = new PlotSymbolCombo(m_groupsView,memberItem);
            memberPlotSym->setCurrentIndex(memberItem->text(2).toInt());
            m_groupsView->setItemWidget(memberItem, 2, memberPlotSym);
            connect(memberPlotSym, SIGNAL(symbolChanged(int,int,QTreeWidgetItem*)), m_groupsView, SLOT(changeGroupPSymbol(int,int,QTreeWidgetItem*)));
        }

        x = m_loadedGroupItems.erase(x);
    }
    doGrouping();
    clearLoadedGroups();
}

void GroupNode::deleteGroup()
{
    if(!m_groupsView->selectedItems().count())
        return;
    QTreeWidgetItem* selected = m_groupsView->selectedItems().front();
    if(!selected)
        return;
    if(QMessageBox::question(this, "Delete?", "Are you sure you want to delete this group?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::No)
        return;
    foreach(QTreeWidgetItem* member, selected->takeChildren())
        delete member;
    delete selected;
}

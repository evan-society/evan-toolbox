#ifndef RENDEREABLETREE_H
#define RENDEREABLETREE_H

#include <QTreeWidgetItem>
#include <osg/Switch>
#include <QMenu>
#include <DataTypes/Surface.h>
#include <DataTypes/WarpGrid.h>
#include <QInputDialog>
#include <osg/ClipPlane>
#include "DataTypes/Volume.h"

class IRenderable;

class RenderableTreeItem : public QTreeWidgetItem
{
private:
    IRenderable* m_itemRenderable;
    osg::ref_ptr<osg::Node> m_itemPtr;

public:
    RenderableTreeItem() : 
		QTreeWidgetItem(QTreeWidgetItem::UserType),
		m_itemRenderable( NULL )
		//, m_itemPtr(NULL)
    { setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled|Qt::ItemIsEditable);}
	
    RenderableTreeItem(QTreeWidget * parent)    :  
		QTreeWidgetItem(parent, QTreeWidgetItem::UserType),
		m_itemRenderable( NULL )
		//, m_itemPtr(NULL)
    { setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled|Qt::ItemIsEditable);}
	
    RenderableTreeItem(QTreeWidgetItem * parent)	:  
		QTreeWidgetItem(parent, QTreeWidgetItem::UserType),
		m_itemRenderable( NULL )
		//, m_itemPtr(NULL)
    { setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled|Qt::ItemIsEditable);}

    osg::ref_ptr<osg::Node> getItemPtr()    const {return m_itemPtr;}
    IRenderable* getItemRenderable()        const {return m_itemRenderable;}
    void setItem(osg::ref_ptr<osg::Node> r, IRenderable* ir)
    {m_itemPtr = r; m_itemRenderable=ir;}
    void refreshName();
};

class VolumeRenderableTreeItem : public QTreeWidgetItem
{
private:
    Volumes* m_itemRenderable;
    osg::ref_ptr<osgVolume::Volume> m_itemPtr;

public:
    VolumeRenderableTreeItem(QTreeWidget * parent) : 
		QTreeWidgetItem(parent, QTreeWidgetItem::UserType),
		m_itemRenderable( NULL )
    {setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);}

    osg::ref_ptr<osgVolume::Volume> getItemPtr()    const {return m_itemPtr;}
    Volumes* getVolumeRenderable()        const   {return m_itemRenderable;}
    void setItem(osg::ref_ptr<osgVolume::Volume> r, Volumes* vol)
    {m_itemPtr = r; m_itemRenderable=vol;}
    //void refreshName();
};

class RenderableTree : public QTreeWidget
{
Q_OBJECT
private:
    bool m_clipPlaneIndex[6];
protected:
    void contextMenuEvent(QContextMenuEvent* event);
    QMenu* contextMenu;
    QMenu* contextVolumeMenu;

public:
    RenderableTree(QWidget * parent = 0) : QTreeWidget(parent)
    {
        connect(this, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(onItemClick(QTreeWidgetItem*,int)));
        connect(this, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
                this, SLOT(onItemChange(QTreeWidgetItem*,QTreeWidgetItem*)));
        connect(this, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(checkName(QTreeWidgetItem*, int)));
        contextMenu = new QMenu();
        contextVolumeMenu = new QMenu();
        createContextMenu();
        createContextVolumeMenu();
        m_clipPlaneIndex[0]=false;
        m_clipPlaneIndex[1]=false;
        m_clipPlaneIndex[2]=false;
        m_clipPlaneIndex[3]=false;
        m_clipPlaneIndex[4]=false;
        m_clipPlaneIndex[5]=false;
    }
    ~RenderableTree()
    {
        delete contextMenu;
    }

    RenderableTreeItem* getItem(const osg::Node* n);
    VolumeRenderableTreeItem* getVolumeItem(const osgVolume::Volume* v);
    QMenu* getContextMenu() {return contextMenu;}
    QMenu* getContextVolumeMenu() {return contextVolumeMenu;}
    void createContextMenu();
    void createContextVolumeMenu();

public slots:
    void onHideAllAction()
    {
        RenderableTreeItem* selected = ((RenderableTreeItem*)selectedItems().front());
        selected->setSelected(false);
        for(int i=0; i<selected->childCount(); ++i)
        {
            selected->child(i)->setSelected(true);
            onHideAction();
            selected->child(i)->setCheckState(0, Qt::Unchecked);
            selected->child(i)->setSelected(false);
        }
    }

    void onShowAllAction()
    {
        RenderableTreeItem* selected = ((RenderableTreeItem*)selectedItems().front());
        selected->setSelected(false);
        for(int i=0; i<selected->childCount(); ++i)
        {
            selected->child(i)->setSelected(true);
            onShowAction();
            selected->child(i)->setCheckState(0, Qt::Checked);
            selected->child(i)->setSelected(false);
        }
    }

    void onResetAction()
    {
        RenderableTreeItem* selected = ((RenderableTreeItem*)selectedItems().front());
        emit renderableReset(selected->getItemRenderable());
    }

    void onShowAction()
    {
        RenderableTreeItem* selected = ((RenderableTreeItem*)selectedItems().front());
        emit renderableToggled(true, selected->getItemPtr());
        selected->setCheckState(0, Qt::Checked);
    }
    void onHideAction()
    {
        RenderableTreeItem* selected = ((RenderableTreeItem*)selectedItems().front());
        emit renderableToggled(false, selected->getItemPtr());
        selected->setCheckState(0, Qt::Unchecked);
        selected->setExpanded(false);
    }
    void onMaterialAction();
    void onBackMaterialAction();
    void onInvertFacesAction(bool);
    void onTransAction()
    {
        RenderableTreeItem* selected = ((RenderableTreeItem*)selectedItems().front());
        if(selected->checkState(0) != Qt::Unchecked)
            emit transManipClicked(selected->getItemPtr());
    }

    void onRotateAction()
    {
        RenderableTreeItem* selected = ((RenderableTreeItem*)selectedItems().front());
        if(selected->checkState(0) != Qt::Unchecked)
            emit rotateManipClicked(selected->getItemPtr());
    }

    void onScaleAction()
    {
        RenderableTreeItem* selected = ((RenderableTreeItem*)selectedItems().front());
        if(selected->checkState(0) != Qt::Unchecked)
            emit scaleManipClicked(selected->getItemPtr());
    }

    void onItemClick(QTreeWidgetItem* item, int column)
    {
        RenderableTreeItem* selected = ((RenderableTreeItem*)item);
        emit renderableToggled(selected->checkState(0)!=Qt::Unchecked, selected->getItemPtr());
    }

    void onItemChange(QTreeWidgetItem* current, QTreeWidgetItem* previous)
    {
        emit renderableChanged(((RenderableTreeItem*)current),((RenderableTreeItem*)previous));
    }

    void toggleTexture();
    void changeLineThickness();
    void slotCameraFocus();
    void clipGrid(bool);
    void flipClipGrid();
    void deleteClipGrid();
    void addBestFitClipPlane();
    void checkName(QTreeWidgetItem*, int);
    void updateName();
    void changeAlphaValue();

signals:
    void renderableReset(IRenderable*);
    void renderableToggled(bool,osg::ref_ptr<osg::Node>);
    void renderableChanged(RenderableTreeItem*,RenderableTreeItem*);
    void transManipClicked(osg::ref_ptr<osg::Node>);
    void rotateManipClicked(osg::ref_ptr<osg::Node>);
    void scaleManipClicked(osg::ref_ptr<osg::Node>);
    void signalFocusCamera(osg::Vec3,float,osg::Matrixd);
    void addClipPlane(osg::ClipPlane*,bool);
    void addMyClipPlane(MyClipPlane*);
    void removeClipPlane(MyClipPlane*);
    void removeManipulator(RenderableTreeItem*);
};

#endif

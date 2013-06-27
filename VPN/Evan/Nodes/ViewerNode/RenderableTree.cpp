#include "RenderableTree.h"
#include "../../DataTypes/IRenderable.h"
#include <QContextMenuEvent>
#include <osg/Material>
#include <osgDB/WriteFile>
#include <QColorDialog>

#include <osg/LightModel>

#include "../../Utilities/Logger.h"

extern ofstream dbgfile; //RP 3/1/09 Toolbox debug file

///////RenderableTreeItem//////////////////////////////////////////////////////////////////////////
void RenderableTreeItem::refreshName()
{
    for (int i=0;i<childCount();++i)
    {
        RenderableTreeItem* childItem = dynamic_cast<RenderableTreeItem*>(child(i));
        if (childItem)
        {
            if (childItem->getItemRenderable()->getRenderableName() != childItem->text(0))
                childItem->setText(0,childItem->getItemRenderable()->getRenderableName());
            childItem->refreshName();
        }
    }
}


///////RenderableTree//////////////////////////////////////////////////////////////////////////
void RenderableTree::contextMenuEvent(QContextMenuEvent* event)
{
    RenderableTreeItem* item = dynamic_cast<RenderableTreeItem*>(itemAt(event->pos()));
    if(item)
    {
        clearSelection();
        item->setSelected(true);

        QAction* showAll = getContextMenu()->findChild<QAction *>("showAll");
        QAction* hideAll = getContextMenu()->findChild<QAction *>("hideAll");
        QAction* tex = getContextMenu()->findChild<QAction *>("texture");
        QAction* thick = getContextMenu()->findChild<QAction *>("thickness");
        QAction* focus = getContextMenu()->findChild<QAction *>("camera");
        QAction* clip = getContextMenu()->findChild<QAction *>("clip");
        QAction* flip = getContextMenu()->findChild<QAction *>("flip");
        QAction* bestFit = getContextMenu()->findChild<QAction *>("bestFit");
        QAction* deleteAction = getContextMenu()->findChild<QAction *>("delete");
        QAction* backMaterial = getContextMenu()->findChild<QAction *>("BMaterial");
        QAction* invertFaces = getContextMenu()->findChild<QAction *>("InvFaces");

        if(item->childCount())
        {
            showAll->setVisible(true);
            hideAll->setVisible(true);
        }
        else
        {
            showAll->setVisible(false);
            hideAll->setVisible(false);
        }
        thick->setVisible(false);
        tex->setVisible(false);
        focus->setVisible(false);
        backMaterial->setVisible(false);
        invertFaces->setVisible(false);
        clip->setVisible(false);
        flip->setVisible(false);
        bestFit->setVisible(false);
        deleteAction->setVisible(false);
        AbstractWarpGrid* warpg = dynamic_cast<AbstractWarpGrid *>(item->getItemRenderable());
        Surface* surf = dynamic_cast<Surface *>(item->getItemRenderable());
        if (warpg)
        {
            focus->setVisible(true);
            if (warpg->asWarpGrid())
            {
                thick->setVisible(true);
                bestFit->setVisible(true);
            }
            if (warpg->asClipPlane())
            {
                clip->setVisible(true);
                clip->setChecked(warpg->asClipPlane()->getClipPlaneIndex()>=0);
                flip->setVisible(warpg->asClipPlane()->getClipPlaneIndex()>=0);
                deleteAction->setVisible(true);
            }
        }
        else if (surf && !surf->hasTexture())
            backMaterial->setVisible(true);
        else if (surf && surf->hasTexture())
        {
            tex->setVisible(true);
            if (surf->isTextured())
            {
                tex->setChecked(true);
            }
            else
            {
               tex->setChecked(false);
            }
            backMaterial->setVisible(true);
        }

        if(surf)
        {
            invertFaces->setVisible(true);
        }
        getContextMenu()->exec( event->globalPos() );
        getContextMenu()->hide();
    }
    else
    {
        VolumeRenderableTreeItem* volumeItem = dynamic_cast<VolumeRenderableTreeItem*>(itemAt(event->pos()));
        if (volumeItem)
        {
            clearSelection();
            volumeItem->setSelected(true);
            getContextVolumeMenu()->exec( event->globalPos() );
            getContextVolumeMenu()->hide();
        }
    }
}

RenderableTreeItem* isOrChildOf(RenderableTreeItem* treeItem, const osg::Node* n)
{
    if(treeItem->getItemPtr().get() == n)
        return treeItem;
    RenderableTreeItem* result = NULL;
    for(int i=0; i<treeItem->childCount(); ++i)
    {
        result = isOrChildOf((RenderableTreeItem*)treeItem->child(i), n);
        if(result)
            break;
    }
    return result;
}

void changeChildrenColor(RenderableTreeItem* item, QColor c)
{
    for(int i=0; i<item->childCount(); ++i)
    {
        RenderableTreeItem* childItem = (RenderableTreeItem*)item->child(i);
//        if(!childItem->getItemPtr()->getStateSet())
        {
            childItem->getItemRenderable()->setFrontMaterial(c, true);
            childItem->setData(0, Qt::DecorationRole, c);
            changeChildrenColor(childItem, c);
        }
    }
}

void RenderableTree::onMaterialAction()
{
    RenderableTreeItem* treeItem = (RenderableTreeItem*)(selectedItems().front());
    osg::ref_ptr<osg::Node> node = treeItem->getItemPtr();
    osg::ref_ptr<osg::StateSet> nodess = node->getStateSet();
    QColor result;
    if (nodess)
    {
        osg::ref_ptr<osg::Material> material = dynamic_cast<osg::Material*>
                                                (nodess->getAttribute(osg::StateAttribute::MATERIAL));

        if(material.valid())
        {
            osg::Vec4 diffuse = material->getDiffuse(osg::Material::FRONT);
            result.setRgbF(diffuse.x(),diffuse.y(),diffuse.z(),diffuse.w());
        }
    }

    bool ok = false;
    result.setRgba(QColorDialog::getRgba(result.rgba(),&ok));
    if(ok)
    {
        treeItem->getItemRenderable()->setFrontMaterial(result, true);
        treeItem->setData(0, Qt::DecorationRole, result);
        changeChildrenColor(treeItem, result);
    }
}

void RenderableTree::onBackMaterialAction()
{
    RenderableTreeItem* treeItem = (RenderableTreeItem*)(selectedItems().front());
    osg::ref_ptr<osg::Node> node = treeItem->getItemPtr();
    osg::ref_ptr<osg::StateSet> nodess = node->getStateSet();
    QColor result;
    if (nodess)
    {
        osg::ref_ptr<osg::Material> material = dynamic_cast<osg::Material*>
                                                (nodess->getAttribute(osg::StateAttribute::MATERIAL));

        if(material.valid())
        {
            osg::Vec4 diffuse = material->getDiffuse(osg::Material::BACK);
            result.setRgbF(diffuse.x(),diffuse.y(),diffuse.z(),diffuse.w());
        }
    }

    result = QColorDialog::getColor(result.rgb());
    if(result.isValid())
        treeItem->getItemRenderable()->setBackMaterial(result, true);
}

void RenderableTree::onInvertFacesAction(bool on)
{
    RenderableTreeItem* treeItem = (RenderableTreeItem*)(selectedItems().front());
    osg::ref_ptr<osg::Node> node = treeItem->getItemPtr();

    if(on)
        node->getOrCreateStateSet()->setMode(GL_CULL_FACE, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
    else
        node->getOrCreateStateSet()->setMode(GL_CULL_FACE, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
}

void RenderableTree::createContextMenu()
{
    QAction *showAll = new QAction("Show All", contextMenu);
    showAll->setObjectName(QString::fromUtf8("showAll"));
    contextMenu->addAction(showAll);
    connect(showAll, SIGNAL(triggered()), this, SLOT(onShowAllAction()));
    QAction *hideAll = new QAction("Hide All", contextMenu);
    hideAll->setObjectName(QString::fromUtf8("hideAll"));
    contextMenu->addAction(hideAll);
    connect(hideAll, SIGNAL(triggered()), this, SLOT(onHideAllAction()));
    contextMenu->addSeparator();

    QAction *resetAction = new QAction("Reset", contextMenu);
    contextMenu->addAction(resetAction);
    connect(resetAction, SIGNAL(triggered()), this, SLOT(onResetAction()));

    QAction *showAction = new QAction("Show", contextMenu);
    contextMenu->addAction(showAction);
    connect(showAction, SIGNAL(triggered()), this, SLOT(onShowAction()));

    QAction *hideAction = new QAction("Hide", contextMenu);
    contextMenu->addAction(hideAction);
    connect(hideAction, SIGNAL(triggered()), this, SLOT(onHideAction()));

    QAction *materialAction = new QAction("Front Material", contextMenu);
    materialAction->setObjectName(QString::fromUtf8("FMaterial"));
    contextMenu->addAction(materialAction);
    connect(materialAction, SIGNAL(triggered()), this, SLOT(onMaterialAction()));

    QAction *backMaterialAction = new QAction("Back Material", contextMenu);
    backMaterialAction->setObjectName(QString::fromUtf8("BMaterial"));
    contextMenu->addAction(backMaterialAction);
    connect(backMaterialAction, SIGNAL(triggered()), this, SLOT(onBackMaterialAction()));

    QAction *invertFacesAction = new QAction("Invert faces ", contextMenu);
    invertFacesAction->setObjectName(QString::fromUtf8("InvFaces"));
    invertFacesAction->setCheckable(true);
    invertFacesAction->setChecked(false);
    contextMenu->addAction(invertFacesAction);
    connect(invertFacesAction, SIGNAL(toggled(bool)), this, SLOT(onInvertFacesAction(bool)));

    QAction *transAction = new QAction("Translate", contextMenu);
    transAction->setObjectName(QString::fromUtf8("Translate"));
    transAction->setCheckable(true);
    contextMenu->addAction(transAction);
    connect(transAction, SIGNAL(triggered()), this, SLOT(onTransAction()));

    QAction *rotateAction = new QAction("Rotate", contextMenu);
    rotateAction->setObjectName(QString::fromUtf8("Rotate"));
    contextMenu->addAction(rotateAction);
    rotateAction->setCheckable(true);
    connect(rotateAction, SIGNAL(triggered()), this, SLOT(onRotateAction()));

    QAction *scaleAction = new QAction("Scale", contextMenu);
    scaleAction->setObjectName(QString::fromUtf8("Scale"));
    contextMenu->addAction(scaleAction);
    scaleAction->setCheckable(true);
    connect(scaleAction, SIGNAL(triggered()), this, SLOT(onScaleAction()));

    contextMenu->addSeparator();

    QAction* toggleTexture = new QAction("Show Texture", contextMenu);
    toggleTexture->setObjectName(QString::fromUtf8("texture"));
    contextMenu->addAction(toggleTexture);
    toggleTexture->setCheckable(true);
    toggleTexture->setChecked(true);
    connect(toggleTexture, SIGNAL(triggered()), this, SLOT(toggleTexture()));

    QAction* lineThickness = new QAction("Change line thickness", contextMenu);
    lineThickness->setObjectName(QString::fromUtf8("thickness"));
    contextMenu->addAction(lineThickness);
    connect(lineThickness, SIGNAL(triggered()), this, SLOT(changeLineThickness()));

    QAction* focus = new QAction("Focus Camera", contextMenu);
    focus->setObjectName(QString::fromUtf8("camera"));
    contextMenu->addAction(focus);
    connect(focus, SIGNAL(triggered()), this, SLOT(slotCameraFocus()));

    QAction* bestFitPlane = new QAction("Make the best fit clipping plane", contextMenu);
    bestFitPlane->setObjectName(QString::fromUtf8("bestFit"));
    contextMenu->addAction(bestFitPlane);
    connect(bestFitPlane, SIGNAL(triggered()), this, SLOT(addBestFitClipPlane()));

    QAction* clip = new QAction("Clip", contextMenu);
    clip->setObjectName(QString::fromUtf8("clip"));
    clip->setCheckable(true);
    contextMenu->addAction(clip);
    connect(clip, SIGNAL(triggered(bool)), this, SLOT(clipGrid(bool)));

    QAction* flip = new QAction("Flip clipping plane", contextMenu);
    flip->setObjectName(QString::fromUtf8("flip"));
    contextMenu->addAction(flip);
    connect(flip, SIGNAL(triggered()), this, SLOT(flipClipGrid()));

    contextMenu->addSeparator();

    QAction* deleteClip = new QAction("Delete", contextMenu);
    deleteClip->setObjectName(QString::fromUtf8("delete"));
    contextMenu->addAction(deleteClip);
    connect(deleteClip, SIGNAL(triggered()), this, SLOT(deleteClipGrid()));

    contextMenu->hide();
}

void RenderableTree::createContextVolumeMenu()
{
    QAction *alphaAction = new QAction("Change Alpha Value", contextVolumeMenu);
    contextVolumeMenu->addAction(alphaAction);
    connect(alphaAction, SIGNAL(triggered()), this, SLOT(changeAlphaValue()));

    contextVolumeMenu->hide();
}

void RenderableTree::toggleTexture()
{
    QAction* tex = getContextMenu()->findChild<QAction *>("texture");
    if (tex)
    {
        RenderableTreeItem* selected = ((RenderableTreeItem*)selectedItems().front());
        if(selected->checkState(0) != Qt::Unchecked){
            tex->setChecked((tex->isChecked()));
            Surface* surf = dynamic_cast<Surface *>(selected->getItemRenderable());
            if (surf){
                if (tex->isChecked())
                {
                    surf->assignTexture();
                }
                else
                {
                    surf->removeTexture();
                }
            }
        }
    }
}

void RenderableTree::changeLineThickness()
{
    RenderableTreeItem* selected = ((RenderableTreeItem*)selectedItems().front());
    if(selected->checkState(0) != Qt::Unchecked){
        WarpGrid* warpg = dynamic_cast<WarpGrid *>(selected->getItemRenderable());
        if (warpg){
            int width = QInputDialog::getInteger(this, QString("Line thickness"), QString("Modify line thickness"), warpg->getLineWidth(), 1, 100);
            warpg->setLineWidth(width);
        }
    }
}

void RenderableTree::slotCameraFocus()
{
    RenderableTreeItem* selected = ((RenderableTreeItem*)selectedItems().front());
    if(selected->checkState(0) != Qt::Unchecked){
        AbstractWarpGrid* warpg = dynamic_cast<AbstractWarpGrid *>(selected->getItemRenderable());
        if (warpg){
            osg::Vec3 center;
            osg::Matrixd mat = warpg->getOsgTransform()->getMatrix();
            center.x()=mat(3,0);
            center.y()=mat(3,1);
            center.z()=mat(3,2);
            emit signalFocusCamera(center,warpg->getOsgGeometry()->getBound().radius(), mat);
        }
    }
}

RenderableTreeItem* RenderableTree::getItem(const osg::Node* n)
{
    RenderableTreeItem* result = NULL;
    for(int i=0; i<topLevelItemCount(); ++i)
    {
        RenderableTreeItem* treeItem = dynamic_cast<RenderableTreeItem*>(topLevelItem(i));
        if (!treeItem)
            continue;
        result = isOrChildOf(treeItem, n);
        if(result)
            break;
    }
    return result;
}

VolumeRenderableTreeItem* RenderableTree::getVolumeItem(const osgVolume::Volume* n)
{
    for(int i=0; i<topLevelItemCount(); ++i)
    {
        VolumeRenderableTreeItem* treeItem = dynamic_cast<VolumeRenderableTreeItem*>(topLevelItem(i));
        if (!treeItem)
            continue;
        if(treeItem->getItemPtr().get() == n)
            return treeItem;
    }
    return NULL;
}

void RenderableTree::clipGrid(bool show)
{
    RenderableTreeItem* selected = ((RenderableTreeItem*)selectedItems().front());
    if(selected->checkState(0) != Qt::Unchecked){
        MyClipPlane* clipp = dynamic_cast<MyClipPlane *>(selected->getItemRenderable());
        if (clipp){ //the item selected is a MyClipPlane
            if (show)
            {
                osg::Plane* plane = clipp->getPlane();
                if (!plane)
                {
                    plane = new osg::Plane; //plane corresponding to the orientation of grid before transform
                    clipp->setPlane(plane);
                }
                plane->set(0.0,0.0,1.0,0.0);
                plane->transform(clipp->getOsgTransform()->getMatrix()); // transform the plane to have the same orientation as the grid
                if (clipp->isFlipped())
                    plane->flip();

                int free=-1;
                for (int i=0;i<6;i++)
                    if (!m_clipPlaneIndex[i])
                        free=i;
                if (free>=0)// There are less than 6 active clipping planes in the scene
                {
                    osg::ClipPlane* clip = new osg::ClipPlane(free,*plane);
                    clipp->setClipPlaneIndex(free);
                    m_clipPlaneIndex[free]=true;
                    clipp->setClipPlane(clip);
                    emit addClipPlane(clip,true);
                }
                else return;
            }
            else
            {
                if (clipp->getClipPlaneIndex()>=0 && clipp->getClipPlane())
                {
                    m_clipPlaneIndex[clipp->getClipPlaneIndex()]=false;
                    clipp->setClipPlaneIndex(-1);
                    emit addClipPlane(clipp->getClipPlane(),false);
                }
            }
        }
    }
}

void RenderableTree::flipClipGrid()
{
    RenderableTreeItem* selected = ((RenderableTreeItem*)selectedItems().front());
    MyClipPlane* clipp = dynamic_cast<MyClipPlane *>(selected->getItemRenderable());
    if (clipp){ //the item selected is a MyClipPlane
        if (clipp->getClipPlaneIndex()>=0){
            clipp->flip();
        }
    }
}

void RenderableTree::deleteClipGrid()
{
    RenderableTreeItem* selected = ((RenderableTreeItem*)selectedItems().front());
    MyClipPlane* clipp = dynamic_cast<MyClipPlane *>(selected->getItemRenderable());
    if (clipp)
    {
        if (clipp->getClipPlaneIndex()>=0){
            m_clipPlaneIndex[clipp->getClipPlaneIndex()]=false;
            clipp->setClipPlaneIndex(-1);
            emit addClipPlane(clipp->getClipPlane(),false);
        }
        clipp->toBeDeleted();
        emit removeManipulator(selected);
        delete clipp;
        delete selected;
    }
}

void RenderableTree::addBestFitClipPlane()
{
    RenderableTreeItem* selected = ((RenderableTreeItem*)selectedItems().front());
    if(selected->checkState(0) != Qt::Unchecked){
        WarpGrid* warpg = dynamic_cast<WarpGrid *>(selected->getItemRenderable());
        if (warpg){
            MyClipPlane* newClip = warpg->makeBestFitPlane();
            if (newClip)
            {
                emit addMyClipPlane(newClip);
            }
        }
    }
}

void RenderableTree::changeAlphaValue()
{
    VolumeRenderableTreeItem* selected = ((VolumeRenderableTreeItem*)selectedItems().front());
    if(selected->checkState(0) != Qt::Unchecked){
        Volumes* vol = dynamic_cast<Volumes *>(selected->getVolumeRenderable());
        if (vol)
        {
            bool ok;
            double val = QInputDialog::getDouble(this, "Alpha Value", "Enter alpha value",  vol->getAlphaValue(), 0 , 1,4, &ok);
            if (ok)
                vol->setAlphaValue(val);
        }
    }
}

void RenderableTree::checkName(QTreeWidgetItem* treeItem, int row)
{
    if (parentWidget()->isVisible())
    {
        RenderableTreeItem* renderable = dynamic_cast<RenderableTreeItem*>(treeItem);
        if (renderable && !renderable->text(row).isEmpty() && renderable->getItemPtr().valid())
        {
            if (renderable->getItemPtr()->getName() != renderable->text(row).toStdString())
                renderable->getItemPtr()->setName(renderable->text(row).toStdString());
        }
    }
}

void RenderableTree::updateName()
{
    for (int i=0;i<topLevelItemCount();++i)
    {
        RenderableTreeItem* item = dynamic_cast<RenderableTreeItem*>(topLevelItem(i));
        if (item)
            item->refreshName();
    }
}

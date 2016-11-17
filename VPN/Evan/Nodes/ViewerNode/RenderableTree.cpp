#include "RenderableTree.h"
#include "../../DataTypes/IRenderable.h"
#include <QContextMenuEvent>
#include <osg/Material>
#include <osgDB/WriteFile>
#include <QColorDialog>

#include <osg/LightModel>

#include "../../Utilities/Logger.h"

#include <typeinfo> //husky

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

        //! QAction* focus = getContextMenu()->findChild<QAction *>("camera");
        QAction* focus = getContextMenu()->findChild<QAction *>("cameraObjectFocus");

        QAction* focusObject = getContextMenu()->findChild<QAction *>("cameraObjectFocus");
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
        focusObject->setVisible(true);
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

/**
    QAction* focus = new QAction("Focus Camera", contextMenu);
    focus->setObjectName(QString::fromUtf8("camera"));
    contextMenu->addAction(focus);
    connect(focus, SIGNAL(triggered()), this, SLOT(slotTreeFocus()));
**/

    QAction* focusObject = new QAction("Focus Camera on Object", contextMenu);
    focusObject->setObjectName(QString::fromUtf8("cameraObjectFocus"));
    contextMenu->addAction(focusObject);
    connect(focusObject, SIGNAL(triggered()), this, SLOT(slotCameraObjectFocus()));

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



static int enumTreeItemsCheckedTest( QTreeWidgetItem *item, int cnt )
{

// this snippets counts all items in tree whether checked or not
//    for( int i = 0; i < item->childCount(); ++i ) {
//        cnt = enumTreeItems( item->child( i ), cnt+1 );
//    }
//    return cnt;


    // count only checked items!
    for( int i = 0; i < item->childCount(); ++i ) {
            if ( item->child( i )->checkState( 0 ) != Qt::Unchecked ) {
                cnt = enumTreeItemsCheckedTest( item->child( i ), cnt+1 );
            } else { // can nodes not be selected while their children potentially are selected?
                cnt = enumTreeItemsCheckedTest( item->child( i ), cnt );
            }
    }
    return cnt;

}


static int enumTreeItems( QTreeWidgetItem *item, int cnt, QList< QTreeWidgetItem* > *qList = NULL )
{

    // append to list even if not checked
    if ( qList != NULL ) {
        qList->push_back( item );
    }

    for( int i = 0; i < item->childCount(); ++i ) {
        cnt = enumTreeItems( item->child( i ), cnt+1, qList );
    }

    return cnt;
}

// count checked items only which are under a checked top-level item
// i.e. checked sub-level items count iff their respecitve top-level
// item is checked, too
static int enumCheckedTreeItems( QTreeWidgetItem *item, int cnt, QList< QTreeWidgetItem* > *qList = NULL )
{

    // append to list even if not checked
    if ( qList != NULL ) {
        qList->push_back( item );
    }


    for( int i = 0; i < item->childCount(); ++i ) {
//        if ( item->child( i )->parent() == NULL ) { // top-level item
//            Logger::getInstance()->log( "enumTreeItems found a top-level item!" );
//            if ( item->child( i )->checkState( 0 ) == Qt::Unchecked ) {
//                Logger::getInstance()->log( "skip all items below this inactive top-level item!" );
//                continue;
//            }
//        }

        if ( item->child( i )->childCount() != 0 ) { // not a leaf item
            if ( item->child( i )->checkState( 0 ) == Qt::Unchecked ) {
                Logger::getInstance()->log( "skip all items below this inactive non-leaf item!" );
                continue;
            }
        }

// count any selected nodes which have all their parents checked up to the root
//        if ( item->child( i )->checkState( 0 ) != Qt::Unchecked ) {
//            cnt = enumTreeItems( item->child( i ), cnt+1, qList );
//        } else { // can nodes not be selected while their children potentially are selected?
//            cnt = enumTreeItems( item->child( i ), cnt, qList );
//        }

// NOTE: non-leaf nodes (e.g. 'Ungrouped' in the GPA sample) may also contain renderable data!
// i.e. adjust the snippet below
// count selected leaf nodes which have all their parents checked up to the root
        if ( item->child( i )->checkState( 0 ) != Qt::Unchecked ) {
            if ( item->child( i )->childCount() != 0 ) { // 'item' is not a leaf item
                cnt = enumTreeItems( item->child( i ), cnt, qList );
            } else { // a leaf node
                cnt = enumTreeItems( item->child( i ), cnt+1, qList );
                RenderableTreeItem *ti = dynamic_cast< RenderableTreeItem* >(item);
                if ( ti != NULL ) {
                   // Logger::getInstance()->log( "could retrieve renderable tree item" );
                } else { // will at least be a QTreeWidgetItem
                    Logger::getInstance()->log( "could not retrieve renderable tree item", Logger::WARNING );
                    Logger::getInstance()->log( QString( "   actually it is of type: " ) +
                                               typeid( *item ).name(), Logger::WARNING );
                    //ti->m_itemPtr
                }
            }
        } else { // can nodes not be selected while their children potentially are selected?
            cnt = enumTreeItems( item->child( i ), cnt, qList );
        }

    }
    return cnt;

}


static void determineBoundingSphere(    const QList< QTreeWidgetItem* > &items,
                                        osg::BoundingSphere &bound )
{
    osg::Group *groupRootAABB = new osg::Group();

    QList< QTreeWidgetItem* >::const_iterator endIt = items.end();
    for ( QList< QTreeWidgetItem* >::const_iterator it = items.begin(); it != endIt; ++it ) {

        RenderableTreeItem *rTempItem = dynamic_cast< RenderableTreeItem* >( *it );
        VolumeRenderableTreeItem *vTempItem = dynamic_cast< VolumeRenderableTreeItem* >( *it );

        if ( rTempItem != NULL ) {
            groupRootAABB->addChild( rTempItem->getItemRenderable()->getOsgNode() );
        } else if ( vTempItem != NULL ) {
            groupRootAABB->addChild( vTempItem->getVolumeRenderable()->getOsgNode() );
        } else {
            continue;
        }

    }

    bound = groupRootAABB->getBound();

    //delete groupRootAABB;
    // delete does not work => ref counted
    for ( QList< QTreeWidgetItem* >::const_iterator it = items.begin(); it != endIt; ++it ) {
        RenderableTreeItem *rTempItem = dynamic_cast< RenderableTreeItem* >( *it );
        VolumeRenderableTreeItem *vTempItem = dynamic_cast< VolumeRenderableTreeItem* >( *it );

        if ( rTempItem != NULL ) {
            groupRootAABB->removeChild( rTempItem->getItemRenderable()->getOsgNode() );
        } else if ( vTempItem != NULL ) {
            groupRootAABB->removeChild( vTempItem->getVolumeRenderable()->getOsgNode() );
        } else {
            continue;
        }
    }

    groupRootAABB->unref();
}

static void determineAABB(  const QList< QTreeWidgetItem* > &items,
                            osg::Vec3 &minAABB,
                            osg::Vec3 &maxAABB )
{
    minAABB = osg::Vec3( +FLT_MAX, +FLT_MAX, +FLT_MAX );
    maxAABB = osg::Vec3( -FLT_MAX, -FLT_MAX, -FLT_MAX );

    // manually determine AABB

    QList< QTreeWidgetItem* >::const_iterator endIt = items.end();
    for ( QList< QTreeWidgetItem* >::const_iterator it = items.begin(); it != endIt; ++it ) {
    //for ( it = m_currentScene.begin() ; it != itEnd; ++it ) {

        RenderableTreeItem *rTempItem = dynamic_cast< RenderableTreeItem* >( *it );
        VolumeRenderableTreeItem *vTempItem = dynamic_cast< VolumeRenderableTreeItem* >( *it );

        osg::BoundingSphere bound;

        if ( rTempItem != NULL ) {
                bound = rTempItem->getItemRenderable()->getOsgGeometry()->getBound();
        } else if ( vTempItem != NULL ) {
            bound = vTempItem->getVolumeRenderable()->getOsgGeometry()->getBound();
        } else {
            continue;
        }

        float r = bound.radius();
        osg::Vec3 c = bound.center();

        osg::Vec3 rVec = osg::Vec3( r, r, r );
        osg::Vec3 minC = c - rVec;
        osg::Vec3 maxC = c + rVec;

        if ( minC.x() < minAABB.x() ) {
            minAABB.x() = minC.x();
        }
        if ( minC.y() < minAABB.y() ) {
            minAABB.y() = minC.y();
        }
        if ( minC.z() < minAABB.z() ) {
            minAABB.z() = minC.z();
        }

        if ( maxC.x() > maxAABB.x() ) {
            maxAABB.x() = maxC.x();
        }
        if ( maxC.y() > maxAABB.y() ) {
            maxAABB.y() = maxC.y();
        }
        if ( maxC.z() > maxAABB.z() ) {
            maxAABB.z() = maxC.z();
        }
    }

//    Logger::getInstance()->log(     QString( "[RenderableTree] determineAABB: " ) +
//                                    QString( "( " ) +   QString::number( minAABB.x() ) + QString( ", " ) +
//                                                        QString::number( minAABB.y() ) + QString( ", " ) +
//                                                        QString::number( minAABB.z() ) + QString( " ), " ) +
//                                    QString( "( " ) +   QString::number( maxAABB.x() ) + QString( ", " ) +
//                                                        QString::number( maxAABB.y() ) + QString( ", " ) +
//                                                        QString::number( maxAABB.z() ) + QString( " )" )  );

}



void RenderableTree::slotCameraObjectFocus()
{
        QTreeWidgetItem *qItem = this->currentItem();
        if ( qItem == NULL ) {
            return;
        }

        RenderableTreeItem *rItem = dynamic_cast< RenderableTreeItem* >( qItem );
        VolumeRenderableTreeItem *vItem = dynamic_cast< VolumeRenderableTreeItem* >( qItem );
        if ( ( rItem == NULL ) && ( vItem == NULL ) ) {
                return;
        }

//        Logger::getInstance()->log( " ----- [RenderableTree] slotCameraObjectFocus START ---- " );

        QList< QTreeWidgetItem* > qItemList;
        int subTreeItemCnt = enumTreeItems( qItem, 1, &qItemList);

        //osg::Vec3 minAABB, maxAABB;
        //determineAABB( qItemList, minAABB, maxAABB );
        osg::BoundingSphere itemBound;
        determineBoundingSphere( qItemList, itemBound );

        QList< QTreeWidgetItem* >::iterator endIt = qItemList.end();
        for ( QList< QTreeWidgetItem* >::iterator it = qItemList.begin(); it != endIt; ++it ) {

            RenderableTreeItem *rTempItem = dynamic_cast< RenderableTreeItem* >( *it );
            VolumeRenderableTreeItem *vTempItem = dynamic_cast< VolumeRenderableTreeItem* >( *it );

            osg::BoundingSphere bound;

            if ( rTempItem != NULL ) {
//                    Logger::getInstance()->log( QString( "  [RenderableTree] slotCameraObjectFocus - " ) +
//                                                rTempItem->getItemRenderable()->getRenderableName() );

                    bound = rTempItem->getItemRenderable()->getOsgGeometry()->getBound();
            } else if ( vTempItem != NULL ) {
//                    Logger::getInstance()->log( QString( "  [RenderableTree] slotCameraObjectFocus - " ) +
//                                                vTempItem->getVolumeRenderable()->getRenderableName() );

                    bound = vTempItem->getVolumeRenderable()->getOsgGeometry()->getBound();
            } else {
                continue;
            }

//            float r = bound.radius();
//            osg::Vec3 c = bound.center();
//
//            Logger::getInstance()->log(     QString( "[RenderableTree] slotCameraObjectFocus() center: " ) +
//                                            QString( "( " ) +   QString::number( c.x() ) + QString( ", " ) +
//                                                                QString::number( c.y() ) + QString( ", " ) +
//                                                                QString::number( c.z() ) + QString( " ), " ) +
//                                            QString( "radius = " ) + QString::number( r ) );

        }

            float r = itemBound.radius();
            osg::Vec3 c = itemBound.center();
//            Logger::getInstance()->log(     QString( "[RenderableTree] slotCameraObjectFocus() center: " ) +
//                                            QString( "( " ) +   QString::number( c.x() ) + QString( ", " ) +
//                                                                QString::number( c.y() ) + QString( ", " ) +
//                                                                QString::number( c.z() ) + QString( " ), " ) +
//                                            QString( "radius = " ) + QString::number( r ) );

            emit signalFocusObjectCamera( c, r );

//        Logger::getInstance()->log( " ----- [RenderableTree] slotCameraObjectFocus END ---- " );

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

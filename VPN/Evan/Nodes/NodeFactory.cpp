#include "RealiseNode/RealiseNode.h"
#include "MultiplyNode/MultiplyNode.h"
#include "AnnotationNode/AnnotationNode.h"
#include "Plotter3DNode/Plotter3DNode.h"
#include "TemplandNode/TemplandNode.h"
#include "GroupNode/GroupNode.h"
#include "ConcatNode/ConcatNode.h"
#include "SubtractNode/SubtractNode.h"
#include "ReflectedRelabelling/ReflectedRelabelling.h"
#include "ExportNode/ExportNode.h"
#include "PCONode/PCONode.h"
#include "ImportNode/ImportNode.h"
#include "PrintNode/PrintNode.h"

#ifdef USE_R_
#include "RNode/RNode.h"
#endif

#include "PlotterNode/PlotterNode.h"
#include "MMRNode/MMRNode.h"
#include "ViewerNode/ViewerNode.h"
#include "TransformNode/TransformNode.h"
#include "GenerateSurfaceNode/GenerateSurfaceNode.h"
#include "PCANode/PCANode.h"
#include "SelectNode/SelectNode.h"
#include "WarperNode/WarperNode.h"
#include "GPANode/GPANode.h"
#include "PLSNode/PLSNode.h"
#include "NodeFactory.h"
//#include "../Utilities/LibraryImport.h"

INode* NodeFactory::getNode(const QString& serializedNode)
{
    INode* newNode = NULL;

    QStringList params = serializedNode.split("::");
    if(params.count() != 4)
        return NULL;

    QString nodeType = params[0];

    //DO NOT EDIT OR REMOVE THIS PART
    //WIZARDMARKER
	if(nodeType == "RealiseNode")
        newNode = new RealiseNode();
	else
	if(nodeType == "MultiplyNode")
        newNode = new MultiplyNode();
	else
	if(nodeType == "AnnotationNode")
		newNode = new AnnotationNode();
	else if(nodeType == "Plotter3DNode")
		newNode = new Plotter3DNode();
	else if(nodeType == "TemplandNode")
		newNode = new TemplandNode();
	else if(nodeType == "GroupNode")
		newNode = new GroupNode();
	else if(nodeType == "ConcatNode")
		newNode = new ConcatNode();
	else if(nodeType == "SubtractNode")
		newNode = new SubtractNode();
	else if(nodeType == "ReflectedRelabelling")
		newNode = new ReflectedRelabelling();
	else if(nodeType == "ExportNode")
		newNode = new ExportNode();
	else if(nodeType == "PCONode")
		newNode = new PCONode();
	else if(nodeType == "ImportNode")
		newNode = new ImportNode();
#ifdef USE_R_
	else if(nodeType == "RNode")
        newNode = new RNode();
#endif
	else if(nodeType == "PlotterNode")
		newNode = new PlotterNode();
	else if(nodeType == "MMRNode")
		newNode = new MMRNode();
	else if(nodeType == "ViewerNode")
		newNode = new ViewerNode();
	else if(nodeType == "TransformNode")
		newNode = new TransformNode();
	else if(nodeType == "GenerateSurfaceNode")
		newNode = new GenerateSurfaceNode();
	else if(nodeType == "PCANode")
		newNode = new PCANode();
	else if(nodeType == "SelectNode")
		newNode = new SelectNode();
	else if(nodeType == "GPANode")
        newNode = new GPANode();
    else if(nodeType == "WarperNode")
        newNode = new WarperNode();
    else if(nodeType == "PLSNode")
        newNode = new PLSNode();
    else if(nodeType == "PrintNode")
        newNode = new PrintNode();
    else
        return NULL;

    newNode->setType(nodeType.toStdString());
    newNode->setName(params[1].toStdString());

    QStringList inputs = params[2].split(',');
    foreach (QString input, inputs)
        if(input!="")
        {
            QStringList inputParams = input.trimmed().split(":");
            if(inputParams.count() != 3)
                continue;
            newNode->addInput(  inputParams[1].trimmed().toStdString(),
                                inputParams[0].trimmed().toStdString(),
                                (NodePort::Optionality)(inputParams[2].toInt()));
        }

    QStringList outputs =params[3].split(',');
    foreach (QString output,outputs)
        if(output!="")
        {
            QStringList outputParams = output.trimmed().split(":");
            if(outputParams.count() != 2)
                continue;
            newNode->addOutput(outputParams[1].trimmed().toStdString(),outputParams[0].trimmed().toStdString());
        }

	return newNode;
}

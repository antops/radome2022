#include "basic_parameters.h"

BasicParameters::BasicParameters()
{
}

BasicParameters::~BasicParameters()
{
}

void BasicParameters::setData(const std::vector<double>& parameter, double scale)
{
	data.resize(parameter.size());
	for (int i = 0; i < parameter.size(); i++)
	{
		data[i] = parameter[i] * scale;
	}
}

void BasicParameters::setGraphTrans(const GraphTrans &graphTrans)
{
	this->graphTrans = graphTrans;
	updateData();
}

QTreeWidgetItem * BasicParameters::getTransformTree()
{
	QTreeWidgetItem * tree = new QTreeWidgetItem;
	tree->setText(0, "Transform");
	QTreeWidgetItem * treeTranslation = new QTreeWidgetItem;
	string tempTranslation = "Translation: ";
	tempTranslation.append(graphTrans.getTransString());
	treeTranslation->setText(0, tempTranslation.c_str());
	QTreeWidgetItem * treeRotation = new QTreeWidgetItem;
	string tempRotation = "Rotation:";
	tree->addChild(treeTranslation);
	tree->addChild(treeRotation);
	treeRotation->setText(0, tempRotation.c_str());
	for (auto buffer : graphTrans.getRotateXYZString()) {
		treeRotation = new QTreeWidgetItem;
		treeRotation->setText(0, buffer.c_str());
		tree->addChild(treeRotation);
	}
	
	return tree;
}

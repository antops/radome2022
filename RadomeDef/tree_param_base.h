#pragma once
// µ¼º½Ê÷base
#include <QTreeWidgetItem>

class TreeParamBase
{
public:
	TreeParamBase() {};
	virtual ~TreeParamBase() {
		if (tree_item_) {
			delete tree_item_;
		}
	};

	QTreeWidgetItem *  GetTreeItem() {
		return tree_item_;
	};

protected:
	QTreeWidgetItem * tree_item_ = NULL;
};
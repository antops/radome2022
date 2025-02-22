/*
*	created by liyun 2017/12/25  Merry Christmas
*   function 基类
*   version 1.0
*/

#pragma once

#include <vector>

#include <QTreeWidgetItem>

#include "../common/include/GraphTrans.h"
#include "../common/include/Vector3.h"


class BasicParameters
{
public:
	BasicParameters();
	virtual ~BasicParameters();

	virtual void updateData() = 0;

	const std::vector<double>& getData() const { return data; }
	void setData(const std::vector<double>&, double scale = 1);

	GraphTrans getGraphTrans() const { return graphTrans; }
	void setGraphTrans(const GraphTrans&);

protected:
	QTreeWidgetItem* getTransformTree();

	GraphTrans graphTrans; // 旋转平移参数
	std::vector<double> data;

};
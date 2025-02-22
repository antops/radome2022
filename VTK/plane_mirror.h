/*
*	created by liyun 2017/10/24
*   function 平面镜(默认的镜子)	  
*	width --> data[0]
*	depth --> data[1];
*   version 1.0
*/

#pragma once

#include "mirror.h"
#include <vector>

class PlaneMirror: public Mirror
{
public:
	// 构造默认的平面镜 没有传入参数
	PlaneMirror(const GraphTrans & _graphTrans);
	// 构造平面镜 传入参数
	PlaneMirror(const GraphTrans & _graphTrans, const std::vector<double> parameter, double scale = 1);
	virtual ~PlaneMirror();

	virtual void calPolyData(vtkSmartPointer<vtkPolyData>&, double ds = 0);

	virtual void updateData();

	virtual QTreeWidgetItem* getTree();

	void setParameter(GraphTrans & _graphTrans, std::vector<double> parameter);

	void setWidth(double);
	double getWidth() const { return data[0]; }

	void setDepth(double);
	double getDepth() const { return data[1]; }


private:




};
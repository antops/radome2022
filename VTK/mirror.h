/*
*	created by liyun 2017/10/23
*   function 镜子的虚基类 
*   version 1.0
*/

#pragma once
#include <vtkActor.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkProperty.h>
#include <QTreeWidgetItem>
#include <QString>
#include <vtkAxesActor.h>
#include <vector>

#include "../../common/include/GraphTrans.h"
#include "basic_parameters.h"

enum MirrorsType
{
	PLANEMIRROR = 0,
	QUADRICSURFACE,
	PARABOLICCYLINDER,
	PARABOLOID,
	ELLIPSOID,
	STLMIRROR,
	PHSCORMIRROR,
	FIRSTORDER
};

class actor;
class Mirror : public BasicParameters
{
public:
	Mirror();
	virtual ~Mirror();

	MirrorsType getMirrorsType() const;

	// cal(计算) and get actor 
	vtkSmartPointer<vtkActor> getActor() const;
	virtual void calActor();

	// cal(计算) and get polyData 
	vtkSmartPointer<vtkPolyData> getPolyData() const;
	virtual void calPolyData(vtkSmartPointer<vtkPolyData>&, double ds = 0) = 0;

	virtual void updateData() = 0;

	void setSelected(bool);

	virtual QTreeWidgetItem* getTree() { return nullptr; };


	void switchIsTransparent();
	void setIsTransparent(bool tmp);
	bool getIsTransparent() const { return isTransparent; }

	void switchIsShow();
	void setIsShow(bool);
	bool getIsShow() const { return isShow; }

	vtkSmartPointer<vtkAxesActor> getActorAxes() const;
	void calcActorAxes();

	// 输出STL 格式文件
	void saveSTL();
	void saveSTLfilename(std::string _filename);


	void genMesh(double ds);

	double getTransEffi() const { return transEffi; }

	void setTransEffi(double);

protected:

	MirrorsType type;

	//保存每个模型的显示和剖分数据
	vtkSmartPointer<vtkActor> actor;
	vtkSmartPointer<vtkPolyData> polyData;

	vtkSmartPointer<vtkAxesActor> actorAxes;


	bool isTransparent; // 是否透明
	bool isShow; // 是否显示
	bool ismeshed; //是否已被剖分


	vtkSmartPointer<vtkProperty> property;


	double transEffi;

private:

};
/*
*	created by liyun 2017/11/27
*   function 计算光线追踪 输入镜子的指针 输出反射光线和交点以及是否相交
*   version 1.0
*/

#ifndef RAYTRACING_H  
#define RAYTRACING_H

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>

#include "../../common/include/Vector3.h"

#include "../../common/include/Matrix4D.h"
#include "../../common/include/Vector3D.h"

#include <vector>
#include <map>

using namespace Common;

class RayTracing
{
public:
	RayTracing();

	~RayTracing();

	bool CalcNormalOfLineByPolyData(
		const Vector3& startPiont,
		const Vector3& direction,
		vtkSmartPointer<vtkPolyData> polyData,
		std::map<double, std::pair<bool, Vector3>>* output);
	bool CalcInterPointBatchDC(
		const std::vector<Vector3>& startPiont,
		const std::vector<Vector3>& direction,
		vtkSmartPointer<vtkPolyData> polyData,
		std::vector<std::vector<Vector3>>& nomal,
		std::vector<std::vector<Vector3>>& intersection,
		std::vector<std::vector<double>>& prot);
	bool CalcInterPointBatch(
		const std::vector<Vector3>& startPiont,
		const std::vector<Vector3>& direction,
		vtkSmartPointer<vtkPolyData> polyData,
		std::vector<std::vector<Vector3>>& nomal,
		std::vector<std::vector<Vector3>>& intersection,
		std::vector<std::vector<double>>& prot);

	// 根据入射光线和法线求反射光线
	static Vector3 reflectLight(const Vector3& a, const Vector3& n);

private:
	// 三角形与直线相交判断
	bool isIntersect(const Vector3 &orig, const Vector3 &dir,
		const Vector3 &v0, const Vector3 &v1, const Vector3 &v2,
		Vector3 &intersection, double &t);

	bool ray_CurvedSurface(const vector<double> & a, Vector3 n, Vector3 org,
		double &t, Vector3 &interPoint);

	// 直线与面相交 可能有两个解
	void line_CurvedSurface(const vector<double> & a, Vector3 n, Vector3 org,
		double &t1, double &t2, bool &isOk1, bool &isOk2, Vector3 &interPoint1,
		Vector3 &interPoint2);

	bool isCalcMatrix;

	// 世界坐标系转到模型的相对坐标系矩阵（逆矩阵）先旋转后平移
	vector<Matrix4D> R_rotatMatrix;
	vector<Matrix4D> R_translateMatrix;

	// 模型的相对坐标系转到世界坐标矩阵
	vector<Matrix4D> rotatMatrix;
	vector<Matrix4D> translateMatrix;

};



#endif // RAYTRACING_H


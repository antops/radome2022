#include "RayTracing.h"

#include "../../CUDADLL/CUDARayTracing.h"
#include <map>
#include "def.h"

RayTracing::RayTracing()
{
}

RayTracing::~RayTracing()
{
}

bool RayTracing::CalcInterPointBatch(
	const std::vector<Vector3>& startPiont,
	const std::vector<Vector3>& direction,
	vtkSmartPointer<vtkPolyData> polyData,
	std::vector<std::vector<Vector3>>& nomal,
	std::vector<std::vector<Vector3>>& intersection,
	std::vector<std::vector<double>>& prot) {

	CUDARayTracing cudaAPI;
	cudaAPI.setSTL(polyData.GetPointer());
	cudaAPI.setRays(startPiont, direction);
	if (cudaAPI.run() != 0) {
		return false;
	}

	cudaAPI.getRes(nomal, intersection, prot);

	//std::map<double, std::pair<bool, Vector3>> output;
	//ray_tracing.CalcNormalOfLineByPolyData(star_point, normal, data, &output);
	//bool is_in = false;
	//for (const auto& x : output) {
	//	if (is_in == x.second.first) {
	//		mesh_tmp[i][j].push_back(x.second.second);
	//		is_in = !is_in;
	//	}
	//	else break;
	//}
	return true;
}

bool RayTracing::CalcInterPointBatchDC(
	const std::vector<Vector3>& startPiont,
	const std::vector<Vector3>& direction,
	vtkSmartPointer<vtkPolyData> polyData,
	std::vector<std::vector<Vector3>>& nomal,
	std::vector<std::vector<Vector3>>& intersection,
	std::vector<std::vector<double>>& prot)
{
	double t = 0.0;
	int EleNum = polyData->GetNumberOfCells();
	for (int i = 0; i < EleNum; i++)  //求与反射面的交点
	{
		vtkIdList * p;
		p = polyData->GetCell(i)->GetPointIds();
		double * point;
		point = polyData->GetPoint(p->GetId(0));
		Vector3 NodesXYZ1(point[0], point[1], point[2]);
		point = polyData->GetPoint(p->GetId(1));
		Vector3 NodesXYZ2(point[0], point[1], point[2]);
		point = polyData->GetPoint(p->GetId(2));
		Vector3 NodesXYZ3(point[0], point[1], point[2]);
		for (int j = 0; j < startPiont.size(); j++) {
			Vector3 tmp_intersection;
			if (this->isIntersect(startPiont[j], direction[j], NodesXYZ1,
				NodesXYZ2, NodesXYZ3, tmp_intersection, t))
			{
				Vector3 tempa = NodesXYZ1 - NodesXYZ2;
				Vector3 tempb = NodesXYZ1 - NodesXYZ3;
				nomal[j].push_back(tempa.Cross(tempb));  //法向量
				prot[j].push_back(t);
				intersection[j].push_back(tmp_intersection);
			}
		}

	}
	return true;
}

bool RayTracing::CalcNormalOfLineByPolyData(
	const Vector3 & startPiont,
	const Vector3 & direction,
	vtkSmartPointer<vtkPolyData> polyData,
	std::map<double, std::pair<bool, Vector3>>* output)
{
	double t = 0.0;
	int EleNum = polyData->GetNumberOfCells();
	Vector3 normal;
	Vector3 intersection;
	for (int i = 0; i < EleNum; i++)  //求与反射面的交点
	{
		vtkIdList * p;
		p = polyData->GetCell(i)->GetPointIds();
		double * point;
		point = polyData->GetPoint(p->GetId(0));
		Vector3 NodesXYZ1(point[0], point[1], point[2]);
		point = polyData->GetPoint(p->GetId(1));
		Vector3 NodesXYZ2(point[0], point[1], point[2]);
		point = polyData->GetPoint(p->GetId(2));
		Vector3 NodesXYZ3(point[0], point[1], point[2]);
		if (this->isIntersect(startPiont, direction, NodesXYZ1,
			NodesXYZ2, NodesXYZ3, intersection, t))
		{
			Vector3 tempa = NodesXYZ1 - NodesXYZ2;
			Vector3 tempb = NodesXYZ1 - NodesXYZ3;
			normal = tempa.Cross(tempb);  //法向量
			if (normal.Dot(direction) > 0.0) {
				output->emplace(t, std::make_pair(false, intersection));
			}
			else {
				output->emplace(t, std::make_pair(true, intersection));
			}
		}
		
	}
	return true;
}

bool RayTracing::isIntersect(const Vector3 & orig, const Vector3 & dir,
	const Vector3 & v0, const Vector3 & v1, const Vector3 & v2, 
	Vector3 & intersection, double & t)
{
	float u, v;
	// E1
	Vector3 E1 = v1 - v0;

	// E2
	Vector3 E2 = v2 - v0;

	// P
	Vector3 P = dir.Cross(E2);

	// determinant
	double det = E1.Dot(P);

	Vector3 T;
	T = orig - v0;

	// If determinant is near zero, ray lies in plane of triangle
	if (det < 0.00000001 && det > -0.00000001)	return false;

	// Calculate u and make sure u <= 1
	u = T.Dot(P);
	u /= det;

	if (u < 0.0 || u > 1)
		return false;

	// Q
	Vector3 Q = T.Cross(E1);

	// Calculate v and make sure u + v <= 1
	v = dir.Dot(Q);
	v /= det;
	if (v < 0.0 || u + v > 1)
		return false;

	// Calculate t, scale parameters, ray intersects triangle
	t = E2.Dot(Q);
	t /= det;

	intersection = orig + dir * t;
	return true;
}

bool RayTracing::ray_CurvedSurface(const vector<double> &a, 
	Vector3 n, Vector3 org, double & t, Vector3 & interPoint)
{
	double x0 = org.x, y0 = org.y, z0 = org.z;
	double x1 = n.x, y1 = n.y, z1 = n.z;

	double A = a[0] * x1 * x1 + a[1] * y1 * y1 + a[2] * z1 * z1 + a[3] * x1 * y1 +
		a[4] * z1 * y1 + a[5] * x1 * z1;
	double B = 2 * a[0] * x1 * x0 + 2 * a[1] * y1 * y0 + 2 * a[2] * z1 * z0 +
		a[3] * (x0 * y1 + x1 * y0) + a[4] * (z0 * y1 + z1 * y0) + a[5] * (z0 * x1 + z1 * x0) +
		a[6] * x1 + a[7] * y1 + a[8] * z1;
	double C = a[0] * x0 * x0 + a[1] * y0 * y0 + a[2] * z0 * z0 +
		a[3] * x0 * y0 + a[4] * z0 * y0 + a[5] * x0 * z0 +
		a[6] * x0 + a[7] * y0 + a[8] * z0 + a[9];

	if (A < -THRESHOLD || A > THRESHOLD)
	{
		double temp = B * B - 4 * A * C;
		if (temp >= 0)
			temp = pow(temp, 0.5);
		else
			return false;

		double tempt1, tempt2;
		tempt1 = (-B + temp) / 2.0 / A;
		tempt2 = (-B - temp) / 2.0 / A; // 求根公式的两个解

		if (tempt1 >= 0.0 && tempt2 >= 0.0) // 都大于等于0 需要先比较是否在都在限制条件内
		{
			bool isIn1 = false;
			Vector3 interPoint1(x0 + x1 * tempt1, y0 + y1 * tempt1, z0 + z1 * tempt1);	
			// 判断是否在给出的区间内
			if (a[10] - THRESHOLD < interPoint1.x  &&
				interPoint1.x < a[11] + THRESHOLD &&
				a[12] - THRESHOLD < interPoint1.y &&
				interPoint1.y < a[13] + THRESHOLD &&
				a[14] - THRESHOLD < interPoint1.z &&
				interPoint1.z < a[15] + THRESHOLD)
			{
				isIn1 = true;
			}
			bool isIn2 = false;
			Vector3 interPoint2(x0 + x1 * tempt2, y0 + y1 * tempt2, z0 + z1 * tempt2);
			// 判断是否在给出的区间内
			if (a[10] - THRESHOLD < interPoint2.x  &&
				interPoint2.x < a[11] + THRESHOLD &&
				a[12] - THRESHOLD < interPoint2.y &&
				interPoint2.y < a[13] + THRESHOLD &&
				a[14] - THRESHOLD < interPoint2.z &&
				interPoint2.z < a[15] + THRESHOLD)
			{
				isIn2 = true;
			}
			if (isIn1 && isIn2)
			{
				if (tempt1 > tempt2)
				{
					interPoint = interPoint1;
					return true;
				}
				else
				{
					interPoint = interPoint2;
					return true;
				}
					
			}
			else if (isIn1 && !isIn2)
			{
				interPoint = interPoint1;
				return true;
			}
			else if (!isIn1 && isIn2)
			{
				interPoint = interPoint2;
				return true;
			}
			else 
			{
				return false;
			}
			
		}
		else if (tempt1 < 0.0 && tempt2 < 0.0) // 都小于0 无解
		{
			return false;
		}
		else                           // 取正值
		{
			if (tempt1 < tempt2)
				t = tempt2;
			else
				t = tempt1;
		}
	}
	else                          // 只有一个交点，与法线平行
		t = -C / B;

	if (t < 0.0)
		return false;
	else
	{
		// 判断是否在给出的区间内
		interPoint.Set(x0 + x1 * t, y0 + y1 * t, z0 + z1 * t);
		if (a[10] - THRESHOLD < interPoint.x  &&
			interPoint.x < a[11] + THRESHOLD &&
			a[12] - THRESHOLD < interPoint.y &&
			interPoint.y < a[13] + THRESHOLD &&
			a[14] - THRESHOLD < interPoint.z &&
			interPoint.z < a[15] + THRESHOLD)
		{
			return true;
		}
		else
			return false;
	}
}

void RayTracing::line_CurvedSurface(const vector<double>& a,
	Vector3 n, Vector3 org, double & t1, double & t2, bool & isOk1, bool & isOk2,
	Vector3 &interPoint1, Vector3 &interPoint2)
{
	double x0 = org.x, y0 = org.y, z0 = org.z;
	double x1 = n.x, y1 = n.y, z1 = n.z;

	double A = a[0] * x1 * x1 + a[1] * y1 * y1 + a[2] * z1 * z1 + a[3] * x1 * y1 +
		a[4] * z1 * y1 + a[5] * x1 * z1;
	double B = 2 * a[0] * x1 * x0 + 2 * a[1] * y1 * y0 + 2 * a[2] * z1 * z0 +
		a[3] * (x0 * y1 + x1 * y0) + a[4] * (z0 * y1 + z1 * y0) + a[5] * (z0 * x1 + z1 * x0) +
		a[6] * x1 + a[7] * y1 + a[8] * z1;
	double C = a[0] * x0 * x0 + a[1] * y0 * y0 + a[2] * z0 * z0 +
		a[3] * x0 * y0 + a[4] * z0 * y0 + a[5] * x0 * z0 +
		a[6] * x0 + a[7] * y0 + a[8] * z0 + a[9];

	if (A < -THRESHOLD || A > THRESHOLD)
	{
		double temp = B * B - 4 * A * C;
		if (temp >= 0)
			temp = pow(temp, 0.5);
		else
		{
			isOk1 = false;
			isOk2 = false;
			return;
		}
		double tempt1, tempt2;
		tempt1 = (-B + temp) / 2.0 / A;
		tempt2 = (-B - temp) / 2.0 / A; // 求根公式的两个解

		interPoint1.Set(x0 + x1 * tempt1, y0 + y1 * tempt1, z0 + z1 * tempt1);
		// 判断是否在给出的区间内
		if (a[10] - THRESHOLD < interPoint1.x  &&
			interPoint1.x < a[11] + THRESHOLD &&
			a[12] - THRESHOLD < interPoint1.y &&
			interPoint1.y < a[13] + THRESHOLD &&
			a[14] - THRESHOLD < interPoint1.z &&
			interPoint1.z < a[15] + THRESHOLD)
		{
			t1 = tempt1;
			isOk1 = true;
		}
		interPoint2.Set(x0 + x1 * tempt2, y0 + y1 * tempt2, z0 + z1 * tempt2);
		// 判断是否在给出的区间内
		if (a[10] - THRESHOLD < interPoint2.x  &&
			interPoint2.x < a[11] + THRESHOLD &&
			a[12] - THRESHOLD < interPoint2.y &&
			interPoint2.y < a[13] + THRESHOLD &&
			a[14] - THRESHOLD < interPoint2.z &&
			interPoint2.z < a[15] + THRESHOLD)
		{
			t2 = tempt2;
			isOk2 = true;
		}
	}
	else // 只有一个交点，与法线平行
	{
		isOk2 = false;
		double tempt1 = -C / B;
		interPoint1.Set(x0 + x1 * tempt1, y0 + y1 * tempt1, z0 + z1 * tempt1);
		if (a[10] - THRESHOLD < interPoint1.x  &&
			interPoint1.x < a[11] + THRESHOLD &&
			a[12] - THRESHOLD < interPoint1.y &&
			interPoint1.y < a[13] + THRESHOLD &&
			a[14] - THRESHOLD < interPoint1.z &&
			interPoint1.z < a[15] + THRESHOLD)
		{
			t1 = tempt1;
			isOk1 = true;
		}
		else
		{
			isOk1 = false;
		}
	}
}


Vector3 RayTracing::reflectLight(const Vector3 & a, const Vector3 & n)
{
	Vector3 tempN = n;
	if (a.Dot(n) > 0)
		tempN = Vector3(0, 0, 0) - n;
	//先单位化
	double absa = pow(a.Dot(a), 0.5);
	double absn = pow(tempN.Dot(tempN), 0.5);
	Vector3 tempa = a * (1 / absa);
	Vector3 tempn = tempN * (1 / absn);
	double I = 2 * tempn.Dot(tempa);
	if (I < 0)
		I = -I;
	else
		tempa = Vector3(0.0, 0.0, 0.0) - tempa;

	return tempn * I + tempa;
}


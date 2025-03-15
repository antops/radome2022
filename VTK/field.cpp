#include "field.h"

#include <vtkImageData.h>
#include <vtkInformation.h>
#include <vtkLookupTable.h>
#include <vtkImageMapToColors.h>
#include <vtkTransform.h>
#include <vtkArrowSource.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkDelaunay2D.h>
#include <vtkPoints.h>
#include <vtkFloatArray.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkProperty.h>

#include <QProgressDialog>
#include <QString>
#include <QCoreApplication>
#include <QTimer>

#include "../../common/include/field_base.h"
#include "../tools/cnpy.h"

const double dB_RABNGE = 60;

complex<double> ConjugateMul(const complex<double> &A, const complex<double> &B) 
{
	return complex<double>(A.real() * B.real() + A.imag() * B.imag(),
		-A.real() * B.imag() + A.imag() * B.real());
}
//using namespace calculation;
Field::Field()
{
	init();
	actor3D = vtkSmartPointer<vtkActor>::New();
	actor = vtkSmartPointer<vtkImageActor>::New();
	efficiency = 1.0;
	isShow = true;
}

Field::Field(const Field & _field)
	:N_width(_field.N_width), M_depth(_field.M_depth), ds(_field.ds),
	isSource(_field.isSource), isPhs(_field.isPhs), isLinear(_field.isLinear),
	is3D(_field.is3D), content(_field.content), efficiency(_field.efficiency),
	scalarCorrelationCoefficient(_field.scalarCorrelationCoefficient),
	vectorCorrelationCoefficient(_field.vectorCorrelationCoefficient)
{
	actor3D = vtkSmartPointer<vtkActor>::New();
	actor = vtkSmartPointer<vtkImageActor>::New();
	data.resize(2);
	data[0] = _field.data[0];
	data[1] = _field.data[1];
	graphTrans = _field.graphTrans;
	allocateMemory();
	for (int i = 0; i < N_width; i++)
		for (int j = 0; j < M_depth; j++)
		{
			Ex[i][j] = _field.Ex[i][j];
			Ey[i][j] = _field.Ey[i][j];
		}

	if (!isSource)
	{
		for (int i = 0; i < N_width; i++)
			for (int j = 0; j < M_depth; j++)
			{
				Ez[i][j] = _field.Ez[i][j];
				Hx[i][j] = _field.Hx[i][j];
				Hy[i][j] = _field.Hy[i][j];
				Hz[i][j] = _field.Hz[i][j];
			}
	}
}

Field::~Field()
{
	freeMemory();

}

void Field::init()
{
	N_width = 0; M_depth = 0;

	ds = 0;
	isSource = false;
	isPhs = false;
	isLinear = true;
	is3D = false;
	content = 1;
	data.resize(2);
}

void Field::freeMemory()
{

}

void Field::allocateMemory()
{
	Ex.resize(N_width);
	Ey.resize(N_width);
	for (int i = 0; i < N_width; ++i)
	{
		Ex[i].resize(M_depth);
		Ey[i].resize(M_depth);
	}

	if (!isSource) // ����Դ�ŷ��������ĳ�����
	{
		Ez.resize(N_width);
		Hx.resize(N_width);
		Hy.resize(N_width);
		Hz.resize(N_width);
		for (int i = 0; i < N_width; ++i)
		{
			Ez[i].resize(M_depth);
			Hx[i].resize(M_depth);
			Hy[i].resize(M_depth);
			Hz[i].resize(M_depth);
		}
	}
}

void Field::setNM(int N, int M)
{
	N_width = N;
	M_depth = M;
}

void Field::getNM(int & N, int & M) const
{
	N = N_width;
	M = M_depth;
}

void Field::setPlane(const GraphTrans & _graphTransPara, double _ds)
{
	graphTrans = _graphTransPara;
	ds = _ds;
	data[0] = N_width * _ds;
	data[1] = M_depth * _ds;

}

//void Field::setField(const FieldBase & tmp)
//{
//	N_width = tmp.N_width;
//	M_depth = tmp.M_depth;
//	ds = tmp.ds;
//	data[0] = N_width * ds;
//	data[1] = M_depth * ds;
//	graphTrans = tmp.graphTransField;
//	isSource = true;
//	allocateMemory();
//	for (int i = 0; i < N_width; i++)
//		for (int j = 0; j < M_depth; j++)
//		{
//			Ex[i][j] = tmp.Ex[i][j];
//			Ey[i][j] = tmp.Ey[i][j];
//			//Ez[i][j] = tmp.Ez[i][j];
//			//Hx[i][j] = tmp.Hx[i][j];
//			//Hy[i][j] = tmp.Hy[i][j];
//			//Hz[i][j] = tmp.Hz[i][j];
//		}
//	calcCorrelationCoefficient();
//}

void Field::setField(complex<double>** _Ex, complex<double>** _Ey,
	complex<double>** _Ez, complex<double>** _Hx, complex<double>** _Hy,
	complex<double>** _Hz)
{
	allocateMemory();
	for (int i = 0; i < N_width; i++)
		for (int j = 0; j < M_depth; j++)
		{
			Ex[i][j] = _Ex[i][j];
			Ey[i][j] = _Ey[i][j];
			Ez[i][j] = _Ez[i][j];
			Hx[i][j] = _Hx[i][j];
			Hy[i][j] = _Hy[i][j];
			Hz[i][j] = _Hz[i][j];
		}
	//calcCorrelationCoefficient();
}

void Field::setField(const vector<vector<complex<double>>>& _Ex, 
	const vector<vector<complex<double>>>& _Ey)
{
	isSource = true;
	allocateMemory();
	for (int i = 0; i < N_width; i++)
		for (int j = 0; j < M_depth; j++)
		{
			Ex[i][j] = _Ex[i][j];
			Ey[i][j] = _Ey[i][j];
		}
	//calcCorrelationCoefficient();
}

void Field::setField(complex<double>** _Ex, complex<double>** _Ey)
{
	isSource = true;
	allocateMemory();
	for (int i = 0; i < N_width; i++)
		for (int j = 0; j < M_depth; j++)
		{
			Ex[i][j] = _Ex[i][j];
			Ey[i][j] = _Ey[i][j];
		}
	//calcCorrelationCoefficient();
}

const vector<vector<complex<double>>>& Field::getEx() const
{
	return Ex;
}

const vector<vector<complex<double>>>& Field::getEy() const
{
	return Ey;
}

void Field::setDs(double _Ds)
{
	ds = _Ds;
}

double Field::getDs() const
{
	return ds;
}

void Field::getSourcePara(GraphTrans & _graphTransPara,
	int & _N_width, int & _M_depth, double & _ds) const
{
	_graphTransPara = graphTrans;

	_N_width = N_width;
	_M_depth = M_depth;
	_ds = ds;
}

void Field::setShowPara(int _content, bool _isLinear, bool _isPhs)
{
	content = _content;
	isLinear = _isLinear;
	isPhs = _isPhs;
}

void Field::getShowPara(int & _content, bool & _isLinear, bool & _isPhs)
{
	_content = content;
	_isLinear = isLinear;
	_isPhs = isPhs;
}

void Field::set3D(bool _is3D)
{
	is3D = _is3D;
}

bool Field::get3D() const
{
	return is3D;
}

void Field::calActor3D()
{
	vtkFloatArray *scalars = vtkFloatArray::New();
	vtkSmartPointer<vtkPoints> points =
		vtkSmartPointer<vtkPoints>::New();
	const vector<vector<complex<double>>> *tempEH = nullptr;
	switch (content)
	{
	case 0:
		tempEH = &Ex;
		break;
	case 1:
		tempEH = &Ey;
		break;
	case 2:
		tempEH = &Ez;
		break;
	case 3:
		tempEH = &Hx;
		break;
	case 4:
		tempEH = &Hy;
		break;
	case 5:
		tempEH = &Hz;
		break;
	default:
		break;
	}
	complex<double> temp;
	double max = 0;
	for (unsigned int x = 0; x < N_width; x++)
	{
		for (unsigned int y = 0; y < M_depth; y++)
		{
			complex<double> temp;
			temp = (*tempEH)[x][y];
			double tempD = sqrt(temp.real() * temp.real() +
				temp.imag() * temp.imag());
			if (max < tempD)
				max = tempD;
		}
	}

	for (unsigned int x = 0; x < N_width; x++)
	{
		for (unsigned int y = 0; y < M_depth; y++)
		{
			complex<double> temp;
			temp = (*tempEH)[x][y];
			double tempD = sqrt(temp.real() * temp.real() +
				temp.imag() * temp.imag());
			double tempD1 = tempD / max / 2;
			tempD = (max - tempD) / max / 2;

			points->InsertNextPoint(x*ds - N_width*ds / 2, y*ds - M_depth*ds / 2, tempD1);
			scalars->InsertTuple1(x*N_width + y, tempD);
		}
	}

	vtkSmartPointer<vtkPolyData> polydata =
		vtkSmartPointer<vtkPolyData>::New();
	polydata->SetPoints(points);
	polydata->GetPointData()->SetScalars(scalars);

	double scalarsRange[2];
	scalars->GetRange(scalarsRange);
	scalars->Delete();

	vtkSmartPointer<vtkDelaunay2D> delaunay =
		vtkSmartPointer<vtkDelaunay2D>::New();
	delaunay->SetInputData(polydata);
	delaunay->Update();


	vtkSmartPointer<vtkLookupTable> colorTable =
		vtkSmartPointer<vtkLookupTable>::New();
	colorTable->SetRange(scalarsRange);
	colorTable->Build();

	vtkSmartPointer<vtkPolyDataMapper> triangulatedMapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	triangulatedMapper->SetInputData(delaunay->GetOutput());
	triangulatedMapper->SetLookupTable(colorTable);
	triangulatedMapper->SetScalarRange(scalarsRange);
	triangulatedMapper->Update();

	vtkSmartPointer<vtkTransform> transform =
		vtkSmartPointer<vtkTransform>::New();
	transform->Translate(graphTrans.getTrans_x(),
		graphTrans.getTrans_y(), graphTrans.getTrans_z());
	//transform->RotateWXYZ(graphTrans.getRotate_theta(), graphTrans.getRotate_x(),
	//	graphTrans.getRotate_y(), graphTrans.getRotate_z());
	//transform->RotateWXYZ(graphTrans.getRotate_x_theta(), 1, 0, 0);
	//transform->RotateWXYZ(graphTrans.getRotate_y_theta(), 0, 1, 0);
	//transform->RotateWXYZ(graphTrans.getRotate_z_theta(), 0, 0, 1);

	for (int i = 0; i < 3; i++) {
		if (graphTrans.getRotate_x_num() == i) {
			transform->RotateWXYZ(graphTrans.getRotate_x_theta(), 1, 0, 0);
		}
		else if (graphTrans.getRotate_y_num() == i) {
			transform->RotateWXYZ(graphTrans.getRotate_y_theta(), 0, 1, 0);
		}
		else if (graphTrans.getRotate_z_num() == i) {
			transform->RotateWXYZ(graphTrans.getRotate_z_theta(), 0, 0, 1);
		}
	}
	//transform->Translate(-para[7] / 2, -para[8] / 2, 0);


	actor3D->SetMapper(triangulatedMapper);
	actor3D->SetUserTransform(transform);

}

void Field::calActor()
{
	auto info = vtkSmartPointer<vtkInformation>::New();
	vtkSmartPointer<vtkImageData> img = vtkSmartPointer<vtkImageData>::New();
	img->SetDimensions(N_width, M_depth, 1);
	//img->SetSpacing(0.005, 0.005, 1);
	img->SetSpacing(0.01*ds / 0.01, 0.01*ds / 0.01, 1);
	img->SetScalarType(VTK_DOUBLE, info);
	img->SetNumberOfScalarComponents(1, info);
	img->AllocateScalars(info);

	double *ptr = (double*)img->GetScalarPointer();
	double max = -100000000, min = 0;
	const vector<vector<complex<double>>> *tempEH = nullptr;
	switch (content)
	{
	case 0:
		tempEH = &Ex;
		break;
	case 1:
		tempEH = &Ey;
		break;
	case 2:
		tempEH = &Ez;
		break;
	case 3:
		tempEH = &Hx;
		break;
	case 4:
		tempEH = &Hy;
		break;
	case 5:
		tempEH = &Hz;
		break;
	default:
		break;
	}

	for (int j = 0; j < M_depth; j++)
		for (int i = 0; i < N_width; i++)
		{
			double tempD;
			complex<double> temp;
			temp = (*tempEH)[i][j];

			if (isPhs)
			{
				if (temp.real() != 0)
					tempD = atan2(temp.imag(),temp.real());
				else
					tempD = 0;
			}
			else
				tempD = pow((temp.real() * temp.real() +
					temp.imag() * temp.imag()), 0.5);
			if (!isLinear && !isPhs)
			{
				tempD = 20 * log(tempD + 0.000000001);
				if (min > tempD)
					min = tempD;
				if (max < tempD)
					max = tempD;
			}
			else
			{
				if (max < tempD)
					max = tempD;
				if (min > tempD)
					min = tempD;
			}
			*ptr++ = tempD;
			
		}
	ptr = (double*)img->GetScalarPointer();
	vtkSmartPointer<vtkLookupTable> colorTable =
		vtkSmartPointer<vtkLookupTable>::New();
	if (!isLinear && !isPhs)
		min = max - dB_RABNGE;
	if (!isPhs)
	{
		for (int i = 0; i<N_width * M_depth * 1; i++, ptr++)
			*ptr = max - *ptr;
		colorTable->SetRange(0, max - min);
	}
	else
		colorTable->SetRange(min, max);
	colorTable->Build();

	vtkSmartPointer<vtkImageMapToColors> colorMap =
		vtkSmartPointer<vtkImageMapToColors>::New();
	colorMap->SetInputData(img);
	colorMap->SetLookupTable(colorTable);
	colorMap->Update();

	vtkSmartPointer<vtkTransform> transform =
		vtkSmartPointer<vtkTransform>::New();
	transform->Translate(graphTrans.getTrans_x(),
		graphTrans.getTrans_y(), graphTrans.getTrans_z());
	//transform->RotateWXYZ(graphTrans.getRotate_theta(), graphTrans.getRotate_x(),
	//	graphTrans.getRotate_y(), graphTrans.getRotate_z());
	/*transform->RotateWXYZ(graphTrans.getRotate_x_theta(), 1, 0, 0);
	transform->RotateWXYZ(graphTrans.getRotate_y_theta(), 0, 1, 0);
	transform->RotateWXYZ(graphTrans.getRotate_z_theta(), 0, 0, 1);*/
	for (int i = 0; i < 3; i++) {
		if (graphTrans.getRotate_x_num() == i) {
			transform->RotateWXYZ(graphTrans.getRotate_x_theta(), 1, 0, 0);
		}
		else if (graphTrans.getRotate_y_num() == i) {
			transform->RotateWXYZ(graphTrans.getRotate_y_theta(), 0, 1, 0);
		}
		else if (graphTrans.getRotate_z_num() == i) {
			transform->RotateWXYZ(graphTrans.getRotate_z_theta(), 0, 0, 1);
		}
	}
	transform->Translate(-data[0] / 2, -data[1] / 2, 0);

	actor->SetInputData(colorMap->GetOutput());
	actor->SetUserTransform(transform);
}

vtkSmartPointer<vtkImageActor> Field::getActor() const
{
	return actor;
}

vtkSmartPointer<vtkActor> Field::getActor3D() const
{
	return actor3D;
}

void Field::SaveFieldBase(const std::string & fileName) const
{
	FieldBase base;
	Vector3 v3;
	v3.x = graphTrans.getRotate_x();
	v3.y = graphTrans.getRotate_y();
	v3.z = graphTrans.getRotate_z();
	base.coor.pos.x = graphTrans.getTrans_x();
	base.coor.pos.y = graphTrans.getTrans_y();
	base.coor.pos.z = graphTrans.getTrans_z();
	base.coor.SetRotate(v3, graphTrans.getRotate_theta());
	base.Ex = Ex;
	base.Ey = Ey;
	base.Ez = Ez;
	base.Hx = Hx;
	base.Hy = Hy;
	base.Hz = Hz;
	base.ds_x = ds;
	base.ds_y = ds;
	base.writeFieldBaseOldASCII(fileName);
}

void Field::ReadPythonText(const std::string& fileName)
{
	ifstream file(fileName);
	string temp;
	getline(file, temp);
	istringstream tempLine(temp);
	/**/
	int tempN, tempM;
	double tempds;
	tempLine >> tempN >> tempM >> tempds;

	// setPlane(trans, tempds * scale);
	N_width = tempN;
	M_depth = tempM;
	ds = tempds;
	isSource = true;

	allocateMemory();

	data.resize(2);
	data[0] = N_width * ds;
	data[1] = M_depth * ds;

	for (int i = 0; i < N_width; i++)
	{
		for (int j = 0; j < M_depth; j++)
		{
			getline(file, temp);
			istringstream tempLine(temp);

			double a1, p1, a2, p2;
			tempLine >> a1 >> p1 >> a2 >> p2;

			Ex[i][j] = complex<double>(a1 * cos(p1 / 180 * Pi),
				a1 * sin(p1 / 180 * Pi));
			Ey[i][j] = complex<double>(a2 * cos(p2 / 180 * Pi),
				a2 * sin(p2 / 180 * Pi));
		}
	}
	file.close();
	updateData();
}

void Field::save(const std::string & fileName) const
{
	if (isSource)
	{
		ofstream outfile(fileName + "_ExEy.txt");
		outfile << "Ex   Ey   Ez" << endl;
		for (int i = 0; i < N_width; i++)
		{
			for (int j = 0; j < M_depth; j++)
			{
				complex<double> temp = Ex[i][j];
				double tempD = pow((temp.real() * temp.real() +
					temp.imag() * temp.imag()), 0.5);
				outfile << tempD << " ";
				if (temp.real() != 0)
					tempD = atan(temp.imag() / temp.real());
				else
					tempD = 0;
				outfile << tempD << " ";

				temp = Ey[i][j];
				tempD = pow((temp.real() * temp.real() +
					temp.imag() * temp.imag()), 0.5);
				outfile << tempD << " ";
				if (temp.real() != 0)
					tempD = atan(temp.imag() / temp.real());
				else
					tempD = 0;
				outfile << tempD << " ";
				outfile << "0 0" << "\n";
			}
		}
		outfile.close();

	}
	else 
	{
		ofstream outfile(fileName + "_ExEyEz.txt");
		outfile << "Ex   Ey   Ez" << endl;
		for (int i = 0; i < N_width; i++)
		{
			for (int j = 0; j < M_depth; j++)
			{
				//Ex
				complex<double> temp = Ex[i][j];
				double tempD = pow((temp.real() * temp.real() +
					temp.imag() * temp.imag()), 0.5);
				outfile << tempD << " ";
				if (temp.real() != 0)
					tempD = atan(temp.imag() / temp.real());
				else
					tempD = 0;
				outfile << tempD << " ";

				//Ey
				temp = Ey[i][j];
				tempD = pow((temp.real() * temp.real() +
					temp.imag() * temp.imag()), 0.5);
				outfile << tempD << " ";
				if (temp.real() != 0)
					tempD = atan(temp.imag() / temp.real());
				else
					tempD = 0;
				outfile << tempD << " ";

				//Ez
				temp = Ez[i][j];
				tempD = pow((temp.real() * temp.real() +
					temp.imag() * temp.imag()), 0.5);
				outfile << tempD << " ";
				if (temp.real() != 0)
					tempD = atan(temp.imag() / temp.real());
				else
					tempD = 0;
				outfile << tempD << " ";

				outfile << "\n";
			}
		}
		outfile.close();

		ofstream outfile1(fileName + "_HxHyHz.txt");
		outfile1 << "Hx   Hy   Hz" << endl;
		for (int i = 0; i < N_width; i++)
		{
			for (int j = 0; j < M_depth; j++)
			{
				//Hx
				complex<double> temp = Hx[i][j];
				double tempD = pow((temp.real() * temp.real() +
					temp.imag() * temp.imag()), 0.5);
				outfile1 << tempD << " ";
				if (temp.real() != 0)
					tempD = atan(temp.imag() / temp.real());
				else
					tempD = 0;
				outfile1 << tempD << " ";

				//Hy
				temp = Hy[i][j];
				tempD = pow((temp.real() * temp.real() +
					temp.imag() * temp.imag()), 0.5);
				outfile1 << tempD << " ";
				if (temp.real() != 0)
					tempD = atan(temp.imag() / temp.real());
				else
					tempD = 0;
				outfile1 << tempD << " ";

				//Hz
				temp = Hz[i][j];
				tempD = pow((temp.real() * temp.real() +
					temp.imag() * temp.imag()), 0.5);
				outfile1 << tempD << " ";
				if (temp.real() != 0)
					tempD = atan(temp.imag() / temp.real());
				else
					tempD = 0;
				outfile1 << tempD << " ";

				outfile << "\n";
			}
		}
		outfile.close();
	}
}

void Field::savesource(const std::string &fileName) const {
	ofstream outfile(fileName);
	outfile << graphTrans.getTrans_x() << " "
		<< graphTrans.getTrans_y() << " "
		<< graphTrans.getTrans_z() << " "
		<< graphTrans.getRotate_x() << " "
		<< graphTrans.getRotate_y() << " "
		<< graphTrans.getRotate_z() << " "
		<< graphTrans.getRotate_theta() << " "
		<< N_width << " " << M_depth << " " 
		<< ds << " " << content << " "
		<< isLinear << " " << isPhs << " "
		<< is3D << " " << isSource << " "
		<< efficiency << " " << scalarCorrelationCoefficient << " "
		<< vectorCorrelationCoefficient << endl;
	for (int i = 0; i < N_width; i++)
		for (int j = 0; j < M_depth; j++)
		{
			outfile
				<< abs(Ex[i][j]) << " " << arg(Ex[i][j]) * 180 / 3.1415926 << " "
				<< abs(Ey[i][j]) << " " << arg(Ey[i][j]) * 180 / 3.1415926 << " ";
				if (!isSource)
				{
					outfile 
						<< abs(Ez[i][j]) << " " << arg(Ez[i][j]) * 180 / 3.1415926 << " "
						<< abs(Hx[i][j]) << " " << arg(Hx[i][j]) * 180 / 3.1415926 << " "
						<< abs(Hy[i][j]) << " " << arg(Hy[i][j]) * 180 / 3.1415926 << " "
						<< abs(Hz[i][j]) << " " << arg(Hz[i][j]) * 180 / 3.1415926 << " "
						<< endl;
				}
				else
				{
					outfile << 0 << " " << 0 << " "
						<< 0 << " " << 0 << " "
						<< 0 << " " << 0 << " "
						<< 0 << " " << 0 << " " << endl;
				}
		}
}

void Field::readsource(const std::string & fileName, double scale) 
{
	const double Pi = 3.1415926;
	ifstream file(fileName);
	string temp;
	getline(file, temp);
	istringstream tempLine(temp);
	/**/
	double tx, ty, tz, rx, ry, rz, rth;
	tempLine >> tx >> ty >> tz >> rx >> ry >> rz >> rth;
	GraphTrans trans;
	trans.setGraphTransPar(tx*scale, ty*scale, tz*scale, rx, ry, rz, rth);
	int tempN, tempM;
	double tempds;
	tempLine >> tempN >> tempM >> tempds;
	
	setPlane(trans, tempds*scale);
	N_width = tempN;
	M_depth = tempM;
	ds = tempds*scale;

	tempLine >> content >> isLinear >> isPhs >> is3D >> isSource
		>> efficiency >> scalarCorrelationCoefficient >>
		vectorCorrelationCoefficient;
	allocateMemory();

	data.resize(2);
	data[0] = N_width * ds;
	data[1] = M_depth * ds;

	for (int i = 0; i < N_width; i++)
	{
		for (int j = 0; j < M_depth; j++)
		{
			getline(file, temp);
			istringstream tempLine(temp);

			double a1, p1, a2, p2;
			tempLine >> a1 >> p1 >> a2 >> p2;

			Ex[i][j] = complex<double>(a1*cos(p1 / 180 * Pi),
				a1*sin(p1 / 180 * Pi));
			Ey[i][j] = complex<double>(a2*cos(p2 / 180 * Pi),
				a2*sin(p2 / 180 * Pi));
			if (!isSource)
			{
				tempLine >> a1 >> p1 >> a2 >> p2;

				Ez[i][j] = complex<double>(a1*cos(p1 / 180 * Pi),
					a1*sin(p1 / 180 * Pi));
				Hx[i][j] = complex<double>(a2*cos(p2 / 180 * Pi),
					a2*sin(p2 / 180 * Pi));

				tempLine >> a1 >> p1 >> a2 >> p2;

				Hy[i][j] = complex<double>(a1*cos(p1 / 180 * Pi),
					a1*sin(p1 / 180 * Pi));
				Hz[i][j] = complex<double>(a2*cos(p2 / 180 * Pi),
					a2*sin(p2 / 180 * Pi));
			}
		}
	}
	file.close();
	//calcCorrelationCoefficient();
}

bool Field::ReadFromNpy(const std::string & fileName)
{
	return true;
}

void Field::updateData()
{
	if (is3D)
		calActor3D();
	else
		calActor();
	setIsShow(isShow);
}

void Field::setIsShow(bool ok)
{
	isShow = ok;
	if (!isShow)
	{
		if (actor3D != NULL)
			actor3D->GetProperty()->SetOpacity(0);		
		actor->SetOpacity(0);
	}
	else
	{
		if (actor3D != NULL)
			actor3D->GetProperty()->SetOpacity(1);
		actor->SetOpacity(1);
	}
}

QTreeWidgetItem * Field::getTree()
{
	QTreeWidgetItem* tree = new QTreeWidgetItem;
	tree->setText(0, "Source");
	return tree;
}

void Field::setIsSource(bool isOK)
{
	isSource = isOK;
}

void Field::setEfficiency(double temp)
{
	if (temp > 0.999)
		this->efficiency = (temp - 0.01) / temp;
	this->efficiency = temp;
}

void Field::clacTransEffi(double tmp)
{
	for (int i = 0; i < N_width; i++) {
		for (int j = 0; j < M_depth; j++) {
			Ex[i][j] *= tmp;
			Ey[i][j] *= tmp;
		}
	}
}


void Field::calcCorrelationCoefficient(double fre)
{
	//scalarCorrelationCoefficient = Scalar_Correlation_Coefficient(fre, ds, N_width, M_depth, Ex, Ey, waistradius);	//Mark20201025
	//vectorCorrelationCoefficient = Vector_Correlation_Coefficient(fre, ds, N_width, M_depth, Ex, Ey);

}

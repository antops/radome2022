#include "farfield.h"

#include <vtkTriangle.h>
#include <vtkFloatArray.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkLookupTable.h>
#include <vtkPointData.h>
#include <vtkTransform.h>
#include <vtkImageData.h>
#include <vtkImageMapToColors.h>
#include <vtkInformation.h>
#include "../def.h"
#include "../tools/cnpy.h"
#include <fstream>

bool isFileExists_ifstream(const std::string& name) {
	ifstream f(name.c_str());
	return f.good();
}

FarField::FarField(const std::string& name)
{	
	name_ = name;
}

FarField::~FarField()
{
}

bool FarField::ReadDataFromFile(const std::string & file)
{
	if (far_field_base_.readFarFieldBaseASCII(file) != 0) {
		return false;
	}
	theta = far_field_base_.thetas;
	phi = far_field_base_.phis;

	E.resize(theta.size());
	EDB.resize(theta.size());
	EDB_origin.resize(theta.size());
	for (size_t i = 0; i < theta.size(); ++i) {
		E[i].resize(phi.size());
		EDB[i].resize(phi.size());
		EDB_origin[i].resize(phi.size());
	}

	for (int i = 0; i < theta.size(); i++) {
		for (int j = 0; j < phi.size(); j++) {
			double theta_real = far_field_base_.Etheta[j][i].real();
			double theta_imag = far_field_base_.Etheta[j][i].imag();
			double phi_real = far_field_base_.Ephi[j][i].real();
			double phi_imag = far_field_base_.Ephi[j][i].imag();
			E[i][j] = sqrt(theta_real*theta_real + theta_imag*theta_imag +
				phi_real*phi_real + phi_imag*phi_imag);
			//E[i][j] = 1000 - j;
			
		}
	}
	min_liner = INT32_MAX;
	max_liner = INT32_MIN;
	min_db = INT32_MAX;
	max_db = INT32_MIN;
	for (int i = 0; i < theta.size(); i++) {
		for (int j = 0; j < phi.size(); j++) {
			double value = E[i][j];
			if (value < min_liner) min_liner = value;
			if (value > max_liner) max_liner = value;
			//value = EDB[i][j];
			//if (value < min_db) min_db = value;
			//if (value > max_db) max_db = value;
		}
	}
	// 归一化
	for (int i = 0; i < theta.size(); i++) {
		for (int j = 0; j < phi.size(); j++) {
			if (E[i][j] <= 0.00000001) EDB_origin[i][j] = 0.0;
			else EDB_origin[i][j] = 10*log10(E[i][j]);
			double value = E[i][j] / max_liner;
			EDB[i][j] = 10 * log10(value);
			if (EDB[i][j] < min_db) min_db = EDB[i][j];
			if (EDB[i][j] > max_db) max_db = EDB[i][j];
		}
	}

	//param_.min = -40;
	//param_.max = 0;

	//double min = INT32_MAX;
	//double max = INT32_MIN;
	//if (param_.is_dB_unit) {
	//	min = param_.min;
	//	max = param_.max;
	//	if (min_db > param_.min) min = min_db;
	//	if (max_db < param_.max) max = max_db;
	//}
	//int x, y;
	//int xmax = 1;
	//int ymax = 1;
	//for (int i = 0; i < theta.size(); i++) {
	//	for (int j = 0; j < phi.size(); j++) {
	//		x = int(sin(theta[i]) * cos(phi[j]) * 100 + 100);
	//		y = int(sin(theta[i]) * sin(phi[j]) * 100 + 100);
	//		if (x > xmax) xmax = x;
	//		if (y > ymax) ymax = y;
	//	}
	//}
	//EDB_trans.resize(xmax);
	//for (size_t i = 0; i < xmax; ++i) {
	//	EDB_trans[i].resize(ymax, 0.0);
	//}
	//for (int i = 0; i < theta.size(); i++) {
	//	for (int j = 0; j < phi.size(); j++) {
	//		double value = EDB[i][j];
	//		if (value > max) value = max;
	//		if (value < min) value = min;
	//		x = int(sin(theta[i]) * cos(phi[j]) * 100 + 100);
	//		y = int(sin(theta[i]) * sin(phi[j]) * 100 + 100);
	//		if (x > xmax) xmax = x;
	//		if (y > ymax) ymax = y;
	//		EDB_trans[x][y] = value;
	//	}
	//}
	return true;
}

bool FarField::ReadFromNpy(const std::string & fileName)
{
	if (!isFileExists_ifstream(fileName + "/farFieldAbs.npy")) {
		return false;
	}
	if (!isFileExists_ifstream(fileName + "/phi.npy")) {
		return false;
	}
	if (!isFileExists_ifstream(fileName + "/theta.npy")) {
		return false;
	}
	cnpy::NpyArray Es_abs = cnpy::npy_load(fileName + "/farFieldAbs.npy");
	cnpy::NpyArray phi_abs = cnpy::npy_load(fileName + "/phi.npy");
	cnpy::NpyArray theta_abs = cnpy::npy_load(fileName + "/theta.npy");

	if (Es_abs.shape.size() != 2 || theta_abs.shape.empty() || phi_abs.shape.empty()) {
		return false;
	}
	if (Es_abs.shape[1] != theta_abs.shape[0] || Es_abs.shape[0] != phi_abs.shape[0]) {
		return false;
	}
	theta.resize(theta_abs.shape[0]);
	phi.resize(phi_abs.shape[0]);
	double* loaded_theta = theta_abs.data<double>();
	double* loaded_phi = phi_abs.data<double>();

	for (int i = 0; i < theta.size(); ++i) {
		theta[i] = loaded_theta[i];
	}
	for (int i = 0; i < phi.size(); ++i) {
		phi[i] = loaded_phi[i];
	}

	double max_theta = theta[0];
	double max_phi = phi[0];

	for (int i = 0; i < theta.size(); ++i) {
		if (max_theta < theta[i]) max_theta = theta[i];
	}
	for (int i = 0; i < phi.size(); ++i) {
		if (max_phi < phi[i]) max_phi = phi[i];
	}

	if (theta.size() < 2) {
		return false;
	}
	if (phi.size() < 2) {
		return false;
	}
	double theta_xx = max_theta / (theta.size()-1)*180/Pi;
	double phi_xx = max_phi / (phi.size()-1) * 180 / Pi;


	E.resize(theta.size());
	EDB.resize(theta.size());
	EDB_origin.resize(theta.size());
	for (size_t i = 0; i < theta.size(); ++i) {
		E[i].resize(phi.size());
		EDB[i].resize(phi.size());
		EDB_origin[i].resize(phi.size());
	}
	double* loaded_data = Es_abs.data<double>();
	std::vector<std::vector<double>> E_tmp;
	std::vector<std::vector<double>> E_tmp_txt;
	E_tmp.resize(phi.size());
	E_tmp_txt.resize(phi.size());
	for (size_t i = 0; i < phi.size(); ++i) {
		E_tmp[i].resize(theta.size());
		E_tmp_txt[i].resize(theta.size());
	}
	for (int i = 0; i < phi.size(); i++) {
		for (int j = 0; j < theta.size(); j++) {
			E_tmp[i][j] = loaded_data[j + i * theta.size()];
		}
	}
	ifstream file;
	file.open(fileName + "/farFieldAbs.txt");
	if (!file.is_open()) { 
		// return false;
	}
	stringstream tempfile;
	tempfile << file.rdbuf();
	for (int i = 0; i < phi.size(); i++) {
		std::string temp;
		getline(tempfile, temp);
		istringstream tempLine(temp);
		for (int j = 0; j < theta.size(); j++) {
			tempLine >> E_tmp_txt[i][j];
			//E_tmp_txt[i][j] = loaded_data[j + i * theta.size()];
		}
	}
	file.close();

	for (int i = 0; i < theta.size(); i++) {
		for (int j = 0; j < phi.size(); j++) {
			E[i][j] = E_tmp_txt[j][i];
		}
	}
	loaded_data[0];
	loaded_data[1];
	loaded_data[2];

	min_liner = INT32_MAX;
	max_liner = INT32_MIN;
	min_db = INT32_MAX;
	max_db = INT32_MIN;
	for (int i = 0; i < theta.size(); i++) {
		for (int j = 0; j < phi.size(); j++) {
			double value = E[i][j];
			if (value < min_liner) min_liner = value;
			if (value > max_liner) max_liner = value;
			//value = EDB[i][j];
			//if (value < min_db) min_db = value;
			//if (value > max_db) max_db = value;
		}
	}
	// 归一化
	double ta_value = 0.0;
	int counter = 0;
	for (int i = 0; i < theta.size(); i++) {
		for (int j = 0; j < phi.size(); j++) {
			double abs_value = E[i][j];
			double tmp = abs_value* abs_value* sin(theta[i]) * Pi * Pi * theta_xx  * phi_xx / (4 * 180 * 180  * Pi);
			ta_value += tmp;
			counter++;
		}
	}
	//double avg_value = ta_value / counter;

	for (int i = 0; i < theta.size(); i++) {
		for (int j = 0; j < phi.size(); j++) {
			double abs_value = E[i][j];
			if (E[i][j] <= 0.00000001) EDB_origin[i][j] = 0.0;
			else {
				EDB_origin[i][j] = 10 * log10(abs_value * abs_value / ta_value);
			}
			double value = E[i][j] / max_liner;
			EDB[i][j] = 10 * log10(value);
			if (EDB[i][j] < min_db) min_db = EDB[i][j];
			if (EDB[i][j] > max_db) max_db = EDB[i][j];
		}
	}
	param_.min = -40;
	param_.max = 0;
	if (isFileExists_ifstream(fileName + "/tbl.npy")) {
		cnpy::NpyArray tbl = cnpy::npy_load(fileName + "/tbl.npy");
		is_clac_tbl_ = true;
		tbl_val_ = *(tbl.data<double>());
	}
	file_name_ = fileName;

	double min = INT32_MAX;
	double max = INT32_MIN;
	if (param_.is_dB_unit) {
		min = param_.min;
		max = param_.max;
		if (min_db > param_.min) min = min_db;
		if (max_db < param_.max) max = max_db;
	}
	int x, y;
	int xmax = 1;
	int ymax = 1;
	for (int i = 0; i < theta.size(); i++) {
		for (int j = 0; j < phi.size(); j++) {
			x = int(sin(theta[i]) * cos(phi[j]) * 100 + 100);
			y = int(sin(theta[i]) * sin(phi[j]) * 100 + 100);
			if (x > xmax) xmax = x;
			if (y > ymax) ymax = y;
		}
	}
	EDB_trans.resize(xmax+1);
	for (size_t i = 0; i < xmax+1; ++i) {
		EDB_trans[i].resize(ymax+1, 0.0);
	}
	for (int i = 0; i < theta.size(); i++) {
		for (int j = 0; j < phi.size(); j++) {
			double value = EDB[i][j];
			if (value > max) value = max;
			if (value < min) value = min;
			x = int(sin(theta[i]) * cos(phi[j]) * 100 + 100);
			y = int(sin(theta[i]) * sin(phi[j]) * 100 + 100);
			if (x < 0 || y < 0) {
				continue;
			}
			if (x >= EDB_trans.size() || y >= ymax) {
				continue;
			}
			EDB_trans[x][y] = value* sin(theta[i]);
		}
	}
	return true;
}

bool FarField::ReadTblFromNpy(const std::string& fileName) {
	if (isFileExists_ifstream(fileName + "/tbl.npy")) {
		cnpy::NpyArray tbl = cnpy::npy_load(fileName + "/tbl.npy");
		is_clac_tbl_ = true;
		tbl_val_ = *(tbl.data<double>());
		return true;
	}
	return false;
}

bool FarField::WriteDataToFile(const std::string& file) const {
	ofstream outfile(file + "/Eabs_" + name_ + ".txt");
	if (!outfile.is_open()) {
		return false;
	}

	ofstream outfile_theta(file + "/theta_" + name_ + ".txt");
	if (!outfile_theta.is_open()) {
		return false;
	}

	ofstream outfile_phi(file + "/phi_" + name_ + ".txt");
	if (!outfile_phi.is_open()) {
		return false;
	}

	for (const auto& Ey : E) {
		for (const auto& Exy : Ey) {
			outfile << Exy << " ";
		}
		outfile << std::endl;
	}
	outfile.close();

	for (int i = 0; i < theta.size(); ++i) {
		outfile_theta << theta[i] << " ";
	}
	outfile_theta.close();

	
	for (int i = 0; i < phi.size(); ++i) {
		outfile_phi << phi[i] << " ";
	}
	outfile_phi.close();
	return true;
}

bool FarField::CreateActor3D()
{
	vtkSmartPointer<vtkPoints> points =
		vtkSmartPointer<vtkPoints>::New();
	vtkFloatArray *scalars = vtkFloatArray::New();
	//单元方向图默认是线性显示
	double ratio = 0.4; //缩放比例
	double min = INT32_MAX;
	double max = INT32_MIN;
	if (param_.is_dB_unit) {
		min = param_.min;
		max = param_.max;
		if (min_db > param_.min) min = min_db;
		if (max_db < param_.max) max = max_db;
	}
	else {
		min = min_liner;
		max = max_liner;
	}
	
	ratio = 30.0;
	double x, y, z;
	for (int i = 0; i < theta.size(); i++) {
		for (int j = 0; j < phi.size(); j++) {
			double value = E[i][j];
			if (param_.is_dB_unit) {
				value = EDB[i][j];
			}
			if (value > max) value = max;
			if (value < min) value = min;
			x = ratio * sin(theta[i]) * cos(phi[j]) * (value - min);
			y = ratio * sin(theta[i]) * sin(phi[j]) * (value - min);
			z = ratio * cos(theta[i]) * (value - min);

			scalars->InsertTuple1(i * phi.size() + j, max - value);
			vtkIdType idtype = points->InsertNextPoint(x, y, z);
		}
	}

	vtkSmartPointer<vtkCellArray>pLineCell = vtkCellArray::New();
	for (int i = 0; i < theta.size() - 1; i++) {
		for (int j = 0; j < phi.size() - 1; j++)
		{
			vtkSmartPointer<vtkTriangle> p1 = vtkSmartPointer<vtkTriangle>::New();
			p1->GetPointIds()->SetId(0, i * phi.size() + j);
			p1->GetPointIds()->SetId(1, (i + 1) * phi.size() + j);
			p1->GetPointIds()->SetId(2, (i + 1) * phi.size() + j + 1);
			pLineCell->InsertNextCell(p1);
			vtkSmartPointer<vtkTriangle> p2 = vtkSmartPointer<vtkTriangle>::New();
			p2->GetPointIds()->SetId(0, i * phi.size() + j);
			p2->GetPointIds()->SetId(1, i * phi.size() + j + 1);
			p2->GetPointIds()->SetId(2, (i + 1) * phi.size() + j + 1);
			pLineCell->InsertNextCell(p2);
		}
		vtkSmartPointer<vtkTriangle> p1 = vtkSmartPointer<vtkTriangle>::New();
		p1->GetPointIds()->SetId(0, i * phi.size() + phi.size() - 1);
		p1->GetPointIds()->SetId(1, (i + 1) * phi.size() + phi.size() - 1);
		p1->GetPointIds()->SetId(2, (i + 1) * phi.size());
		pLineCell->InsertNextCell(p1);
		vtkSmartPointer<vtkTriangle> p2 = vtkSmartPointer<vtkTriangle>::New();
		p2->GetPointIds()->SetId(0, i * phi.size() + phi.size() - 1);
		p2->GetPointIds()->SetId(1, i * phi.size());
		p2->GetPointIds()->SetId(2, (i + 1) * phi.size());
		pLineCell->InsertNextCell(p2);
	}
	

	vtkSmartPointer<vtkPolyData>pointsData = vtkSmartPointer<vtkPolyData>::New();
	pointsData->SetPoints(points); //获得网格模型中的几何数据：点集  
	pointsData->SetStrips(pLineCell);
	pointsData->GetPointData()->SetScalars(scalars);


	vtkSmartPointer<vtkLookupTable> colorTable =
		vtkSmartPointer<vtkLookupTable>::New();
	colorTable->SetRange(0, max - min);
	colorTable->Build();

	vtkSmartPointer<vtkPolyDataMapper> pointMapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	pointMapper->SetInputData(pointsData);
	pointMapper->SetLookupTable(colorTable);
	pointMapper->SetScalarRange(0, max - min);
	pointMapper->Update();

	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	// 用户自定义平移旋转 (先移动后旋转)
	transform->Translate(0,0,0);
	transform->RotateWXYZ(180, 1, 0, 0);

	actor3D_ = vtkActor::New();
	actor3D_->SetMapper(pointMapper);
	actor3D_->SetUserTransform(transform);

	return true;
}

bool FarField::CreateActor2D()
{
	auto info = vtkSmartPointer<vtkInformation>::New();
	vtkSmartPointer<vtkImageData> img = vtkSmartPointer<vtkImageData>::New();
	img->SetDimensions(theta.size(), phi.size(), 1);
	img->SetSpacing(0.005, 0.005, 1);
	//img->SetSpacing(0.01*ds / 0.01, 0.01*ds / 0.01, 1);
	img->SetScalarType(VTK_DOUBLE, info);
	img->SetNumberOfScalarComponents(1, info);
	img->AllocateScalars(info);

	double min = INT32_MAX;
	double max = INT32_MIN;
	if (param_.is_dB_unit) {
		min = param_.min;
		max = param_.max;
		if (min_db > param_.min) min = min_db;
		if (max_db < param_.max) max = max_db;
	}
	else {
		min = min_liner;
		max = max_liner;
	}

	double *ptr = (double*)img->GetScalarPointer();
	for (int i = 0; i < theta.size(); i++) {
		for (int j = 0; j < phi.size(); j++) {
			double value = 0.0;
			if (param_.is_dB_unit) {
				value = EDB[i][j];
			}
			else {
				value = E[i][j];
			}
			if (value > max) value = max;
			if (value < min) value = min;
			*ptr++ = max - value;
		}
	}
	
	ptr = (double*)img->GetScalarPointer();
	vtkSmartPointer<vtkLookupTable> colorTable =
		vtkSmartPointer<vtkLookupTable>::New();
	colorTable->SetRange(0, max - min);
	
	colorTable->Build();

	vtkSmartPointer<vtkImageMapToColors> colorMap =
		vtkSmartPointer<vtkImageMapToColors>::New();
	colorMap->SetInputData(img);
	colorMap->SetLookupTable(colorTable);
	colorMap->Update();
	actor_ = vtkSmartPointer<vtkImageActor>::New();
	actor_->SetInputData(colorMap->GetOutput());
	return true;
}

QTreeWidgetItem * FarField::GetTree()
{
	
	QTreeWidgetItem *tree_item = new QTreeWidgetItem(QStringList(
		QString::fromLocal8Bit(name_.c_str())));
	tree_item->setData(0, Qt::UserRole, QVariant(Def::farfield_type));
	tree_item->setData(1, Qt::UserRole, index_);
	tree_item->setExpanded(true);
	return tree_item;
}

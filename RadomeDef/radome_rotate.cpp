#include "radome_rotate.h"

#include "vtkTransform.h"
#include "vtkCamera.h"
#include "vtkInformation.h"
#include "vtkLineSource.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkNew.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyDataNormals.h"
#include "vtkProperty.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkQuadRotationalExtrusionFilter.h"
#include "vtkBooleanOperationPolyDataFilter.h"
#include "vtkTriangleFilter.h"
#include "vtkTransformPolyDataFilter.h"

#include "vtkDelaunay3D.h"
#include "vtkDataSetMapper.h"
#include "vtkVertexGlyphFilter.h"
#include "vtkDelaunay2D.h"
#include "vtkPolyLine.h"
#include "vtkCellArray.h"

#include <QString>
#include <QStringList>
#include <QVariant>

#include "../Calculation/RayTracing.h"
// 颜色池

Vector3 CalcInterPonit(const Vector3& a, const Vector3& b, double z) {
	Vector3 point;
	point.z = z;
	point.y = 0.0;
	point.x = a.x + (z - a.z) * (b.x - a.x) / (b.z - a.z);
	return point;
}

RadomeRotate::RadomeRotate()
{
}

RadomeRotate::RadomeRotate(const std::vector<RotateLineBase*>& lines, double x, double y, double cut) {
	rotate_line_vec_ = lines;
	x_scale_ = x;
	y_scale_ = y;
	type_ = 1;
	cut_ = cut;
}

RadomeRotate::~RadomeRotate()
{
}

bool RadomeRotate::UpdateSurface()
{
	double min_z = INT32_MAX;
	for (auto&rotate_line : rotate_line_vec_) {
		auto& lines = rotate_line->GetLines();
		if (lines.back().z < min_z) {
			min_z = lines.back().z;
		}
	}

	double ratio = 180.0;
	for (auto&rotate_line : rotate_line_vec_) {
		// Create a set of points
		vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
		for (auto& vec : rotate_line->GetLines()) {
			if (vec.z > min_z) break;
			double angle = 0;
			while (angle <= 2.0 * vtkMath::Pi() + (vtkMath::Pi() / ratio))
			{
				points->InsertNextPoint(vec.x * cos(angle),
					vec.x * sin(angle), vec.z);
				angle = angle + (vtkMath::Pi() / ratio);
			}

		}

		vtkSmartPointer<vtkPolyData> polydata =
			vtkSmartPointer<vtkPolyData>::New();
		polydata->SetPoints(points);

		// Create a Delaunay2D filter
		vtkSmartPointer<vtkDelaunay2D> delaunay = vtkSmartPointer<vtkDelaunay2D>::New();
		delaunay->SetInputData(polydata);
		delaunay->Update();


		vtkSmartPointer<vtkTransform> transform =
			vtkSmartPointer<vtkTransform>::New();
		transform->Scale(x_scale_, y_scale_, 1.0);
		transform->RotateWXYZ(180, 1, 0, 0);
		transform->Translate(0, 0, -min_z);


		vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter =
			vtkSmartPointer<vtkTransformPolyDataFilter>::New();
		transformFilter->SetInputData(delaunay->GetOutput());
		transformFilter->SetTransform(transform); //use vtkTransform (or maybe vtkLinearTransform)
		transformFilter->Update();


		vtkSmartPointer<vtkTriangleFilter> triangleFilter =
			vtkSmartPointer<vtkTriangleFilter>::New();
		triangleFilter->SetInputData(transformFilter->GetOutput());
		triangleFilter->Update();


		surface_poly_data_vec_.push_back(triangleFilter->GetOutput());
	}
	
	return true;
}

bool RadomeRotate::JudgeLegal(const std::vector<Vector3>& line1,
	const std::vector<Vector3>& line2)
{
	if (line1.size() < 2 || line2.size() < 2) return false;
	// if (line1[0].x > 1e-10 || line2[0].x > 1e-10) return false;
	if (line1[0].z > line2[0].z) return false;
	return true;
}

vtkSmartPointer<vtkPolyData> RadomeRotate::GenShowPolyData(
		const std::vector<Vector3>& line1,
		const std::vector<Vector3>& line2)
{
	if (!JudgeLegal(line1, line2)) return NULL;
	std::vector<Vector3> cut_surface;
	Vector3 end_ponit;
	const std::vector<Vector3>* other_line = NULL;
	if (line1[line1.size()-1].z < line2[line2.size()-1].z) {
		end_ponit = line1[line1.size()-1];
		cut_surface = line1;
		other_line = &line2;
	} else {
		end_ponit = line2[line2.size()-1];
		cut_surface = line2;
		other_line = &line1;
	}
	Vector3 other_end_ponit;
	int index = other_line->size() - 2;

	for (; index >= 0; index--) {
		if (end_ponit.z > (*other_line)[index].z) break;
	}
	cut_surface.push_back(
		CalcInterPonit((*other_line)[index], (*other_line)[index + 1], end_ponit.z));
	for (int i = index; i >= 0; i--) {
		cut_surface.push_back((*other_line)[i]);
	}

	return GenPolyData(cut_surface);
}

bool RadomeRotate::Update(std::string* err_msg)
{
	if (rotate_line_vec_.size() < 2) {
		return false;
	}
	polyData_vec_.clear();
	material_vec_.clear();
	material_name_vec_.clear();
	for (int i = 0; i < rotate_line_vec_.size() - 1; i++) {
		vtkSmartPointer<vtkPolyData> poly_data = GenShowPolyData(
			rotate_line_vec_[i]->GetLines(), rotate_line_vec_[i+1]->GetLines());
		if (poly_data == NULL) {
			return false;
		}
		// 生成默认介电常数
		material_vec_.push_back(0);
		polyData_vec_.push_back(poly_data);
		material_name_vec_.push_back("金属");
	}

	int index = 0;
	for (auto & data : polyData_vec_) {
		vtkSmartPointer<vtkPolyDataMapper>mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper->SetInputData(data);
		mapper->ScalarVisibilityOff();
		vtkSmartPointer<vtkActor> actor =
			vtkSmartPointer<vtkActor>::New();
		actor->SetMapper(mapper);
		if (index < COLOR_POOL_SIZE) {
			actor->GetProperty()->SetColor(COLOR_RADOME_POOL[index]);
		}
		actor->GetProperty()->SetOpacity(0.4);
		index++;
		actor_vec_.push_back(actor);
	}
	UpdateTreeItem();
	return true;
}

vtkSmartPointer<vtkPolyData> RadomeRotate::GenPolyData(const std::vector<Vector3>& lines) {
	// Create a line source
	vtkSmartPointer<vtkPoints> points =
		vtkSmartPointer<vtkPoints>::New();
	double max_z = 0;
	for (auto& line : lines) {
		if (cut_ > 0.0 && cut_ < line.z) {
			break;
		}
		points->InsertNextPoint(line.x, line.y, line.z);
		if (max_z < line.z) max_z = line.z;
	}

	vtkSmartPointer <vtkLineSource> line = vtkSmartPointer <vtkLineSource>::New();
	line->SetPoints(points);
	line->Update();

	// Create mapper for line segment
	vtkNew<vtkPolyDataMapper> lineMapper;
	lineMapper->SetInputConnection(line->GetOutputPort());

	// Create multi-block data set for quad-based sweep
	vtkNew<vtkMultiBlockDataSet> lineMB;
	lineMB->SetNumberOfBlocks(1);
	lineMB->GetMetaData(static_cast<unsigned>(0))->Set(vtkCompositeDataSet::NAME(), "Line");
	lineMB->SetBlock(0, line->GetOutput());

	// Create 3/4 of a cylinder by rotational extrusion
	vtkNew<vtkQuadRotationalExtrusionFilter> lineSweeper;
	lineSweeper->SetResolution(60);
	lineSweeper->SetInputData(lineMB.GetPointer());
	lineSweeper->SetDefaultAngle(360);
	lineSweeper->Update();

	// Retrieve polydata output
	vtkMultiBlockDataSet* cylDS = vtkMultiBlockDataSet::SafeDownCast(lineSweeper->GetOutputDataObject(0));
	vtkPolyData* cyl = vtkPolyData::SafeDownCast(cylDS->GetBlock(0));

	vtkSmartPointer<vtkTransform> transform =
		vtkSmartPointer<vtkTransform>::New();
	transform->Scale(x_scale_, y_scale_, 1.0);
	transform->RotateWXYZ(180, 1, 0, 0);
	transform->Translate(0, 0, -max_z);


	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter =
		vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	transformFilter->SetInputData(cyl);
	transformFilter->SetTransform(transform); //use vtkTransform (or maybe vtkLinearTransform)
	transformFilter->Update();

	vtkSmartPointer<vtkTriangleFilter> triangleFilter =
		vtkSmartPointer<vtkTriangleFilter>::New();
	triangleFilter->SetInputData(transformFilter->GetOutput());
	triangleFilter->Update();
	return triangleFilter->GetOutput();
}

void RadomeRotate::UpdateTreeItem() 
{
	tree_item_ = new QTreeWidgetItem(QStringList(QString::fromLocal8Bit("旋转罩体")));
	tree_item_->setData(0, Qt::UserRole, QVariant(Def::model_type));
	int num = 1;

	for (auto& x : material_name_vec_) {
		QTreeWidgetItem *child_tmp = new QTreeWidgetItem(
			QStringList(QString::fromLocal8Bit("天线罩层 ") + QString::number(num)));
		child_tmp->setData(0, Qt::UserRole, QVariant(Def::rotate_layer_type));
		child_tmp->setData(1, Qt::UserRole, QVariant(num-1));
		tree_item_->addChild(child_tmp);
		QTreeWidgetItem *child_eps;
		child_eps = new QTreeWidgetItem(
			QStringList(QString::fromLocal8Bit("材料=") +
				QString::fromLocal8Bit(x.c_str())));
		child_tmp->addChild(child_eps);
		child_tmp->setExpanded(true);
		num++;
	}
	tree_item_->setExpanded(true);

}

void RadomeRotate::Mesh(const MeshOption& option, int* step) {
	//double z_len = option.max_p.z - option.min_p.z;
	//double y_len = option.max_p.y - option.min_p.y;
	//double x_len = option.max_p.x - option.min_p.x;

	//int z_num = int(z_len / option.scale);
	//int y_num = int(y_len / option.scale);
	//int x_num = int(x_len / option.scale);
	//RayTracing ray_tracing;
	//int count = 1;
	//for (int polyData_num = 0; polyData_num < polyData_vec_.size(); polyData_num++) {
	//	std::vector<std::vector<std::vector<Vector3>>> mesh_tmp(z_num);
	//	for (int i = 0; i < z_num; i++) {
	//		mesh_tmp[i].resize(y_num);
	//	}
	//	std::vector<Vector3> start_piont_vec;
	//	start_piont_vec.reserve(z_num*y_num);
	//	std::vector<Vector3> direction_vec;
	//	direction_vec.reserve(z_num*y_num);
	//	for (int i = 0; i < z_num; i++) {
	//		for (int j = 0; j < y_num; j++) {
	//			Vector3 star_point;
	//			star_point.z = option.min_p.z + i*option.scale;
	//			star_point.y = option.min_p.y + j*option.scale;
	//			star_point.x = option.min_p.x;
	//			Vector3 normal(1.0, 0.0, 0.0);
	//			start_piont_vec.push_back(star_point);
	//			direction_vec.push_back(normal);
	//		}
	//	}
	//	std::vector<std::vector<Vector3>> nomal;
	//	std::vector<std::vector<Vector3>> intersection;
	//	std::vector<std::vector<double>> prot;
	//	nomal.resize(start_piont_vec.size());
	//	intersection.resize(start_piont_vec.size());
	//	prot.resize(start_piont_vec.size());
	//	ray_tracing.CalcInterPointBatch(
	//		start_piont_vec, direction_vec, polyData_vec_[polyData_num], nomal, intersection, prot);

	//	for (int i = 0; i < nomal.size(); i++) {
	//		std::map<double, std::pair<bool, Vector3>> tmp_data;
	//		for (int j = 0; j < nomal[i].size(); j++) {
	//			if (nomal[i][j].Dot(direction_vec[i]) > 0.0) {
	//				tmp_data.emplace(prot[i][j], std::make_pair(false, intersection[i][j]));
	//			}
	//			else {
	//				tmp_data.emplace(prot[i][j], std::make_pair(true, intersection[i][j]));
	//			}
	//		}
	//		bool is_in = true;
	//		for (const auto& x : tmp_data) {
	//			if (is_in == x.second.first) {
	//				mesh_tmp[i / y_num][i % y_num].push_back(x.second.second);
	//				is_in = !is_in;
	//			}
	//			// else break;
	//		}
	//	}

	//	hex_mesh_.x_n = x_num;
	//	hex_mesh_.y_n = y_num;
	//	hex_mesh_.z_n = z_num;
	//	// 设置背景材料
	//	hex_mesh_.ResetAll(float(background_material_index_));
	//	for (int i = 0; i < z_num; i++) {
	//		for (int j = 0; j < y_num; j++) {
	//			auto& zy = mesh_tmp[i][j];
	//			for (int n = 0; n < (int)(zy.size()) - 1; n += 2) {
	//				int index = (zy[n].x - option.min_p.x) / option.scale;
	//				while ((index*option.scale + option.min_p.x) < zy[n+1].x) {
	//					if (index >= hex_mesh_.mesh_data.size()) {
	//						break;
	//					}
	//					hex_mesh_.mesh_data[index][j][i] = material_vec_[polyData_num];
	//					index++;
	//				}
	//				
	//			}
	//		}
	//	}
	//	HexMesh::MaterialParam param;
	//	hex_mesh_.material_book.emplace(background_material_index_, param);
	//	hex_mesh_.material_book.emplace(material_vec_[polyData_num], param);
	//	hex_mesh_.WriteFile("./model_dat");

	//	vtkSmartPointer<vtkPoints> points =
	//		vtkSmartPointer<vtkPoints>::New();
	//	vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();
	//	for (int i = 0; i < x_num; i++) {
	//		for (int j = 0; j < y_num; j++) {
	//			for (int k = 0; k < z_num; k++) {
	//				if (hex_mesh_.mesh_data[i][j][k] < 1.5f) continue;
	//				vtkIdType idtype = points->InsertNextPoint(
	//					option.min_p.x + i*option.scale,
	//					option.min_p.y + j*option.scale, 
	//					option.min_p.z + k*option.scale);
	//				cells->InsertNextCell(1, &idtype);
	//			}
	//		}
	//	}
	//	//for (int i = 0; i < z_num; i++) {
	//	//	for (int j = 0; j < y_num; j++) {
	//	//		auto& zy = mesh_tmp[i][j];
	//	//		for (int n = 0; n < (int)(zy.size())-1; n+=2) {
	//	//			int step = (int)((zy[n + 1].x - zy[n].x) / option.scale);
	//	//			for (int k = 0; k <= step; k++) {
	//	//				vtkIdType idtype = points->InsertNextPoint(zy[n].x+ option.scale*k, zy[n].y, zy[n].z);
	//	//				cells->InsertNextCell(1, &idtype);
	//	//			}
	//	//			vtkIdType idtype = points->InsertNextPoint(zy[n + 1].x, zy[n].y, zy[n].z);
	//	//			cells->InsertNextCell(1, &idtype);
	//	//		}
	//	//	}
	//	//}

	//	vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
	//	polyData->SetPoints(points);
	//	polyData->SetVerts(cells);

	//	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	//	mapper->SetInputData(polyData);

	//	actor_mesh_ = vtkSmartPointer<vtkActor>::New();
	//	actor_mesh_->SetMapper(mapper);
	//	actor_mesh_->GetProperty()->SetPointSize(5);
	//	actor_mesh_->GetProperty()->SetColor(0., .749, 1.); // deep sky blue

	//	is_mesh_ = true;
	//	count++;
	//}
	//if (step) { *step = 100; }
}

bool RadomeRotate::SaveToPoints(const std::string& path, std::vector<std::string>* path_vec)
{
	path_vec->clear();
	for (int i = rotate_line_vec_.size() - 1; i >= 0; --i) {
		std::string filepath = path + "/radome_p_" + std::to_string(i) + ".txt";
		rotate_line_vec_[i]->SaveToFile(filepath);
		path_vec->emplace_back(filepath);
	}
	return true;
}

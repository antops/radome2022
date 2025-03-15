#include "radome_stl.h"

#include "vtkTransform.h"
#include "vtkPolyDataMapper.h"
#include "vtkSTLReader.h"

#include <QString>
#include <QStringList>
#include <QVariant>

#include "../def.h"


RadomeSTL::RadomeSTL()
{
	type_ = 2;
}


RadomeSTL::~RadomeSTL()
{
}

bool RadomeSTL::ReadSTLFromFile(const std::vector<std::string>& paths)
{
	material_name_vec_.clear();
	material_vec_.clear();
	volume_path_vec_.clear();
	for (auto path : paths) {
		vtkSmartPointer<vtkSTLReader> reader =
			vtkSmartPointer<vtkSTLReader>::New();
		reader->SetFileName(path.c_str());
		reader->Update();
		polyData_vec_.push_back(reader->GetOutput());
		material_vec_.push_back(0);
		material_name_vec_.push_back("金属");
		volume_path_vec_.push_back(path);
	}
	return true;
}

bool RadomeSTL::Update(std::string* err_msg)
{
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
		index++;
		actor->GetProperty()->SetOpacity(0.4);
		actor_vec_.push_back(actor);
	}
	UpdateTreeItem();
	return true;
}

void RadomeSTL::UpdateTreeItem()
{
	tree_item_ = new QTreeWidgetItem(QStringList(QString::fromLocal8Bit("旋转罩体")));
	tree_item_->setData(0, Qt::UserRole, QVariant(Def::model_type));
	int num = 1;

	for (int i = 0; i < material_name_vec_.size(); i++) {
		const auto& material_name = material_name_vec_[i];
		const auto& volume_path = volume_path_vec_[i];
		QTreeWidgetItem *child_tmp = new QTreeWidgetItem(
			QStringList(QString::fromLocal8Bit("天线罩层 ") + QString::number(num)));
		child_tmp->setData(0, Qt::UserRole, QVariant(Def::rotate_layer_type));
		child_tmp->setData(1, Qt::UserRole, QVariant(num-1));
		tree_item_->addChild(child_tmp);
		QTreeWidgetItem *child_eps;
		child_eps = new QTreeWidgetItem(
			QStringList(QString::fromLocal8Bit("材料=") + 
				QString::fromLocal8Bit(material_name.c_str())));
		child_tmp->addChild(child_eps);

		QTreeWidgetItem* child_path = new QTreeWidgetItem(
			QStringList(QString::fromLocal8Bit("路径=") +
				QString::fromLocal8Bit(volume_path.c_str())));
		child_path->setToolTip(0, QString::fromLocal8Bit("路径=") +
			QString::fromLocal8Bit(volume_path.c_str()));
		child_tmp->addChild(child_path);
		child_tmp->setExpanded(true);
		num++;
	}
	tree_item_->setExpanded(true);

}

//清空原来的tree_item_
void RadomeSTL::ClearTreeItem() {
	if (tree_item_) {
		delete tree_item_;
		tree_item_ = nullptr;
	}	
}
void RadomeSTL::Mesh(const MeshOption& option, int* step) {
}

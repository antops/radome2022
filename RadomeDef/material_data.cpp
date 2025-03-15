#include "material_data.h"
#include "../def.h"

MaterialData::MaterialData(const std::string& name, int index)
{
	name_ = name;
	index_ = index;
}

MaterialData::~MaterialData()
{
}

void MaterialData::SetMaterialData(double eps0, double mu0, double lossT)
{
	eps0_ = eps0;
	mu0_ = mu0;
	lossT_ = lossT;
}

void  MaterialData::Update() {
	tree_item_ = new QTreeWidgetItem(QStringList(QString::fromLocal8Bit(name_.c_str())));
	tree_item_->setData(0, Qt::UserRole, QVariant(Def::material_type));
	tree_item_->setData(1, Qt::UserRole, QVariant(index_));

	QTreeWidgetItem *child_eps;
	child_eps = new QTreeWidgetItem(
		QStringList(QString::fromLocal8Bit("eps0  = ") + QString::number(eps0_)));
	QTreeWidgetItem *child_mu;
	child_mu = new QTreeWidgetItem(
		QStringList(QString::fromLocal8Bit("mu  = ") + QString::number(mu0_)));

	QTreeWidgetItem *child_loss;
	child_loss = new QTreeWidgetItem(
		QStringList(QString::fromLocal8Bit("lossT  = ") + QString::number(lossT_)));
	tree_item_->addChild(child_eps);
	// tree_item_->addChild(child_mu);
	tree_item_->addChild(child_loss);

	tree_item_->setExpanded(true);
}
//
//HexMesh::MaterialParam MaterialData::GetMaterialParam() const
//{
//	HexMesh::MaterialParam param;
//	param.eps0 = eps0_;
//	param.mu0 = mu0_;
//	return param;
//}

MaterialData* MaterialDataFactory::GetMaterialData(MaterialDataType type)
{
	MaterialData* data = nullptr;
	if (type == MaterialMetal) {
		data = new MaterialData("½éÖÊ0", MaterialMetal);
		data->SetMaterialData(3.15, 1.0, 0.005);
		data->Update();
	} else if (type == MaterialAir) {
       		data = new MaterialData("¿ÕÆø", MaterialAir);
		data->SetMaterialData(1.0, 1.0, 0.0);
		data->Update();
	}
	return data;
}

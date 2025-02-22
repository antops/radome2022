#include "mesh_option_widget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>


MeshOptionDialog::MeshOptionDialog(QWidget *parent)
{
	QVBoxLayout *layout = new QVBoxLayout(this);

	scale_combobox_ = new QComboBox();
	mesh_scale_label_ = new QLabel(QString::fromLocal8Bit("剖分精度"));;
	QHBoxLayout* hlayout = new QHBoxLayout;
	hlayout->addWidget(mesh_scale_label_);
	hlayout->addWidget(scale_combobox_);

	okBtn = new QPushButton(QString::fromLocal8Bit("确认"));

	layout->addLayout(hlayout);
	layout->addWidget(okBtn);

	scale_combobox_->addItem(QString::fromLocal8Bit("Coarse"));
	scale_combobox_->addItem(QString::fromLocal8Bit("Standard"));
	scale_combobox_->addItem(QString::fromLocal8Bit("Fine"));

	connect(okBtn, SIGNAL(clicked()), this, SLOT(on_ok_btn()));
}

MeshOptionDialog::~MeshOptionDialog()
{

}


int MeshOptionDialog::GetScale() const {
	return scale_combobox_->currentIndex();
}

void MeshOptionDialog::on_ok_btn() {
	accept();
}
#pragma once

#include <QtWidgets/QDialog>
#include <qcombobox.h>
#include <QLineEdit>
#include <QLabel>
#include <QHBoxLayout>
#include <QPushButton>
#include <QComboBox>

class MeshOptionDialog : public QDialog
{
	Q_OBJECT
public:
	MeshOptionDialog(QWidget *parent = 0);
	~MeshOptionDialog();

	int GetScale() const;
private slots:
    
	void on_ok_btn();

private:
	QLabel *mesh_scale_label_;
	QComboBox * scale_combobox_;
	QPushButton * okBtn;
	double fre;

};

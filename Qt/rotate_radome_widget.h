#pragma once

#include <map>
#include <QtWidgets/QDialog>
#include <QTabWidget>
#include <QGroupBox>
#include <QGridLayout>
#include <Qlabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCloseEvent>
#include <QTabWidget>
#include "qcustomplot.h"

#include "../RadomeDef/rotate_param/rotate_line_base.h"

class RotateRadomeWidget : public QDialog
{
	Q_OBJECT

public:
	RotateRadomeWidget(QWidget *parent = 0);
	~RotateRadomeWidget();

	std::vector<RotateLineBase*> GetLineData();
	void GetScale(double* x, double* y) {
		*x = x_scale_;
		*y = y_scale_;
	}

	void SetLineData(const std::vector<RotateLineBase*>& vec) {
		line_data_map_.clear();
		for (int i = 0; i < vec.size(); i++) {
			line_data_map_.emplace(i, vec[i]);
		}
	}

	double GetCut() { return cut_x_; }

signals:
	void UpdateRotateRadome();

private slots :
	void OnButtonAdd();
	void OnButtonDel();
	void OntreeWidgetLeftPressed(QTreeWidgetItem*, int);
	// 左键点击
	void OnLeftPressedLineTypeBoxChanged(int);

	void OnButtonShow();
	void OnButtonConform();
	void OnButtonOK();
	void OnButtonSave();
	void OnButtonLoad();

	void OnImportBt();

private:
	void InitCustomPlot();
	void InitLineTreeItem();
	void InitScaleParam();
	void InitLineParam();

	void UpdatePlot();
	void CreateStraightLine(int number);
	void CreateConicLine(int number);
	void CreateCircularStraightLine(int number);
	void CreateKarmanLine(int number);
	void CreateImportLine(int number);
	void CreateKarmanRadiusLine(int number);

	bool ReadData(std::vector<double>* data);
private:
	// 第一行
	QCustomPlot *custom_plot_;

	// 第二行
	QLabel * x_scale_lable_;
	QLineEdit * x_scale_edit_;
	QLabel * y_scale_lable_;
	QLineEdit * y_scale_edit_;
	QGroupBox * scale_box_;
	double x_scale_ = 1.0;
	double y_scale_ = 1.0;

	QTreeWidget * tree_widget_;
	QPushButton * add_line_btn_;
	QPushButton * del_line_btn_;

	QTreeWidgetItem* line_tree_item_;
	QLabel * line_sel_name_label_;
	QLabel * line_name_label_;

	QLabel * help_pic_label_;

	QTabWidget * tab_widget_;

	QWidget* line_para_widget_;
	QWidget* line_para_widget2_;
	QWidget* line_para_widget3_;
	QWidget* line_para_widget4_;
	QWidget* line_para_widget5_;
	QWidget* line_para_widget6_;
	//QGroupBox * line_param_box_;

	QLabel * line_type_label_;
	QComboBox* line_type_box_;

	QPushButton * undo_btn_;
	QPushButton * show_btn_;
	QPushButton * conform_btn_;
	QGroupBox * line_param_box_;

	// 第三行
	QPushButton * okbtn_;
	QPushButton * save_btn_;
	QPushButton * load_btn_;

	QLabel * sampling_label_;
	QLineEdit * sampling_edit_;

	// 直线参数 
	QLabel * y_lable_;
	QLineEdit * b_edit_;
	QLabel * x_lable_;
	QLineEdit * c_edit_;
	QLabel * range_lable_;
	QLineEdit * range_min_edit_;
	QLabel * range_lable_2_;
	QLineEdit * range_max_edit_;

	// 二次曲线参数 
	QLabel * conic_y_lable_;
	QLineEdit * conic_a_edit_;
	QLabel * conic_x2_lable_;
	QLineEdit * conic_b_edit_;
	QLabel * conic_x_lable_;
	QLineEdit * conic_c_edit_;
	QLabel * conic_range_lable_;
	QLineEdit * conic_range_min_edit_;
	QLabel * conic_range_lable_2_;
	QLineEdit * conic_range_max_edit_;

	// 圆头直线参数 
	QLabel * x0_lable_;
	QLineEdit * x0_edit_;
	QLabel * r1_lable_;
	QLineEdit * r1_edit_;
	QLabel * r2_lable_;
	QLineEdit * r2_edit_;
	QLabel * theta_lable_;
	QLineEdit * theta_edit_;

	// 冯卡门曲线
	QLabel * fen_a_lable_;
	QLineEdit * fen_a_edit_;
	QLabel * fen_b_lable_;
	QLineEdit * fen_b_edit_;
	QLabel * fen_c_lable_;
	QLineEdit * fen_c_edit_;
	QLabel * fen_x_lable_;
	QLineEdit * fen_x_edit_;

	// 冯卡门倒角曲线
	QLabel* fen_r_a_lable_;
	QLineEdit* fen_r_a_edit_;
	QLabel* fen_r_b_lable_;
	QLineEdit* fen_r_b_edit_;
	QLabel* fen_r_c_lable_;
	QLineEdit* fen_r_c_edit_;
	QLabel* fen_r_r_lable_;
	QLineEdit* fen_r_r_edit_;
	QLabel* fen_r_x_lable_;
	QLineEdit* fen_r_x_edit_;

	// 截断
	QLabel* cut_label_;
	QLineEdit* cut_edit_;


	// 导入
	QLabel * import_lable_;
	QLineEdit * import_edit_;
	QPushButton* import_bt_;

	QTreeWidgetItem * select_tree_ = NULL;

	int line_num_ = 0;
	int line_index_ = 0;
	std::map<int, RotateLineBase*> line_data_map_;

	double cut_x_ = 0.0;
};

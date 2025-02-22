#ifndef RADOME_H
#define RADOME_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMainWindow>
#include <QVTKWidget.h>
#include <QAction>
#include <QTreeWidget>
#include <QDockWidget>
#include <QLabel>
#include <QPushButton>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QstatusBar>
#include <QTreeWidgetItem>
#include <QButtonGroup>
#include <QRadioButton>
#include <QComboBox>
#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkAxesActor.h>
#include <vtkOrientationMarkerWidget.h>

#include "Qt/rotate_radome_widget.h"
#include "Qt/radome_param_detail_widget.h"
#include "Qt/material_param_detail_widget.h"
#include "Qt/aperture_field_widget.h"
#include "Qt/gaussian_widget.h"
#include "Qt/far_field_param_widget.h"
#include "Qt/process_show_widget.h"
#include "Qt/far_field_show_widget.h"
#include "Qt/taile_source_widget.h"
#include "Qt/taile_only_source_widget.h"
#include "Qt/far_field_show_v2_widget.h"

#include "data_manager.h"

class Radome : public QMainWindow
{
	Q_OBJECT

public:
	Radome(QWidget *parent = 0);
	~Radome();

private slots :
    void UpdateRotateRadome();
	void UpdateRotateRadomeParam(int);
	void UpdateMaterialRadomeParam(int);
	void DelMaterialRadomeParam(int);
	void OnTreeWidgetContextMenuRequested(QPoint pos);
	void OnTreeWidgetLeftPressed(QTreeWidgetItem * item, int column);

	void OnMeshAction();
	void OnSTLModelAction();
	void OnAddMaterialAction();
	void OnNewSourceAction();
	void OnNewTaileSourceAction();
	void OnNewTaileOnlySourceAction();
	void OnNewGaussianSourceAction();
	void OnNewCalcalteAction();
	void OnNewQuickCalcalteAction();
	void OnCalcalteFarFieldAction();
	void OnSaveFarfield();
	void UpdateProgress(int);
	void OnNewScanCalcAction();
	void OnNewGAOptAction();
	void OnChangeSource();

	void OnSaveAction();
	void OnLoadAction();

	void ToApertureField(int);
	void ToReceiveGaussian(int);
	void ToTaileSource(int);
	void ToTaileOnlySource(int);
	void UpdateMesh();
	void UpdateFarField();
	void FarFieldFinished(int);

private:
	void CreateMenus();
	void CreateRightMenu();
	void CreateActions();
	void CreateTreeWidgetItem();
	void UpdateVtk();

	bool ReadData(const std::string& path, std::vector<double>* data);

private:
	vtkSmartPointer<vtkOrientationMarkerWidget> widget1;
	QVTKWidget widget; // vtk 显示窗口
	QVTKWidget widgetRadiator;
	QTabWidget * tabWidget; // 主窗口

	vtkSmartPointer<vtkRenderWindowInteractor> interactor; // 交互
	vtkSmartPointer<vtkRenderer> renderer;
	vtkSmartPointer<vtkAxesActor> axes; // 坐标

	QDockWidget * leftWidget; //放置treeWidget的窗口
	QTreeWidget * tree_widget_;

	QDockWidget * detailsDockWidget;
	QWidget * detailsWidget;

	QLabel * locationLabel;  // 状态栏标签
	QPushButton * unitBtn;   // 单位按钮

	//----------- Menu ----------------- 
	QMenu * fileMenu;  //菜单栏
	QMenu * viewMenu;  //显示栏
	QMenu * eidtMenu;  //编辑栏
	QMenu * ModelMenu;  //模型栏
	QMenu * SourceMenu;  //源栏
	QMenu * CalMenu;  //计算栏
	QMenu *R_Tree_FarFieldMenu; // 右键菜单
	QMenu *R_BlankMenu;
	QMenu* R_Tree_SourceMenu; // 右键菜单

	//----------- Action ----------------- 
	 //文件菜单项
	QAction * saveFileAction;
	QAction * openFileAction;
	QAction * newFileAction;

	QAction * mesh_action_;
	QAction * stl_model_action_;
	QAction * add_material_action_;
	QAction * new_source_action_;
	QAction * new_gaussian_source_action_;
	QAction * new_taile_source_action_;
	QAction * new_taile_only_source_action_;
	QAction * change_source_action_;

	QAction * calcalte_action_;
	QAction * calcalte_quick_action_;
	QAction * scan_calc_action_;
	QAction * ga_action_; // 遗传算法

	QAction * calcalte_farfield_action_;

	QAction * save_farfield_action_;

	//文件菜单--view
	QAction * viewAction;  // 视角
	QLabel * viewLabel;
	QComboBox * viewComboBox;

	//----------- Tree ----------------- 
	QTreeWidgetItem * material_tree_item_;
	QTreeWidgetItem * normal_material_tree_item_;
	QTreeWidgetItem * custom_material_tree_item_;
	QTreeWidgetItem * definitions_tree_item_;
	QTreeWidgetItem * model_tree_item_;
	QTreeWidgetItem * source_tree_item_;
	QTreeWidgetItem * field_tree_item_;

	RotateRadomeWidget* rotate_radome_widget_;
	RadomeParamDatailWidget* radome_details_widget_;
	MaterialParamDatailWidget* material_details_widget_;
	ApertureFieldWidget* aperture_field_widget_ = nullptr;
	GaussianWidget* gaussian_widget_ = nullptr;
	FarFieldParamWidget* far_field_param_widget_ = nullptr;
	ProcessShowWidget* process_show_widget_ = nullptr;
	TaileSourceWidget* taile_source_widget_ = nullptr;
	TaileOnlySourceWidget* taile_only_source_widget_ = nullptr;

	DataManager data_manager_;

	// 进度条
	QProgressDialog *progress_dialog_;
	Mirror* temp_mirror_ = nullptr;
	FarFieldShowV2Widget* far_widget_;
	int selected_farfield_index_ = -1;

	int scan_widget_index_ = 1;

	int source_type_ = -1;
	bool is_source_window_open_ = false;
};


#endif // RADOME_H

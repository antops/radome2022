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
#include <QVTKOpenGLNativeWidget.h> 

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkColorTransferFunction.h>
#include <vtkLookupTable.h>
#include <vtkScalarBarActor.h>
#include <vtkPointData.h>
#include <vtkPolyDataNormals.h>
#include <vtkWindowedSincPolyDataFilter.h>
#include <vtkSurfaceReconstructionFilter.h>
#include <vtkContourFilter.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkDelaunay2D.h>
#include <vtkCamera.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkDoubleArray.h>
#include <vtkDelaunay3D.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkUnstructuredGrid.h> 
#include <vtkPCANormalEstimation.h> 
////��ʾ����0208
#include <vtkAxesActor.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkCaptionActor2D.h>
#include <vtkTextActor.h>
#include <vtkArrowSource.h>
////

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

////0114:�����±�ǩҳѡ��
#include "Qt/far_field_show_tab_widget.h"
////
#include "data_manager.h"
#include "Qt/graph_trans_widget.h"

////0217������Ŀ
#include <QSettings>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFileDialog>
#include <QFile>
#include <QByteArray>
#include "tools/save_input_project.h"
#include "VTK/aperture_field.h"
#include <QProcess>
////

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
	////0114:�����±�ǩҳѡ��
	void OnNewResult2DAction();
	void OnNewResult3DAction();
	void closeResultTab(int);
	////
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
	////0217��������Ŀ��������Ŀ
	void OnSaveProject();
	void onLoadProject();


private:
	void CreateMenus();
	void CreateRightMenu();
	void CreateActions();
	void CreateTreeWidgetItem();
	void UpdateVtk();

	bool ReadData(const std::string& path, std::vector<double>* data);

private:
	////0116:3D
	//QVTKWidget* widget_;
	QVTKOpenGLNativeWidget* widget_;
	
	////
	vtkSmartPointer<vtkOrientationMarkerWidget> widget1;
	QVTKWidget widget; // vtk ��ʾ����
	QVTKWidget widgetRadiator;
	QTabWidget * tabWidget; // ������

	vtkSmartPointer<vtkRenderWindowInteractor> interactor; // ����
	vtkSmartPointer<vtkRenderer> renderer;
	vtkSmartPointer<vtkAxesActor> axes; // ����

	QDockWidget * leftWidget; //����treeWidget�Ĵ���
	QTreeWidget * tree_widget_;

	QDockWidget * detailsDockWidget;
	QWidget * detailsWidget;

	QLabel * locationLabel;  // ״̬����ǩ
	QPushButton * unitBtn;   // ��λ��ť

	//----------- Menu ----------------- 
	QMenu * fileMenu;  //�˵���
	QMenu * viewMenu;  //��ʾ��
	QMenu * eidtMenu;  //�༭��
	QMenu * ModelMenu;  //ģ����
	QMenu * SourceMenu;  //Դ��
	QMenu * CalMenu;  //������
	////0114
    QMenu * ResultMenu;
	QMenu* ResultSubMenu;
	////
	QMenu *R_Tree_FarFieldMenu; // �Ҽ��˵�
	QMenu *R_BlankMenu;
	QMenu* R_Tree_SourceMenu; // �Ҽ��˵�

	//----------- Action ----------------- 
	//�ļ��˵���
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
	QAction * ga_action_; // �Ŵ��㷨
	////0114
	QAction * addresult_show_action;
	QAction * addresult_show2D_action;
	QAction * addresult_show3D_action;
	////

	QAction * calcalte_farfield_action_;

	QAction * save_farfield_action_;

	//�ļ��˵�--view
	QAction * viewAction;  // �ӽ�
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

	// ������
	QProgressDialog *progress_dialog_;
	Mirror* temp_mirror_ = nullptr;
	////0114:�����±�ǩҳѡ��
	QTabWidget * tabWidget_1;
	int pageIndex = 1;
	////
	int selected_farfield_index_ = -1;

	int scan_widget_index_ = 1;

	int source_type_ = -1;
	bool is_source_window_open_ = false;

	int calc_index_ = 1;


	////0116:3D
	std::map<int, int> data_index_map_;
	////
	////0211��ά�����tab
	FarFieldShowTab * far_widget_2;	
	////0213��ά�������
	QGroupBox* switch_box_;
	QButtonGroup* switch_group_button_unit_;
	QRadioButton* linear_button_unit_;
	QRadioButton* dB_button_unit_;
	bool is_dB_ = true;

	QGroupBox* basic_info_;

	// ab ���
	QGroupBox* ab_group_box_;
	QComboBox* combo_box_a_;
	QLabel* a_label_;
	QPushButton* ab_ok_btn_;
	// ����
	//QCustomPlot* custom_plot_;
	QGroupBox* plot_box_;

	QWidget* layout_main_widget;
	////0220
	std::vector<std::string> results_filesave;
	////
	////0221
	RadomeSTL* tmp_data;
	std::vector<double> data_;
	int N_width = 0;
	int M_depth = 0;
	double fre_save;
	PlaneMirror * planeMirror;
	////
	// ////0225:̩��Դ����
	// double taile_theta_ = 0.0;
	// double taile_phi = 0.0;
	// double taile_fre = 17;
	// double taile_radius = 0.075;
	// double taile_s11 = 30;
	// double taile_dx = 0.5;
	// double taile_dy = 0.5;
	// double taile_ds = 0.5;
	// int taile_meshN = 161;
	// std::string taile_result_path;
	// ////
	////0225:�����Դ�ı�־λ
	int source_save_flag = 0;
	////
	////0305
	std::vector<double> trans_rotate_para = std::vector<double>(9);
	////
>>>>>>> 99de62f (feat(save_import)：项目的保存与导兄1�7)
};
#endif // RADOME_H

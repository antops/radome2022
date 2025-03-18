#include "radome.h"

#include <thread>
#include <fstream>

#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkRenderWindow.h>
#include <vtkCaptionActor2D.h>
#include <vtkTextProperty.h>
#include <vtkProperty.h>
#include <vtkCamera.h>
#include <vtkOrientationMarkerWidget.h>
#include <QApplication>
#include <QMessageBox>
#include <QProgressDialog>
#include <qdebug.h>
#include <QThread>
#include <vtkCubeAxesActor.h>

#include "RadomeDef/radome_rotate.h"
#include "RadomeDef/radome_stl.h"
#include "def.h"
#include "Qt/mesh_option_widget.h"
#include "Qt/program_thread.h"
#include "VTK/plane_mirror.h"
#include "Calculation/python_executor.h"
#include "Qt/global_config.h"
#include "Qt/debug_log.h"
#include "Qt/quick_calc_thread.h"
#include "Qt/scan_param_widget.h"
#include "Qt/calc_config_widget.h"
#include "Qt/fdtd_calc_config_widget.h"
#include "Qt/ga_opt_param_widget.h"
#include "Qt/ga_opt_thread.h"
#include "Qt/fdtd_calc_thread.h"
#include "Qt/input_stl_widget.h"




Radome::Radome(QWidget* parent)
	: QMainWindow(parent)
{

	GlobalConfig::Instance()->Init(QCoreApplication::applicationDirPath().toStdString() +
		"/config/config.ini");
	setWindowTitle(QString::fromLocal8Bit("?????????????-????EML v1.0Beta"));
	tabWidget = new QTabWidget;
	tabWidget->addTab(&widget, QString::fromLocal8Bit("??????"));
	rotate_radome_widget_ = new RotateRadomeWidget;
	connect(rotate_radome_widget_, SIGNAL(UpdateRotateRadome()),
		this, SLOT(UpdateRotateRadome()));
	tabWidget->addTab(rotate_radome_widget_, QString::fromLocal8Bit("???????????????"));
	//tabWidget->setTabsClosable(true);
	////0114:?????±??????
	
	tabWidget_1 = new QTabWidget;
	tabWidget_1->setTabsClosable(true);
	// connect(tabWidget_1, SIGNAL(tabCloseRequested(int)), this, SLOT(closeResultTab(int)));
	connect(tabWidget_1, SIGNAL(tabCloseRequested(int)), this, SLOT(closeResultTab(int)));
	// QTabWidget * tabWidget_2 = new QTabWidget;
	////
	tabWidget->addTab(tabWidget_1, QString::fromLocal8Bit("??????"));
	setCentralWidget(tabWidget);

	//setCentralWidget(&widget);
	resize(1200, 800);

	double R = 1.0;
	double G = 1.0;
	double B = 1.0;
	renderer = vtkSmartPointer<vtkRenderer>::New();
	renderer->SetBackground(R, G, B);

	auto window = widget.GetRenderWindow();
	window->AddRenderer(renderer);
	double axesScale = 1000.0;
	// ?????vtk????
	axes = vtkSmartPointer<vtkAxesActor>::New();
	axes->GetXAxisShaftProperty()->SetColor(0, 0, 1);//???X???????????
	axes->GetXAxisTipProperty()->SetColor(0, 0, 1);
	axes->GetXAxisCaptionActor2D()->GetCaptionTextProperty()->SetColor(0, 0, 1);//???X???????????  
	axes->GetYAxisCaptionActor2D()->GetCaptionTextProperty()->SetColor(0, 2, 0);//???Y???????????  
	axes->GetZAxisCaptionActor2D()->GetCaptionTextProperty()->SetColor(1, 0, 0);//???Z???????????
	axes->GetZAxisShaftProperty()->SetColor(1, 0, 0);
	axes->GetZAxisTipProperty()->SetColor(1, 0, 0);

	axes->SetConeRadius(0.3);
	axes->SetConeResolution(20);
	//axes->SetTotalLength(10, 10, 10); //?????????

	interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	interactor->SetRenderWindow(window);

	auto style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
	interactor->SetInteractorStyle(style);
	interactor->Initialize();

	vtkCamera* aCamera = vtkCamera::New();
	aCamera->SetViewUp(0, 0, 1);//?????λ?? 
	aCamera->SetPosition(0, -3 * axesScale, 0);//???????λ
	aCamera->SetFocalPoint(0, 0, 0);//?轹?? 
	aCamera->ComputeViewPlaneNormal();//???
	renderer->SetActiveCamera(aCamera);

	auto axes_actor = vtkSmartPointer<vtkAxesActor>::New();
	axes_actor->SetCylinderRadius(0.001);
	axes_actor->SetConeRadius(0.1);
	axes_actor->GetXAxisShaftProperty()->SetColor(0, 0, 1);
	axes_actor->GetXAxisTipProperty()->SetColor(0, 0, 1);
	axes_actor->GetXAxisCaptionActor2D()->GetCaptionTextProperty()->SetColor(0, 0, 1);
	axes_actor->GetYAxisCaptionActor2D()->GetCaptionTextProperty()->SetColor(0, 1, 0);
	axes_actor->GetZAxisCaptionActor2D()->GetCaptionTextProperty()->SetColor(1, 0, 0);
	axes_actor->GetZAxisShaftProperty()->SetColor(1, 0, 0);
	axes_actor->GetZAxisTipProperty()->SetColor(1, 0, 0);
	//axes_actor->SetCone
	// axes_actor->GetXAxisShaftProperty().;
	renderer->AddActor(axes_actor);
	//renderer->SetBackground(0.1, 0.2, 0.4);
	renderer->ResetCamera();
	window->Render();

	widget1 = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
	widget1->SetOutlineColor(0.9300, 0.5700, 0.1300);
	widget1->SetOrientationMarker(axes);
	widget1->SetInteractor(interactor);
	widget1->SetViewport(0.0, 0.0, 0.25, 0.25);
	widget1->SetEnabled(1);
	widget1->InteractiveOff();

	// ????dock????
	leftWidget = new QDockWidget("Navigation", this);
	leftWidget->setFeatures(QDockWidget::DockWidgetMovable);
	leftWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	//leftWidget->setWindowFlags(Qt::FramelessWindowHint);
	addDockWidget(Qt::LeftDockWidgetArea, leftWidget);

	// treeWidget
	tree_widget_ = new QTreeWidget;
	tree_widget_->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(tree_widget_, SIGNAL(customContextMenuRequested(QPoint)),
		this, SLOT(OnTreeWidgetContextMenuRequested(QPoint)));
	connect(tree_widget_, SIGNAL(itemPressed(QTreeWidgetItem*, int)),
		this, SLOT(OnTreeWidgetLeftPressed(QTreeWidgetItem*, int)));
	tree_widget_->setHeaderHidden(true);  // ??????
	leftWidget->setWidget(tree_widget_);
	tree_widget_->setWordWrap(true);

	//RightWidget
	detailsDockWidget = new QDockWidget("Details", this);
	detailsDockWidget->setFeatures(QDockWidget::DockWidgetMovable);
	detailsDockWidget->setFeatures(QDockWidget::DockWidgetClosable);
	detailsDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	detailsDockWidget->close();
	//leftWidget->setWindowFlags(Qt::FramelessWindowHint);
	addDockWidget(Qt::LeftDockWidgetArea, detailsDockWidget);

	radome_details_widget_ = new RadomeParamDatailWidget();
	connect(radome_details_widget_, SIGNAL(UpdateRotateRadomeParam(int)),
		this, SLOT(UpdateRotateRadomeParam(int)));

	material_details_widget_ = new MaterialParamDatailWidget(true);
	connect(material_details_widget_, SIGNAL(UpdateMaterialRadomeParam(int)),
		this, SLOT(UpdateMaterialRadomeParam(int)));
	connect(material_details_widget_, SIGNAL(DelMaterialRadomeParam(int)),
		this, SLOT(DelMaterialRadomeParam(int)));

	far_field_param_widget_ = new FarFieldParamWidget;
	connect(far_field_param_widget_, SIGNAL(UpdateFarField()),
		this, SLOT(UpdateFarField()));

	CreateActions();
	CreateMenus();
	CreateRightMenu();
	CreateTreeWidgetItem();

}

Radome::~Radome()
{

}

void Radome::UpdateRotateRadome() {
	auto old_radome = data_manager_.GetRadomeData();
	if (old_radome) {
		QMessageBox::StandardButton rb =
			QMessageBox::question(NULL, "question", QString::fromLocal8Bit("??????????????????滻"),
				QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		if (rb == QMessageBox::No)
		{
			return;
		}
	}
	std::vector<RotateLineBase*> line_vec = rotate_radome_widget_->GetLineData();
	double x_scale, y_scale;
	rotate_radome_widget_->GetScale(&x_scale, &y_scale);
	RadomeRotate* tmp_radome_data = new RadomeRotate(line_vec, x_scale, y_scale, rotate_radome_widget_->GetCut());

	if (!tmp_radome_data->Update()) {
		delete tmp_radome_data;
		QMessageBox::warning(NULL, "Error", QString::fromLocal8Bit("?????????????"));
		return;
	}

	if (old_radome) {
		const auto& old_actors = old_radome->GetActors();
		for (const auto& actor : old_actors) {
			renderer->RemoveActor(actor);
		}
		model_tree_item_->removeChild(old_radome->GetTreeItem());
	}

	const auto& actors = tmp_radome_data->GetActors();
	for (const auto& actor : actors) {
		renderer->AddActor(actor);
	}
	auto tmp_tree = tmp_radome_data->GetTreeItem();
	model_tree_item_->addChild(tmp_tree);
	model_tree_item_->setExpanded(true);
	tmp_tree->setExpanded(true);
	for (int i = 0; i < tmp_tree->childCount(); i++) {
		tmp_tree->child(i)->setExpanded(true);
	}
	data_manager_.ResetRadome(tmp_radome_data);
	tabWidget->setCurrentIndex(0);
	renderer->ResetCamera();
	UpdateVtk();
}

void Radome::UpdateVtk()
{
	auto window = widget.GetRenderWindow();

	//window->AddRenderer(renderer);
	window->Render();
}

void Radome::CreateTreeWidgetItem() {
	definitions_tree_item_ = new QTreeWidgetItem(tree_widget_, QStringList(QString::fromLocal8Bit("????????")));
	definitions_tree_item_->setExpanded(true);

	QTreeWidgetItem* child1;
	child1 = new QTreeWidgetItem;
	child1->setText(0, tr("eps0") + tr(" = ") +
		tr("8.85418781761e-12"));
	child1->setTextColor(0, "gray");
	definitions_tree_item_->addChild(child1);

	QTreeWidgetItem* child2;
	child2 = new QTreeWidgetItem;
	child2->setText(0, tr("Pi") + tr(" = ") +
		tr("3.14159265358979"));
	child2->setTextColor(0, "gray");
	definitions_tree_item_->addChild(child2);

	QTreeWidgetItem* child3;
	child3 = new QTreeWidgetItem;
	child3->setText(0, tr("mu0") + tr(" = ") +
		tr("Pi*4e-7"));
	child3->setTextColor(0, "gray");
	definitions_tree_item_->addChild(child3);

	QTreeWidgetItem* child4;
	child4 = new QTreeWidgetItem;
	child4->setText(0, tr("c0") + tr(" = ") +
		tr("1/sqrt(eps0*mu0)"));
	child4->setTextColor(0, "gray");
	definitions_tree_item_->addChild(child4);

	QTreeWidgetItem* child5;
	child5 = new QTreeWidgetItem;
	child5->setText(0, tr("zf0") + tr(" = ") +
		tr("sqrt(mu0/eps0))"));
	child5->setTextColor(0, "gray");
	definitions_tree_item_->addChild(child5);

	material_tree_item_ = new QTreeWidgetItem(tree_widget_, QStringList(QString::fromLocal8Bit("????")));

	//data_manager_.SetMaterialData(MaterialDataFactory::MaterialAir,
	//	MaterialDataFactory::GetMaterialData(MaterialDataFactory::MaterialAir));
	//data_manager_.SetMaterialData(MaterialDataFactory::MaterialMetal,
	//	MaterialDataFactory::GetMaterialData(MaterialDataFactory::MaterialMetal));
	normal_material_tree_item_ = new QTreeWidgetItem(QStringList(QString::fromLocal8Bit("???ò???")));
	//normal_material_tree_item_->addChild(data_manager_.GetMaterialData(MaterialDataFactory::MaterialAir)->GetTreeItem());
	//normal_material_tree_item_->addChild(data_manager_.GetMaterialData(MaterialDataFactory::MaterialMetal)->GetTreeItem());
	// material_tree_item_->addChild(normal_material_tree_item_);

	custom_material_tree_item_ = new QTreeWidgetItem(QStringList(QString::fromLocal8Bit("????????")));
	material_tree_item_->addChild(custom_material_tree_item_);

	material_tree_item_->setExpanded(true);

	model_tree_item_ = new QTreeWidgetItem(tree_widget_, QStringList(QString::fromLocal8Bit("??????")));
	source_tree_item_ = new QTreeWidgetItem(tree_widget_, QStringList(QString::fromLocal8Bit("?")));
	field_tree_item_ = new QTreeWidgetItem(tree_widget_, QStringList(QString::fromLocal8Bit("????????")));
}

void Radome::CreateMenus()
{
	fileMenu = this->menuBar()->addMenu(QString::fromLocal8Bit("???"));

	saveFileAction = new QAction(QString::fromLocal8Bit("???????"), this);
	fileMenu->addAction(saveFileAction);
	saveFileAction->setShortcut(QKeySequence::Save);
	saveFileAction->setStatusTip(QString::fromLocal8Bit("???????"));
	connect(saveFileAction, SIGNAL(triggered()), this, SLOT(OnSaveProject()));

	openFileAction = new QAction(QString::fromLocal8Bit("???????"), this);
	fileMenu->addAction(openFileAction);
	openFileAction->setShortcut(QKeySequence::Open);
	openFileAction->setStatusTip(QString::fromLocal8Bit("???????"));
	connect(openFileAction, SIGNAL(triggered()), this, SLOT(onLoadProject()));

	// fileMenu->addAction(newFileAction);
	// newFileAction->setShortcut(QKeySequence::New);
	// newFileAction->setStatusTip(QString::fromLocal8Bit("???"));
	// connect(newFileAction, SIGNAL(triggered()), this, SLOT(OnNewFileAction()));

	//fileMenu->addAction(ExportAction);
	//fileMenu->addAction(ImportAction);
	//fileMenu->addSeparator();
	//fileMenu->addAction(LightSourceAction);

	// viewMenu = this->menuBar()->addMenu(QString::fromLocal8Bit("???"));
	//viewMenu->addAction(viewAction);

	eidtMenu = this->menuBar()->addMenu(QString::fromLocal8Bit("??"));
	eidtMenu->addAction(add_material_action_);
	ModelMenu = this->menuBar()->addMenu(QString::fromLocal8Bit("???"));
	ModelMenu->addAction(stl_model_action_);
	ModelMenu->addAction(mesh_action_);
	SourceMenu = this->menuBar()->addMenu(QString::fromLocal8Bit("?"));
	SourceMenu->addAction(new_gaussian_source_action_);
	SourceMenu->addAction(new_source_action_);
	SourceMenu->addAction(new_taile_source_action_);
	SourceMenu->addAction(new_taile_only_source_action_);

	CalMenu = this->menuBar()->addMenu(QString::fromLocal8Bit("????"));
	CalMenu->addAction(calcalte_action_);
	CalMenu->addAction(calcalte_quick_action_);
	CalMenu->addAction(ga_action_);
	CalMenu->addAction(scan_calc_action_);
	// CalMenu->addAction(calcalte_farfield_action_);

	////0114:?????±????? TODO:????????????????????tab??
	ResultSubMenu = new QMenu(QString::fromLocal8Bit("??????"), this);
	ResultMenu = this->menuBar()->addMenu(QString::fromLocal8Bit("???"));
	ResultMenu->addAction(addresult_show_action);
	ResultSubMenu->addAction(addresult_show2D_action);
	ResultSubMenu->addAction(addresult_show3D_action);

	// ??????????? addresult_show_action ??
	addresult_show_action->setMenu(ResultSubMenu);
	////
}

void Radome::CreateActions() {
	mesh_action_ = new QAction(QString::fromLocal8Bit("????"), this);
	connect(mesh_action_, SIGNAL(triggered()), this, SLOT(OnMeshAction()));

	stl_model_action_ = new QAction(QString::fromLocal8Bit("????stl???"), this);
	connect(stl_model_action_, SIGNAL(triggered()), this, SLOT(OnSTLModelAction()));

	add_material_action_ = new QAction(QString::fromLocal8Bit("???????"), this);
	connect(add_material_action_, SIGNAL(triggered()), this, SLOT(OnAddMaterialAction()));

	new_source_action_ = new QAction(QString::fromLocal8Bit("?????"), this);
	connect(new_source_action_, SIGNAL(triggered()), this, SLOT(OnNewSourceAction()));

	new_taile_source_action_ = new QAction(QString::fromLocal8Bit("????"), this);
	connect(new_taile_source_action_, SIGNAL(triggered()), this, SLOT(OnNewTaileSourceAction()));

	new_taile_only_source_action_ = new QAction(QString::fromLocal8Bit("????????"), this);
	connect(new_taile_only_source_action_, SIGNAL(triggered()), this, SLOT(OnNewTaileOnlySourceAction()));

	new_gaussian_source_action_ = new QAction(QString::fromLocal8Bit("????"), this);
	connect(new_gaussian_source_action_, SIGNAL(triggered()), this, SLOT(OnNewGaussianSourceAction()));

	calcalte_action_ = new QAction(QString::fromLocal8Bit("FDTD????"), this);
	connect(calcalte_action_, SIGNAL(triggered()), this, SLOT(OnNewCalcalteAction()));

	calcalte_quick_action_ = new QAction(QString::fromLocal8Bit("???????"), this);
	connect(calcalte_quick_action_, SIGNAL(triggered()), this, SLOT(OnNewQuickCalcalteAction()));

	ga_action_ = new QAction(QString::fromLocal8Bit("??????"), this);
	connect(ga_action_, SIGNAL(triggered()), this, SLOT(OnNewGAOptAction()));

	scan_calc_action_ = new QAction(QString::fromLocal8Bit("??????"), this);
	connect(scan_calc_action_, SIGNAL(triggered()), this, SLOT(OnNewScanCalcAction()));

	//calcalte_farfield_action_ = new QAction(QString::fromLocal8Bit("????????????"), this);
	//connect(calcalte_farfield_action_, SIGNAL(triggered()), this, SLOT(OnCalcalteFarFieldAction()));

	////0114:?????±????? 
	addresult_show_action = new QAction(QString::fromLocal8Bit("??????"), this);
	// connect(addresult_show_action, SIGNAL(triggered()), this, SLOT(OnNewResultAction()));

	addresult_show2D_action = new QAction(QString::fromLocal8Bit("??????"), this);
	connect(addresult_show2D_action, SIGNAL(triggered()), this, SLOT(OnNewResult2DAction()));

	addresult_show3D_action = new QAction(QString::fromLocal8Bit("??????"), this);
	connect(addresult_show3D_action, SIGNAL(triggered()), this, SLOT(OnNewResult3DAction()));
	////
}

void Radome::OnTreeWidgetContextMenuRequested(QPoint pos)
{

}

void Radome::OnTreeWidgetLeftPressed(QTreeWidgetItem* item, int column)
{
	if (data_manager_.GetRadomeData()) {
		data_manager_.GetRadomeData()->SetUnSelected(-1);
	}
	if (item->parent() == NULL) {
		// ?????
		detailsDockWidget->close();
		UpdateVtk();
		return;
	}
	if (qApp->mouseButtons() == Qt::LeftButton)
	{
		if (item->data(0, Qt::UserRole) == Def::rotate_layer_type)
		{
			int num = item->data(1, Qt::UserRole).toInt();
			data_manager_.GetRadomeData()->SetSelected(num);
			detailsDockWidget->setWidget(radome_details_widget_);
			auto param = data_manager_.GetRadomeData()->GetMaterialId(num);
			const auto& material_book = data_manager_.GetMaterialDataAll();

			radome_details_widget_->SetParam(num, param, material_book);
			detailsDockWidget->show();
		}
		else if (item->data(0, Qt::UserRole) == Def::material_type)
		{
			int num = item->data(1, Qt::UserRole).toInt();
			MaterialData* data = data_manager_.GetMaterialData(num);
			if (data == nullptr) return;
			material_details_widget_->SetParam(*data, num);
			detailsDockWidget->setWidget(material_details_widget_);
			detailsDockWidget->show();
		}
		else if (item->data(0, Qt::UserRole) == Def::farfield_type) {
			//detailsDockWidget->setWidget(far_field_param_widget_);
			//detailsDockWidget->show();
		}
		else if (item->data(0, Qt::UserRole) == Def::model_type)
		{
			data_manager_.GetRadomeData()->SetSelected(-1);
		}
		else {
			detailsDockWidget->close();
		}
	}
	else if (qApp->mouseButtons() == Qt::RightButton) {
		if (item->data(0, Qt::UserRole) == Def::farfield_type) {
			selected_farfield_index_ = item->data(1, Qt::UserRole).toInt();
			R_Tree_FarFieldMenu->exec(QCursor::pos());
		}
		else if (item->data(0, Qt::UserRole) == Def::gaussian_source_type) {
			source_type_ = Def::gaussian_source_type;
			R_Tree_SourceMenu->exec(QCursor::pos());
		}
		else if (item->data(0, Qt::UserRole) == Def::taile_source_type) {
			source_type_ = Def::taile_source_type;
			R_Tree_SourceMenu->exec(QCursor::pos());
		}
		else if (item->data(0, Qt::UserRole) == Def::aperture_source_type) {
			source_type_ = Def::aperture_source_type;
			R_Tree_SourceMenu->exec(QCursor::pos());
		}
		else if (item->data(0, Qt::UserRole) == Def::taile_only_source_type) {
			source_type_ = Def::taile_only_source_type;
			R_Tree_SourceMenu->exec(QCursor::pos());
		}
	}
	UpdateVtk();
}

void Radome::CreateRightMenu()
{
	R_Tree_FarFieldMenu = new QMenu(this);
	R_BlankMenu = new QMenu(this);

	save_farfield_action_ = new QAction(QString::fromLocal8Bit("???????"), this);
	// save_farfield_action_->setStatusTip(QString::fromLocal8Bit("?????STL"));
	connect(save_farfield_action_, SIGNAL(triggered()), this, SLOT(OnSaveFarfield()));

	R_Tree_FarFieldMenu->addAction(save_farfield_action_);
	

	R_Tree_SourceMenu = new QMenu(this);

	change_source_action_ = new QAction(QString::fromLocal8Bit("???"), this);
	connect(change_source_action_, SIGNAL(triggered()), this, SLOT(OnChangeSource()));
	R_Tree_SourceMenu->addAction(change_source_action_);
}

void Radome::UpdateRotateRadomeParam(int index) {
	auto mat_id = radome_details_widget_->GetParam();
	auto* mat = data_manager_.GetMaterialData(mat_id);
	if (mat == nullptr) {
		QMessageBox::warning(NULL, "Warning",
			QString::fromLocal8Bit("???ò??????"));
		return;
	}
	data_manager_.GetRadomeData()->SetMaterialId(index, mat_id, mat->GetName());
	model_tree_item_->removeChild(data_manager_.GetRadomeData()->GetTreeItem());
	data_manager_.GetRadomeData()->UpdateTreeItem();
	auto tmp_tree = data_manager_.GetRadomeData()->GetTreeItem();
	model_tree_item_->addChild(tmp_tree);
	model_tree_item_->setExpanded(true);
	tmp_tree->setExpanded(true);
	for (int i = 0; i < tmp_tree->childCount(); i++) {
		tmp_tree->child(i)->setExpanded(true);
	}
}

void Radome::UpdateMaterialRadomeParam(int index) {
	auto data = data_manager_.GetMaterialData(index);
	if (data == nullptr) return;
	QTreeWidgetItem* tree = nullptr;
	if (index < 100) tree = normal_material_tree_item_;
	else tree = custom_material_tree_item_;
	tree->removeChild(data->GetTreeItem());
	data->SetMaterialData(material_details_widget_->GetEps0(),
		material_details_widget_->GetMu0(),
		material_details_widget_->GetlossT());
	data->Update();
	auto tmp_tree = data->GetTreeItem();
	tree->addChild(tmp_tree);
	tree->setExpanded(true);
	tmp_tree->setExpanded(true);
	for (int i = 0; i < tmp_tree->childCount(); i++) {
		tmp_tree->child(i)->setExpanded(true);
	}
	detailsDockWidget->close();
}

void Radome::OnMeshAction() {
	auto radome_data = data_manager_.GetRadomeData();
	if (radome_data == NULL) return;
	if (radome_data->IsMesh()) {
		QMessageBox::StandardButton rb =
			QMessageBox::question(NULL, "question", QString::fromLocal8Bit("????????????????????????"),
				QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		if (rb == QMessageBox::No)
		{
			return;
		}
	}
	int* step = new int(1);
	Vector3 min_p, max_p;
	// ??????????????????????
	if (!radome_data->GetMaxXYZ(&min_p, &max_p)) {
		return;
	}
	MeshOption option;
	option.max_p = max_p;
	option.min_p = min_p;
	option.scale = 0.01;
	if (data_manager_.GetLimitBox()) {
		renderer->RemoveActor(data_manager_.GetLimitBox()->GetActor());
	}
	LimitBox* limit_box = new LimitBox;
	limit_box->SetParam(min_p, max_p);
	data_manager_.ResetLimitBox(limit_box);
	radome_data->Mesh(option, step);
	renderer->AddActor(limit_box->GetActor());
	renderer->AddActor(radome_data->GetMeshActor());
	UpdateVtk();

	/*
	MeshOptionDialog mesh_option;
	if (mesh_option.exec() != QDialog::Accepted) {
		return;
	}

	progress_dialog_ = new QProgressDialog(this);

	progress_dialog_->setWindowModality(Qt::WindowModal);
	progress_dialog_->setMinimumDuration(5);
	progress_dialog_->setWindowTitle(tr("Please Wait"));
	progress_dialog_->setLabelText(tr("Mesh..."));
	progress_dialog_->setCancelButtonText(tr("Cancel"));
	progress_dialog_->setRange(0, 100);

	ProgramThreadFunc* progress_thread = new ProgramThreadFunc;
	progress_thread->SetFunc([&]() {radome_data->Mesh(0.05, step); });
	connect(progress_thread, SIGNAL(RunDone()), this, SLOT(UpdateMesh()));
	progress_thread->start();*/
	//ProgramThread * thread2 = new ProgramThread;
	//connect(thread2, SIGNAL(UpdateProgress(int)), this, SLOT(UpdateProgress(int)));
	//thread2->SetParam(step);
	//thread2->start();
}

void Radome::OnSTLModelAction()
{
	InputStlNumberWidget tmp_widget;
	if (tmp_widget.exec() != QDialog::Accepted) {
		return;
	}
	if (tmp_widget.GetNumber() <= 0) return;
	if (tmp_widget.GetNumber() > 10) {
		return;
	}
	InputStlWidget stl_widget(tmp_widget.GetNumber());
	if (stl_widget.exec() != QDialog::Accepted) {
		return;
	}
	auto old_radome = data_manager_.GetRadomeData();
	if (old_radome) {
		QMessageBox::StandardButton rb =
			QMessageBox::question(NULL, "question", QString::fromLocal8Bit("??????????????????滻"),
				QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		if (rb == QMessageBox::No)
		{
			return;
		}
	}
	if (old_radome) {
		const auto& old_actors = old_radome->GetActors();
		for (const auto& actor : old_actors) {
			renderer->RemoveActor(actor);
		}
		model_tree_item_->removeChild(old_radome->GetTreeItem());
	}
	auto results = stl_widget.GetResults();
	//??results?????results_filesave
	results_filesave = results;
	RadomeSTL* tmp_data = new RadomeSTL();
	tmp_data->ReadSTLFromFile(results);
	tmp_data->Update();
	tmp_data->SetSurfacePathVec(stl_widget.GetSurfaceResults());
	const auto& actors = tmp_data->GetActors();
	for (const auto& actor : actors) {
		renderer->AddActor(actor);
	}

	auto tmp_tree = tmp_data->GetTreeItem();
	model_tree_item_->addChild(tmp_tree);
	model_tree_item_->setExpanded(true);
	tmp_tree->setExpanded(true);
	for (int i = 0; i < tmp_tree->childCount(); i++) {
		tmp_tree->child(i)->setExpanded(true);
	}
	data_manager_.ResetRadome(tmp_data);
	UpdateVtk();
}

void Radome::UpdateProgress(int num) {
	progress_dialog_->setValue(num);
}

void Radome::UpdateMesh() {
	auto radome_data = data_manager_.GetRadomeData();
	if (radome_data == NULL) return;
	renderer->AddActor(radome_data->GetMeshActor());
	UpdateVtk();
}

void Radome::UpdateFarField()
{
	auto far_field = data_manager_.GetFarField(0);
	if (far_field == nullptr) return;
	far_field->SetFarFieldParam(far_field_param_widget_->GetFarFieldParam());
	renderer->RemoveActor(far_field->GetActor());

	far_field->CreateActor3D();
	renderer->AddActor(far_field->GetActor());

	UpdateVtk();
}

void Radome::OnAddMaterialAction() {
	MaterialParamDatailWidget widget(false, this);
	if (widget.exec() != QDialog::Accepted) {
		return;
	}
	int index = data_manager_.GetMaterialId();
	MaterialData* data = new MaterialData("????_" + std::to_string(index - 99), index);
	*data = widget.GetData();
	data->SetIndex(index);
	data->Update();
	data_manager_.SetMaterialData(index, data);
	auto tmp_tree = data->GetTreeItem();
	custom_material_tree_item_->addChild(tmp_tree);
	custom_material_tree_item_->setExpanded(true);
	tmp_tree->setExpanded(true);
	for (int i = 0; i < tmp_tree->childCount(); i++) {
		tmp_tree->child(i)->setExpanded(true);
	}
}

void Radome::OnNewSourceAction()
{
	if (is_source_window_open_)
	{
		// ????д??????
		QMessageBox::warning(NULL, "Warning",
			"A window has been opened. Please close and continue!");

		return;
	}

	temp_mirror_ = new PlaneMirror(GraphTrans());
	temp_mirror_->setSelected(true);
	renderer->AddActor(temp_mirror_->getActor());
	aperture_field_widget_ = new ApertureFieldWidget;
	aperture_field_widget_->setMirror(temp_mirror_);

	aperture_field_widget_->setWindowFlags(Qt::WindowStaysOnTopHint); // ???????????
	aperture_field_widget_->show();
	is_source_window_open_ = true;
	connect(aperture_field_widget_, SIGNAL(sendData(int)),
		this, SLOT(ToApertureField(int)));
}

void Radome::OnNewTaileSourceAction() {
	if (is_source_window_open_)
	{
		// ????д??????
		QMessageBox::warning(NULL, "Warning",
			"A window has been opened. Please close and continue!");

		return;
	}

	temp_mirror_ = new PlaneMirror(GraphTrans());
	temp_mirror_->setSelected(true);
	renderer->AddActor(temp_mirror_->getActor());
	taile_source_widget_ = new TaileSourceWidget;
	taile_source_widget_->setMirror(temp_mirror_);
	taile_source_widget_->setWindowFlags(Qt::WindowStaysOnTopHint); // ???????????
	taile_source_widget_->show();
	is_source_window_open_ = true;
	connect(taile_source_widget_, SIGNAL(sendData(int)),
		this, SLOT(ToTaileSource(int)));
}

void Radome::ToTaileSource(int caseIndex)
{
	if (1 == caseIndex)
	{
		if (nullptr != data_manager_.GetSource()) // ?????????? ???????????
		{
			taile_source_widget_->setHidden(true);
			// ?ж???????????????????
			switch (QMessageBox::question(this, tr("Question"),
				QString::fromLocal8Bit("?????????????滻?"),
				QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok))
			{
			case QMessageBox::Ok:
				renderer->RemoveActor(data_manager_.GetSource()->getActor());
				renderer->RemoveActor(data_manager_.GetSource()->getActor3D());
				source_tree_item_->removeChild(source_tree_item_->child(0));
				break;
			case QMessageBox::Cancel:
				ToTaileSource(0);
				return;
			default:
				break;
			}
		}
		Field* temPtr;
		if (!taile_source_widget_->getField(temPtr))
		{
			taile_source_widget_->setHidden(true);
			QMessageBox::warning(NULL, "Warning",
				QString::fromLocal8Bit("???????????????"));
			ToTaileSource(0);
			return;
		}
		renderer->AddActor(temPtr->getActor());
		auto tree_ptr = temPtr->getTree();
		tree_ptr->setData(0, Qt::UserRole, QVariant(Def::taile_source_type));

		source_tree_item_->addChild(tree_ptr);
		source_tree_item_->setExpanded(true);
		for (int i = 0; i < source_tree_item_->childCount(); i++) {
			source_tree_item_->child(i)->setExpanded(true);
		}
		data_manager_.SetSource(temPtr);
		////0303:???浼?????para
        //????data??д??meta.json
		data_.resize(7);
		data_[0] = 1.0;
		data_[1] = 1.0;
		data_[2] = 1.0;
		data_[3] = 0.0;
		data_[4] = 2.0;
		data_[5] = 1.0;
		data_[6] = 0.0;
		M_depth = taile_source_widget_->getM();
	    N_width = taile_source_widget_->getN();
		fre_save = temPtr->GetFre();
		//???TransRotate????
		taile_source_widget_->GetTransRotate(trans_rotate_para);
        ////
		ToTaileSource(0);
	}
	else if (0 == caseIndex)// ??????
	{
		renderer->RemoveActor(temp_mirror_->getActor());
		//delete taile_source_widget_;
		//taile_source_widget_ = nullptr;
		//delete temp_mirror_;
		//temp_mirror_ = nullptr;
		is_source_window_open_ = false;
		taile_source_widget_->close();
	}


	UpdateVtk();
}

void Radome::OnNewTaileOnlySourceAction() {
	if (is_source_window_open_)
	{
		// ????д??????
		QMessageBox::warning(NULL, "Warning",
			"A window has been opened. Please close and continue!");

		return;
	}

	temp_mirror_ = new PlaneMirror(GraphTrans());
	temp_mirror_->setSelected(true);
	renderer->AddActor(temp_mirror_->getActor());
	taile_only_source_widget_ = new TaileOnlySourceWidget;
	taile_only_source_widget_->setMirror(temp_mirror_);
	taile_only_source_widget_->setWindowFlags(Qt::WindowStaysOnTopHint); // ???????????
	taile_only_source_widget_->show();
	is_source_window_open_ = true;
	connect(taile_only_source_widget_, SIGNAL(sendData(int)), this, SLOT(ToTaileOnlySource(int)));
}

void Radome::ToTaileOnlySource(int caseIndex)
{
	if (1 == caseIndex)
	{
		if (nullptr != data_manager_.GetSource()) // ?????????? ???????????
		{
			taile_only_source_widget_->setHidden(true);
			// ?ж???????????????????
			switch (QMessageBox::question(this, tr("Question"),
				QString::fromLocal8Bit("?????????????滻?"),
				QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok))
			{
			case QMessageBox::Ok:
				renderer->RemoveActor(data_manager_.GetSource()->getActor());
				renderer->RemoveActor(data_manager_.GetSource()->getActor3D());
				source_tree_item_->removeChild(source_tree_item_->child(0));
				break;
			case QMessageBox::Cancel:
				ToTaileOnlySource(0);
				return;
			default:
				break;
			}
		}
		Field* temPtr;
		if (!taile_only_source_widget_->getField(temPtr))
		{
			taile_only_source_widget_->setHidden(true);
			QMessageBox::warning(NULL, "Warning",
				QString::fromLocal8Bit("???????????????"));
			ToTaileOnlySource(0);
			return;
		}
		renderer->AddActor(temPtr->getActor());
		auto tree_ptr = temPtr->getTree();
		tree_ptr->setData(0, Qt::UserRole, QVariant(Def::taile_only_source_type));

		source_tree_item_->addChild(tree_ptr);
		source_tree_item_->setExpanded(true);
		for (int i = 0; i < source_tree_item_->childCount(); i++) {
			source_tree_item_->child(i)->setExpanded(true);
		}
		data_manager_.SetSource(temPtr);
		////0303:???浼?????para
        // taile_only_source_widget_->getParameter(data);
        //????data??д??meta.json
		data_.resize(7);
		data_[0] = 1.0;
		data_[1] = 1.0;
		data_[2] = 1.0;
		data_[3] = 0.0;
		data_[4] = 2.0;
		data_[5] = 1.0;
		data_[6] = 0.0;
		M_depth = taile_only_source_widget_->getM();
	    N_width = taile_only_source_widget_->getN();
		fre_save = temPtr->GetFre();
		//???TransRotate????
		taile_only_source_widget_->GetTransRotate(trans_rotate_para);
        ////
		ToTaileOnlySource(0);
	}
	else if (0 == caseIndex)// ??????
	{
		renderer->RemoveActor(temp_mirror_->getActor());;
		is_source_window_open_ = false;
		taile_only_source_widget_->close();
	}


	UpdateVtk();
}

void Radome::ToApertureField(int caseIndex)
{
	if (1 == caseIndex)
	{
		if (nullptr != data_manager_.GetSource()) // ?????????? ???????????
		{
			aperture_field_widget_->setHidden(true);
			// ?ж???????????????????
			switch (QMessageBox::question(this, tr("Question"),
				QString::fromLocal8Bit("?????????????滻?"),
				QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok))
			{
			case QMessageBox::Ok:
				renderer->RemoveActor(data_manager_.GetSource()->getActor());
				renderer->RemoveActor(data_manager_.GetSource()->getActor3D());
				source_tree_item_->removeChild(source_tree_item_->child(0));
				break;
			case QMessageBox::Cancel:
				return;
			default:
				break;
			}
		}
		Field* temPtr;
		if (!aperture_field_widget_->getField(temPtr))
		{
			aperture_field_widget_->setHidden(true);
			QMessageBox::warning(NULL, "Warning",
				QString::fromLocal8Bit("???????????????"));
			ToApertureField(0);
			return;
		}
		renderer->AddActor(temPtr->getActor());
		auto tree_ptr = temPtr->getTree();
		tree_ptr->setData(0, Qt::UserRole, QVariant(Def::aperture_source_type));

		source_tree_item_->addChild(tree_ptr);
		source_tree_item_->setExpanded(true);
		for (int i = 0; i < source_tree_item_->childCount(); i++) {
			source_tree_item_->child(i)->setExpanded(true);
		}
		data_manager_.SetSource(temPtr);
		////0221:???浼?????para
        aperture_field_widget_->getParameter(data_);
		//???data
		for (auto& x : data_) {
		//???x
			qDebug() << "data" << x;
		}
		M_depth = aperture_field_widget_->getM();
	    N_width = aperture_field_widget_->getN();
		fre_save = temPtr->GetFre();
		//???TransRotate????
		aperture_field_widget_->GetTransRotate(trans_rotate_para);
        ////
		ToApertureField(0);
	}
	else if (0 == caseIndex)// ??????
	{
		renderer->RemoveActor(temp_mirror_->getActor());
		//delete aperture_field_widget_;
		//aperture_field_widget_ = nullptr;
		//delete temp_mirror_;
		//temp_mirror_ = nullptr;
		is_source_window_open_ = false;
		aperture_field_widget_->close();
	}

	UpdateVtk();
}

void Radome::OnNewGaussianSourceAction()
{
	if (is_source_window_open_)
	{
		// ????д??????
		QMessageBox::warning(NULL, "Warning",
			"A window has been opened. Please close and continue!");

		return;
	}

	temp_mirror_ = new PlaneMirror(GraphTrans());
	temp_mirror_->setSelected(true);
	renderer->AddActor(temp_mirror_->getActor());
	gaussian_widget_ = new GaussianWidget;
	gaussian_widget_->setMirror(temp_mirror_);
	gaussian_widget_->setWindowFlags(Qt::WindowStaysOnTopHint); // ???????????
	gaussian_widget_->show();
	is_source_window_open_ = true;
	connect(gaussian_widget_, SIGNAL(sendData(int)),
		this, SLOT(ToReceiveGaussian(int)));
}

void Radome::OnNewCalcalteAction()
{
	const auto& radome_data = data_manager_.GetRadomeData();

	auto source = data_manager_.GetSource();
	if (source == nullptr) {
		QMessageBox::warning(NULL, "Warning",
			QString::fromLocal8Bit("δ?????!"));

		return;
	}
	FDTDCalcConfigWidget widget(source->GetFre(), this);
	if (widget.exec() != QDialog::Accepted) {
		return;
	}

	qDebug("star OnNewQuickCalcalteAction");

	auto process_show_widget = new ProcessShowWidget;
	process_show_widget->SetWidgetText(QString::fromLocal8Bit("???????FDTD?????????????..."));
	process_show_widget->setWindowFlags(Qt::WindowStaysOnTopHint);
	process_show_widget->resize(800, 600);
	// process_show_widget_->SetTextBrowserHidden(true);
	process_show_widget->show();

	//bool is_calc_non_radome = widget.GetIsCalcNonrodome();
	double fre = source->GetFre();
	int mesh_type = widget.GetMeshType();
	std::string fre_str = QString::number(fre/1e9, 'g', 4).toStdString();

	FDTDCalcThread thread(data_manager_, process_show_widget);
	//thread.SetCalcNonRadome(is_calc_non_radome);
	thread.SetFre(fre);
	thread.SetMeshType(mesh_type);

	if (!GlobalConfig::Instance()->IsSkipQuickCalc()) {
		thread.start();

		while (!thread.isFinished()) {
			// this->processEvents();
			QCoreApplication::processEvents();
		}

		if (thread.GetStatus() == -100) {
			process_show_widget->close();
			QMessageBox::warning(NULL, "Warning",
				QString::fromLocal8Bit("δ?????!"));

			return;
		}
		else if (thread.GetStatus() == -200) {
			process_show_widget->close();
			QMessageBox::warning(NULL, "Warning",
				QString::fromLocal8Bit("δ???????!"));

			return;
		}
		else if (thread.GetStatus() == -201) {
			process_show_widget->close();
			QMessageBox::warning(NULL, "Warning",
				QString::fromLocal8Bit("???δ???ò???????!"));

			return;
		}
		else if (thread.GetStatus() == -300) {
			process_show_widget->close();
			QMessageBox::warning(NULL, "Warning",
				QString::fromLocal8Bit("??????????????"));
			return;
		}
		else if (thread.GetStatus() != 0) {
			process_show_widget->close();
			QMessageBox::warning(NULL, "Warning",
				QString::fromLocal8Bit("???????"));
			return;
		}
	}
	// ????QProcess?????????Python????·???????
	QProcess process;
	process_show_widget->QAppend(QString::fromLocal8Bit(" "));
	process_show_widget->QAppend(QString::fromLocal8Bit("??????????????..."));

	process_show_widget->QAppend(
		QCoreApplication::applicationDirPath() + "/output/fdtd/3DCore_2.1AVX.exe");
	process_show_widget->QAppend(
		std::string(thread.GetDirPath() + "/input.txt").c_str());


	std::string exe_file = std::string(QCoreApplication::applicationDirPath().toStdString()
		+ "/output/fdtd/3DCore_2.1AVX.exe");
	if (GlobalConfig::Instance()->IsSkipFDTDCalc()) exe_file = "";

	process.start(exe_file.c_str(), QStringList() << std::string(thread.GetDirPath() + "/input.txt").c_str());

	process_show_widget->SetProcess(&process);
	connect(&process, SIGNAL(readyReadStandardOutput()), process_show_widget, SLOT(OnReadoutput()));
	connect(&process, SIGNAL(readyReadStandardError()), process_show_widget, SLOT(OnReadError()));


	// ???????????????????????
	while (process.state() != QProcess::NotRunning) {
		if (process_show_widget->IsCanceled()) {
			process.kill();
			break;
		}
		QCoreApplication::processEvents();
	}
	bool is_success = true;

	std::ifstream ifs;

	//3????????????ж???????
	std::string result_path = thread.GetResultPath() + "/Farfield.txt";
	std::string non_radome_result_path = thread.GetResultPath() + "/Farfield.txt";

	// result_path = "C:/Users/46512/Desktop/Radome2022/x64/Debug/output/fdtd/fdtd_result/demo/Farfield.txt";
	if (!GlobalConfig::Instance()->GetFDTDResultPath().empty()) {
		// debug
		result_path = GlobalConfig::Instance()->GetFDTDResultPath();
	}

	// non_radome_result_path = "C:/Users/46512/Desktop/Radome2022/x64/Debug/output/fdtd/fdtd_result/demo/Farfield_inc.txt";
	if (!GlobalConfig::Instance()->GetFDTDNonRadomeReultPath().empty()) {
		non_radome_result_path = GlobalConfig::Instance()->GetFDTDNonRadomeReultPath();
	}

	ifs.open(result_path, ios::in);
	if (!ifs.is_open()) {
		is_success = false;
	}
	else {
		ifs.close();
	}
	ifs.open(non_radome_result_path, ios::in);
	if (!ifs.is_open()) {
		is_success = false;
	}
	else {
		ifs.close();
	}

	process_show_widget->QAppend(QString::fromLocal8Bit("=========================="));
	if (is_success) {
		process_show_widget->QAppend(QString::fromLocal8Bit("???????????????."));
	}
	else {
		process_show_widget->QAppend(QString::fromLocal8Bit("?????????????????????????????."));
	}
	process_show_widget->FinishProcess();
	while (true) {
		if (process_show_widget->IsCanceled()) {
			break;
		}
		QCoreApplication::processEvents();
	}
	process_show_widget->close();
	if (!is_success) {
		return;
	}
	FarField* far_field = new FarField("fdtd_farfield");


	qDebug((std::string("read fdtd: ") + result_path).c_str());
	if (!far_field->ReadDataFromFile(result_path)) {
		qDebug((std::string("read fdtd failed: ") + thread.GetResultPath()).c_str());
		QMessageBox::warning(NULL, "Warning",
			QString::fromLocal8Bit("???????????????"));
		return;
	}

	int index = data_manager_.SetFarSource(far_field);
	far_field->SetIndex(index);
	QDateTime data_time = QDateTime::currentDateTime();
	QString current_time = data_time.toString("yyyy_MM_dd_hh_mm_ss");
	far_field->SetName(std::string("FDTD???_f") + fre_str + "_" + current_time.toStdString());
	// far_field->SetPicFilePath(result_path + "/" + GlobalConfig::Instance()->GetFarfieldPicName());

	field_tree_item_->addChild(far_field->GetTree());
	field_tree_item_->setExpanded(true);

	// noradome
	FarField* non_far_field = new FarField("fdtd_non_radome_farfield");

	qDebug((std::string("read non fdtd: ") + non_radome_result_path).c_str());
	if (!non_far_field->ReadDataFromFile(non_radome_result_path)) {
		qDebug((std::string("read fdtd failed: ") + non_radome_result_path).c_str());
		QMessageBox::warning(NULL, "Warning",
			QString::fromLocal8Bit("???????????????????"));
		return;
	}
	int non_index = data_manager_.SetFarSource(non_far_field);
	non_far_field->SetIndex(non_index);
	non_far_field->SetName(std::string("FDTD??????_f") + fre_str
		+ "_" + current_time.toStdString());
	// non_far_field->SetPicFilePath(non_radome_result_path + "/" + GlobalConfig::Instance()->GetFarfieldPicName());

	field_tree_item_->addChild(non_far_field->GetTree());
	field_tree_item_->setExpanded(true);
	data_manager_.CalcSourceFarfieldDone();
	//process_show_widget_ = new ProcessShowWidget;
	//process_show_widget_->SetWidgetText(QString::fromLocal8Bit("???????FDTD???????????????..."));
	//process_show_widget_->setWindowFlags(Qt::WindowStaysOnTopHint);
	//process_show_widget_->resize(800, 600);
	//// process_show_widget_->SetTextBrowserHidden(true);
	//process_show_widget_->show();

	//QProcess process;
	//process_show_widget_->QAppend(QString::fromLocal8Bit(" "));
	//process_show_widget_->QAppend(QString::fromLocal8Bit("??????????????..."));

	//data_manager_.GetSource()->SaveFieldBase("pf2fffield");
	//process.start("./PF2FF.exe", QStringList());

	//process_show_widget_->SetProcess(&process);
	//connect(&process, SIGNAL(readyReadStandardOutput()), process_show_widget_, SLOT(OnReadoutput()));
	//connect(&process, SIGNAL(readyReadStandardError()), process_show_widget_, SLOT(OnReadError()));


	//// ???????????????????????
	//while (process.state() != QProcess::NotRunning) {
	//	if (process_show_widget_->IsCanceled()) {
	//		process.kill();
	//		break;
	//	}
	//	QCoreApplication::processEvents();
	//}
	tabWidget->setCurrentIndex(2);

	UpdateVtk();
}

void Radome::OnNewQuickCalcalteAction()
{
	auto source = data_manager_.GetSource();
	if (source == nullptr) {
		QMessageBox::warning(NULL, "Warning",
			QString::fromLocal8Bit("δ?????!"));
		return;
	}

	// ??????
	auto radome_data = data_manager_.GetRadomeData();
	if (radome_data == NULL) {
		QMessageBox::warning(NULL, "Warning",
			QString::fromLocal8Bit("δ???????!"));
		return;
	}

	// ??????????????
	const auto& materials_index = radome_data->GetAllMaterials();
	for (auto index : materials_index) {
		if (data_manager_.GetMaterialData(index) == nullptr) {
			QMessageBox::warning(NULL, "Warning",
				QString::fromLocal8Bit("δ???ò???!"));
			return;
		}
	}
	CalcConfigWidget widget(source->GetFre(), this);
	if (widget.exec() != QDialog::Accepted) {
		return;
	}

	qDebug("star OnNewQuickCalcalteAction");

	process_show_widget_ = new ProcessShowWidget;
	process_show_widget_->SetWidgetText(QString::fromLocal8Bit("??????п?????????????????..."));
	process_show_widget_->setWindowFlags(Qt::WindowStaysOnTopHint);
	process_show_widget_->resize(800, 600);
	// process_show_widget_->SetTextBrowserHidden(true);
	process_show_widget_->show();

	bool is_calc_non_radome = widget.GetIsCalcNonrodome();
	
	CalcConf conf = widget.GetCalcConf();
	double fre = source->GetFre();
	int polarization_type = widget.GetPolarizationType();
	std::string fre_str = QString::number(fre/ 1e9, 'g', 4).toStdString();

	QuickCalcThread thread(data_manager_, process_show_widget_);
	thread.SetCalcNonRadome(is_calc_non_radome);
	thread.SetFre(fre);
	thread.SetCalcConf(conf);
	thread.SetPolarizationType(source->GetPolarizationType());
	thread.SetCustomPath(widget.GetCustomPath());

	if (!GlobalConfig::Instance()->IsSkipQuickCalc()) {
		thread.start();

		while (!thread.isFinished()) {
			// this->processEvents();
			QCoreApplication::processEvents();
		}

		if (thread.GetStatus() == -100) {
			process_show_widget_->close();
			QMessageBox::warning(NULL, "Warning",
				QString::fromLocal8Bit("δ?????!"));

			return;
		}
		else if (thread.GetStatus() == -200) {
			process_show_widget_->close();
			QMessageBox::warning(NULL, "Warning",
				QString::fromLocal8Bit("δ???????!"));

			return;
		}
		else if (thread.GetStatus() == -201) {
			process_show_widget_->close();
			QMessageBox::warning(NULL, "Warning",
				QString::fromLocal8Bit("???δ???ò???????!"));

			return;
		}
		else if (thread.GetStatus() == -300) {
			process_show_widget_->close();
			QMessageBox::warning(NULL, "Warning",
				QString::fromLocal8Bit("??????????????"));
			return;
		}
		else if (thread.GetStatus() == -400) {
			process_show_widget_->close();
			QMessageBox::warning(NULL, "Warning",
				QString::fromLocal8Bit("????meta??????"));
			return;
		}
		else if (thread.GetStatus() != 0) {
			process_show_widget_->close();
			QMessageBox::warning(NULL, "Warning",
				QString::fromLocal8Bit("???????"));
			return;
		}
	}
	QTreeWidgetItem* tree_conf_main = new QTreeWidgetItem;
	tree_conf_main->setText(0, QString::fromLocal8Bit("快速计算结果") + QString::number(calc_index_++)+ QString::fromLocal8Bit(":"));
	QTreeWidgetItem* tree_conf_point = new QTreeWidgetItem;
	tree_conf_point->setText(0, QString::fromLocal8Bit("起始点:"));
	tree_conf_main->addChild(tree_conf_point);
	QTreeWidgetItem* tree_conf = new QTreeWidgetItem;
	tree_conf->setText(0, QString::fromLocal8Bit("theta:[") + QString::number(conf.min_theta) + "," + QString::number(conf.max_theta) + "],"
		+ QString::fromLocal8Bit("num:") + QString::number(conf.num_theta));
	tree_conf_point->addChild(tree_conf);
	tree_conf = new QTreeWidgetItem;
	tree_conf->setText(0, QString::fromLocal8Bit("phi:[") + QString::number(conf.min_phi) + "," + QString::number(conf.max_phi) + "],"
		+ QString::fromLocal8Bit("num:") + QString::number(conf.num_phi));
	tree_conf_point->addChild(tree_conf);
	tree_conf = new QTreeWidgetItem;
	tree_conf->setText(0, QString::fromLocal8Bit("保存路径:") + QString::fromLocal8Bit(thread.GetResultPath().c_str()));
	tree_conf->setToolTip(0, QString::fromLocal8Bit("保存路径:") + QString::fromLocal8Bit(thread.GetResultPath().c_str()));
	tree_conf_main->addChild(tree_conf);
	tree_conf_main->addChild(tree_conf);

	field_tree_item_->addChild(tree_conf_main);
	field_tree_item_->setExpanded(true);
	tree_conf_main->setExpanded(true);
	tree_conf_point->setExpanded(true);

	//// ????
	if (!GlobalConfig::Instance()->IsSkipQuickCalcMetaGen())  {

		// ????QProcess?????????Python????·???????
		QProcess process;
		process_show_widget_->QAppend(QString::fromLocal8Bit(" "));
		process_show_widget_->QAppend(QString::fromLocal8Bit("??????????????..."));

		process_show_widget_->QAppend(
			QCoreApplication::applicationDirPath() + "/script/quick_calc_py/mainforNlayer.py");
			//???QCoreApplication::applicationDirPath()
			qDebug() << QCoreApplication::applicationDirPath();	
		process_show_widget_->QAppend(
			std::string(thread.GetDirPath() + "/meta.json").c_str());


		std::string exe_file = std::string(QCoreApplication::applicationDirPath().toStdString()
			+ "/script/quick_calc/mainforNlayer.exe ");
		if (GlobalConfig::Instance()->IsQuickCalcByPython()) {
			process.start("python ",
				QStringList() << QCoreApplication::applicationDirPath() + "/script/quick_calc_py/mainforNlayer.py"
				<< std::string(thread.GetDirPath() + "/meta.json").c_str());
		}
		else {
			process.start(exe_file.c_str(),
				QStringList() << std::string(thread.GetDirPath() + "/meta.json").c_str());
		}

		process_show_widget_->SetProcess(&process);
		connect(&process, SIGNAL(readyReadStandardOutput()), process_show_widget_, SLOT(OnReadoutput()));
		connect(&process, SIGNAL(readyReadStandardError()), process_show_widget_, SLOT(OnReadError()));


		// ???????????????????????
		while (process.state() != QProcess::NotRunning) {
			if (process_show_widget_->IsCanceled()) {
				process.kill();
				break;
			}
			QCoreApplication::processEvents();
		}
		bool is_success = false;

		std::ifstream ifs;

		//3????????????ж???????
		std::string status_file = thread.GetResultPath() + "/status.txt";
		ifs.open(status_file, ios::in);
		if (ifs.is_open()) {
			string temp;
			getline(ifs, temp);
			if (!temp.empty() && temp[0] == '1') {
				is_success = true;
			}
			ifs.close();
		}

		process_show_widget_->QAppend(QString::fromLocal8Bit("=========================="));
		if (is_success) {
			process_show_widget_->QAppend(QString::fromLocal8Bit("??????·??:")+ QString::fromLocal8Bit(thread.GetResultPath().c_str()));
			if (is_calc_non_radome) {
				process_show_widget_->QAppend(QString::fromLocal8Bit("??????·??(????):") + QString::fromLocal8Bit(thread.GetResultNonRadomePath().c_str()));
			}
			process_show_widget_->QAppend(QString::fromLocal8Bit("???????????????."));
		}
		else {
			process_show_widget_->QAppend(QString::fromLocal8Bit("?????????????????????????????."));
		}
		process_show_widget_->FinishProcess();
		while (true) {
			if (process_show_widget_->IsCanceled()) {
				break;
			}
			QCoreApplication::processEvents();
		}
		if (!is_success) {
			return;
		}
	}
	process_show_widget_->close();

	// ???
	FarField* far_field = new FarField("farfield");

	std::string result_path = thread.GetResultPath();
	if (!GlobalConfig::Instance()->GetResultPath().empty()) {
		// debug
		result_path = GlobalConfig::Instance()->GetResultPath();
	}
	qDebug((std::string("read npy: ") + result_path).c_str());
	if (!far_field->ReadFromNpy(result_path)) {
		qDebug((std::string("read npy failed: ") + thread.GetResultPath()).c_str());
		QMessageBox::warning(NULL, "Warning",
			QString::fromLocal8Bit("???????????????"));
		return;
	}

	int index = data_manager_.SetFarSource(far_field);
	far_field->SetIndex(index);
	QDateTime data_time = QDateTime::currentDateTime();
	QString current_time = data_time.toString("yyyy_MM_dd_hh_mm_ss");
	far_field->SetName(std::string("???_f") + fre_str  + "_" + current_time.toStdString());
	far_field->SetPicFilePath(result_path + "/" + GlobalConfig::Instance()->GetFarfieldPicName());

	auto* tree = far_field->GetTree();

	tree_conf_main->addChild(tree);
	tree_conf_main->setExpanded(true);
	for (int i = 0; i < tree_conf_main->childCount(); i++) {
		tree_conf_main->child(i)->setExpanded(true);
	}
	
	if (is_calc_non_radome) {
		FarField* non_far_field = new FarField("non_radome_farfield");
		std::string non_radome_result_path = thread.GetResultNonRadomePath();
		if (!GlobalConfig::Instance()->GetNonRadomeReultPath().empty()) {
			non_radome_result_path = GlobalConfig::Instance()->GetNonRadomeReultPath();
		}
		qDebug((std::string("read non npy: ") + non_radome_result_path).c_str());
		if (!non_far_field->ReadFromNpy(non_radome_result_path)) {
			qDebug((std::string("read npy failed: ") + thread.GetResultNonRadomePath()).c_str());
			QMessageBox::warning(NULL, "Warning",
				QString::fromLocal8Bit("???????????????????"));
			return;
		}
		int non_index = data_manager_.SetFarSource(non_far_field);
		non_far_field->SetIndex(non_index);
		non_far_field->SetName(std::string("??????_f") + fre_str
			+ "_" + current_time.toStdString());
		non_far_field->SetPicFilePath(non_radome_result_path + "/" + GlobalConfig::Instance()->GetFarfieldPicName());


		auto* non_far_tree = non_far_field->GetTree();

		tree_conf_main->addChild(non_far_tree);
		tree_conf_main->setExpanded(true);
		for (int i = 0; i < tree_conf_main->childCount(); i++) {
			tree_conf_main->child(i)->setExpanded(true);
		}

		data_manager_.CalcSourceFarfieldDone();
	}

	//far_widget_1->UpdateCombox();
	tabWidget->setCurrentIndex(2);

	UpdateVtk();
}

void Radome::OnCalcalteFarFieldAction()
{
	if (nullptr == data_manager_.GetSource()) {
		QMessageBox::warning(NULL, "Warning",
			QString::fromLocal8Bit("δ?????!"));
		return;
	}
	data_manager_.GetSource()->SaveFieldBase("pf2fffield");
	QProcess* process = new QProcess;
	QString file_path = "./PF2FF.exe";
	QStringList param;

	process_show_widget_ = new ProcessShowWidget;
	process_show_widget_->setWindowFlags(Qt::WindowStaysOnTopHint);
	process_show_widget_->show();
	process_show_widget_->SetProcess(process);

	connect(process, SIGNAL(readyReadStandardOutput()), process_show_widget_, SLOT(OnReadoutput()));
	connect(process, SIGNAL(readyReadStandardError()), process_show_widget_, SLOT(OnReadError()));
	connect(process, SIGNAL(finished(int)), this, SLOT(FarFieldFinished(int)));

	process->start(file_path, param);
}

void Radome::ToReceiveGaussian(int caseIndex)
{
	if (1 == caseIndex)
	{
		if (nullptr != data_manager_.GetSource()) // ?????????? ???????????
		{
			gaussian_widget_->setHidden(true);
			// ?ж???????????????????
			switch (QMessageBox::question(this, tr("Question"),
				QString::fromLocal8Bit("?????????????滻?"),
				QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok))
			{
			case QMessageBox::Ok:
				renderer->RemoveActor(data_manager_.GetSource()->getActor());
				renderer->RemoveActor(data_manager_.GetSource()->getActor3D());
				source_tree_item_->removeChild(source_tree_item_->child(0));
				break;
			case QMessageBox::Cancel:
				ToReceiveGaussian(0);
				return;
			default:
				break;
			}
		}
		Field* temPtr;
		if (!gaussian_widget_->getField(temPtr))
		{
			gaussian_widget_->setHidden(true);
			QMessageBox::warning(NULL, "Warning",
				QString::fromLocal8Bit("???????????????"));
			ToReceiveGaussian(0);
			return;
		}
		renderer->AddActor(temPtr->getActor());
		auto tree_ptr = temPtr->getTree();
		tree_ptr->setData(0, Qt::UserRole, QVariant(Def::gaussian_source_type));
		source_tree_item_->addChild(tree_ptr);
		source_tree_item_->setExpanded(true);
		for (int i = 0; i < source_tree_item_->childCount(); i++) {
			source_tree_item_->child(i)->setExpanded(true);
		}
		data_manager_.SetSource(temPtr);
		////0303:???浼?????para
        // gaussian_widget_->getParameter(data);
        //????data??д??meta.json
		data_.resize(7);
		data_[0] = 1.0;
		data_[1] = 1.0;
		data_[2] = 1.0;
		data_[3] = 0.0;
		data_[4] = 2.0;
		data_[5] = 1.0;
		data_[6] = 0.0;
		M_depth = gaussian_widget_->getM();
	    N_width = gaussian_widget_->getN();
		fre_save = temPtr->GetFre();
		gaussian_widget_->GetTransRotate(trans_rotate_para);
        ////
		ToReceiveGaussian(0);
	}
	else if (0 == caseIndex)// ??????
	{
		renderer->RemoveActor(temp_mirror_->getActor());

		//delete temp_mirror_;
		//temp_mirror_ = nullptr;
		//delete gaussian_widget_;
		//gaussian_widget_ = nullptr;
		gaussian_widget_->close();
		is_source_window_open_ = false;
	}


	UpdateVtk();
}

void Radome::DelMaterialRadomeParam(int index) {
	auto data = data_manager_.GetMaterialData(index);
	if (data == nullptr) return;
	QTreeWidgetItem* tree = nullptr;
	if (index < 100) tree = normal_material_tree_item_;
	else tree = custom_material_tree_item_;
	tree->removeChild(data->GetTreeItem());
	data_manager_.RemoveMaterialData(index);
	detailsDockWidget->close();
}

void Radome::FarFieldFinished(int) {
	process_show_widget_->Append("load farfield.....");
	FarField* far_field = new FarField("farfield");
	//std::string file = "FarField_inc_ForRead.txt";
	std::string file = "FarField_ForRead.txt";
	//std::string file = "Farfield.txt";
	if (!far_field->ReadDataFromFile(file)) {
		delete process_show_widget_;
		QMessageBox::warning(NULL, "Warning",
			QString::fromLocal8Bit("???????????????"));
		return;
	}
	far_field->CreateActor3D();
	if (data_manager_.GetSourceFarField()) {
		renderer->RemoveActor(data_manager_.GetSourceFarField()->GetActor());
	}
	renderer->AddActor(far_field->GetActor());

	int index = data_manager_.SetSourceFarField(far_field);
	far_field->SetIndex(index);
	field_tree_item_->addChild(far_field->GetTree());
	field_tree_item_->setExpanded(true);
	delete process_show_widget_;

	UpdateVtk();
}

void Radome::OnSaveFarfield() {
	auto farfiled = data_manager_.GetFarField(selected_farfield_index_);
	if (farfiled == nullptr) {
		qDebug((std::string("not find farfield index:")+std::to_string(selected_farfield_index_)).c_str());
		QMessageBox::warning(NULL, "Warning",
			QString::fromLocal8Bit("?????????????????"));
		return;
	}
	QString filename = QFileDialog::getExistingDirectory(this,
		tr("Save the FarField"),
		"");
	if (filename.isEmpty())
	{
		return;
	}
	if (!farfiled->WriteDataToFile(filename.toStdString())) {
		QMessageBox::warning(NULL, "Warning",
			QString::fromLocal8Bit("???????????????????????·????????"));
		return;
	}
}

////0114:?????±??????
void Radome::OnNewResult2DAction() {
	QString tabName = QString::fromLocal8Bit("??????") + QString::number(pageIndex); // ???????????
	FarFieldShowV2Widget* far_widget_1 = new FarFieldShowV2Widget(&data_manager_);//???????//?????????//????????????????
	tabWidget_1->addTab(far_widget_1, tabName); // ?????????tabWidget_1
	pageIndex++; // ??????????
	//?????????
	tabWidget_1->setCurrentIndex(tabWidget_1->count() - 1);
	far_widget_1->UpdateCombox();
	tabWidget->setCurrentIndex(2);
}

void Radome::OnNewResult3DAction() {
	QString tabName = QString::fromLocal8Bit("??????") + QString::number(pageIndex); // ???????????
	pageIndex++; // ??????????

	far_widget_2 = new FarFieldShowTab(&data_manager_);
	tabWidget_1->addTab(far_widget_2, tabName);
	//?????????
	tabWidget_1->setCurrentIndex(tabWidget_1->count() - 1);
	far_widget_2->UpdateCombox();
	tabWidget->setCurrentIndex(2);
}


void Radome::closeResultTab(int index)
{
	// ????????????????
	QWidget* tab = tabWidget_1->widget(index);

	// ??tabWidget?????????
	tabWidget_1->removeTab(index);

	// ???????????
	delete tab;

	pageIndex--;
}
////
void Radome::OnNewScanCalcAction() {
	ScanParamWidget* widget = new ScanParamWidget(&data_manager_, scan_widget_index_);
	tabWidget->addTab(widget, QString::fromLocal8Bit("???") + QString::number(scan_widget_index_));
	tabWidget->setCurrentIndex(scan_widget_index_ + 2);
	scan_widget_index_++;
}

void Radome::OnNewGAOptAction() {
	auto source = data_manager_.GetSource();
	if (source == nullptr) {
		QMessageBox::warning(NULL, "Warning",
			QString::fromLocal8Bit("δ?????!"));

		return;
	}

	auto radome_data = data_manager_.GetRadomeData();
	if (radome_data == nullptr) {
		QMessageBox::warning(NULL, "Warning",
			QString::fromLocal8Bit("δ???????!"));

		return;
	}

	if (radome_data->GetType() != 1) {
		QMessageBox::warning(NULL, "Warning",
			QString::fromLocal8Bit("??????????????!"));

		return;
	}

	auto rotate_lines = ((RadomeRotate*)(radome_data))->GetRotateLineVec();
	for (auto line : rotate_lines) {
		if (line->GetLineType() != RotateLineBase::karman_line_radius_type) {
			QMessageBox::warning(NULL, "Warning",
				QString::fromLocal8Bit("??????????????!"));
			return;
		}
	}


	GaOptParamWidget widget(rotate_lines.size() - 1, this);
	if (widget.exec() != QDialog::Accepted) {
		return;
	}

	const auto& ga_params = widget.GetGaParams();

	auto process_show_widget = new ProcessShowWidget;
	process_show_widget->SetWidgetText(QString::fromLocal8Bit("???????????????????????..."));
	process_show_widget->setWindowFlags(Qt::WindowStaysOnTopHint);
	process_show_widget->resize(800, 600);
	// process_show_widget_->SetTextBrowserHidden(true);
	process_show_widget->show();

	GaOptcThread thread(data_manager_, process_show_widget, ga_params);
	double fre = 17;
	thread.SetFre(fre * 1e9);

	thread.start();

	while (!thread.isFinished()) {
		QCoreApplication::processEvents();
	}

	if (thread.GetStatus() == -100) {
		process_show_widget->close();
		QMessageBox::warning(NULL, "Warning",
			QString::fromLocal8Bit("δ?????!"));

		return;
	}
	else if (thread.GetStatus() == -200) {
		process_show_widget->close();
		QMessageBox::warning(NULL, "Warning",
			QString::fromLocal8Bit("δ???????!"));

		return;
	}
	else if (thread.GetStatus() == -201) {
		process_show_widget->close();
		QMessageBox::warning(NULL, "Warning",
			QString::fromLocal8Bit("???δ???ò???????!"));

		return;
	}
	else if (thread.GetStatus() == -300) {
		process_show_widget->close();
		QMessageBox::warning(NULL, "Warning",
			QString::fromLocal8Bit("??????????????"));
		return;
	}
	else if (thread.GetStatus() != 0) {
		process_show_widget->close();
		QMessageBox::warning(NULL, "Warning",
			QString::fromLocal8Bit("???????"));
		return;
	}


	// ????QProcess?????????Python????·???????
	QProcess process;
	process_show_widget->QAppend(QString::fromLocal8Bit(" "));
	process_show_widget->QAppend(QString::fromLocal8Bit("??????????????..."));

	process_show_widget->QAppend(
		QCoreApplication::applicationDirPath() + "/script/quick_calc_py/GA_clac.py");
	process_show_widget->QAppend(
		std::string(thread.GetDirPath() + "/meta_ga.json").c_str());

	process.start("python ",
		QStringList() << QCoreApplication::applicationDirPath() + "/script/quick_calc_py/GA_clac.py"
		<< std::string(thread.GetDirPath() + "/meta_ga.json").c_str());


	process_show_widget->SetProcess(&process);
	connect(&process, SIGNAL(readyReadStandardOutput()), process_show_widget, SLOT(OnReadoutput()));
	connect(&process, SIGNAL(readyReadStandardError()), process_show_widget, SLOT(OnReadError()));


	// ???????????????????????
	while (process.state() != QProcess::NotRunning) {
		if (process_show_widget->IsCanceled()) {
			process.kill();
			break;
		}
		QCoreApplication::processEvents();
	}
	bool is_success = false;

	std::ifstream ifs;

	//3????????????ж???????
	std::string status_file = thread.GetResultPath() + "/status.txt";
	ifs.open(status_file, ios::in);
	if (ifs.is_open()) {
		string temp;
		getline(ifs, temp);
		if (!temp.empty() && temp[0] == '1') {
			is_success = true;
		}
		ifs.close();
	}

	std::ifstream results_ifs;
	std::string results_file = thread.GetResultPath() + "/results.txt";
	results_ifs.open(results_file, ios::in);
	std::vector<double> d_lines;
	if (results_ifs.is_open()) {
		string temp;
		int lineNumber = 0;
		while (getline(results_ifs, temp)) { // ???ж?????  
			lineNumber++; // ?????к??к??1  

			// ?????????????  
			if (lineNumber % 2 == 1) {
				std::istringstream iss(temp); // ???istringstream???????????  
				double value;
				if (iss >> value) { // ??????????????double  
					d_lines.push_back(value); // ?????????????vector??  
				}
				else {
					is_success = false;
					QMessageBox::warning(NULL, "Warning",
						QString::fromLocal8Bit("?????????!"));
					return;
				}
			}
		}
		results_ifs.close();
	}
	if (d_lines.size() != rotate_lines.size() - 1) {
		is_success = false;
		QMessageBox::warning(NULL, "Warning",
			QString::fromLocal8Bit("????????!"));
		return;
	}

	process_show_widget->QAppend(QString::fromLocal8Bit("=========================="));
	if (is_success) {
		process_show_widget->QAppend(QString::fromLocal8Bit("???????????????????."));
	}
	else {
		process_show_widget->QAppend(QString::fromLocal8Bit("?????????????????????????????."));
	}

	if (!is_success) {
		return;
	}

	Json::Value js;
	js["version"] = 1;
	js["n"] = rotate_lines.size()-1;
	js["sampling"] = 1500;

	Json::Value& js_model_meta = js["model_meta"];
	for (int i = 1; i < rotate_lines.size(); i++) {
		Json::Value js_model;
		Json::Value& para_meta = js_model["para"];
		para_meta["para"] = d_lines[i-1];
		para_meta["r"] = rotate_lines[i]->GetParam().at(6) * 1000;
		js_model_meta.append(js_model);
	}

	js["result_path"] = thread.GetResultPath();

	std::ofstream outfile(thread.GetResultPath() + "/meta_auto_curve.json");
	if (!outfile.is_open()) {

		QMessageBox::warning(NULL, "Warning",
			QString::fromLocal8Bit("????????!"));
		return;
	}
	outfile << js.toStyledString();
	outfile.close();
	for (auto line : rotate_lines) {
		if (line->GetLineType() != RotateLineBase::karman_line_radius_type) {
			QMessageBox::warning(NULL, "Warning",
				QString::fromLocal8Bit("??????????????!"));
			return;
		}
	}

	QProcess process_calc_line;

	process_show_widget->QAppend(
		QCoreApplication::applicationDirPath() + "/script/quick_calc_py/auto_curve.py");
	process_show_widget->QAppend(
		std::string(thread.GetResultPath() + "/meta_auto_curve.json").c_str());
	process_calc_line.start("python ",
		QStringList() << QCoreApplication::applicationDirPath() + "/script/quick_calc_py/auto_curve.py"
		<< std::string(thread.GetResultPath() + "/meta_auto_curve.json").c_str());

	process_show_widget->SetProcess(&process_calc_line);
	connect(&process_calc_line, SIGNAL(readyReadStandardOutput()), process_show_widget, SLOT(OnReadoutput()));
	connect(&process_calc_line, SIGNAL(readyReadStandardError()), process_show_widget, SLOT(OnReadError()));

	// ???????????????????????
	while (process_calc_line.state() != QProcess::NotRunning) {
		if (process_show_widget->IsCanceled()) {
			process_calc_line.kill();
			break;
		}
		QCoreApplication::processEvents();
	}
	process_show_widget->QAppend(QString::fromLocal8Bit("?????????????????????????."));
	process_show_widget->FinishProcess();
	process_show_widget->QAppend(QString::fromLocal8Bit("??????????????????????."));

	while (true) {
		if (process_show_widget->IsCanceled()) {
			break;
		}
		QCoreApplication::processEvents();
	}
	process_show_widget->close();

	std::vector<RotateLineBase*> line_vec;
	{
		std::vector<double> param;
		std::string path = thread.GetResultPath() + "/0.txt";
		auto line_data = RotateLineBaseFactory::GenRotateLine(RotateLineBase::import_point_type);
		/*if (!ReadData(path, &param)) {
			QMessageBox::warning(NULL, "Warning",
				QString::fromLocal8Bit("?????????"));
			return;
		}*/
	}
	// rotate_radome_widget_->SetLineData(line_vec);
	UpdateRotateRadome();
}

bool Radome::ReadData(const std::string& path, std::vector<double>* data) {
	QString q_path = QString::fromStdString(path);

	QFile file(q_path);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QMessageBox::warning(NULL, "Warning",
			QString::fromLocal8Bit("????????"));
		return false;
	}
	data->clear();
	QTextStream in(&file);
	double max_x = 0;
	while (!in.atEnd()) {
		QString line = in.readLine();
		QStringList fields = line.split(" ");
		if (fields.size() < 2) {
			QMessageBox::warning(NULL, "Warning",
				QString::fromLocal8Bit("????????:") + line);
			return false;
		}

		bool ok, ok1;
		double valuex = fields[0].toDouble(&ok);
		double valuey = fields[2].toDouble(&ok1);
		if (ok && ok1) {
			if (valuex > valuex) {
				max_x = valuex;
			}
			data->push_back(valuex);
			data->push_back(valuey);
		}
		else {
			QMessageBox::warning(NULL, "Warning",
				QString::fromLocal8Bit("????????:") + line);
			return false;
		}

	}
	for (int i = 0; i < data->size(); i += 2) {
		double tmp = data->at(i);
		(*data)[i] = max_x - tmp;
	}
	return true;
}

void Radome::OnChangeSource() {
	if (is_source_window_open_)
	{
		// ????д??????
		QMessageBox::warning(NULL, "Warning",
			"A window has been opened. Please close and continue!");

		return;
	}
	if (source_type_ == Def::gaussian_source_type) {
		temp_mirror_->setSelected(true);
		renderer->AddActor(temp_mirror_->getActor());
		// gaussian_widget_ = new GaussianWidget;
		gaussian_widget_->setMirror(temp_mirror_);
		gaussian_widget_->setWindowFlags(Qt::WindowStaysOnTopHint); // ???????????
		gaussian_widget_->show();
		is_source_window_open_ = true;
	}
	else if (source_type_ == Def::taile_source_type) {
		temp_mirror_->setSelected(true);
		renderer->AddActor(temp_mirror_->getActor());
		// taile_source_widget_ = new TaileSourceWidget;
		taile_source_widget_->setMirror(temp_mirror_);
		taile_source_widget_->setWindowFlags(Qt::WindowStaysOnTopHint); // ???????????
		taile_source_widget_->show();
		is_source_window_open_ = true;
	}
	else if (source_type_ == Def::taile_only_source_type) {
		temp_mirror_->setSelected(true);
		renderer->AddActor(temp_mirror_->getActor());
		// taile_source_widget_ = new TaileSourceWidget;
		taile_only_source_widget_->setMirror(temp_mirror_);
		taile_only_source_widget_->setWindowFlags(Qt::WindowStaysOnTopHint); // 子窗口保持置顶
		taile_only_source_widget_->show();
		is_source_window_open_ = true;
	}
	else if (source_type_ == Def::aperture_source_type) {
		temp_mirror_->setSelected(true);
		renderer->AddActor(temp_mirror_->getActor());
		aperture_field_widget_->setMirror(temp_mirror_);
		aperture_field_widget_->setWindowFlags(Qt::WindowStaysOnTopHint); // 子窗口保持置顶
		aperture_field_widget_->show();
		is_source_window_open_ = true;
	}
}

void Radome::OnSaveAction() {
	QString filename = QFileDialog::getExistingDirectory(this,
		tr("Save the FarField"),
		"");
	if (filename.isEmpty())
	{
		return;
	}
	Json::Value param_js;
	// ?
	SAVE_WIDGET_TO_JSON(taile_source_widget_, param_js);
	//Json
	//if (data_manager_.GetSource()) {

	//}
	// ???
	// ????
	// ??????

}

void Radome::OnLoadAction() {
	Json::Value param_js;
	QObject::blockSignals(true);
	READ_WIDGET_TO_JSON(taile_source_widget_, param_js, TaileSourceWidget);


	QObject::blockSignals(false);
}

void Radome::OnSaveProject() {
	QString savePath = QFileDialog::getSaveFileName(
		this, QString::fromLocal8Bit("???????"), "", "JSON Files (*.json)"
	);
	if (savePath.isEmpty()) return;

	QJsonObject project;

	std::string dir_path_save;
	std::string result_path_;
	std::string result_non_radome_path_;
	bool is_calc_non_radome_ = false;
	int status_ = -1;
	double fre_ = fre_save;
	int polarization_type_ = 1; // ?????????1, ??1??????
	QDir dir;

	QDateTime data_time = QDateTime::currentDateTime();
	QString current_time = data_time.toString("yyyy_MM_dd_hh_mm_ss");
	GenQuickCalcMeta gen_meta(data_manager_);
	////?·??????
	dir_path_save = savePath.toStdString();
	// ???????????txt????????????
	std::string source_path = dir_path_save + "/source_znx.txt";
	auto source = data_manager_.GetSource();
	if (NULL == source) {
		status_ = -100;
	}
	else {
		if(!dir.exists(dir_path_save.c_str()))
		{
			dir.mkpath(dir_path_save.c_str());	
		}
		source->SaveFieldBase(source_path);
	}
	gen_meta.SetSourcePath(source_path);

	std::string dir_stl_path = dir_path_save + "/stl/";
	dir.mkpath(dir_stl_path.c_str());
	result_path_ = dir_path_save + "/result/" + current_time.toStdString() + "/";

	// Set	
	//???浼????????para??M??N
	gen_meta.SetSourcePara(data_);
	gen_meta.SetN_width(N_width);
	gen_meta.SetM_depth(M_depth);
	//??data_manager_.GetSource()???????????ds
	if(data_manager_.GetSource())
	{
		gen_meta.SetDs(data_manager_.GetSource()->getDs());
	}
    //??????TransRotate????
    gen_meta.SetTransRotatePara(trans_rotate_para);

	result_path_ = dir_path_save + "/result/" + current_time.toStdString() + "/";
	dir.mkpath(result_path_.c_str());
	gen_meta.SetResultPath(result_path_);
	if (is_calc_non_radome_) {
		result_non_radome_path_ = dir_path_save + "/result/nr_" + current_time.toStdString() + "/";
		dir.mkpath(result_non_radome_path_.c_str());
		gen_meta.SetResultNonRadomePath(result_non_radome_path_);
	}	

	//stl·??????
	for (auto& x : results_filesave) {
		//???x
		qDebug() << ".json stl path" << x.c_str();
		gen_meta.AddSTLPath(x);
	}
	gen_meta.SetOutputPath(dir_path_save);
	gen_meta.SetFre(fre_);
	gen_meta.SetPolarizationType(polarization_type_);

	gen_meta.WriteMetaMsg(dir_path_save + "/meta.json");

	status_ = 0;

	QMessageBox::information(this, QString::fromLocal8Bit("???"), QString::fromLocal8Bit("?????????"));
}
void Radome::onLoadProject() {
	//filter
	QString filePath = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("???????"), "", "JSON Files (*.json)");
	if (filePath.isEmpty()) return;

	//??????????????м??????
	QProgressDialog progressDialog(QString::fromLocal8Bit("?????????????????..."), QString::fromLocal8Bit("???"), 0, 100, this);
	progressDialog.setWindowTitle(QString::fromLocal8Bit("???????"));
	progressDialog.setWindowModality(Qt::WindowModal);
	progressDialog.setMinimumDuration(0);
	progressDialog.setAutoClose(false);
	progressDialog.setAutoReset(false);
	progressDialog.show();

	QApplication::processEvents();
	
	//////////////////////////////////////////////////////.stl??????????vtk???
	progressDialog.setValue(10); // ???????? 10%
	if (progressDialog.wasCanceled()) {
		return; // ??????????????????????
	}
	QApplication::processEvents();

	SaveInputProject stl_load(filePath);
	results_filesave = stl_load.GetResults();//???stl???·?????????results_filesave??????α???
	auto surface_results = stl_load.GetSurfaceResults();

	tmp_data = new RadomeSTL();
	tmp_data->ReadSTLFromFile(results_filesave);
	tmp_data->Update();
	tmp_data->SetSurfacePathVec(surface_results);

	////0219???????????
	auto old_radome = data_manager_.GetRadomeData();
	if (old_radome) {
	 	QMessageBox::StandardButton rb =
	 		QMessageBox::question(NULL, "question", QString::fromLocal8Bit("??????????????????滻"),
	 			QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
	 	if (rb == QMessageBox::No)
	 	{
	 		return;
	 	}
	}
	if (old_radome) {
	 	const auto& old_actors = old_radome->GetActors();
	 	for (const auto& actor : old_actors) {
	 		renderer->RemoveActor(actor);
	 	}
	 	model_tree_item_->removeChild(old_radome->GetTreeItem());
	}
	////
	auto tmp_tree = tmp_data->GetTreeItem();
	model_tree_item_->addChild(tmp_tree);
	model_tree_item_->setExpanded(true);
	tmp_tree->setExpanded(true);
	for (int i = 0; i < tmp_tree->childCount(); i++) {
		tmp_tree->child(i)->setExpanded(true);
	}
	data_manager_.ResetRadome(tmp_data);
	const auto& actors = tmp_data->GetActors();
	for (const auto& actor : actors) {
		renderer->AddActor(actor);
	}
	UpdateVtk();
	
	/////////////////////////////////////////////////////////////????????vtk???
	///////////////////????????????/////////////////////////////////
		progressDialog.setValue(30); // ???????? 30%
		if (progressDialog.wasCanceled()) {
			return; // ??????????????????????
		}
		QApplication::processEvents();
		std::vector<double> para;	
		std::vector<double> trans_rotate_load;	
		// ???????data????para???????N??M??fre????
		stl_load.getSourcePara_(para);
		stl_load.getTransRotatePara_(trans_rotate_load);
        //????trans_rotate_para??????????ε?????
	   	trans_rotate_para.resize(trans_rotate_load.size());
		for (int i = 0; i < trans_rotate_load.size(); i++)
		{
			trans_rotate_para[i] = trans_rotate_load[i];
		}

		data_.resize(para.size());
		for (int i = 0; i < para.size(); i++)
		{
			data_[i] = para[i];
		}

		N_width = stl_load.getNWidth();
		M_depth = stl_load.getMDepth();
		double fre_meta = stl_load.getF0();
		fre_save = fre_meta;
		
		QString EfieldFile = stl_load.getSourcePath();

		if (nullptr != data_manager_.GetSource()) // ?????????? ???????????
		{
			//taile_source_widget_->setHidden(true);
			// ?ж???????????????????
			switch (QMessageBox::question(this, tr("Question"),
				QString::fromLocal8Bit("?????????????滻?"),
				QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok))
			{
			case QMessageBox::Ok:
				renderer->RemoveActor(data_manager_.GetSource()->getActor());
				renderer->RemoveActor(data_manager_.GetSource()->getActor3D());
				source_tree_item_->removeChild(source_tree_item_->child(0));
				break;
			case QMessageBox::Cancel:
				ToTaileSource(0);
				return;
			default:
				break;
			}
		}
		
		Field * ptr;
		temp_mirror_ = new PlaneMirror(GraphTrans());
		temp_mirror_->setSelected(true);
		renderer->AddActor(temp_mirror_->getActor());

		GraphTransWidget* graphTransWidget = new GraphTransWidget;
		graphTransWidget->setMirror_load(temp_mirror_, trans_rotate_load);

		ptr = new ApertureField(temp_mirror_->getGraphTrans(), para);
		ptr->setNM(N_width, M_depth);
		ptr->setDs(para[0] / (N_width - 1));
		ptr->SetFre(fre_meta);
		ptr->SetPolarizationType(2);
		dynamic_cast<ApertureField*>(ptr)->setFileAddress(EfieldFile.toStdString());
		dynamic_cast<ApertureField*>(ptr)->readData();
		ptr->updateData();

		renderer->AddActor(ptr->getActor());

		source_tree_item_->addChild(ptr->getTree());
		data_manager_.SetSource(ptr);

		if (temp_mirror_ != nullptr) {
		renderer->RemoveActor(temp_mirror_->getActor());
		delete temp_mirror_;
		temp_mirror_ = nullptr;
		}
	UpdateVtk();
	//////////////////////////////////////////////////////////////////////////
	
	//////////////////////////////////////////////////////////////////????????????treeitem???????datamanager????
	progressDialog.setValue(90); // ???????? 90%
	if (progressDialog.wasCanceled()) {
		return; // ??????????????????????
	}
	QApplication::processEvents();

    std::vector<int> index_material = std::vector<int>(10);
    std::vector<std::string> material_name = std::vector<std::string>(10);
    std::vector<double> eps = std::vector<double>(10);
    std::vector<double> mu = std::vector<double>(10);
    std::vector<double> loss = std::vector<double>(10);
 
	stl_load.GetMaterialIndex(index_material);
	stl_load.GetMaterialName(material_name);
	stl_load.GetMaterialEps(eps);
	stl_load.GetMaterialMu(mu);
	stl_load.GetMaterialLoss(loss);
	for (int i = 0; i < index_material.size(); i++) {
		    const auto& material_name_ = material_name[i];
			MaterialData* data_material_ = new MaterialData(material_name_, index_material[i]);	
			data_material_->SetMaterialData(eps[i], mu[i], loss[i]);
			data_material_->SetIndex(index_material[i]);
			data_material_->Update();
			data_manager_.SetMaterialData(index_material[i], data_material_);//????????????material_book?С?
			auto tmp_tree_material_ = data_material_->GetTreeItem();
		    if (!tmp_tree_material_) {
			qDebug() << "Error: Failed to get tree item for MaterialData at index " << i;
			continue;
   			}
			custom_material_tree_item_->addChild(tmp_tree_material_);
			custom_material_tree_item_->setExpanded(true);
			tmp_tree_material_->setExpanded(true);
			for (int j = 0; j < tmp_tree_material_->childCount(); j++) {
				tmp_tree_material_->child(j)->setExpanded(true);
			}

		//????????????????treeitem??
			auto mat_id = index_material[i];
			auto* mat = data_manager_.GetMaterialData(mat_id);
			if (mat == nullptr) {
				QMessageBox::warning(NULL, "Warning",
					QString::fromLocal8Bit("???ò??????"));
				return;
			}
			data_manager_.SyncMaterialIndex();
			data_manager_.GetRadomeData()->SetMaterialId(i, mat_id, mat->GetName());
			
			model_tree_item_->removeChild(data_manager_.GetRadomeData()->GetTreeItem());
			data_manager_.GetRadomeData()->UpdateTreeItem();
			auto tmp_tree = data_manager_.GetRadomeData()->GetTreeItem();
			model_tree_item_->addChild(tmp_tree);
			model_tree_item_->setExpanded(true);
			tmp_tree->setExpanded(true);
			for (int s = 0; s < tmp_tree->childCount(); s++) {
				tmp_tree->child(s)->setExpanded(true);
			}
		
	}
	//////////////////////////////////////////////////////////////////////////
	// ????????????
	progressDialog.setValue(100);
	QApplication::processEvents();
	QMessageBox::information(this, QString::fromLocal8Bit("???????"), QString::fromLocal8Bit("??????????"));
	progressDialog.close();
	// ????????
}


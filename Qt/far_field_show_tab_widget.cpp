#include "far_field_show_tab_widget.h"
////0116:3D
//#include <vtkSmartPointer.h>
#include <vtkDoubleArray.h> 

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
////显示坐标轴0208
#include <vtkAxesActor.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkTextProperty.h>
#include <vtkProperty.h>
#include <vtkCaptionActor2D.h>
#include <vtkTextActor.h>
#include <vtkArrowSource.h>
#include <vtkInteractorStyleTrackballCamera.h>
////

FarFieldShowTab::FarFieldShowTab(DataManager* data_manager) :
	data_manager_(data_manager)
{
	////0213：右边选项卡
	switch_group_button_unit_ = new QButtonGroup();
	linear_button_unit_ = new QRadioButton(QString::fromLocal8Bit("linear"));
	dB_button_unit_ = new QRadioButton(QString::fromLocal8Bit("dB"));
	switch_group_button_unit_->addButton(linear_button_unit_, 0);
	switch_group_button_unit_->addButton(dB_button_unit_, 1);
	linear_button_unit_->setChecked(false);
	dB_button_unit_->setChecked(true);
	connect(switch_group_button_unit_, SIGNAL(buttonClicked(int)), this, SLOT(OnUnitChange(int)));

	QHBoxLayout* unit_layout = new QHBoxLayout;
	unit_layout->addWidget(linear_button_unit_);
	unit_layout->addWidget(dB_button_unit_);

	QVBoxLayout* basic_layout = new QVBoxLayout;
	basic_layout->addLayout(unit_layout);

	switch_box_ = new QGroupBox;
	switch_box_->setTitle(QString::fromLocal8Bit("option"));
	switch_box_->setLayout(basic_layout);
	switch_box_->setMaximumHeight(100);
	switch_box_->setMaximumWidth(400);

	a_label_ = new QLabel(QString::fromLocal8Bit("result:"));
	a_label_->setMaximumWidth(50);
	combo_box_a_ = new QComboBox;
	connect(combo_box_a_, SIGNAL(currentIndexChanged(int)), this, SLOT(OnComboBoxChange(int)));
	QHBoxLayout* a_labe_layout = new QHBoxLayout;
	a_labe_layout->addWidget(a_label_);
	a_labe_layout->addWidget(combo_box_a_);

	ab_ok_btn_ = new QPushButton(QString::fromLocal8Bit("ok"));
	ab_ok_btn_->setMaximumWidth(150);
    connect(ab_ok_btn_, &QPushButton::clicked, this, &FarFieldShowTab::OnAbOkBtn);
	QVBoxLayout* ab_layout = new QVBoxLayout;
	ab_layout->addLayout(a_labe_layout);

    //加入文件导入控件，浏览资源管理器，选择文件
    QPushButton *fileButton = new QPushButton("选择文件");
    connect(fileButton, &QPushButton::clicked, this, &FarFieldShowTab::OnfileBtn);
    //显示选择的文件名
    QLabel *fileNameLabel = new QLabel(fileName);
	QHBoxLayout* file_labe_layout = new QHBoxLayout;
	file_labe_layout->addWidget(fileButton);
	file_labe_layout->addWidget(fileNameLabel);
 
    
    // 添加文件路径显示控件
    filePathLabel = new QLabel(this);
    filePathLabel->setText("未选择文件");
    file_labe_layout->addWidget(filePathLabel);

    ab_layout->addLayout(file_labe_layout);

	ab_group_box_ = new QGroupBox;
	ab_group_box_->setLayout(ab_layout);
	ab_group_box_->setMaximumHeight(400);
	ab_group_box_->setMaximumWidth(400);

	UpdateCombox();

	QVBoxLayout* layout_left = new QVBoxLayout();
	layout_left->addWidget(switch_box_);
	layout_left->addWidget(ab_group_box_);
	layout_left->addWidget(ab_ok_btn_);

	widget_ = new QVTKOpenGLNativeWidget;
	QHBoxLayout* plot_a_layout = new QHBoxLayout;
	plot_a_layout->addWidget(widget_);
	plot_box_ = new QGroupBox;
	plot_box_->setTitle(QString::fromLocal8Bit("result"));
	plot_box_->setLayout(plot_a_layout);
	QHBoxLayout* layout_main = new QHBoxLayout(this);
	layout_main->addWidget(plot_box_);
	layout_main->addLayout(layout_left);


   // 创建Renderer、RenderWindow和RenderWindowInteractor
   vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	auto renderWindow = widget_->GetRenderWindow();
   renderWindow->AddRenderer(renderer);
   vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
   renderWindowInteractor->SetRenderWindow(renderWindow);

   auto style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
   style->SetDefaultRenderer(renderer);  // 确保关联渲染器
   renderWindowInteractor->SetInteractorStyle(style);
   renderWindowInteractor->Initialize();

   axes = vtkSmartPointer<vtkAxesActor>::New();
   axes->GetXAxisShaftProperty()->SetColor(0, 0, 1);//修改X字体颜色为红色
   axes->GetXAxisTipProperty()->SetColor(0, 0, 1);
   axes->GetXAxisCaptionActor2D()->GetCaptionTextProperty()->SetColor(0, 0, 1);//修改X字体颜色为红色  
   axes->GetYAxisCaptionActor2D()->GetCaptionTextProperty()->SetColor(0, 2, 0);//修改Y字体颜色为绿色  
   axes->GetZAxisCaptionActor2D()->GetCaptionTextProperty()->SetColor(1, 0, 0);//修改Z字体颜色为蓝色
   axes->GetZAxisShaftProperty()->SetColor(1, 0, 0);
   axes->GetZAxisTipProperty()->SetColor(1, 0, 0);

   axes->SetConeRadius(0.3);
   axes->SetConeResolution(20);

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

   renderer->AddActor(axes_actor);

   renderer->ResetCamera();
   renderWindow->Render();

   widget1 = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
   widget1->SetOutlineColor(0.9300, 0.5700, 0.1300);
   widget1->SetOrientationMarker(axes);
   widget1->SetInteractor(renderWindowInteractor);
   widget1->SetViewport(0.0, 0.0, 0.25, 0.25);
   widget1->SetEnabled(1);
   widget1->InteractiveOff();

    renderer->SetBackground(1.0,1.0,1.0); // 设置背景颜色
}

////0213：将输入farfield改为输入datamanager
void FarFieldShowTab::UpdateCombox() {
	data_index_map_.clear();
	auto all_field = data_manager_->GetAllFarFieldMap();
	int count = 0;
	for (auto& cp : all_field) {
		data_index_map_.emplace(count, cp.first);
		count++;
	}
	auto func = [](QComboBox*combo_box, DataManager* data_manager) {
		int cur_index = combo_box->currentIndex();
		auto all_field = data_manager->GetAllFarFieldMap();
		combo_box->clear();
		for (auto& cp : all_field) {
			combo_box->addItem(QString::fromLocal8Bit(cp.second->GetName().c_str()));
		}
		combo_box->setCurrentIndex(cur_index);
	};
	func(combo_box_a_, data_manager_);
}
////
FarFieldShowTab::~FarFieldShowTab()
{
}

QVTKOpenGLNativeWidget* FarFieldShowTab::Getwidget_(){
    return widget_;
}

void FarFieldShowTab::OnComboBoxChange(int _index) {
	qDebug() << "OnComboBoxChange";
	int index = combo_box_a_->currentIndex();
	auto all_field = data_manager_->GetAllFarFieldMap();
	
	int filed_index = data_index_map_[index];
	if (all_field.find(filed_index) == all_field.end()) {
		return;
	}
	QVector<double> x_a_vec;
	double x_max = 0.0;
	double x_min = 0.0;
	
    far_filed = all_field[filed_index];

    //在combo_box_a_列表显示文字
	combo_box_a_->setItemText(index, QString::fromLocal8Bit(far_filed->GetName().c_str()));
    file_flag = 0;
}

void FarFieldShowTab::OnAbOkBtn() {
    //定义E指针
    const std::vector<std::vector<double>>* E_ptr = nullptr;
    if (file_flag == 1) {
        if (is_dB_) {
            E_ptr = &EDB;

        }
        else {
            E_ptr = &E;
        }
        
        
    }
    else {

        if (is_dB_) {
            E_ptr = &far_filed->GetEDB();

        }
        else {
            E_ptr = &far_filed->GetE();
        }

        
    }
    const std::vector<std::vector<double>>& E_ = *E_ptr;
    int nx = E_.size(), ny = E_[0].size();
    if (E_.empty() || E_[0].empty()) {
        qDebug() << "Error: E data is empty!";
        return;
    }
    else {
        qDebug() << "E data is not empty!";
    }

    // 创建点集
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();

    // 创建单元集（这里直接使用三角形）
    vtkSmartPointer<vtkCellArray> triangles = vtkSmartPointer<vtkCellArray>::New();

    // 创建标量数组来存储每个点的值
    vtkSmartPointer<vtkDoubleArray> scalars = vtkSmartPointer<vtkDoubleArray>::New();
    scalars->SetName("Scalars");

    double min = E_[0][0], max = E_[0][0];
    for (const auto& row : E_) {
        for (double val : row) {
            if (val < min) min = val;
            if (val > max) max = val;
        }
    }

    // 填充点集和标量数组
    for (int j = 0; j < ny; ++j) {
        for (int i = 0; i < nx; ++i) {
            // 将极坐标转换为笛卡尔坐标
            //将E[i][j]归一化到[0,1],最大值归一化到1，最小值归一化到0
            double r = (E_[i][j] - min) / (max - min);
            double x = r * sin(i * M_PI / 180 / 5) * cos(j * M_PI / 180 * 2);
            double y = r * sin(i * M_PI / 180 / 5) * sin(j * M_PI / 180 * 2);
            double z = r * cos(i * M_PI / 180 / 5);

            vtkIdType pid = points->InsertNextPoint(x, y, z);
            scalars->InsertNextValue(r);

            // points->InsertNextPoint(x, y, z);            
        }
    }

    // 创建三角形
    for (int j = 0; j < ny - 1; ++j) {
        for (int i = 0; i < nx - 1; ++i) {
            vtkIdType tri1[3];
            tri1[0] = i + j * nx;
            tri1[1] = (i + 1) + j * nx;
            tri1[2] = i + (j + 1) * nx;
            triangles->InsertNextCell(3, tri1);

            vtkIdType tri2[3];
            tri2[0] = (i + 1) + j * nx;
            tri2[1] = (i + 1) + (j + 1) * nx;
            tri2[2] = i + (j + 1) * nx;
            triangles->InsertNextCell(3, tri2);
        }
    }

    // 创建PolyData对象
    vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
    polyData->SetPoints(points);
    polyData->SetPolys(triangles);
    polyData->GetPointData()->SetScalars(scalars);

    // 创建颜色映射
    vtkSmartPointer<vtkColorTransferFunction> colorTransferFunction = vtkSmartPointer<vtkColorTransferFunction>::New();

    // 添加彩虹色控制点（从蓝到红）
    colorTransferFunction->AddRGBPoint(0.0, 0.0, 0.0, 1.0); // 蓝
    colorTransferFunction->AddRGBPoint(0.25, 0.0, 1.0, 1.0); // 青
    colorTransferFunction->AddRGBPoint(0.5, 0.0, 1.0, 0.0); // 绿
    colorTransferFunction->AddRGBPoint(0.75, 1.0, 1.0, 0.0); // 黄
    colorTransferFunction->AddRGBPoint(1.0, 1.0, 0.0, 0.0); // 红

    // ================= 2. 自动获取数据范围 =================
    double scalarRange[2];
    polyData->GetScalarRange(scalarRange); // 获取实际标量范围

    // ================= 3. 正确配置Mapper =================
    // 创建PolyDataMapper
    //// 定义 smoother
    vtkSmartPointer<vtkSmoothPolyDataFilter> smoother = vtkSmartPointer<vtkSmoothPolyDataFilter>::New();
    // 设置 smoother 的输入和其他参数
    smoother->SetInputData(polyData);
    smoother->SetNumberOfIterations(100);
    smoother->Update();
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(smoother->GetOutputPort());
    mapper->SetLookupTable(colorTransferFunction);
    mapper->SetScalarRange(scalarRange); // 动态设置范围
    mapper->ScalarVisibilityOn(); // 必须启用标量映射

    // 创建Actor
    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);

    // ================= 生成经纬线网格 =================
    vtkSmartPointer<vtkCellArray> gridLines = vtkSmartPointer<vtkCellArray>::New();

    // 定义经纬线密度（每隔10个点画一条线）
    //const int latitudeStep = nx > 10 ? nx / 10 : 1;   // 至少画1条
    //const int longitudeStep = ny > 10 ? ny / 10 : 1;
    const int latitudeStep = 5;
    const int longitudeStep = 6;

    // 生成纬度线（固定i，遍历j）
    for (int i = 0; i < nx; i += latitudeStep) {
        for (int j = 0; j < ny - 1; ++j) {
            vtkIdType line[2];
            line[0] = i + j * nx;
            line[1] = i + (j + 1) * nx;
            gridLines->InsertNextCell(2, line);
        }
    }

    // 生成经度线（固定j，遍历i）
    for (int j = 0; j < ny; j += longitudeStep) {
        for (int i = 0; i < nx - 1; ++i) {
            vtkIdType line[2];
            line[0] = i + j * nx;
            line[1] = (i + 1) + j * nx;
            gridLines->InsertNextCell(2, line);
        }
    }

    // 创建网格线PolyData
    vtkSmartPointer<vtkPolyData> gridPolyData = vtkSmartPointer<vtkPolyData>::New();
    gridPolyData->SetPoints(polyData->GetPoints());  // 直接使用原有点集
    gridPolyData->SetLines(gridLines);

    // ================= 网格线可视化 =================
    vtkSmartPointer<vtkPolyDataMapper> gridMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    gridMapper->SetInputData(gridPolyData);

    vtkSmartPointer<vtkActor> gridActor = vtkSmartPointer<vtkActor>::New();
    gridActor->SetMapper(gridMapper);
    gridActor->GetProperty()->SetColor(0, 0, 0);   // 黑色
    gridActor->GetProperty()->SetLineWidth(1.5);   // 线宽

    // 创建Renderer、RenderWindow和RenderWindowInteractor
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    auto renderWindow = widget_->GetRenderWindow();
    renderer->RemoveAllViewProps();
    renderWindow->AddRenderer(renderer);
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);

    auto style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    style->SetDefaultRenderer(renderer);  // 确保关联渲染器
    renderWindowInteractor->SetInteractorStyle(style);
    renderWindowInteractor->Initialize();

    axes = vtkSmartPointer<vtkAxesActor>::New();
    axes->GetXAxisShaftProperty()->SetColor(0, 0, 1);//修改X字体颜色为红色
    axes->GetXAxisTipProperty()->SetColor(0, 0, 1);
    axes->GetXAxisCaptionActor2D()->GetCaptionTextProperty()->SetColor(0, 0, 1);//修改X字体颜色为红色  
    axes->GetYAxisCaptionActor2D()->GetCaptionTextProperty()->SetColor(0, 2, 0);//修改Y字体颜色为绿色  
    axes->GetZAxisCaptionActor2D()->GetCaptionTextProperty()->SetColor(1, 0, 0);//修改Z字体颜色为蓝色
    axes->GetZAxisShaftProperty()->SetColor(1, 0, 0);
    axes->GetZAxisTipProperty()->SetColor(1, 0, 0);

    axes->SetConeRadius(0.3);
    axes->SetConeResolution(20);

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

    renderer->AddActor(axes_actor);
    //renderer->SetBackground(0.1, 0.2, 0.4);
    renderer->ResetCamera();
    renderWindow->Render();

    widget1 = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
    widget1->SetOutlineColor(0.9300, 0.5700, 0.1300);
    widget1->SetOrientationMarker(axes);
    widget1->SetInteractor(renderWindowInteractor);
    widget1->SetViewport(0.0, 0.0, 0.25, 0.25);
    widget1->SetEnabled(1);
    widget1->InteractiveOff();

    ////0211添加彩虹图例
    //设置彩虹图的颜色缩放轴
    vtkSmartPointer<vtkColorTransferFunction> colorTransferFunction2 = vtkSmartPointer<vtkColorTransferFunction>::New();
    colorTransferFunction2->AddRGBPoint(min, 0.0, 0.0, 1.0); // 蓝
    colorTransferFunction2->AddRGBPoint(0.25 * (max - min) + min, 0.0, 1.0, 1.0); // 青
    colorTransferFunction2->AddRGBPoint(0.5 * (max - min) + min, 0.0, 1.0, 0.0); // 绿
    colorTransferFunction2->AddRGBPoint(0.75 * (max - min) + min, 1.0, 1.0, 0.0); // 黄
    colorTransferFunction2->AddRGBPoint(max, 1.0, 0.0, 0.0); // 红

    // 创建一个标量条
    vtkSmartPointer<vtkScalarBarActor> scalarBar = vtkSmartPointer<vtkScalarBarActor>::New();
    scalarBar->SetLookupTable(colorTransferFunction2);
    scalarBar->SetTitle("EDB");  // 设置标量条的标题
    scalarBar->SetNumberOfLabels(5);  // 设置标签数量
    scalarBar->SetPosition(0.05, 0.1);  // 设置标量条的位置，相对于视口的比例
    scalarBar->SetWidth(0.04);  // 设置标量条的宽度，相对于视口的比例
    scalarBar->SetHeight(0.32);  // 设置标量条的高度，相对于视口的比例
    // 将标量条添加到渲染器
    renderer->AddActor(scalarBar);
    ////

    // 添加Actor到Renderer
    renderer->AddActor(actor);
    renderer->AddActor(gridActor);  // 添加网格线Actor
    renderer->SetBackground(1.0, 1.0, 1.0); // 设置背景颜色

    // 重置相机
    renderer->ResetCamera();

    // 渲染窗口
    renderWindow->Render();

	
}

void FarFieldShowTab::OnfileBtn() {
    QString filePath = QFileDialog::getOpenFileName(this, "选择文件", "","NumPy Files (*.npy);;All Files (*)");
    cnpy::NpyArray Es_abs;
    if (!filePath.isEmpty()) {
        // 在这里处理选择的文件路径	
        qDebug() << "选择的文件路径：" << filePath;

        //获取选择的文件名称
        QFileDialog fileDialog(this);
        fileDialog.setFileMode(QFileDialog::ExistingFile);
        fileDialog.setNameFilter("NumPy Files (*.npy)");

        std::string stdFilePath = filePath.toStdString();
        Es_abs = cnpy::npy_load(stdFilePath);
        if (Es_abs.data_holder == nullptr) {
        qDebug() << "Failed to load .npy file: " << QString::fromStdString(stdFilePath);
        return; 
        }
        qDebug() << "选择的文件名称：" << fileName;
        fileDialog.close();
        filePathLabel->setText(filePath);  // 更新显示的文件路径

    } else {
        qDebug() << "用户取消了文件选择";
        filePathLabel->setText("no selected file");
        return;
    }
    
	double* loaded_data = Es_abs.data<double>();

    size_t numRows = Es_abs.shape[1];
    size_t numCols = Es_abs.shape[0];

    // 调整 E 的大小
    E.resize(numRows);
    for (auto& row : E) {
        row.resize(numCols);
    }
    // 调整 EDB 的大小
    EDB.resize(numRows);
    for (auto& row : EDB) {
        row.resize(numCols);
    }

    for (size_t i = 0; i < numRows; ++i) {
        for (size_t j = 0; j < numCols; ++j) {
            E[i][j] = loaded_data[j + i * numCols];
            //qDebug() << "E[" << i << "][" << j << "] = " << E[i][j];
        }
    }

    //处理线性与dB单位转换
    double min_liner = INT32_MAX;
    double max_liner = INT32_MIN;
    double min_db = INT32_MAX;
    double max_db = INT32_MIN;
    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numCols; j++) {
            double value = E[i][j];
            if (value < min_liner) min_liner = value;
            if (value > max_liner) max_liner = value;
            //value = EDB[i][j];
            //if (value < min_db) min_db = value;
            //if (value > max_db) max_db = value;
        }
    }
    // 归一化
    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numCols; j++) {
            double value = E[i][j] / max_liner;
            EDB[i][j] = 10 * log10(value);
            if (EDB[i][j] < min_db) min_db = EDB[i][j];
            if (EDB[i][j] > max_db) max_db = EDB[i][j];
        }
    }

    file_flag = 1;

}

void FarFieldShowTab::OnUnitChange(int){
    if (linear_button_unit_->isChecked()) {
        is_dB_ = false;
    }
    else {
        is_dB_ = true;
    }
}
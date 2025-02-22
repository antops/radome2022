#include "input_stl_widget.h"

#include <fstream>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QGroupBox>
#include <QMessageBox>
#include <QDir>
#include <QMessageBox>
#include <QCoreApplication>
#include <QProcess>
#include <vtkjsoncpp/json/json.h>

#include "global_config.h"


InputStlWidget::InputStlWidget(int paramCount, QWidget *parent)
{
    QVBoxLayout* Vlayout = new QVBoxLayout(this);
    QLabel* label1 = new QLabel(QString::fromLocal8Bit("从外到内"));
    Vlayout->addWidget(label1);
    // 根据参数个数生成子界面
    for (int i = 0; i < paramCount; ++i) {
        QHBoxLayout* subLayout = new QHBoxLayout();

        // 添加导入文件名的编辑框及对应提示标签
        QLabel* fileNameLabel = new QLabel(QString::fromLocal8Bit("导入文件名:"));
        QLineEdit* fileNameEdit = new QLineEdit();
        subLayout->addWidget(fileNameLabel);
        subLayout->addWidget(fileNameEdit);

        QPushButton* BBtn = new QPushButton(tr("Browse..."));
        subLayout->addWidget(BBtn);
        connect(BBtn, SIGNAL(clicked()), this, SLOT(OnBrowseBtn()));

        file_name_line_vec_.push_back(fileNameEdit);
        btn_vec_.push_back(BBtn);

        QGroupBox* combobox = new QGroupBox();
        combobox->setTitle(QString::fromLocal8Bit("第 ") + QString::number(i + 1) + QString::fromLocal8Bit(" 个"));
        combobox->setLayout(subLayout);

        Vlayout->addWidget(combobox);
    }

	okBtn = new QPushButton(QString::fromLocal8Bit("确认"));

    Vlayout->addWidget(okBtn);

	connect(okBtn, SIGNAL(clicked()), this, SLOT(on_ok_btn()));
}

InputStlWidget::~InputStlWidget()
{

}



void InputStlWidget::on_ok_btn() {
    results_.clear();
    int i = 0;
    for (auto file_name_line : file_name_line_vec_) {
        std::string file_name = file_name_line->text().toStdString();
        results_.push_back(file_name);
        if (file_name.empty()) {
            QMessageBox::warning(NULL, "Warning",
                QString::fromLocal8Bit("未设置第") + 
            QString::number(i+1) + QString::fromLocal8Bit("参数"));
            return;
        }
        i++;
    }
    GenSurface();
    accept();
}

void InputStlWidget::OnBrowseBtn() {
    QString filename = QFileDialog::getOpenFileName(this,
        tr("open file"),
        "",
        tr("*.stl"));
    if (filename.isEmpty())
    {
        return;
    }
    QObject* senderObj = QObject::sender();
    for (int i = 0; i < btn_vec_.size(); i ++) {
        if (senderObj == btn_vec_[i]) {
            file_name_line_vec_[i]->setText(filename);
        }
    }
}

void InputStlWidget::GenMeta() {

    Json::Value js;
    js["version"] = 1;
    std::string output_path = std::string(QCoreApplication::applicationDirPath().toStdString() + "/output/stl");
    Json::Value js_path;
    Json::Value output_js_path;
    int i = 0;
    surface_results_.clear();
    for (const auto& file_name : results_) {
        Json::Value tmp_js;
        tmp_js["readpath"] = file_name;
        js_path.append(tmp_js);
        Json::Value output_tmp_js;
        std::string surface_path = output_path + "/radome" + std::to_string(i++);
        output_tmp_js["writepath"] = surface_path;
        output_js_path.append(output_tmp_js);
        surface_results_.push_back(surface_path + ".stl");
    }
    Json::Value output_tmp_js;
    std::string surface_path = output_path + "/radome" + std::to_string(i);
    output_tmp_js["writepath"] = surface_path;
    surface_results_.push_back(surface_path + ".stl");
    output_js_path.append(output_tmp_js);
    js["stlpath"] = js_path;
    js["output"] = output_js_path; ;

    std::ofstream outfile(dir_path_ + "/stl_surface_meta.json");
    if (!outfile.is_open())
    {
        return;
    }
    outfile << js.toStyledString();
    outfile.close();
}

void InputStlWidget::GenSurface() {
    dir_path_ = QCoreApplication::applicationDirPath().toStdString() + "/output/stl_surface";
    QDir dir;
    dir.mkpath(dir_path_.c_str());
    GenMeta();
    QProcess process;

    if (GlobalConfig::Instance()->IsQuickCalcByPython()) {
        process.start("python ",
            QStringList() << QCoreApplication::applicationDirPath() + "/script/quick_calc_py/stlsplit.py"
            << std::string(dir_path_ + "/stl_surface_meta.json").c_str());
    }
    else {
        std::string exe_file = std::string(QCoreApplication::applicationDirPath().toStdString()
            + "/script/quick_calc/stlsplit.exe ");
        process.start(exe_file.c_str(),
            QStringList() << std::string(dir_path_ + "/stl_surface_meta.json").c_str());
    }

    // 等待进程结束，并显示进度条
    while (process.state() != QProcess::NotRunning) {
        QCoreApplication::processEvents();
    }
}
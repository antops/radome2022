#include "save_input_project.h"

SaveInputProject::SaveInputProject(const QString& filePath)
{
   QFile file(filePath);
   //打开filePath目录下的.json文件

   if (!file.open(QIODevice::ReadOnly)) {
       qWarning() << "无法打开文件:" << filePath;
       return;
   }

   QByteArray jsonData = file.readAll();
   file.close();

   QJsonDocument doc = QJsonDocument::fromJson(jsonData);
   if (doc.isNull()) {
       qWarning() << "JSON解析失败";
       return;
   }

   QJsonObject root = doc.object();
   extractParameters(root);
}
SaveInputProject::~SaveInputProject()
{

}
double SaveInputProject::getDs() {
   return ds_; 
}

double SaveInputProject::getF0() {
   return f0_;  
}
int SaveInputProject::getMaxphi() {
   return maxphi_; 
}
int SaveInputProject::getMaxtheta() {
   return maxtheta_; 
}
int SaveInputProject::getNumphi() {
   return numphi_; 
}
int SaveInputProject::getNumtheta() {
   return numtheta_; 
}
int SaveInputProject::getNWidth() {
   return n_width_;
}
int SaveInputProject::getMDepth() {
   return m_depth_; 
}
void SaveInputProject::getSourcePara_(std::vector<double>& source_para) {
   	source_para.resize(source_para_.size());
	for (int i = 0; i < source_para_.size(); i++)
	{
		source_para[i] = source_para_[i];
	}
}
void SaveInputProject::getTransRotatePara_(std::vector<double>& trans_rotate_para) {
   	trans_rotate_para.resize(trans_rotate_para_.size());
	for (int i = 0; i < trans_rotate_para_.size(); i++)
	{
		trans_rotate_para[i] = trans_rotate_para_[i];
	}	
}


QString SaveInputProject::getNonradomePath() {
   return nonradomePath_;
}
QString SaveInputProject::getResultPath() {
   return resultPath_; 
}
QString SaveInputProject::getSourcePath() {
   return sourcePath; 
}
int SaveInputProject::getSourceMode() {
   return source_save_flag_; 
}


void SaveInputProject::extractParameters(const QJsonObject& root) {
    // 提取背景材料参数
    QJsonObject backgroundMaterial = root["background_material"].toObject();
    double eps0 = backgroundMaterial["eps0"].toDouble();
    double lossT = backgroundMaterial["lossT"].toDouble();
    double mu0 = backgroundMaterial["mu0"].toDouble();
    qDebug() << "背景材料参数: eps0 =" << eps0 << ", lossT =" << lossT << ", mu0 =" << mu0;

    // 提取网格参数
    double ds = root["ds"].toDouble();
    qDebug() << "网格参数: ds =" << ds;

    // 提取NM参数
    f0_ = root["f0"].toDouble();
    qDebug() << "频率参数: f0 =" << f0_;

    //提取源参数
    n_width_ = root["N_width"].toInt();
    m_depth_ = root["M_depth"].toInt();
    qDebug() << "NM参数: n_width =" << n_width_ << ", m_depth =" << m_depth_;

    //提取源参数
    QJsonArray source = root["para_meta"].toArray();
    // std::vector<double> source_para;
    for (const QJsonValue& value : source) {
        double para = value.toObject()["para_"].toDouble();
        source_para_.push_back(para);
    }
    //提取源的平移旋转参数
    QJsonArray trans_rotate = root["trans_rotate_para"].toArray();
    for (const QJsonValue& value : trans_rotate) {
        double para = value.toObject()["para_trans_rotate"].toDouble();
        trans_rotate_para_.push_back(para);
    }
    // //输出源参数
    // for (int i = 0; i < source_para_.size(); i++)
    // {
    //     qDebug() << "源参数: para =" << source_para_[i];
    // }

    // 提取远场参数
    QJsonObject farfield = root["farfield"].toObject();
    int maxphi = farfield["maxphi"].toInt();
    int maxtheta = farfield["maxtheta"].toInt();
    int numphi = farfield["numphi"].toInt();
    int numtheta = farfield["numtheta"].toInt();
    qDebug() << "远场参数: maxphi =" << maxphi << ", maxtheta =" << maxtheta
             << ", numphi =" << numphi << ", numtheta =" << numtheta;

    // 提取模型材料参数
    QJsonArray modelMeta = root["model_meta"].toArray();
    index_.resize(modelMeta.size()); // 调整数组大小
    name_.resize(modelMeta.size());
    modelEps0_.resize(modelMeta.size());
    modelLossT_.resize(modelMeta.size());
    modelMu0_.resize(modelMeta.size());
    int i = 0;
    for (const QJsonValue& value : modelMeta) {
        QJsonObject material = value.toObject()["material"].toObject();
        index_[i] = material["index"].toInt();
        //读取name并从utf-8编码格式转换为local8bit、std::string格式
        QString q_material_name = material["name"].toString();
        QByteArray local8bit_data = QTextCodec::codecForLocale()->fromUnicode(q_material_name);
        name_[i] = local8bit_data.toStdString();

        modelEps0_[i] = material["eps0"].toDouble();
        modelLossT_[i] = material["lossT"].toDouble();
        modelMu0_[i] = material["mu0"].toDouble();
        qDebug() << "模型材料参数: eps0 =" << modelEps0_[i] << ", lossT =" << modelLossT_[i] << ", mu0 =" << modelMu0_[i] <<",name =" << q_material_name << ",index =" << index_[i];
        i++;
    }

    // 提取输出路径参数
    QJsonObject output = root["output"].toObject();
    int isCalcNonradome = output["is_calc_nonradome"].toInt();
    QString nonradomePath = output["nonradome_path"].toString();
    QString resultPath = output["result_path"].toString();
    qDebug() << "输出路径参数: is_calc_nonradome =" << isCalcNonradome
             << ", nonradome_path =" << nonradomePath
             << ", result_path =" << resultPath;

    // 提取源模式参数和源txt文件路径
    int sourceMode = root["source_mode"].toInt();
    sourcePath = root["source_path"].toString();
    qDebug() << "源模式参数: source_mode =" << sourceMode
             << ", source_path =" << sourcePath;

    // 提取STL文件路径
    QJsonArray stlMeta = root["stl_meta"].toArray();
    for (const QJsonValue& value : stlMeta) {
        QString stlPath = value.toObject()["stl_path"].toString();
        qDebug() << "STL文件路径:" << stlPath;
    }
    // //stl文件数量赋值给stl_num_
    // int stl_num;
    // stl_num_ = stlMeta.size();
    //将上面从.json文件中提取出来的stl文件路径赋值给file_name_line_vec_
    //在赋值给file_name_line_vec_之前，先将向量清空
    file_name_line_vec_.clear();
    for (const QJsonValue& value : stlMeta) {
        QString stlPath = value.toObject()["stl_path"].toString();
        file_name_line_vec_.push_back(stlPath);
    }
    //先将stl文件表示的壳转换成片，并进行片的路径的设置为显示做准备
    SetInputStl();

    // 提取线程数
    int threads = root["threads"].toInt();
    qDebug() << "线程数:" << threads;

    // 提取版本号
    int version = root["version"].toInt();
    qDebug() << "版本号:" << version;

    //提取源保存标志位
    source_save_flag_ = root["source_save_flag"].toInt();
    qDebug() << "源保存标志位:" << source_save_flag_;
}


void SaveInputProject::SetInputStl() {
   results_.clear();
   int i = 0;
   for (auto file_name_line : file_name_line_vec_) {
       //将std::vector<QString>类型中的向量赋值给std::string类型的file_name
       std::string file_name = file_name_line.toStdString();
       results_.push_back(file_name);
       i++;
   }
   GenSurface();
}
void SaveInputProject::GenMeta() {

   Json::Value js;
   js["version"] = 1;
   std::string output_path = std::string(QCoreApplication::applicationDirPath().toStdString() + "/output/stl");
   // std::string output_path = filePath_.toStdString() + "/output/stl";
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

   std::ofstream outfile(dir_path_load + "/stl_surface_meta.json");
   if (!outfile.is_open())
   {
       return;
   }
   outfile << js.toStyledString();
   outfile.close();
}

void SaveInputProject::GenSurface() {
   dir_path_load = QCoreApplication::applicationDirPath().toStdString() + "/output/stl_surface";
   // dir_path_load = filePath_.toStdString() + "/output/stl_surface";
   QDir dir;
   dir.mkpath(dir_path_load.c_str());
   GenMeta(); 
   QProcess process;

   if (GlobalConfig::Instance()->IsQuickCalcByPython()) {
       process.start("python ",
           QStringList() << QCoreApplication::applicationDirPath() + "/script/quick_calc_py/stlsplit.py"
           << std::string(dir_path_load + "/stl_surface_meta.json").c_str());
   }
   else {
       std::string exe_file = std::string(QCoreApplication::applicationDirPath().toStdString()
           + "/script/quick_calc/stlsplit.exe ");
       process.start(exe_file.c_str(),
           QStringList() << std::string(dir_path_load + "/stl_surface_meta.json").c_str());
   }
   // 等待进程结束，并显示进度条
   while (process.state() != QProcess::NotRunning) {
      QCoreApplication::processEvents();
   }
}


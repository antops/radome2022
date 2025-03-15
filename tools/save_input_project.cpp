#include "save_input_project.h"

SaveInputProject::SaveInputProject(const QString& filePath)
{
   QFile file(filePath);
   //��filePathĿ¼�µ�.json�ļ�

   if (!file.open(QIODevice::ReadOnly)) {
       qWarning() << "�޷����ļ�:" << filePath;
       return;
   }

   QByteArray jsonData = file.readAll();
   file.close();

   QJsonDocument doc = QJsonDocument::fromJson(jsonData);
   if (doc.isNull()) {
       qWarning() << "JSON����ʧ��";
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
    // ��ȡ�������ϲ���
    QJsonObject backgroundMaterial = root["background_material"].toObject();
    double eps0 = backgroundMaterial["eps0"].toDouble();
    double lossT = backgroundMaterial["lossT"].toDouble();
    double mu0 = backgroundMaterial["mu0"].toDouble();
    qDebug() << "�������ϲ���: eps0 =" << eps0 << ", lossT =" << lossT << ", mu0 =" << mu0;

    // ��ȡ�������
    double ds = root["ds"].toDouble();
    qDebug() << "�������: ds =" << ds;

    // ��ȡNM����
    f0_ = root["f0"].toDouble();
    qDebug() << "Ƶ�ʲ���: f0 =" << f0_;

    //��ȡԴ����
    n_width_ = root["N_width"].toInt();
    m_depth_ = root["M_depth"].toInt();
    qDebug() << "NM����: n_width =" << n_width_ << ", m_depth =" << m_depth_;

    //��ȡԴ����
    QJsonArray source = root["para_meta"].toArray();
    // std::vector<double> source_para;
    for (const QJsonValue& value : source) {
        double para = value.toObject()["para_"].toDouble();
        source_para_.push_back(para);
    }
    //��ȡԴ��ƽ����ת����
    QJsonArray trans_rotate = root["trans_rotate_para"].toArray();
    for (const QJsonValue& value : trans_rotate) {
        double para = value.toObject()["para_trans_rotate"].toDouble();
        trans_rotate_para_.push_back(para);
    }
    // //���Դ����
    // for (int i = 0; i < source_para_.size(); i++)
    // {
    //     qDebug() << "Դ����: para =" << source_para_[i];
    // }

    // ��ȡԶ������
    QJsonObject farfield = root["farfield"].toObject();
    int maxphi = farfield["maxphi"].toInt();
    int maxtheta = farfield["maxtheta"].toInt();
    int numphi = farfield["numphi"].toInt();
    int numtheta = farfield["numtheta"].toInt();
    qDebug() << "Զ������: maxphi =" << maxphi << ", maxtheta =" << maxtheta
             << ", numphi =" << numphi << ", numtheta =" << numtheta;

    // ��ȡģ�Ͳ��ϲ���
    QJsonArray modelMeta = root["model_meta"].toArray();
    index_.resize(modelMeta.size()); // ���������С
    name_.resize(modelMeta.size());
    modelEps0_.resize(modelMeta.size());
    modelLossT_.resize(modelMeta.size());
    modelMu0_.resize(modelMeta.size());
    int i = 0;
    for (const QJsonValue& value : modelMeta) {
        QJsonObject material = value.toObject()["material"].toObject();
        index_[i] = material["index"].toInt();
        //��ȡname����utf-8�����ʽת��Ϊlocal8bit��std::string��ʽ
        QString q_material_name = material["name"].toString();
        QByteArray local8bit_data = QTextCodec::codecForLocale()->fromUnicode(q_material_name);
        name_[i] = local8bit_data.toStdString();

        modelEps0_[i] = material["eps0"].toDouble();
        modelLossT_[i] = material["lossT"].toDouble();
        modelMu0_[i] = material["mu0"].toDouble();
        qDebug() << "ģ�Ͳ��ϲ���: eps0 =" << modelEps0_[i] << ", lossT =" << modelLossT_[i] << ", mu0 =" << modelMu0_[i] <<",name =" << q_material_name << ",index =" << index_[i];
        i++;
    }

    // ��ȡ���·������
    QJsonObject output = root["output"].toObject();
    int isCalcNonradome = output["is_calc_nonradome"].toInt();
    QString nonradomePath = output["nonradome_path"].toString();
    QString resultPath = output["result_path"].toString();
    qDebug() << "���·������: is_calc_nonradome =" << isCalcNonradome
             << ", nonradome_path =" << nonradomePath
             << ", result_path =" << resultPath;

    // ��ȡԴģʽ������Դtxt�ļ�·��
    int sourceMode = root["source_mode"].toInt();
    sourcePath = root["source_path"].toString();
    qDebug() << "Դģʽ����: source_mode =" << sourceMode
             << ", source_path =" << sourcePath;

    // ��ȡSTL�ļ�·��
    QJsonArray stlMeta = root["stl_meta"].toArray();
    for (const QJsonValue& value : stlMeta) {
        QString stlPath = value.toObject()["stl_path"].toString();
        qDebug() << "STL�ļ�·��:" << stlPath;
    }
    // //stl�ļ�������ֵ��stl_num_
    // int stl_num;
    // stl_num_ = stlMeta.size();
    //�������.json�ļ�����ȡ������stl�ļ�·����ֵ��file_name_line_vec_
    //�ڸ�ֵ��file_name_line_vec_֮ǰ���Ƚ��������
    file_name_line_vec_.clear();
    for (const QJsonValue& value : stlMeta) {
        QString stlPath = value.toObject()["stl_path"].toString();
        file_name_line_vec_.push_back(stlPath);
    }
    //�Ƚ�stl�ļ���ʾ�Ŀ�ת����Ƭ��������Ƭ��·��������Ϊ��ʾ��׼��
    SetInputStl();

    // ��ȡ�߳���
    int threads = root["threads"].toInt();
    qDebug() << "�߳���:" << threads;

    // ��ȡ�汾��
    int version = root["version"].toInt();
    qDebug() << "�汾��:" << version;

    //��ȡԴ�����־λ
    source_save_flag_ = root["source_save_flag"].toInt();
    qDebug() << "Դ�����־λ:" << source_save_flag_;
}


void SaveInputProject::SetInputStl() {
   results_.clear();
   int i = 0;
   for (auto file_name_line : file_name_line_vec_) {
       //��std::vector<QString>�����е�������ֵ��std::string���͵�file_name
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
   // �ȴ����̽���������ʾ������
   while (process.state() != QProcess::NotRunning) {
      QCoreApplication::processEvents();
   }
}


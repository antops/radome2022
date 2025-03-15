#include <QFile>
#include <QProcess>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QFileDialog>
#include <QGroupBox>
#include <QMessageBox>
#include <QDir>
#include <QCoreApplication>
#include <vtkjsoncpp/json/json.h>
#include <fstream>
#include <QTextCodec>
#include "Qt/global_config.h"

class SaveInputProject
{
public:
	SaveInputProject(const QString& filePath);
    ~SaveInputProject() ;

    const std::vector<std::string>& GetResults() const {
	    return results_;
    }

    const std::vector<std::string>& GetSurfaceResults() const {
        return surface_results_;
    }
    void SetInputStl();
    void GenMeta();
    void GenSurface();

    double getDs();
    double getF0();
    int getMaxphi();
    int getMaxtheta();
    int getNumphi();
    int getNumtheta();
    QString getNonradomePath();//TODO
    QString getResultPath();//TODO
    QString getSourcePath();
    int getSourceMode();
    int getMDepth();
    int getNWidth();
    void getSourcePara_(std::vector<double>& source_para);
    void getTransRotatePara_(std::vector<double>& trans_rotate_para_);
    void SetFilePath(const QString& filePath){
        filePath_ = filePath;
    }
    int GetSourceSaveFlag();
    ////0305:介质材料配置
    void GetMaterialIndex(std::vector<int>& index){
        index.resize(index_.size());
        for (int i = 0; i < index_.size(); i++)
        {
            index[i] = index_[i];
        }

    }
    void GetMaterialName(std::vector<std::string>& name){
        name.resize(name_.size());
        for (int i = 0; i < name_.size(); i++)
        {
            name[i] = name_[i];
        }

    }
    void GetMaterialEps(std::vector<double>& modelEps0){
        modelEps0.resize(modelEps0_.size());
        for (int i = 0; i < modelEps0_.size(); i++)
        {
            modelEps0[i] = modelEps0_[i];
        }	
    }
    void GetMaterialMu(std::vector<double>& modelMu0){
        modelMu0.resize(modelMu0_.size());
        for (int i = 0; i < modelMu0_.size(); i++)
        {
            modelMu0[i] = modelMu0_[i];
        }	
    }
    void GetMaterialLoss(std::vector<double>& modelLossT){
        modelLossT.resize(modelLossT_.size());
        for (int i = 0; i < modelLossT_.size(); i++)
        {
            modelLossT[i] = modelLossT_[i];
        }	
    }
    ////
private:

    void extractParameters(const QJsonObject& root);

private:
	QString filePath_;
    double ds_;
    double f0_;
    int maxphi_;
    int maxtheta_;
    int numphi_;
    int numtheta_;
    int m_depth_;
    int n_width_;
    QString nonradomePath_;
    QString resultPath_;
    QString sourcePath;
    int sourceMode_;
	std::vector<QString> file_name_line_vec_;//输入的文件名 注意变量格式 设置set函数直接设置该变量，将变量作为输入
	// std::vector<QPushButton*> btn_vec_;////用不上，
	std::vector<std::string> results_;//放文件名的中间变量
	std::vector<std::string> surface_results_;//中间变量 放生成的表面文件路径
	std::string dir_path_load;
    std::vector<double> source_para_;
    std::vector<double> trans_rotate_para_;
    ////0305:介质材料配置
    std::vector<int> index_ = std::vector<int>(10);
    std::vector<std::string> name_ = std::vector<std::string>(10);
    std::vector<double> modelEps0_ = std::vector<double>(10);
    std::vector<double> modelMu0_ = std::vector<double>(10);
    std::vector<double> modelLossT_ = std::vector<double>(10);
    ////
    int source_save_flag_;
};
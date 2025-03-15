#pragma once

#include "../../common/include/GraphTrans.h"
#include "../../common/include/farfield_base.h"

#include <vtkSmartPointer.h>
#include <vtkImageActor.h>
#include <vtkActor.h>
#include <QTreeWidgetItem>

using namespace Common;

struct FarFieldParam {
	bool is_dB_unit = true;
	double max = 0.0;
	double min = -40.0;
};

class FarField
{
public:
	FarField(const std::string& name);
	~FarField();

	void SetName(const std::string& name) {
		name_ = name;
	}

	const std::string& GetName() {
		return name_;
	}
	
	bool ReadDataFromFile(const std::string& file);
	bool ReadFromNpy(const std::string &fileName);

	bool ReadTblFromNpy(const std::string& fileName);
	bool WriteDataToFile(const std::string& file) const;

	bool CreateActor3D();
	bool CreateActor2D();
	vtkSmartPointer<vtkActor> GetActor() { return actor3D_; }
	vtkSmartPointer<vtkImageActor>  GetActor2D() { return actor_; }

	QTreeWidgetItem* GetTree();
	void SetFarFieldParam(const FarFieldParam& param) { param_ = param; }
	const FarFieldParam& GetFarFieldParam() { return param_; }
	void SetIndex(int index) { index_ = index; }

	const std::vector<double>& GetTheta() { return theta; }
	const std::vector<double>& GetPhi() { return phi; }
	const std::vector<std::vector<double>>& GetE() { return E; }
	const std::vector<std::vector<double>>& GetEDB() { return EDB; }
	const std::vector<std::vector<double>>& GetEDBOrigin() { return EDB_origin; }

	void SetPicFilePath(const std::string& pic_file_path) {
		pic_file_path_ = pic_file_path;
	}

	const std::string& GetPicFilePath() const { return pic_file_path_; }

	bool IsCalcTBL() const { return is_clac_tbl_; }
	double GetTBL() const { return tbl_val_; }

	std::string GetFileNpyPath() {
		return file_name_;
	}
private:
	vtkSmartPointer<vtkActor> actor3D_;
	vtkSmartPointer<vtkImageActor> actor_;

	FarFieldBase far_field_base_;

	// œ‘ æ≤Œ ˝
	FarFieldParam param_;

	std::vector<double> theta;
	std::vector<double> phi;
	std::vector<std::vector<double>> E;
	std::vector<std::vector<double>> EDB;
	std::vector<std::vector<double>> EDB_origin;

	std::vector<std::vector<double>> EDB_trans;

	std::string name_;
	int index_ = 0;
	double min_liner = INT32_MAX;
	double max_liner = INT32_MIN;
	double min_db = INT32_MAX;
	double max_db = INT32_MIN;

	std::string pic_file_path_;
	double tbl_val_ = 0.0;
	bool is_clac_tbl_ = false;
	std::string file_name_;
};
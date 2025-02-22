#include <fstream>

#include "calc_tbl.h"
#include "../Qt/global_config.h"
#include <vtkjsoncpp/json/json.h>
#include <QProcess>
#include <QCoreApplication>

bool CalcTbl::Run()
{
	Json::Value js;
	js["version"] = 1;
	Json::Value& js_output = js["output"];
	js_output["nonradome_path"] = far_field_a_->GetFileNpyPath();
	js_output["result_path"] = far_field_b_->GetFileNpyPath();

	std::string tbl_path = QCoreApplication::applicationDirPath().toStdString() + "/output/tbl_meta.json";
	std::ofstream outfile(tbl_path);
	if (!outfile.is_open())
	{
		return false;
	}
	outfile << js.toStyledString();
	outfile.close();

	QProcess process;
	if (GlobalConfig::Instance()->IsQuickCalcByPython()) {
		process.start("python ",
			QStringList() << QCoreApplication::applicationDirPath() + "/script/quick_calc_py/tblCalc.py"
			<< tbl_path.c_str());
	}
	else {
		std::string exe_file = std::string(QCoreApplication::applicationDirPath().toStdString()
			+ "/script/quick_calc/tblCalc.exe ");
		process.start(exe_file.c_str(),
			QStringList() << tbl_path.c_str());
	}

	process.waitForFinished();

	std::string status_file = far_field_b_->GetFileNpyPath() + "/tbl_status.txt";
	std::ifstream ifs;
	ifs.open(status_file, ios::in);
	bool is_success = false;
	if (ifs.is_open()) {
		string temp;
		getline(ifs, temp);
		if (!temp.empty() && temp[0] == '1') {
			is_success = true;
		}
		ifs.close();
	}
	if (!is_success) {
		return false;
	}
	far_field_b_->ReadTblFromNpy(far_field_b_->GetFileNpyPath());

	return true;
}

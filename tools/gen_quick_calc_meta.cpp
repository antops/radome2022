#include <fstream>

#include "gen_quick_calc_meta.h"
#include "../Qt/global_config.h"


bool GenQuickCalcMeta::WriteMetaMsg(const std::string & path)
{
	Json::Value js;
	js["version"] = 1;
	js["threads"] = GlobalConfig::Instance()->GraphicsThread();

	auto* mat = data_manager_.GetMaterialData(1);
	if (mat == nullptr) {
		return false;
	}
	js["background_material"] = mat->ToJson();

	Json::Value& js_model_meta = js["stl_meta"];
	for (const auto& x : stl_path_) {
		Json::Value js_model;
		js_model["stl_path"] = x;
		
		js_model_meta.append(js_model);
	}

	const auto& materials_index = data_manager_.GetRadomeData()->GetAllMaterials();

	Json::Value& js_material_meta = js["model_meta"];
	for (const auto& index : materials_index) {
		Json::Value js_model;
		mat = data_manager_.GetMaterialData(index);
		if (mat == nullptr) {
			return false;
		}
		js_model["material"] = mat->ToJson();
		js_material_meta.append(js_model);
	}

	js["source_path"] = source_path_;
	js["f0"] = fre_;
	js["ds"] = ds_;
	js["source_mode"] = polarization_type_;

	Json::Value& js_farfield = js["farfield"];
	js_farfield["maxtheta"] = 20;
	js_farfield["numtheta"] = 101;
	js_farfield["maxphi"] = 360;
	js_farfield["numphi"] = 181;

	Json::Value& js_output = js["output"];
	if (is_calc_non_radome_) {
		js_output["is_calc_nonradome"] = 1;
		js_output["nonradome_path"] = result_non_radome_path_;
	}
	else {
		js_output["is_calc_nonradome"] = 0;
	}
	js_output["result_path"] = result_path_;

	std::ofstream outfile(path);
	if (!outfile.is_open())
	{
		return false;
	}
	outfile << js.toStyledString();
	outfile.close();
	return true;
}

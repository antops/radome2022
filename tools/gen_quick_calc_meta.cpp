#include <fstream>

#include "gen_quick_calc_meta.h"
#include "../Qt/global_config.h"


bool GenQuickCalcMeta::WriteMetaMsg(const std::string & path)
{
	Json::Value js;
	js["version"] = 1;
	js["threads"] = GlobalConfig::Instance()->GraphicsThread();

	auto* mat = new MaterialData("����", 1);
	mat->SetMaterialData(1.0, 1.0, 0.0);
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
	js["source_diff_flag"] = source_diff_flag_;
	js["zero_depth_d"] = zero_depth_d_;

	Json::Value& js_farfield = js["farfield"];
	js_farfield["maxtheta"] = conf_.max_theta;
	js_farfield["numtheta"] = conf_.num_theta;
	js_farfield["maxphi"] = conf_.max_phi;
	js_farfield["numphi"] = conf_.num_phi;
	js_farfield["minphi"] = conf_.min_phi;
	js_farfield["mintheta"] = conf_.min_theta;

	Json::Value& js_output = js["output"];
	if (is_calc_non_radome_) {
		js_output["is_calc_nonradome"] = 1;
		js_output["nonradome_path"] = result_non_radome_path_;
	}
	else {
		js_output["is_calc_nonradome"] = 0;
	}
	js_output["result_path"] = result_path_;

	Json::Value js_graph_trans;
	js_graph_trans["originX"] = graph_trans_.getTrans_x();
	js_graph_trans["originY"] = graph_trans_.getTrans_x();
	js_graph_trans["originZ"] = graph_trans_.getTrans_x();

	js_graph_trans["rotUnum"] = graph_trans_.getRotate_x_num();
	js_graph_trans["rotVnum"] = graph_trans_.getRotate_y_num();
	js_graph_trans["rotNnum"] = graph_trans_.getRotate_z_num();

	js_graph_trans["rotU"] = graph_trans_.getRotate_x_theta();
	js_graph_trans["rotV"] = graph_trans_.getRotate_y_theta();
	js_graph_trans["rotN"] = graph_trans_.getRotate_z_theta();

	js["source_pos"] = js_graph_trans;

	std::ofstream outfile(path);
	if (!outfile.is_open())
	{
		return false;
	}
	outfile << js.toStyledString();
	outfile.close();
	return true;
}

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
    ////0311
	if(data_manager_.GetRadomeData())
	{
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
	}
	////
	js["source_path"] = source_path_;
	js["f0"] = fre_;
	js["ds"] = ds_;
	js["source_mode"] = polarization_type_;
    ////0221:保存导入源的para
	Json::Value& js_model_meta_2 = js["para_meta"];
	for (const auto& x : source_para_) {
		Json::Value js_model;
		js_model["para_"] = x;
		
		js_model_meta_2.append(js_model);
	}
	//保存平移旋转参数
	Json::Value& js_model_meta_3 = js["trans_rotate_para"];
	for (const auto& x : trans_rotate_para_) {
		Json::Value js_model;
		js_model["para_trans_rotate"] = x;

		js_model_meta_3.append(js_model);
	}
	js["M_depth"] = m_depth_;
	js["N_width"] = n_width_;
    ////
	////0225:设置source_save_flag
	js["source_save_flag"] = source_save_flag_;
	////
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

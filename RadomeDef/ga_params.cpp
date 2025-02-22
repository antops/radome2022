#include "ga_params.h"
#include <vtkCubeSource.h>
#include <vtkPolyDataMapper.h>

GaParams::GaParams()
{
}

GaParams::~GaParams()
{
}

bool GaParams::WriteMetaMsg(const std::string& path, const std::string& result_path) const {
	Json::Value js;
	js["version"] = version;
	js["n"] = model_meta_vec.size();

	Json::Value& js_model_meta = js["model_meta"];
	for (const auto& x : model_meta_vec) {
		Json::Value js_model;
		Json::Value& para_meta = js_model["para"];
		para_meta["d_min"] = x.d_min;
		para_meta["d_max"] = x.d_max;
		para_meta["e_min"] = x.e_min;
		para_meta["e_max"] = x.e_max;
		js_model_meta.append(js_model);
	}
	js["size_pop"] = size_pop;
	js["prob_mut"] = prob_mut;
	js["max_iter"] = max_iter;

	js["f0"] = 17;

	js["result_path"] = result_path;

	std::ofstream outfile(path);
	if (!outfile.is_open())
	{
		return false;
	}
	outfile << js.toStyledString();
	outfile.close();
	return true;
}
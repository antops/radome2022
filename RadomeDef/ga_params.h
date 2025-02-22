#pragma once


#include <vtkjsoncpp/json/json.h>

class GaParams
{
public:
	GaParams();
	~GaParams();

	struct GaModelMeta {
		double d_min;
		double d_max;
		double e_min;
		double e_max;
	};

	bool WriteMetaMsg (const std::string& path, const std::string& result_path) const;

public:
	int version = 1;
	int size_pop = 50;
	double prob_mut;
	int max_iter;
	std::vector<GaModelMeta> model_meta_vec;
};

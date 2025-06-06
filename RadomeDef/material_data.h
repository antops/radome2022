#pragma once
#include "tree_param_base.h"
#include <string>

#include <vtkjsoncpp/json/json.h>

class MaterialData : public TreeParamBase
{
public:
	MaterialData() {}
	MaterialData(const std::string& name, int index);
	virtual ~MaterialData();

	void Update();
	void SetMaterialData(double eps0, double mu0, double lossT);
	void SetIndex(int index) {
		index_ = index;
	}
	void SetName(const std::string& name) {
		name_ = name;
	}

	// HexMesh::MaterialParam GetMaterialParam() const;

	const std::string& GetName() const { return name_; }

	double GetEps0() const { return eps0_; }
	double GetMu0() const { return mu0_; }
	double GetLossT() const { return lossT_; }

	Json::Value ToJson() {
		Json::Value js;
		js["eps0"] = eps0_;
		js["mu0"] = mu0_;
		js["lossT"] = lossT_;
		return js;
	}
private:
	double eps0_ = 0.0;
	double mu0_ = 0.0;
	double lossT_ = 0.0;
	std::string name_;
	int index_;
};

class MaterialDataFactory {
public:
	enum MaterialDataType
	{
		MaterialMetal = 0,
		MaterialAir = 1,
	};
	static MaterialData* GetMaterialData(MaterialDataType type);
};

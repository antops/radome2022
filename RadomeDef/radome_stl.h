#pragma once
// 旋转对称天线罩

#include <vector>
#include "radome_data_base.h"


class RadomeSTL : public RadomeBaseData
{
public:
	RadomeSTL();
	~RadomeSTL();

	bool ReadSTLFromFile(const std::vector<std::string>& paths);

	virtual bool Update(std::string* err_msg = NULL) override;
	virtual void UpdateTreeItem() override;
	virtual void Mesh(const MeshOption& option, int* step) override;
	void SetSurfacePathVec(const std::vector<std::string>& surface_path_vec) {
		surface_path_vec_ = surface_path_vec;
	}
	virtual bool SaveToStl(const std::string& path, std::vector<std::string>* path_vec) {
		for (int i = surface_path_vec_.size() - 1; i >= 0; i--) {
			path_vec->push_back(surface_path_vec_[i]);
		}
		return true;
	}

protected:
	bool UpdateSurface() override { return true; }

private:
	std::string path_;
	std::vector<std::string> surface_path_vec_;
};


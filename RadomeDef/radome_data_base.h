#pragma once
// 天线罩数据base 类

#include <vtkActor.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vector>
#include "vtkProperty.h"
#include "tree_param_base.h"
#include "../def.h"
#include "../../common/include//Vector3.h"

using namespace Common;

struct MeshOption {
	double scale;
	// mesh 的盒子大小
	Vector3 min_p;
	Vector3 max_p;
};


class RadomeBaseData : public TreeParamBase
{
public:
	RadomeBaseData() {};
	virtual ~RadomeBaseData() {};

	virtual bool Update(std::string* err_msg = NULL) = 0;
	virtual void UpdateTreeItem() = 0;
	virtual void Mesh(const MeshOption& option, int* step) = 0;
	virtual bool GetMaxXYZ(Vector3* min_p, Vector3* max_p);
	bool IsMesh() { return is_mesh_; }

	void SetSelected(int num) {
		// num小于0 表示全部选中
		if (num < 0) {
			for (auto& actor : actor_vec_) {
				actor->GetProperty()->SetOpacity(1);
			}
		}
		else if (num < actor_vec_.size()) {
			actor_vec_[num]->GetProperty()->SetOpacity(1);
		}

		
	}
	void SetUnSelected(int num) {
		for (auto& actor : actor_vec_) {
			actor->GetProperty()->SetOpacity(0.4);
		}
	}

	int GetMaterialId(int num) const {
		if (material_vec_.size() <= num || num < 0) return -1;
		return material_vec_[num];
	}

	void SetMaterialId(int num, int material_id, const std::string& name) {
		if (material_vec_.size() <= num || num < 0) return;
		material_vec_[num] = material_id;
		material_name_vec_[num] = name;
	}

	const std::vector<int>& GetAllMaterials() const {
		return material_vec_;
	}

	const std::vector<vtkSmartPointer<vtkActor>>& GetActors() { return actor_vec_; }
	const std::vector<vtkSmartPointer<vtkPolyData>>& GetPolyDatas() { return polyData_vec_; }
	const vtkSmartPointer<vtkActor>& GetMeshActor() { return actor_mesh_; }
	// 面
	virtual bool SaveToStl(const std::string& path, std::vector<std::string>* path_vec);
	// 体
	bool SaveToStlVolume(const std::string& path, std::vector<std::string>* path_vec);
	// 点
	virtual bool SaveToPoints(const std::string& path, std::vector<std::string>* path_vec);

	int GetType() const {
		return type_;
	}

protected:
	virtual bool UpdateSurface() = 0;
	vtkSmartPointer<vtkActor> actor_mesh_;
	std::vector<vtkSmartPointer<vtkActor>> actor_vec_;
	std::vector<vtkSmartPointer<vtkPolyData>> polyData_vec_;
	std::vector<vtkSmartPointer<vtkPolyData>> surface_poly_data_vec_;

	// 材料
	std::vector<int> material_vec_;
	std::vector<std::string> material_name_vec_;
	//mesh结果
	// HexMesh hex_mesh_;
	bool is_mesh_ = false;
	int background_material_index_ = 1; // 默认空气
	bool is_need_update_ = true;

	int type_ = -1;
	const int COLOR_POOL_SIZE = 3;
	double COLOR_RADOME_POOL[3][3]{
		{1.0,0.0,0.0},
		{ 0.0,0.0,1.0 },
		{ 0.0,1.0,1.0 }
	};
};




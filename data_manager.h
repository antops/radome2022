#pragma once
#include "RadomeDef/radome_data_base.h"
#include <vector>

#include "RadomeDef/limit_box.h"
#include "RadomeDef/rotate_param/rotate_line_base.h"
#include "RadomeDef/material_data.h"

#include "VTK/field.h"
#include "VTK/farfield.h"

class DataManager
{
public:
	DataManager();
	~DataManager();

	bool ResetRadome(RadomeBaseData* radome_data);
	RadomeBaseData* GetRadomeData() const;

	bool ResetLimitBox(LimitBox* limit_box);
	LimitBox* GetLimitBox();

	bool SetMaterialData(int index, MaterialData* data);
	MaterialData* GetMaterialData(int index) const;
	int GetMaterialId();
	bool RemoveMaterialData(int index);

	const std::map<int, MaterialData*>& GetMaterialDataAll() {return material_book_;}

	Field* GetSource() const { return source_field_; }
	void SetSource(Field* source);
	bool IsNeedCalcSourceFarfield() const {return is_need_calc_farfield_;}
	void CalcSourceFarfieldDone() { is_need_calc_farfield_ = false; }

	FarField* GetFarField(int index);
	int SetFarSource(FarField* source);
	FarField* GetSourceFarField();
	int SetSourceFarField(FarField* far_field);

	const std::map<int, FarField*>& GetAllFarFieldMap() { return far_field_map_; }

private:
	RadomeBaseData* radome_data_ = nullptr; // 天线罩数据结构
	// 源
	Field* source_field_ = nullptr;
	bool is_need_calc_farfield_ = false;  // 是否需要计算无罩的远场，和源绑定

	// 远场
	std::map<int,FarField*> far_field_map_;
	int far_index_ = 100; //  0 为源

	// 材料
	// 0~100系统保留   1 空气，0金属，2介质
	// >100自定义介质
	std::map<int, MaterialData*> material_book_ ;
	int custom_index_ = 100;
	
	// 盒子
	LimitBox* limit_box_ = nullptr;

	int background_material = 1; // 默认空气
};

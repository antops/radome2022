#pragma once
#include "RadomeDef/radome_data_base.h"
#include <vector>

#include "RadomeDef/limit_box.h"
#include "RadomeDef/rotate_param/rotate_line_base.h"
#include "RadomeDef/material_data.h"

#include "VTK/field.h"
#include "VTK/farfield.h"
#include "RadomeDef/radome_stl.h"


class DataManager
{
public:
	DataManager();
	~DataManager();
	////0220:���ԭ��������
    // ������ȡ��ǰRadomeSTL����ķ���
    RadomeSTL* GetCurrentRadome() const {
        return current_radome_;
    }

    // �޸�ResetRadome����������ɶ���
    void ResetRadome_(RadomeSTL* new_radome) {
        if (current_radome_ != nullptr) {
            //delete current_radome_;
            current_radome_ = nullptr;
        }
        current_radome_ = new_radome;
    }
	////
	////0308
	void DataManager::SyncMaterialIndex();
	////
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

	////0220:���ԭ��������
    RadomeSTL* current_radome_ = nullptr;
	////
	RadomeBaseData* radome_data_ = nullptr; // ���������ݽṹ
	// Դ
	Field* source_field_ = nullptr;
	bool is_need_calc_farfield_ = false;  // �Ƿ���Ҫ�������ֵ�Զ������Դ��

	// Զ��
	std::map<int,FarField*> far_field_map_;
	int far_index_ = 100; //  0 ΪԴ

	// ����
	// 0~100ϵͳ����   1 ������0������2����
	// >100�Զ������
	std::map<int, MaterialData*> material_book_ ;
	int custom_index_ = 100;
	
	// ����
	LimitBox* limit_box_ = nullptr;

	int background_material = 1; // Ĭ�Ͽ���
};


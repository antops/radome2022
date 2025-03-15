#include "data_manager.h"
#include "RadomeDef/radome_rotate.h"

DataManager::DataManager()
{
}

DataManager::~DataManager()
{
}

bool DataManager::ResetRadome(RadomeBaseData* radome_data)
{
	if (radome_data_) {
		delete radome_data_;
	}
	radome_data_ = radome_data;
	return true;
}

RadomeBaseData* DataManager::GetRadomeData() const {
	return radome_data_;
}

bool DataManager::ResetLimitBox(LimitBox* limit_box) {
	if (limit_box_) {
		delete limit_box_;
	}
	limit_box_ = limit_box;
	return true;
}

LimitBox* DataManager::GetLimitBox() {
	return limit_box_;
}

bool DataManager::SetMaterialData(int index, MaterialData*  data)
{
	auto tmp = GetMaterialData(index);
	if (tmp) {
		delete tmp;
	}
	material_book_[index] = data;
	return true;
}

MaterialData * DataManager::GetMaterialData(int index) const
{
	auto it = material_book_.find(index);
	if (it != material_book_.end()) return it->second;
	return nullptr;
}

int DataManager::GetMaterialId()
{
	int index = custom_index_;
	////0307
	custom_index_++;
	//if (material_book_.empty()) {
	//	custom_index_ = 0;
	//}
	//else {
	//	auto max_key = std::max_element(material_book_.begin(), material_book_.end(),
	//		[](auto& a, auto& b) { return a.first < b.first; })->first;
	//	custom_index_ = max_key + 1;
	//}
	////
	return index;
}

void DataManager::SyncMaterialIndex() {
	if (material_book_.empty()) {
		custom_index_ = 0;
	}
	else {
		auto max_key = std::max_element(material_book_.begin(), material_book_.end(),
			[](auto& a, auto& b) { return a.first < b.first; })->first;
		custom_index_ = max_key + 1;
	}
}

bool DataManager::RemoveMaterialData(int index)
{
	auto it = material_book_.find(index);
	if (it != material_book_.end()) {
		delete it->second;
		material_book_.erase(it);
		return true;
	}
	return false;
}

void DataManager::SetSource(Field * source)
{
	if (source_field_) {
		delete source_field_;
	}
	source_field_ = source;
	is_need_calc_farfield_ = true;
}

FarField * DataManager::GetFarField(int index)
{
	auto it = far_field_map_.find(index);
	if (it != far_field_map_.end()) return it->second;
	return nullptr;
}

int DataManager::SetFarSource(FarField * far_field)
{
	int index = far_index_;
	far_field_map_[index] = far_field;
	far_index_++;
	return index;
}

FarField * DataManager::GetSourceFarField()
{
	return GetFarField(0);
}

int DataManager::SetSourceFarField(FarField * far_field)
{
	if (GetFarField(0) != nullptr) {
		delete GetFarField(0);
	}
	far_field_map_[0] = far_field;
	return 0;
}


#include <fstream>

#include "gen_FDTD_meta.h"
#include "../Qt/global_config.h"


bool GenFDTDCalcMeta::WriteMetaMsg(const std::string & path)
{
	std::ofstream outfile(path);
	if (!outfile.is_open())
	{
		return false;
	}
	const auto& materials_index = data_manager_.GetRadomeData()->GetAllMaterials();
	outfile << fre_ << std::endl;
	outfile << point_path_.size() - 1 << std::endl;
	outfile << cpu_core_ << std::endl;
	for (auto& index : materials_index) {
		auto mat = data_manager_.GetMaterialData(index);
		if (mat == nullptr) {
			return false;
		}
		outfile << mat->GetEps0() << " " << mat->GetMu0() << std::endl;
	}
	for (int i = point_path_.size() - 1; i >= 0; --i) {
		outfile << point_path_[i] << std::endl;
	}
	outfile << mesh_type_ << std::endl;
	outfile << 0 << std::endl;
	outfile << source_path_ << std::endl;
	double ExcPos = 0.0;
	if (data_manager_.GetSource()->getGraphTrans().getTrans_z() > 0) ExcPos = data_manager_.GetSource()->getGraphTrans().getTrans_z();
	outfile << ExcPos << std::endl;
	outfile << result_path_ << std::endl;
	outfile.close();
	return true;
}
/*
1 第1行 计算频率(单位Hz)
2 第2行 罩子层数，注意这里是三层罩子
3 第3行 CPU线程数(并行)
4 第4-6行，介电常数实部，虚部，这里对应3层，所以有3行。
5 同上 4-6行，从上到下依此是最外层，中间层，最内层
6 同上
7 第7-10行，回转曲线文件目录，注意：3层罩子，4个界面，
8 同上，7-10行，从上到下依此是最外层，中间层，最内层
9
10
11 剖分级别控制：0: 1/20, 1: 1/25, 2: 1/30; 3: 1/40.
12 回转曲线模式，详见2.2节说明
13 输入激励场文件目录 详见2.3节说明
14 输入场纵向位置 ExcPos(单位m)，详见2.2节说明
15 结果输出目录
*/

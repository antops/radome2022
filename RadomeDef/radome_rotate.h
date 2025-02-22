#pragma once
// 旋转对称天线罩

#include <vector>
#include "radome_data_base.h"

#include "rotate_param/rotate_line_base.h"


class RadomeRotate : public RadomeBaseData
{
public:
	RadomeRotate();
	RadomeRotate(const std::vector<RotateLineBase*>& lines, double x, double y, double cut);
	~RadomeRotate();

	virtual bool Update(std::string* err_msg = NULL) override;
	virtual void UpdateTreeItem() override;
	virtual void Mesh(const MeshOption& option, int* step) override;

	// 点
	bool SaveToPoints(const std::string& path, std::vector<std::string>* path_vec) override;

	const std::vector<RotateLineBase*>& GetRotateLineVec() {
		return rotate_line_vec_;
	}

protected:
	bool UpdateSurface() override;

private:
	bool JudgeLegal(const std::vector<Vector3>& line1,
					const std::vector<Vector3>& line2);
	vtkSmartPointer<vtkPolyData> GenShowPolyData(
		const std::vector<Vector3>& line1,
		const std::vector<Vector3>& line2);
	vtkSmartPointer<vtkPolyData> GenPolyData(const std::vector<Vector3>& lines);

private:
	std::vector<RotateLineBase*> rotate_line_vec_;
	double x_scale_ = 1.0;
	double y_scale_ = 1.0;
	double cut_ = -1.0;
};


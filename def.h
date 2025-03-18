#pragma once

#include <iostream>
#include <fstream>
#include <string>

// 导航树第一列
namespace Def {
	enum TreeVar1Type
	{
		default_type = -1,
		model_type = 1,
		rotate_layer_type = 11, // 标识旋转天线罩体的层数
		material_type = 21,
		farfield_type = 31,
		gaussian_source_type = 41,
		taile_source_type = 42,
		aperture_source_type = 43,
		taile_only_source_type = 44,
	};
	// 介电常数
	struct Permittivity {
		double eps = 0.0;
		double mu = 0.0;
	};

}

#define THRESHOLD 1e-8

// 读edit_line
#define PARSE_EDIT_LINE_TO_DOUBLE(edit_line, data) bool ok##edit_line;\
data = edit_line->text().toDouble(&ok##edit_line);\
if (!ok##edit_line) { \
	edit_line->setStyleSheet("background-color:rgba(255,0,0,255)");\
	return;\
}

// 读edit_line
#define PARSE_EDIT_LINE_TO_DOUBLE_RETURN_BOOL(edit_line, data) bool ok##edit_line;\
data = edit_line->text().toDouble(&ok##edit_line);\
if (!ok##edit_line) { \
	edit_line->setStyleSheet("background-color:rgba(255,0,0,255)");\
	return false;\
}

// 读edit_line
#define PARSE_EDIT_LINE_TO_DOUBLE_RETURN(edit_line, data, min, max) bool ok##edit_line;\
data = edit_line->text().toDouble(&ok##edit_line);\
if (!ok##edit_line) { \
	edit_line->setStyleSheet("background-color:rgba(255,0,0,255)");\
	return false;\
} \
if (data > max || data < min) {\
	edit_line->setStyleSheet("background-color:rgba(255,0,0,255)"); \
	return false;\
}\
edit_line->setStyleSheet("background-color:rgba(255,255,255,255)");


// 读edit_line
#define PARSE_EDIT_LINE_TO_INT_RETURN(edit_line, data, min, max) bool ok##edit_line;\
data = edit_line->text().toInt(&ok##edit_line);\
if (!ok##edit_line) { \
	edit_line->setStyleSheet("background-color:rgba(255,0,0,255)");\
	return false;\
} \
if (data > max || data < min) {\
	edit_line->setStyleSheet("background-color:rgba(255,0,0,255)"); \
	return false;\
}\
edit_line->setStyleSheet("background-color:rgba(255,255,255,255)");

// 保存
#define SAVE_WIDGET_TO_JSON(name, js) \
if (name) name->SaveParam(&js[#name]);

//
#define READ_WIDGET_TO_JSON(name, js, type) \
if (js.isMember(#name)) {\
	if (name == nullptr) name = new type; \
	name->LoadParam(js[#name]);\
}

// 保存 edit
#define SAVE_EDIT_TO_JSON(edit_name, js) \
js[#edit_name] = edit_name->text().toStdString();


// 保存 edit
#define SAVE_COMBOX_TO_JSON(name, js) \
js[#name] = name->currentIndex();

//
#define READ_EDIT_TO_JSON(edit_name, js) \
edit_name->setText(QString::fromLocal8Bit(js[#edit_name].asString().c_str()));

//
#define READ_COMBOX_TO_JSON(name, js) \
name->setCurrentIndex(js[#name].asInt())


struct CalcConf {
	double min_theta = 0.0;
	double min_phi = 0.0;
	double max_theta = 180.0;
	double max_phi = 360.0;
	int num_theta = 181;
	int num_phi = 361;
};
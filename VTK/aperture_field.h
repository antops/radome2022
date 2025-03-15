/*
*	created by liyun 2017/12/28
*   function ���ɸ�˹���泡
*	width --> data[0]  depth --> data[1]
*	scale --> data[2]  phase --> data[3]
*	line  --> data[4]  Column--> data[5]
*   isAmPhs  --> data[6] 0Ϊ�� 1Ϊ��
*   version 1.0
*/
#pragma once

#include <string>
#include <complex>
#include <vector>
#include <QString>

#include "field.h"



class ApertureField : public Field
{
public:
	ApertureField(const GraphTrans & _graphTrans,
		const std::vector<double> parameter);

	ApertureField(const ApertureField & _ApertureField);

	~ApertureField();

	void setFileAddress(const std::string & file);

	void setUnit(double _unit);

	void updateData();

	void readData();

	virtual QTreeWidgetItem* getTree();

private:

	double factor;

	std::string fileAddress;

	bool isAmPhs; // �Ƿ�����λ����ʵ���鲿

};
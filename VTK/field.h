/*
*	created by liyun 2017/12/27
*   function ��ʾ�� 3D 2D ��λ ���� dB
*   version 2.0 �ع��˱������ݵ����ݽṹ
*/
#pragma once

#include <vector>
#include <fstream>
#include <complex>
#include <String>

#include "../common/include/GraphTrans.h"
//#include "../interface/FieldBase.h"

#include "../common/include/Vector3.h"

#include <vtkSmartPointer.h>
#include <vtkImageActor.h>
#include <vtkActor.h>
#include "basic_parameters.h"


class Field: public BasicParameters
{
public:
	Field();
	Field(const Field & _field); // ��д���캯��
	//Field& operator =(const Field & _field);

	virtual ~Field();

	void init();

	void freeMemory();
	void allocateMemory();

	void setNM(int N, int M);
	void getNM(int& N, int &M) const;
	void setPlane(const GraphTrans & _graphTransPara, double _ds);
	//void setField(const FieldBase &);

	void setField(complex<double> ** _Ex, complex<double> ** _Ey, complex<double> ** _Ez,
		complex<double> ** _Hx, complex<double> ** _Hy, complex<double> ** _Hz);
	void setField(const vector<vector<complex<double>>>& _Ex, const vector<vector<complex<double>>>& _Ey);
	void setField(complex<double> ** _Ex, complex<double> ** _Ey);

	const vector<vector<complex<double>>>& getEx() const;
	const vector<vector<complex<double>>>& getEy() const;

	void setDs(double _Ds);
	double getDs() const;

	void SetFre(double fre) { fre_ = fre; }
	double GetFre() const { return fre_; }

	int GetPolarizationType() const { return polarization_type_; }
	void SetPolarizationType(int polarization_type) { polarization_type_ = polarization_type; }

	int GetSourceDiffFlag() const { return source_diff_flag_; }

	//�õ����Ĳ���
	void getSourcePara(GraphTrans & _graphTransPara, 
		int & _N_width, int & _M_depth, double &_ds) const;

	void setShowPara(int _content, bool _isLinear, bool _isPhs);
	void getShowPara(int &_content, bool &_isLinear, bool &_isPhs);
	void set3D(bool _is3D);
	bool get3D() const;

	void calActor3D();
	void calActor();

	vtkSmartPointer<vtkImageActor> getActor() const;
	vtkSmartPointer<vtkActor> getActor3D() const;

	double getWidth() const { return data[0]; }
	double getDepth() const { return data[1]; }

	void SaveFieldBase(const std::string &fileName) const;
	void ReadPythonText(const std::string& fileName);
	void save(const std::string &fileName) const;
	void savesource(const std::string &fileName) const;
	void readsource(const std::string &fileName, double scale = 1);

	bool ReadFromNpy(const std::string &fileName);

	virtual void updateData();

	// �Ƿ���ʾ
	void setIsShow(bool);

	virtual QTreeWidgetItem* getTree(); 
	void SetQTreeWidgetItem(QTreeWidgetItem* tree) {
		tree_ = tree;
	}

	void setIsSource(bool);
	bool getIsSource() const { return isSource; }

	void setEfficiency(double);
	double getEfficiency() { return efficiency; }

	double getScalarCorrelationCoefficient() const { return scalarCorrelationCoefficient; }
	double getVectorCorrelationCoefficient() const { return vectorCorrelationCoefficient; }
	double getEqWaistRadius() const { return waistradius; }	//Mark 20201025
	//double getPower() const;
	//double getPowerFreqList(int _num) const;
	void clacTransEffi(double);
	void calcCorrelationCoefficient(double fre);

protected:
	//�����˹ϵ��
	//void calcCorrelationCoefficient();

	// ������
	vector<vector<complex<double>>> Ex, Ey, Ez;
	vector<vector<complex<double>>> Hx, Hy, Hz;

	int content;
	bool isLinear;
	bool isPhs;
	bool is3D;
	// ��Ҫ��ͼ������ content 0~5��ʾEx~H
	// isLinear �Ƿ�������
	// isPhs ����λ ���Ƿ���

	int N_width, M_depth;  //N_width = para[0] /ds

	double ds;
	
	vtkSmartPointer<vtkImageActor> actor;
	vtkSmartPointer<vtkActor> actor3D;

	bool isSource;
	double efficiency;
	double scalarCorrelationCoefficient;
	double vectorCorrelationCoefficient;
	double waistradius;	//Mark 20201025
	//int Name; // �ж���ʲô��

	bool isShow;

	double fre_ = 10.0e10;

	int polarization_type_ = 0; // �������� ��ֱ����-Ey- source_mode=1��ˮƽ����- Ex-source_mode=0������Դsource_mode=2
	int source_diff_flag_ = 0; // �Ͳ���Ϊ0�����Ϊ1

	QTreeWidgetItem* tree_ = nullptr;
};


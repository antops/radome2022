#pragma once
//����һ�������˹�����ֲ���ר�ú���
//�ø�˹��������Z�������򴫲�������W0λ����Z=0��
//by XD 20180205

#ifndef GAUSSIAN_BEAM_CIRCULAR_H
#define GAUSSIAN_BEAM_CIRCULAR_H

#include <iostream>
#include <cmath>
#include <complex>//�����࣬C++�Դ�
#include <string>
#include "../../common/include/Constant_Var.h"
#include "../../common/include/Vector3.h"

using namespace std;
using namespace Common;

//���ش���z���Ⱥ�ĸ�˹����Wֵ
double Gauss_Omega_Circular(double frequency0, double w0, double z0);

//����Բ��˹������P��x,y,z���ĸ���������ֵ
complex<double> Gauss_E_Circular(double frequency0, double w0, Vector3 P);

//����Բ��˹������P��x,y,z���Ļ�����ʽ��δ�۵���λֵ
double Gauss_Phase_Circular(double frequency0, double w0, Vector3 P);

#endif

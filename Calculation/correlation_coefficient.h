#pragma once

//这是一个计算场分布的矢量与标量高斯相关系数的专用函数
//输入为场正交切向场分量EX和EY（复数形式）
//每次计算一定要按顺序输入EX和EY
//by XD 20180321
//20180527将输入点阵改为M0*N0

#ifndef CORRELATION_COEFFICIENT_H
#define CORRELATION_COEFFICIENT_H

#include <iostream>
#include <cmath>
#include <complex>//复数类，C++自带
#include <string>
#include <vector>
#include "../common/include//Vector3.h"
#include "../common/include//Constant_Var.h"
#include "gaussian_beam_circular.h"

using namespace std;
using namespace Common;

//无脑计算最佳匹配高斯相关系数时的对应点的下标以及高斯波束束腰
void Correlation_Coefficient_Omega(double frequency0, double ds,int M0, int N0,
	const vector <vector <complex<double>>> &EX,
	const vector <vector <complex<double>>> &EY,//M0*N0
	int &index, int &MaxE_i, int &MaxE_j, double &W0);
//index为0,主极化为EX；index为1,主极化为EY
//MaxE_i和MaxE_j是主极化最大场强处的下标
//WO是经过匹配得到的高斯波束束腰

//无脑计算最佳匹配高斯标量相关系数
double Scalar_Correlation_Coefficient(double frequency0, double ds, int M0, int N0,
	const vector <vector <complex<double>>> &EX,
	const vector <vector <complex<double>>> &EY);

double Scalar_Correlation_Coefficient(double frequency0, double ds, int M0, int N0,
	const vector <vector <complex<double>>> &EX,
	const vector <vector <complex<double>>> &EY, double& _wasitradius);	//Mark 20201025


//无脑计算最佳匹配高斯矢量相关系数
double Vector_Correlation_Coefficient(double frequency0, double ds, int M0, int N0,
	const vector <vector <complex<double>>> &EX,
	const vector <vector <complex<double>>> &EY);

//无脑计算最佳匹配高斯矢量相关系数
double Vector_Correlation_Coefficient(double frequency0, double ds, int M0, int N0,
	const vector <vector <complex<double>>> &EX,
	const vector <vector <complex<double>>> &EY, double& _wasitradius);

//计算和已有目标束腰W0的高斯标量相关系数
double Scalar_Correlation_Coefficient_TargetW0(double frequency0, double ds, int M0, int N0,
	const vector <vector <complex<double>>> &EX,
	const vector <vector <complex<double>>> &EY,
	double Target_W0);

//计算和已有目标束腰W0的高斯矢量相关系数
double Vector_Correlation_Coefficient_TargetW0(double frequency0, double ds, int M0, int N0,
	const vector <vector <complex<double>>> &EX,
	const vector <vector <complex<double>>> &EY,
	double Target_W0);

#endif

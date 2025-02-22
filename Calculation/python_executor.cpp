#include "python_executor.h"

#include "Python.h"

PythonExecutor::PythonExecutor()
{	//第一步：初始化Python
	//在调用Python的提供的给C的API之前，通过执行初始化
	//来添加Python的内建模块、__main__、sys等
	Py_Initialize();

	//char *argv[2];
	//argv[0] = "arg1";
	//argv[1] = "arg2";
	//PySys_SetArgv(2, argv);

	//第四步：执行调用脚本文件命令,注意文件的路径
	
}

PythonExecutor::~PythonExecutor()
{
	Py_Finalize();
}

bool PythonExecutor::Init()
{	//检查初始化是否完成
	if (!Py_IsInitialized())
	{
		return false;
	}
	PyRun_SimpleString("import sys");
	return true;
}

void PythonExecutor::SetRunPath(const std::string & path)
{

	PyRun_SimpleString("import os");
	std::string exe_cmd = "os.chdir('" + path + "')";
	PyRun_SimpleString(exe_cmd.c_str());
}

bool PythonExecutor::RunExe(const std::string & path)
{
	std::string exe_cmd = "os.system('python " + path + "')";
	PyRun_SimpleString(exe_cmd.c_str());
	return true;
}

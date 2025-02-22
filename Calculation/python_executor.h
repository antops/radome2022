#pragma once

#include <string>

class PythonExecutor {
public:
	PythonExecutor();
	~PythonExecutor();

	bool Init();

	void SetRunPath(const std::string& path);

	bool RunExe(const std::string& path);

};
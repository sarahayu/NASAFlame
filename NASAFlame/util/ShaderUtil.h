#pragma once
#include <string>

namespace ShaderUtil
{
	unsigned loadShader(const std::string &vertexFile, const std::string &fragmentFile = "");
	unsigned loadTexture(const std::string &file);
};
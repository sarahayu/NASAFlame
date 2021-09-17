#pragma once

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace ShaderUtil
{
	unsigned loadShader(const std::string &vertexFile, const std::string &fragmentFile = "");
	unsigned loadTexture(const std::string &file);
	unsigned createColorBuffer(const glm::vec2 &dimensions, const unsigned &attachment = GL_COLOR_ATTACHMENT0);
	void reloadColorBuffer(const unsigned &textureID, const glm::vec2 &newDimensions);
};
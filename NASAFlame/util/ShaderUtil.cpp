#include "ShaderUtil.h"
#include <SFML/Graphics.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <array>
#include <glm/glm.hpp>

namespace
{
	void load(const GLenum & shaderType, const std::string & file, unsigned & shader)
	{
		int success;
		char infoLog[512];

		std::ifstream shaderFile;
		shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		std::stringstream shaderStream;

		try
		{
			shaderFile.open(file);
			shaderStream << shaderFile.rdbuf();
		}
		catch (...)
		{
			std::cout << "\nShader file could not be read!";
		}

		std::string shaderString = shaderStream.str();
		const char *shaderCode = shaderString.c_str();
		const bool isVertex = shaderType == GL_VERTEX_SHADER;
		shader = glCreateShader(shaderType);
		glShaderSource(shader, 1, &shaderCode, NULL);
		glCompileShader(shader);
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 512, NULL, infoLog);
			std::cout << "\n" << (isVertex ? "Vertex" : "Fragment") << " compilation failed! " << infoLog;

		}
	}
}

unsigned ShaderUtil::loadShader(const std::string & vertexFile, const std::string & fragmentFile)
{
	unsigned vert, frag;
	int success;
	char infoLog[512];

	load(GL_VERTEX_SHADER, "resources/shaders/" + vertexFile + ".vs", vert);
	load(GL_FRAGMENT_SHADER, "resources/shaders/" + (fragmentFile == "" ? vertexFile : fragmentFile) + ".fs", frag);

	unsigned ID = glCreateProgram();
	glAttachShader(ID, vert);
	glAttachShader(ID, frag);
	glLinkProgram(ID);
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(ID, 512, NULL, infoLog);
		std::cout << "\nShader linking failed!";
	}
	else
	{
		std::cout << "\nShader '" << vertexFile << "' successfully loaded!";
	}
	glDeleteShader(vert);
	glDeleteShader(frag);

	return ID;
}

unsigned ShaderUtil::loadTexture(const std::string & file)
{
	sf::Image image;
	if (!image.loadFromFile("resources/images/" + file)) throw std::runtime_error("Could not load file '" + file + "'!");

	unsigned textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.getSize().x, image.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());
	glGenerateMipmap(GL_TEXTURE_2D);/*
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);*/
	return textureID;
}
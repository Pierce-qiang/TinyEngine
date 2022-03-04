#include "Shader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "glm/gtc/type_ptr.hpp"

namespace TEngine {
	Shader::Shader(const std::unordered_map<std::string, std::string>& shaderPaths)
	{
		std::vector<ShaderSourceInfo> shaderSources;
		std::string vertexCode;
		std::string fragmentCode;
		std::ifstream vertexFile;
		std::ifstream fragmentFile;

		for (auto it = shaderPaths.begin(); it != shaderPaths.end(); it++)
		{
			GLenum shaderType = ShaderTypeFromString(it->first);
			std::string path = it->second;

			std::ifstream shaderFile;
			shaderFile.open(path);

			// read files and close
			std::stringstream shaderStream;
			shaderStream << shaderFile.rdbuf();
			shaderFile.close();

			// convert to string
			std::string shaderCode = shaderStream.str();

			// store the shader code
			shaderSources.push_back({ shaderType,path, shaderCode });
		}

		CompileShaderSource(shaderSources);
	}

	// TODO: give more info in checkerror, need to create a new structure here maybe.
	void Shader::CompileShaderSource(const std::vector<ShaderSourceInfo>& shaderSources)
	{
		mShaderID = glCreateProgram();

		// compile each shader
		for (auto& item : shaderSources)
		{
			GLenum type = item.shaderType;
			const std::string& source = item.shaderSource;
			const std::string& path = item.shaderPath;


			unsigned int shader = glCreateShader(type);
			const char* shaderSource = source.c_str();
			glShaderSource(shader, 1, &shaderSource, NULL);
			glCompileShader(shader);

			// TODO:Change GL_ENUM type to string for error message
			//std::cout << shaderSource << ": \n";
			CheckCompileErrors(shader, "COMPILE", path);

			glAttachShader(mShaderID, shader);
			glDeleteShader(shader);
		}

		// link shader
		glLinkProgram(mShaderID);
		CheckCompileErrors(mShaderID, "LINK", "");
	}

	void Shader::CheckCompileErrors(GLuint shader, std::string type, std::string path)
	{
		GLint success;
		GLchar infoLog[1024];
		if (type != "LINK")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "Fail TO COMPILE" << path << "\n";
				std::cout << "ERROR::SHADER_COMPILATION_ERROR: " << "\n" << infoLog
					<< "\n -- --------------------------------------------------- -- " << std::endl;
			}

		}
		else if (type == "LINK")
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::SHADER_LINKING_ERROR: \n" << infoLog
					<< "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}

	}





	Shader::~Shader()
	{
		glDeleteProgram(mShaderID);
	}

	void Shader::Bind()
	{
		glUseProgram(mShaderID);
	}

	void Shader::UnBind()
	{
		glUseProgram(0);
	}

	void Shader::SetUniform(const char* name, float value)
	{
		glUniform1f(glGetUniformLocation(mShaderID, name), value);
	}

	void Shader::SetUniform(const char* name, int value)
	{
		glUniform1i(glGetUniformLocation(mShaderID, name), value);
	}

	void Shader::SetUniform(const char* name, const glm::vec2& vector)
	{
		glUniform2f(glGetUniformLocation(mShaderID, name), vector.x, vector.y);
	}

	void Shader::SetUniform(const char* name, const glm::ivec2& vector)
	{
		glUniform2i(glGetUniformLocation(mShaderID, name), vector.x, vector.y);
	}

	void Shader::SetUniform(const char* name, const glm::vec3& vector)
	{
		glUniform3f(glGetUniformLocation(mShaderID, name), vector.x, vector.y, vector.z);
	}

	void Shader::SetUniform(const char* name, const glm::ivec3& vector)
	{
		glUniform3i(glGetUniformLocation(mShaderID, name), vector.x, vector.y, vector.z);
	}

	void Shader::SetUniform(const char* name, const glm::vec4& vector)
	{
		glUniform4f(glGetUniformLocation(mShaderID, name), vector.x, vector.y, vector.z, vector.w);
	}

	void Shader::SetUniform(const char* name, const glm::ivec4& vector)
	{
		glUniform4i(glGetUniformLocation(mShaderID, name), vector.x, vector.y, vector.z, vector.w);
	}

	void Shader::SetUniform(const char* name, const glm::mat3& matrix)
	{
		glUniformMatrix3fv(glGetUniformLocation(mShaderID, name), 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void Shader::SetUniform(const char* name, const glm::mat4& matrix)
	{
		glUniformMatrix4fv(glGetUniformLocation(mShaderID, name), 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void Shader::SetUniformArray(const char* name, int arraySize, float* value)
	{
		glUniform1fv(glGetUniformLocation(mShaderID, name), arraySize, value);
	}

	void Shader::SetUniformArray(const char* name, int arraySize, int* value)
	{
		glUniform1iv(glGetUniformLocation(mShaderID, name), arraySize, value);
	}

	void Shader::SetUniformArray(const char* name, int arraySize, glm::vec2* value)
	{
		glUniform2fv(glGetUniformLocation(mShaderID, name), arraySize, glm::value_ptr(*value));
	}

	void Shader::SetUniformArray(const char* name, int arraySize, glm::ivec2* value)
	{
		glUniform2iv(glGetUniformLocation(mShaderID, name), arraySize, glm::value_ptr(*value));
	}

	void Shader::SetUniformArray(const char* name, int arraySize, glm::vec3* value)
	{
		glUniform3fv(glGetUniformLocation(mShaderID, name), arraySize, glm::value_ptr(*value));
	}

	void Shader::SetUniformArray(const char* name, int arraySize, glm::ivec3* value)
	{
		glUniform3iv(glGetUniformLocation(mShaderID, name), arraySize, glm::value_ptr(*value));
	}

	void Shader::SetUniformArray(const char* name, int arraySize, glm::vec4* value)
	{
		glUniform4fv(glGetUniformLocation(mShaderID, name), arraySize, glm::value_ptr(*value));
	}

	void Shader::SetUniformArray(const char* name, int arraySize, glm::ivec4* value)
	{
		glUniform4iv(glGetUniformLocation(mShaderID, name), arraySize, glm::value_ptr(*value));
	}

	GLenum Shader::ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex")
			return GL_VERTEX_SHADER;
		else if (type == "fragment")
			return GL_FRAGMENT_SHADER;
		else if (type == "geometry")
			return GL_GEOMETRY_SHADER;
		else if (type == "hull")
			return GL_TESS_CONTROL_SHADER;
		else if (type == "domain")
			return GL_TESS_EVALUATION_SHADER;
		else if (type == "compute")
			return GL_COMPUTE_SHADER;
		else
			std::cout << "unknown shader type:" << type << "\n";

		return 0;
	}
}
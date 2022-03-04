#pragma once
#include <string>
#include <unordered_map>

#include "glad/glad.h"
#include "glm/glm.hpp"

namespace TEngine {
	struct ShaderSourceInfo
	{
		GLenum shaderType;
		std::string shaderPath;
		std::string shaderSource;
	};

	class Shader
	{
	public:
		// the pair is type - path, e.g. "vertex" - "./example.vert"
		Shader(const std::unordered_map<std::string, std::string>& shaderPaths);
		~Shader();

		void Bind();
		void UnBind();

		void SetUniform(const char* name, float value);
		void SetUniform(const char* name, int value);
		void SetUniform(const char* name, const glm::vec2& vector);
		void SetUniform(const char* name, const glm::ivec2& vector);
		void SetUniform(const char* name, const glm::vec3& vector);
		void SetUniform(const char* name, const glm::ivec3& vector);
		void SetUniform(const char* name, const glm::vec4& vector);
		void SetUniform(const char* name, const glm::ivec4& vector);
		void SetUniform(const char* name, const glm::mat3& matrix);
		void SetUniform(const char* name, const glm::mat4& matrix);

		void SetUniformArray(const char* name, int arraySize, float* value);
		void SetUniformArray(const char* name, int arraySize, int* value);
		void SetUniformArray(const char* name, int arraySize, glm::vec2* value);
		void SetUniformArray(const char* name, int arraySize, glm::ivec2* value);
		void SetUniformArray(const char* name, int arraySize, glm::vec3* value);
		void SetUniformArray(const char* name, int arraySize, glm::ivec3* value);
		void SetUniformArray(const char* name, int arraySize, glm::vec4* value);
		void SetUniformArray(const char* name, int arraySize, glm::ivec4* value);

		inline unsigned int getShaderID() { return mShaderID; }
	private:
		GLenum ShaderTypeFromString(const std::string& type);
		void CompileShaderSource(const std::vector<ShaderSourceInfo>& shaderSources);
		void CheckCompileErrors(GLuint shader, std::string type, std::string path);


	private:
		unsigned int mShaderID;
	};
}



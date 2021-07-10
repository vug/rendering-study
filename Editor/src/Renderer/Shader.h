#pragma once

#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

// TODO: Remove
typedef unsigned int GLenum;

class Shader {
public:
	Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
	Shader(const std::string& filepath);
	~Shader();

	void Bind() const;
	void Unbind() const;

	void UploadUniformInt(const std::string& name, int value);
	void UploadUniformFloat(const std::string& name, float value);
	void UploadUniformFloat2(const std::string& name, const glm::vec2& values);
	void UploadUniformFloat3(const std::string& name, const glm::vec3& values);
	void UploadUniformFloat4(const std::string& name, const glm::vec4& values);

	void UploadUniformMat2(const std::string& name, const glm::mat2& matrix);
	void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
	void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);
private:
	std::string ReadFile(const std::string& filepath);
	std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
	void Compile(std::unordered_map<GLenum, std::string>& shaderSources);
private:
	uint32_t rendererID;
};
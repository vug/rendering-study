#pragma once

#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

// TODO: Remove
typedef unsigned int GLenum;

class Shader {
public:
	Shader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
	Shader(const std::string& filepath);
	~Shader();

	void Bind() const;
	void Unbind() const;

	const std::string& GetName() const { return name; }

	void UploadUniformInt(const std::string& name, int value);
	void UploadUniformFloat(const std::string& name, float value);
	void UploadUniformFloat2(const std::string& name, const glm::vec2& values);
	void UploadUniformFloat3(const std::string& name, const glm::vec3& values);
	void UploadUniformFloat4(const std::string& name, const glm::vec4& values);

	void UploadUniformMat2(const std::string& name, const glm::mat2& matrix);
	void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
	void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);

	void UploadUniformFloats(const std::string& name, const std::vector<float>& values);
	void UploadUniformFloat2s(const std::string& name, const std::vector<glm::vec2>& values);
	void UploadUniformFloat3s(const std::string& name, const std::vector<glm::vec3>& values);
	void UploadUniformFloat4s(const std::string& name, const std::vector<glm::vec4>& values);
private:
	std::string ReadFile(const std::string& filepath);
	std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
	void Compile(std::unordered_map<GLenum, std::string>& shaderSources);
private:
	uint32_t rendererID;
	std::string name;
};

class ShaderLibrary {
public: 
	void Add(const std::string& name, const std::shared_ptr<Shader>& shader);
	void Add(const std::shared_ptr<Shader>& shader);
	std::shared_ptr<Shader> Load(const std::string& filepath);
	std::shared_ptr<Shader> Load(const std::string& name, const std::string& filepath);

	std::shared_ptr<Shader> Get(const std::string& name);
	bool Exists(const std::string& name) const;

	static ShaderLibrary& Instance() { static ShaderLibrary instance; return instance; }
	ShaderLibrary(ShaderLibrary const&) = delete;
	ShaderLibrary& operator=(ShaderLibrary const&) = delete;
private:
	ShaderLibrary() = default;
	std::unordered_map<std::string, std::shared_ptr<Shader>> shaders;
};
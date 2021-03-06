#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "glad/glad.h"
#include "glm/gtc/type_ptr.hpp"

#include "Shader.h"

static GLenum ShaderTypeFromString(const std::string& type) {
	if (type == "vertex")
		return GL_VERTEX_SHADER;
	if (type == "fragment" || type == "pixel") 
		return GL_FRAGMENT_SHADER;
	if (type == "geometry")
		return GL_GEOMETRY_SHADER;

	assert(false); // Unknown shader type
	return 0;
}

Shader::Shader(const std::string& filepath) {
	std::string source = ReadFile(filepath);
	auto shaderSources = PreProcess(source);
	Compile(shaderSources);

	std::filesystem::path path = filepath;
	name = path.stem().string();
}

Shader::Shader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource) 
	: name(name) {
	std::unordered_map<GLenum, std::string> sources;
	sources[GL_VERTEX_SHADER] = vertexSource;
	sources[GL_FRAGMENT_SHADER] = fragmentSource;
	Compile(sources);
}

Shader::~Shader() {
	glDeleteProgram(rendererID);
}

std::string Shader::ReadFile(const std::string& filepath) {
	std::ifstream in(filepath, std::ios::in | std::ios::binary);
	std::string result;
	if (in) {
		in.seekg(0, std::ios::end);
		result.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&result[0], result.size());
		in.close();
	}
	else {
		std::cerr << "Could not open file " << filepath << std::endl;
	}
	return result;
}

std::unordered_map<GLenum, std::string> Shader::PreProcess(const std::string& source) {
	std::unordered_map<GLenum, std::string> shaderSources;

	const char* typeToken = "#type";
	size_t typeTokenLength = strlen(typeToken);
	size_t pos = source.find(typeToken, 0);
	while (pos != std::string::npos) {
		size_t eol = source.find_first_of("\r\n", pos);
		assert(eol != std::string::npos); // Syntax Error
		size_t begin = pos + typeTokenLength + 1;
		std::string type = source.substr(begin, eol - begin);
		assert(ShaderTypeFromString(type)); // Invalid shader type specification

		size_t nextLinePos = source.find_first_not_of("\r\n", eol);
		pos = source.find(typeToken, nextLinePos);
		shaderSources[ShaderTypeFromString(type)] = (pos == std::string::npos) ? 
			source.substr(nextLinePos) : 
			source.substr(nextLinePos, pos - nextLinePos);
	}

	return shaderSources;
}

void Shader::Compile(std::unordered_map<GLenum, std::string>& shaderSources) {
	GLuint program = glCreateProgram();
	assert(shaderSources.size() <= 3); // We only support a geometry, a vertex and a fragment shader for now.
	std::array<GLenum, 3> glShaderIDs;
	int glShaderIdIndex = 0;
	for (auto& kv : shaderSources) {
		GLenum type = kv.first;
		const std::string& source = kv.second;

		// Taken from https://www.khronos.org/opengl/wiki/Shader_Compilation#Example and modified
		GLuint shader = glCreateShader(type);

		const GLchar* sourceCStr = (const GLchar*)source.c_str();
		glShaderSource(shader, 1, &sourceCStr, 0);

		glCompileShader(shader);

		GLint isCompiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);

		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

			glDeleteShader(shader);

			std::cerr << "Shader compilation failure." << std::endl
				<< infoLog.data() << std::endl;

			return;
		}

		glAttachShader(program, shader);
		glShaderIDs[glShaderIdIndex++] = shader;
	}

	glLinkProgram(program);

	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

		glDeleteProgram(program);
		for (auto id : glShaderIDs) 
			glDeleteShader(id);

		std::cerr << "Shader link failure." << std::endl
			<< infoLog.data() << std::endl;

		return;
	}

	for (auto id : glShaderIDs) {
		glDetachShader(program, id);
		glDeleteShader(id);
	}

	rendererID = program;
}

void Shader::Bind() const {
	glUseProgram(rendererID);
}

void Shader::Unbind() const {
	glUseProgram(0);
}

void Shader::UploadUniformInt(const std::string& name, int value) {
	GLint location = glGetUniformLocation(rendererID, name.c_str());
	glUniform1i(location, value);
}

void Shader::UploadUniformFloat(const std::string& name, float value) {
	GLint location = glGetUniformLocation(rendererID, name.c_str());
	glUniform1f(location, value);
}

void Shader::UploadUniformFloat2(const std::string& name, const glm::vec2& values) {
	GLint location = glGetUniformLocation(rendererID, name.c_str());
	glUniform2f(location, values.x, values.y);
}

void Shader::UploadUniformFloat3(const std::string& name, const glm::vec3& values) {
	GLint location = glGetUniformLocation(rendererID, name.c_str());
	glUniform3f(location, values.x, values.y, values.z);
}

void Shader::UploadUniformFloat4(const std::string& name, const glm::vec4& values) {
	GLint location = glGetUniformLocation(rendererID, name.c_str());
	glUniform4f(location, values.x, values.y, values.z, values.w);
}

void Shader::UploadUniformMat2(const std::string& name, const glm::mat2& matrix) {
	GLint location = glGetUniformLocation(rendererID, name.c_str());
	glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix) {
	GLint location = glGetUniformLocation(rendererID, name.c_str());
	glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix) {
	GLint location = glGetUniformLocation(rendererID, name.c_str());
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::UploadUniformFloats(const std::string& name, const std::vector<float>& values) {
	GLint location = glGetUniformLocation(rendererID, name.c_str());
	glUniform1fv(location, values.size(), values.data());
}

void Shader::UploadUniformFloat2s(const std::string& name, const std::vector<glm::vec2>& values) {
	GLint location = glGetUniformLocation(rendererID, name.c_str());
	glUniform2fv(location, values.size() * 2, glm::value_ptr(values.data()[0]));
}

void Shader::UploadUniformFloat3s(const std::string& name, const std::vector<glm::vec3>& values) {
	GLint location = glGetUniformLocation(rendererID, name.c_str());
	glUniform3fv(location, values.size() * 3, glm::value_ptr(values.data()[0]));
}

void Shader::UploadUniformFloat4s(const std::string& name, const std::vector<glm::vec4>& values) {
	GLint location = glGetUniformLocation(rendererID, name.c_str());
	glUniform4fv(location, values.size() * 4, glm::value_ptr(values.data()[0]));
}

// Shader Library
void ShaderLibrary::Add(const std::string& name, const std::shared_ptr<Shader>& shader) {
	assert(!Exists(name)); // Shader already exists!
	shaders[name] = shader;
}

void ShaderLibrary::Add(const std::shared_ptr<Shader>& shader) {
	auto& name = shader->GetName();
	Add(name, shader);
}

std::shared_ptr<Shader> ShaderLibrary::Load(const std::string& filepath) {
	auto shader = std::make_shared<Shader>(filepath);
	Add(shader);
	return shader;
}

std::shared_ptr<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath) {
	auto shader = std::make_shared<Shader>(filepath);
	Add(name, shader);
	return shader;
}

std::shared_ptr<Shader> ShaderLibrary::Get(const std::string& name) {
	assert(Exists(name)); // Shader not found!
	return shaders[name];
}

bool ShaderLibrary::Exists(const std::string& name) const {
	return shaders.find(name) != shaders.end();
}


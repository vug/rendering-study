#include <iostream>
#include <vector>

#include "glad/glad.h"
#include "glm/gtc/type_ptr.hpp"

#include "Shader.h"

Shader::Shader(const std::string& vertexSource, const std::string& fragmentSource) {
	// Copy-pasted from: https://www.khronos.org/opengl/wiki/Shader_Compilation#Example

	// Create an empty vertex shader handle
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

	// Send the vertex shader source code to GL
	// Note that std::string's .c_str is NULL character terminated.
	const GLchar* source = (const GLchar*)vertexSource.c_str();
	glShaderSource(vertexShader, 1, &source, 0);

	// Compile the vertex shader
	glCompileShader(vertexShader);

	GLint isCompiled = 0;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

		// We don't need the shader anymore.
		glDeleteShader(vertexShader);

		// Use the infoLog as you see fit.
		std::cerr << "Vertex shader compilation failure." << std::endl
			<< infoLog.data() << std::endl;

		// In this simple program, we'll just leave
		return;
	}

	// Create an empty fragment shader handle
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	// Send the fragment shader source code to GL
	// Note that std::string's .c_str is NULL character terminated.
	source = (const GLchar*)fragmentSource.c_str();
	glShaderSource(fragmentShader, 1, &source, 0);

	// Compile the fragment shader
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);

		// We don't need the shader anymore.
		glDeleteShader(fragmentShader);
		// Either of them. Don't leak shaders.
		glDeleteShader(vertexShader);

		// Use the infoLog as you see fit.
		std::cerr << "Fragment shader compilation failure." << std::endl
			<< infoLog.data() << std::endl;

		// In this simple program, we'll just leave
		return;
	}

	// Vertex and fragment shaders are successfully compiled.
	// Now time to link them together into a program.
	// Get a program object.
	rendererID = glCreateProgram();
	GLuint program = rendererID;

	// Attach our shaders to our program
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	// Link our program
	glLinkProgram(program);

	// Note the different functions here: glGetProgram* instead of glGetShader*.
	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

		// We don't need the program anymore.
		glDeleteProgram(program);
		// Don't leak shaders either.
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		// Use the infoLog as you see fit.
		std::cerr << "Shader link failure." << std::endl
			<< infoLog.data() << std::endl;

		// In this simple program, we'll just leave
		return;
	}

	// Always detach shaders after a successful link.
	glDetachShader(program, vertexShader);
	glDetachShader(program, fragmentShader);
}

Shader::~Shader() {
	glDeleteProgram(rendererID);
}

void Shader::Bind() const {
	glUseProgram(rendererID);
}

void Shader::Unbind() const {
	glUseProgram(0);
}

void Shader::UploadUniformFloat4(const std::string& name, const glm::vec4& values) {
	GLint location = glGetUniformLocation(rendererID, name.c_str());
	glUniform4f(location, values.x, values.y, values.z, values.w);
}

void Shader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
{
	GLint location = glGetUniformLocation(rendererID, name.c_str());
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}


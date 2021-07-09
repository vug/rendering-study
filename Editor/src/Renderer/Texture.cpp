#include "Texture.h"

#include <cassert>

#include <glad/glad.h>
#include <stb/stb_image.h>

Texture2D::Texture2D(const std::string& path) 
	: path(path) {
	int w, h, channels;
	stbi_set_flip_vertically_on_load(1); // OpenGL and stbi are expecting image layout in opposite vertical directions.
	stbi_uc* data = stbi_load(path.c_str(), &w, &h, &channels, 0);
	assert(data); // Failed to load image
	width = w;
	height = h;

	GLenum internalFormat = 0, dataFormat = 0; // storage format, format of file
	if (channels == 4) {
		internalFormat = GL_RGBA8;
		dataFormat = GL_RGBA;
	}
	else if (channels == 3) {
		internalFormat = GL_RGB8;
		dataFormat = GL_RGB;
	}
	assert(internalFormat && dataFormat); // Format not supported.

	glCreateTextures(GL_TEXTURE_2D, 1, &rendererID);
	// GL_RED for one channel texture
	glTextureStorage2D(rendererID, 1, internalFormat, width, height);

	glTextureParameteri(rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(rendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTextureSubImage2D(rendererID, 0, 0, 0, width, height, dataFormat, GL_UNSIGNED_BYTE, data);

	stbi_image_free(data);
}

Texture2D::~Texture2D() {
	glDeleteTextures(1, &rendererID);
}

void Texture2D::Bind(uint32_t slot) const {
	glBindTextureUnit(slot, rendererID);
}

//std::shared_ptr<Texture2D> Texture2D::Create(const std::string& path) {}
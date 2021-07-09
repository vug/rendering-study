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

	glCreateTextures(GL_TEXTURE_2D, 1, &rendererID);
	glTextureStorage2D(rendererID, 1, GL_RGB8, width, height);

	glTextureParameteri(rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(rendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTextureSubImage2D(rendererID, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);

	stbi_image_free(data);
}

Texture2D::~Texture2D() {
	glDeleteTextures(1, &rendererID);
}

void Texture2D::Bind(uint32_t slot) const {
	glBindTextureUnit(slot, rendererID);
}

//std::shared_ptr<Texture2D> Texture2D::Create(const std::string& path) {}
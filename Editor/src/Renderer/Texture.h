#pragma once

#include <memory>
#include <stdint.h>
#include <string>

class Texture {
public:
	virtual ~Texture() = default;
	virtual uint32_t GetWidth() const = 0;
	virtual uint32_t GetHeight() const = 0;
	virtual void Bind(uint32_t slot = 0) const = 0;
};

class Texture2D : public Texture {
public:
	Texture2D(const std::string& path);
	~Texture2D();

	uint32_t GetWidth() const { return width; }
	uint32_t GetHeight() const { return height; }

	void Bind(uint32_t slot = 0) const;

	//static std::shared_ptr<Texture2D> Create(const std::string& path);
private:
	std::string path;
	uint32_t width, height;
	uint32_t rendererID;
};

#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../vendor/stb_image.h"
//~ #include "vendor/stb_image_write.h"
Texture::Texture():data(),id(0),width(0), height(0), bpp(0)
{	
	//~ std::cout << "creating texture object... " << "\n";	
}

Texture::Texture(const Texture& other):
	data(other.data),
	id(other.id),
	width(other.width), 
	height(other.height),
	bpp(other.bpp)
	
{
	printf("copy texture ...\n");
}

void Texture::load(std::string path)
{
	
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	
	stbi_set_flip_vertically_on_load(1);
	unsigned char* buffer = stbi_load(path.c_str(), &width, &height, &bpp, 4);
	
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

	

	
	glBindTexture(GL_TEXTURE_2D, 0);
	
	if( buffer == nullptr)
	{
		std::cout << "stbi_failure_reason()\n";
		std::cout << stbi_failure_reason() << "\n";
		is_valid = false;
	}
	
	if(buffer)
	{
		data = std::vector<unsigned char>(buffer, buffer + width * height * 4);
		stbi_image_free(buffer);	
		is_valid = true;
	}
}

void Texture::setData(int _width, int _height, unsigned char* buffer, int _bpp)
{
	
	//~ glDeleteTextures(1, &id);
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
		
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &buffer[0]);

	glBindTexture(GL_TEXTURE_2D, 0);	
	
	data.clear();
	data = std::vector<unsigned char>(buffer, buffer + _width * _height * _bpp);
	
	is_valid = true;
	width = _width;
	height = _height;
	setBPP(_bpp);
	//~ std::cout << "__ setting texture data : " << _width << "/" << _height  << "\n";
}

void Texture::bind(){
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, id);
}

void Texture::unbind(){
	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture()
{
	//~ std::cout << "deleting texture -- ID : " << id << "\n";
	glDeleteTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, 0);
	
}


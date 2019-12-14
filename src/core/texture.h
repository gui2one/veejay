#ifndef TEXTURE_H
#define TEXTURE_H
#include "../../pch.h"


#include <iostream>
class Texture
{
	public:
		
		Texture();
		Texture(const Texture& other);
		
		virtual ~Texture();
		void load(std::string path);
		void setData(int width, int height, unsigned char* buffer, int bpp = 4);

		inline unsigned int getID(){ return id; }
		inline int getWidth(){ return width; }
		inline int getHeight(){ return height; }
		inline int getBPP(){ return bpp; }
		inline void setBPP(int _bpp){ bpp = _bpp;}
		void bind();
		void unbind();
		
		bool is_valid = false;
		std::vector<unsigned char> data;
	private:
		unsigned int id;

		int width, height, bpp;	
	
		/* add your private declarations */
};

#endif /* TEXTURE_H */ 

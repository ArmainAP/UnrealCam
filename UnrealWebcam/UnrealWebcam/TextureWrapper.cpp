
#include "stdafx.h"
#include "TextureWrapper.h"

namespace mray
{
TextureWrapper::TextureWrapper() 
{
	_data = 0;
	_width = 0;
	_height = 0;
}
TextureWrapper::~TextureWrapper()
{
	if (_data)
		delete[] _data;
	_data = 0;
}

bool TextureWrapper::SendImage(const unsigned char* data, int width, int height)
{
	if (!data)
		return false;

	if (!Ready())
		return false;

	return SharedImageWrapper::SendImage(data, width, height);

	return false;
}

}



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

bool TextureWrapper::SendImage(void* _TextureNativePtr)
{
	if (!_TextureNativePtr)
		return false;

	if (!Ready())
		return false;

	/////
	/*TODO IMPLEMENT THE SHIT*/
	////


	return false;
}

}


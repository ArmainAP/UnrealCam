#include "stdafx.h"
#include "SharedImageWrapper.h"

using namespace mray;

extern "C" bool __declspec(dllexport) SendTexture(const unsigned char* data, int width, int height)
{
	return SharedImageWrapper::SendImage(data, width, height);
}

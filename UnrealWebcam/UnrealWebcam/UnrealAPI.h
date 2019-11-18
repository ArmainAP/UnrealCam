#include "stdafx.h"
#include "SharedImageWrapper.h"

using namespace mray;

extern "C"
{
	__declspec(dllexport) bool SendTexture(const unsigned char* data, int width, int height);
}

SharedImageWrapper* wrapper;
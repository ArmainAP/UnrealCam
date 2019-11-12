#include "stdafx.h"
#include "UnityTextureWrapper.h"

using namespace mray;


extern "C" void __declspec(dllexport) *CreateTextureWrapper()
{
	TextureWrapper* w = new TextureWrapper();
	return w;
}
extern "C" void __declspec(dllexport) DeleteTextureWrapper(void* wrapper)
{
	TextureWrapper* w = static_cast<TextureWrapper*>(wrapper);
	if (!w)
		return;
	delete w;
}
extern "C" bool __declspec(dllexport) SendTexture(void* wrapper, void* TextureID)
{
	TextureWrapper* w = static_cast<TextureWrapper*>(wrapper);
	if (!w)
		return false;

	return w->SendImage(TextureID);
}

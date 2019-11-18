#include "SharedImageWrapper.h"

namespace mray
{
class TextureWrapper :public SharedImageWrapper
{
protected:
	void* _data;
	int _width;
	int _height;

public:
	TextureWrapper();
	~TextureWrapper();

	bool SendImage(const unsigned char* data, int width, int height);

};

}
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
using namespace std;

int main() {

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); //OpenGL和手提包库在y轴上是反向的
	unsigned char* data = stbi_load("pic.png", &width, &height, &nrChannels, 0);
	
	for (size_t i = 0; i < 50; i++)
	{
		cout << (int)data[i] << endl;
	}

	stbi_image_free(data);

	return 0;
}
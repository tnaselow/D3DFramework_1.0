
#include "Window_DX.h"
#include "Renderer_D3D.h"

int main(int argc, char **argv)
{
	Window_DX window;
	while (!window.Loop());

	Renderer_D3D::CleanUp();
	return 0;
}

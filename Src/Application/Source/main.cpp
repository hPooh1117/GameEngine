#include <assert.h>
#include <cstdlib>
#include <ctime>
#include "Application.h"


int APIENTRY WinMain(
	_In_      HINSTANCE hInstance,
	_In_opt_  HINSTANCE hPrevInstance,
	_In_      LPSTR     lpCmdLine,
	_In_      int       nShowCmd)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	srand((unsigned int)time(NULL));

	Application app;
	app.Init();
	app.Run();
	//app.UnInit();
	return 1;
}
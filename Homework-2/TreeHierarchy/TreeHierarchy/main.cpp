#include<iostream>
#include "FileHandler.hpp"
#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_crtBreakAlloc = 278;

	FileHandler::getInstance().exe();

	_CrtDumpMemoryLeaks();
	return 0;
}
#include<iostream>
#include "FileHandler.hpp"
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

int main()
{
	FileHandler::getInstance().exe();
	return 0;
}
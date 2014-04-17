// main.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Application.h"


int _tmain(int argc, _TCHAR* argv[])
{
    try {
        Application app(argc, argv);
        return app.Main();
    }
    catch (std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}


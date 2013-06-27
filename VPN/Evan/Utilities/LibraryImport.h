#ifndef LIBRARYIMPORT_H
#define LIBRARYIMPORT_H

#ifdef WIN32
    #include <windows.h>
#else
    #include <dlfcn.h>
#endif

#include<string>
using namespace std;

void* loadLibrary(char* libfileName)
{
	string fileName = libfileName;
	#ifdef WIN32
        fileName += ".dll";
		return (void*)LoadLibraryA(fileName.c_str());
	#else
        fileName += ".so";
		return dlopen(fileName.c_str(),RTLD_LAZY);
	#endif
}

void* getFunction(void* lib, char* funcName)
{
    #ifdef WIN32
        return (void*)GetProcAddress((HINSTANCE)lib,funcName);
    #else
        return dlsym(lib,funcName);
    #endif
}

bool freeLibrary(void* lib)
{
    #ifdef WIN32
        return FreeLibrary((HINSTANCE)lib);
    #else
        return dlclose(lib);
    #endif
}


#endif


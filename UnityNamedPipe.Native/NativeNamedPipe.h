// NativeNamedPipe.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include "connection.h"
#include <iostream>

#if defined(_WIN32)
#define NATIVEPIPE_API __declspec(dllexport)
#else
#define NATIVEPIPE_API __attribute__((visibility("default")))
#endif

extern "C" NATIVEPIPE_API BaseNamedPipeClient *CreateClient();
extern "C" NATIVEPIPE_API void DestroyClient(BaseNamedPipeClient *client);

extern "C" NATIVEPIPE_API bool IsConnected(BaseNamedPipeClient *client);
extern "C" NATIVEPIPE_API int ReadFrame(BaseNamedPipeClient *client, unsigned char *buffer, int length);
extern "C" NATIVEPIPE_API int WriteFrame(BaseNamedPipeClient *client, unsigned char *buffer, int length);
extern "C" NATIVEPIPE_API int Open(BaseNamedPipeClient *client, const char *pipename);
extern "C" NATIVEPIPE_API bool Exists(BaseNamedPipeClient *client, const char *pipename);
extern "C" NATIVEPIPE_API void Close(BaseNamedPipeClient *client);
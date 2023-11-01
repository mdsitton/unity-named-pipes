// NativeNamedPipe.cpp : Defines the entry point for the application.
//

#include "NativeNamedPipe.h"
extern "C" NATIVEPIPE_API BaseNamedPipeClient *CreateClient() { return BaseNamedPipeClient::Create(); }

extern "C" NATIVEPIPE_API void DestroyClient(BaseNamedPipeClient *client)
{
	if (client != NULL)
	{
		// close the client
		client->Close();

		// delete and nullify the client
		delete client;
		client = NULL;
	}
}

extern "C" NATIVEPIPE_API bool IsConnected(BaseNamedPipeClient *client) { return client->IsConnected(); }
extern "C" NATIVEPIPE_API int ReadFrame(BaseNamedPipeClient *client, unsigned char *buffer, int length) { return client->ReadFrame(buffer, length); }
extern "C" NATIVEPIPE_API int WriteFrame(BaseNamedPipeClient *client, unsigned char *buffer, int length) { return client->WriteFrame(buffer, length); }
extern "C" NATIVEPIPE_API int Open(BaseNamedPipeClient *client, const char *pipename) { return client->Open(pipename); }

extern "C" NATIVEPIPE_API bool Exists(BaseNamedPipeClient *client, const char *pipename) { return client->Exists(pipename); }
extern "C" NATIVEPIPE_API void Close(BaseNamedPipeClient *client) { client->Close(); }
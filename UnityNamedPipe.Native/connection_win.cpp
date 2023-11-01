#include "connection.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class NamedPipeClientWindows : public BaseNamedPipeClient
{
public:
	NamedPipeClientWindows()
	{
		handle = INVALID_HANDLE_VALUE;
		isOpened = false;
	}

	~NamedPipeClientWindows() = default;

	bool IsConnected() override
	{
		return handle != NULL && isOpened && handle != INVALID_HANDLE_VALUE;
	}

	int ReadFrame(unsigned char *buffer, int length) override
	{

		if (length == 0)
		{
			return -1;
		}
		if (!IsConnected())
		{
			return -2;
		}

		// Prepare how many bytes we have read
		DWORD bytesAvailable = 0;

		// Attempt to peek at the available pipe
		if (::PeekNamedPipe(handle, nullptr, 0, nullptr, &bytesAvailable, nullptr))
		{
			// Check if we have bytes available to read
			if (bytesAvailable > 0)
			{
				// Read the bytes.
				// TODO: Make the Bytes Read appart of the output
				DWORD bytesToRead = (DWORD)length;
				DWORD bytesRead = 0;

				// Attempt to read the bytes
				if (::ReadFile(handle, buffer, bytesToRead, &bytesRead, nullptr) == TRUE)
				{
					return bytesRead;
				}
				else
				{
					// We failed to read anything, close the pipe (broken)
					Close();
					return -3;
				}
			}
			else
			{
				// We failed to read as there were no bytes available
				return 0;
			}
		}
		else
		{
			// We have failed to peek. The pipe is probably broken
			Close();
			return -5;
		}
	}

	int WriteFrame(unsigned char *buffer, int length) override
	{
		if (length == 0)
			return 0;
		if (!IsConnected())
			return -1;

		// Prepare the size
		const DWORD bytesLength = (DWORD)length;
		DWORD bytesWritten = 0;

		// Write and return its success
		bool success = ::WriteFile(handle, buffer, bytesLength, &bytesWritten, nullptr);
		if (!success)
			return -2;
		return bytesWritten;
	}

	int Open(const char *pipename) override
	{
		// Creates a connection to the pipe
		handle = ::CreateFileA(pipename, GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
		if (handle != INVALID_HANDLE_VALUE)
		{
			isOpened = true;
			return 0;
		}

		// We have a error, better find out why.
		auto lasterr = GetLastError();

		// Pipe wasnt found
		if (lasterr == ERROR_FILE_NOT_FOUND)
			return lasterr;

		// The path wasnt found?
		if (lasterr == ERROR_PATH_NOT_FOUND)
			return lasterr;

		// Pipe is just busy
		if (lasterr == ERROR_PIPE_BUSY)
		{
			if (!WaitNamedPipeA(pipename, 1000))
			{
				// Failed to open, terminate
				return lasterr;
			}
			else
			{
				// Attempt to open it again
				return open(pipename);
			}
		}

		// Some other magically error occured
		return lasterr;
	}

	bool Exists(const char *pipename) override
	{
		WIN32_FIND_DATA fileData;
		HANDLE file = FindFirstFileA(pipename, &fileData);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			FindClose(file);
			return true;
		}
		return false;
	}

	void Close(void) override
	{
		::CloseHandle(handle);
		handle = INVALID_HANDLE_VALUE;
		isOpened = false;
	}

private:
	HANDLE handle;
	bool isOpened;
};

BaseNamedPipeClient *BaseNamedPipeClient::Create() { return (BaseNamedPipeClient *)(new NamedPipeClientWindows()); }

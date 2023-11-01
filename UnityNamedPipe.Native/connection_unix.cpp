#include "connection.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <unistd.h>
#include <iostream>

#ifdef MSG_NOSIGNAL
static int MsgFlags = MSG_NOSIGNAL;
#else
static int MsgFlags = 0;
#endif

class NamedPipeClientUnix : BaseNamedPipeClient
{
public:
	NamedPipeClientUnix()
	{
		sock = -1;
		isOpened = false;
	}

	bool IsConnected() override
	{
		return isOpened && sock != -1;
	}

	int ReadFrame(unsigned char *buffer, int length) override
	{
		if (length == 0)
			return 0;
		if (!IsConnected())
			return -4200;

		size_t bytesLength = (size_t)length;
		int res = (int)recv(sock, buffer, bytesLength, MsgFlags);
		if (res < 0)
		{
			if (errno == 11)
				return 0;
			return -errno;
		}

		return res;
	}

	int WriteFrame(unsigned char *buffer, int length) override
	{
		if (length == 0)
			return 0;
		if (!IsConnected())
			return -4200;

		size_t bytesLength = (size_t)length;
		ssize_t sentBytes = send(sock, buffer, length, MsgFlags);
		if (sentBytes < 0)
			return -errno;

		return (int)sentBytes;
	}

	int Open(const char *pipename) override
	{
		std::cout << "Open socket" << std::endl;
		sockaddr_un addr;
		addr.sun_family = AF_UNIX;

		// Create the socket
		sock = socket(AF_UNIX, SOCK_STREAM, 0);
		if (sock < 0)
			return errno;
		std::cout << "Set non blocking" << std::endl;

		// Yes.. do what this does
		fcntl(sock, F_SETFL, O_NONBLOCK);

#ifdef SO_NOSIGPIPE
		int optval = 1;
		setsockopt(sock, SOL_SOCKET, SO_NOSIGPIPE, &optval, sizeof(optval));
#endif

		std::cout << "format path" << std::endl;

		// Update the address
		snprintf(addr.sun_path, sizeof(addr.sun_path), "%s", pipename);
		std::cout << "connect path" << std::endl;
		int response = connect(sock, (const sockaddr *)&addr, sizeof(addr));
		if (response < 0)
		{
			std::cout << "failed to connect to path" << std::endl;
			// Prepare the error code (to return later) and close the connect
			int errorcode = errno * 1000;
			Close();

			// Return the errorcode.
			return errorcode;
		}

		// We have opened, success!
		isOpened = true;
		return 0;
	}

	bool Exists(const char *pipename) override
	{
		struct stat buffer;
		if (stat(pipename, &buffer) == 0)
		{
			return S_ISSOCK(buffer.st_mode);
		}
		return false;
	}

	void Close(void) override
	{
		std::cout << "closing socket " << sock << std::endl;
		if (sock != -1)
		{
			close(sock);
		}
		sock = -1;
		isOpened = false;
		std::cout << "Finished with socket" << std::endl;
	}

private:
	int sock;
	bool isOpened;
};

BaseNamedPipeClient *BaseNamedPipeClient::Create() { return (BaseNamedPipeClient *)(new NamedPipeClientUnix()); }

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
		sockaddr_un addr;
		addr.sun_family = AF_UNIX;

		// Create the socket
		sock = socket(AF_UNIX, SOCK_STREAM, 0);
		if (sock < 0)
			return errno;

		// Yes.. do what this does
		fcntl(sock, F_SETFL, O_NONBLOCK);

#ifdef SO_NOSIGPIPE
		int optval = 1;
		setsockopt(sock, SOL_SOCKET, SO_NOSIGPIPE, &optval, sizeof(optval));
#endif

		// Update the address
		snprintf(addr.sun_path, sizeof(addr.sun_path), "%s", pipename);
		int response = connect(sock, (const sockaddr *)&addr, sizeof(addr));
		if (response < 0)
		{

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
		if (sock != -1)
		{
			close(sock);
		}
		sock = -1;
		isOpened = false;
	}

private:
	int sock;
	bool isOpened;
};

BaseNamedPipeClient *BaseNamedPipeClient::Create() { return (BaseNamedPipeClient *)(new NamedPipeClientUnix()); }

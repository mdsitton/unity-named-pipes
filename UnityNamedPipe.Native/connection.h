class BaseNamedPipeClient
{
public:
	static BaseNamedPipeClient *Create();

	virtual ~BaseNamedPipeClient() = default;

	virtual bool IsConnected() = 0;
	virtual int ReadFrame(unsigned char *buffer, int length) = 0;
	virtual int WriteFrame(unsigned char *buffer, int length) = 0;

	virtual int Open(const char *pipename) = 0;
	virtual bool Exists(const char *pipename) = 0;
	virtual void Close(void) = 0;
};
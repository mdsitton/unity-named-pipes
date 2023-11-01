#include "NativeNamedPipe.h"
#include <iostream>
#include <string>
#include <fmt/format.h>
#include <cstdlib>
#include <thread>
#if defined(_WIN32)
std::string paths[] = {"discord-ipc-"};
#else
std::string paths[] = {
    "/discord-ipc-",
    "/app/com.discordapp.Discord/discord-ipc-",
    "/snap.discord-canary/discord-ipc-",
    "/snap.discord/discord-ipc-"};
#endif

static const char *
get_temp_path()
{
#if defined(_WIN32)
    return "\\\\.\\pipe\\";
#else
    const char *temp = std::getenv("XDG_RUNTIME_DIR");
    temp = temp ? temp : std::getenv("TMPDIR");
    temp = temp ? temp : std::getenv("TMP");
    temp = temp ? temp : std::getenv("TEMP");
    temp = temp ? temp : "/tmp";
    return temp;
#endif
}

int main()
{
    fmt::println("Starting sockets!");
    BaseNamedPipeClient *client = CreateClient();
    try
    {
        while (true)
        {
            int pathCount = sizeof(paths) / sizeof(std::string);

            for (int i = 0; i < pathCount; ++i)
            {
                for (int j = 0; j < 10; ++j)
                {
                    std::string fmtPath = fmt::format("{}{}{}", get_temp_path(), paths[i], j);
                    fmt::println("Checking path: {}", fmtPath);
                    if (!Exists(client, fmtPath.c_str()))
                    {
                        fmt::println("Socket does not exist skipping!");
                        continue;
                    }
                    else
                    {
                        fmt::println("Found socket at {}!", fmtPath);
                    }
                    int status = Open(client, fmtPath.c_str());

                    if (status == 0)
                    {
                        fmt::println("open success!");
                    }
                    else
                    {
                        fmt::println("open failure: {}", status);
                    }

                    fmt::println("Connected: {}", IsConnected(client));
                    Close(client);
                }
            }
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    DestroyClient(client);
}
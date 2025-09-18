#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOUSER
#include <windows.h>

// Undefine problematic macros that conflict with raylib
#ifdef near
#undef near
#endif
#ifdef far
#undef far
#endif
#ifdef PlaySound
#undef PlaySound
#endif

#include <iostream>
#include <string>

class Stockfish {
private:
    HANDLE hChildStdinWr;
    HANDLE hChildStdoutRd;
    PROCESS_INFORMATION pi;

public:
    Stockfish(const std::string& path);

    ~Stockfish();

    void sendCommand(const std::string& cmd);

    std::string readResponse();
};

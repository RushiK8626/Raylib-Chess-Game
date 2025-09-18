#include <string>
#include "stockfish.h"

Stockfish::Stockfish(const std::string& path) {
    // Security attributes to allow handle inheritance
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE };

    HANDLE hChildStdinRd, hChildStdoutWr;

    // Create pipes
    CreatePipe(&hChildStdoutRd, &hChildStdoutWr, &sa, 0);
    SetHandleInformation(hChildStdoutRd, HANDLE_FLAG_INHERIT, 0);

    CreatePipe(&hChildStdinRd, &hChildStdinWr, &sa, 0);
    SetHandleInformation(hChildStdinWr, HANDLE_FLAG_INHERIT, 0);

    // Setup startup info
    STARTUPINFOW si = {};
    si.cb = sizeof(si);
    // Store process information in member 'pi'
    ZeroMemory(&pi, sizeof(pi));
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = hChildStdinRd;
    si.hStdOutput = hChildStdoutWr;
    si.hStdError = hChildStdoutWr;

    // Convert path to wide string
    int wlen = MultiByteToWideChar(CP_UTF8, 0, path.c_str(), -1, nullptr, 0);
    std::wstring wpath(wlen, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, path.c_str(), -1, &wpath[0], wlen);

    // Launch Stockfish
    if (!CreateProcessW(wpath.c_str(), nullptr, nullptr, nullptr, TRUE, 0, nullptr, nullptr, &si, &pi)) {
        throw std::runtime_error("Failed to start Stockfish");
    }

    // Close unused ends
    CloseHandle(hChildStdinRd);
    CloseHandle(hChildStdoutWr);
}

Stockfish::~Stockfish() {
    CloseHandle(hChildStdinWr);
    CloseHandle(hChildStdoutRd);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Stockfish::sendCommand(const std::string& cmd) {
    DWORD written;
    WriteFile(hChildStdinWr, cmd.c_str(), cmd.size(), &written, nullptr);
}

std::string Stockfish::readResponse() {
    char buffer[4096];
    DWORD read;
    std::string result;
    
    // Set a timeout for reading
    DWORD timeout = 5000; // 5 seconds
    DWORD startTime = GetTickCount();

    while (true) {
        // Check for timeout
        if (GetTickCount() - startTime > timeout) {
            if (!result.empty()) {
                return result; // Return what we have
            }
            break;
        }
        
        // Use PeekNamedPipe to check if data is available
        DWORD bytesAvailable = 0;
        if (!PeekNamedPipe(hChildStdoutRd, nullptr, 0, nullptr, &bytesAvailable, nullptr)) {
            break;
        }
        
        if (bytesAvailable > 0) {
            if (ReadFile(hChildStdoutRd, buffer, sizeof(buffer) - 1, &read, nullptr) && read > 0) {
                buffer[read] = '\0';
                result += buffer;
                
                // Check for bestmove line
                size_t pos = result.find("bestmove ");
                if (pos != std::string::npos) {
                    size_t end = result.find('\n', pos);
                    if (end != std::string::npos) {
                        return result.substr(pos, end - pos);
                    } else {
                        return result.substr(pos);
                    }
                }
                
                // Check for uciok
                if (result.find("uciok") != std::string::npos) {
                    return result;
                }
                
                // Check for readyok
                if (result.find("readyok") != std::string::npos) {
                    return "readyok";
                }
            }
        } else {
            // No data available, sleep briefly to avoid busy waiting
            Sleep(10);
        }
    }
    return result;
}
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
    BOOL success = CreateProcessW(
        NULL,
        const_cast<wchar_t*>(wpath.c_str()),
        NULL, NULL, TRUE, CREATE_NO_WINDOW,
        NULL, NULL, &si, &pi
    );

    if (!success) {
        // Clean up any pipe handles that were created
        if (hChildStdinWr) { CloseHandle(hChildStdinWr); hChildStdinWr = NULL; }
        if (hChildStdoutRd) { CloseHandle(hChildStdoutRd); hChildStdoutRd = NULL; }
        CloseHandle(hChildStdinRd);
        CloseHandle(hChildStdoutWr);
        ZeroMemory(&pi, sizeof(pi));
        throw std::runtime_error("Failed to start Stockfish");
    }

    // Close unused ends
    CloseHandle(hChildStdinRd);
    CloseHandle(hChildStdoutWr);
}

Stockfish::~Stockfish() {
    if (hChildStdinWr && hChildStdinWr != INVALID_HANDLE_VALUE) CloseHandle(hChildStdinWr);
    if (hChildStdoutRd && hChildStdoutRd != INVALID_HANDLE_VALUE) CloseHandle(hChildStdoutRd);
    if (pi.hProcess && pi.hProcess != INVALID_HANDLE_VALUE) CloseHandle(pi.hProcess);
    if (pi.hThread && pi.hThread != INVALID_HANDLE_VALUE) CloseHandle(pi.hThread);
}

void Stockfish::sendCommand(const std::string& cmd) {
    if (hChildStdinWr == NULL || hChildStdinWr == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("Stockfish pipe is not valid");
    }
    std::string cmdWithNewline = cmd + "\n";
    DWORD written;
    if (!WriteFile(hChildStdinWr, cmdWithNewline.c_str(), cmdWithNewline.size(), &written, nullptr)) {
        throw std::runtime_error("Failed to write to Stockfish pipe");
    }
}

std::string Stockfish::readResponse() {
    if (hChildStdoutRd == NULL || hChildStdoutRd == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("Stockfish pipe is not valid");
    }

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
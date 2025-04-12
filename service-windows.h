#pragma once

#include "service.h"

#include <Windows.h>
#include <winsvc.h>

extern BlockingQueue<std::string> m_queueMsg;
extern Log service_log;

SERVICE_STATUS_HANDLE service_status_handle;

std::thread thread_bind;

const char* PIPE_NAME = "\\\\.\\pipe\\EasyTier-Service";

void WINAPI ServiceCtrlHandler(DWORD control) {
    if (control == SERVICE_CONTROL_STOP) {
        // 更新服务状态为“正在停止”
        SERVICE_STATUS status;
        
        status.dwCurrentState = SERVICE_WIN32_OWN_PROCESS;
        status.dwCurrentState = SERVICE_STOP_PENDING;
        status.dwControlsAccepted = 0;
        status.dwWin32ExitCode = NO_ERROR;
        status.dwCheckPoint = 0;
        status.dwWaitHint = 3000;

        SetServiceStatus(service_status_handle, &status);

        // 触发主循环退出
        exit_requested.store(true);
    }
}


void ServiceMain(DWORD argc, LPTSTR* argv) {
    service_status_handle = RegisterServiceCtrlHandler(
        "EasyTier-Service", ServiceCtrlHandler);

    // 初始状态为“运行中”
    SERVICE_STATUS status;

    status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    status.dwCurrentState = SERVICE_STOP_PENDING;
    status.dwControlsAccepted = 0;
    status.dwWin32ExitCode = NO_ERROR;
    status.dwCheckPoint = 0;
    status.dwWaitHint = 3000;
    
    SetServiceStatus(service_status_handle, &status);
}

void bind_thread_main() {
    while (!exit_requested.load()) {
        HANDLE hPipe = CreateNamedPipe(
            PIPE_NAME,
            PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            PIPE_UNLIMITED_INSTANCES,
            1024,  // Output buffer size
            1024,  // Input buffer size
            0,     // Default timeout
            NULL);

        if (hPipe == INVALID_HANDLE_VALUE) {
            service_log.push(LEVEL_WARN, "Failed to create pipe: %d", GetLastError());
            continue;
        }

        if (ConnectNamedPipe(hPipe, NULL) ||
            GetLastError() == ERROR_PIPE_CONNECTED) {

            char buffer[1024];
            DWORD bytesRead;

            if (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL)) {
                buffer[bytesRead] = '\0';
                m_queueMsg.push(buffer);
                service_log.push(LEVEL_VERBOSE, "Message received: %s", buffer);
            }

            FlushFileBuffers(hPipe);
            DisconnectNamedPipe(hPipe);
            CloseHandle(hPipe);
        }
        else {
            CloseHandle(hPipe);
        }
    }
}

bool send_message(const std::string& msg) {
    if (!WaitNamedPipe(PIPE_NAME, NMPWAIT_WAIT_FOREVER)) {
        service_log.push(LEVEL_ERROR, "Failed to find valid pipe: %d", GetLastError());
        return false;
    }

    HANDLE hPipe = CreateFile(
        PIPE_NAME,
        GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if (hPipe == INVALID_HANDLE_VALUE) {
        service_log.push(LEVEL_ERROR, "Failed to connect: %d", GetLastError());
        return false;
    }

    DWORD bytesWritten;
    if (WriteFile(hPipe, msg.c_str(), (DWORD)msg.size(), &bytesWritten, NULL)) {
        service_log.push(LEVEL_VERBOSE, "Message sent: %s", msg.c_str());
        CloseHandle(hPipe);
        return true;
    }
    else {
        service_log.push(LEVEL_ERROR, "Message (%s) send failed: %d", msg.c_str(),GetLastError());
        CloseHandle(hPipe);
        return false;
    }

}

bool register_service() {
    SERVICE_TABLE_ENTRY dispatch_table[] = {
        { (LPSTR)"EasyTier-Service", (LPSERVICE_MAIN_FUNCTION)ServiceMain },
        { nullptr, nullptr }
    };
    StartServiceCtrlDispatcher(dispatch_table);

    thread_bind = std::thread(bind_thread_main);

    return true;
}

bool clean_service() {
    SERVICE_STATUS status;

    status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    status.dwCurrentState = SERVICE_STOPPED;
    status.dwControlsAccepted = 0;
    status.dwWin32ExitCode = NO_ERROR;
    status.dwCheckPoint = 0;
    status.dwWaitHint = 3000;

    SetServiceStatus(service_status_handle, &status);

    if (thread_bind.joinable())
        thread_bind.join();

    return true;
}
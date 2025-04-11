#pragma once

#include "service.h"

#include <Windows.h>
#include <winsvc.h>

SERVICE_STATUS_HANDLE service_status_handle;

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

bool register_service() {
    SERVICE_TABLE_ENTRY dispatch_table[] = {
        { (LPSTR)"EasyTier-Service", (LPSERVICE_MAIN_FUNCTION)ServiceMain },
        { nullptr, nullptr }
    };
    StartServiceCtrlDispatcher(dispatch_table);
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
    return true;
}
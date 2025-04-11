#pragma once

#include "service.h"

#include <csignal>

void signal_handler(int signal) {
    if (signal == SIGTERM || signal == SIGINT) {
        exit_requested.store(true);
    }
}

bool register_service() {
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGTERM, &sa, nullptr);
    sigaction(SIGINT, &sa, nullptr);  // ≤∂ªÒCtrl+C£®ø…—°£©
    return true;
}

bool clean_service(){
    return true;
}
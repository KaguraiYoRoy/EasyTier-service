#pragma once

#include "service.h"

#include <csignal>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <cstring>

extern BlockingQueue<std::string> m_queueMsg;
extern Log service_log;

std::thread thread_bind;

const char* SOCKET_PATH = "/var/run/easytier-service.sock";
const int SOCKET_TIMEOUT=1; // second

void signal_handler(int signal) {
    if (signal == SIGTERM || signal == SIGINT) {
        exit_requested.store(true);
    }
}

void bind_thread_main() {
    unlink(SOCKET_PATH);

    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        service_log.push(LEVEL_FATAL, "Failed to create socket");
        exit_requested.store(true);
        return;
    }

    struct timeval tv;
    tv.tv_sec = SOCKET_TIMEOUT;
    tv.tv_usec = 0;
    setsockopt(server_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (bind(server_fd, (sockaddr*)&addr, sizeof(addr)) == -1) {
        service_log.push(LEVEL_FATAL, "Bind failed");
        close(server_fd);
        exit_requested.store(true);
        return;
    }

    if (listen(server_fd, 5) == -1) {
        service_log.push(LEVEL_FATAL, "Listen failed");
        close(server_fd);
        exit_requested.store(true);
        return;
    }

    while (!exit_requested.load()) {
        int client_fd = accept(server_fd, nullptr, nullptr);
        if (client_fd != -1) {
            char buffer[1024];
            int bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
            if (bytes_read > 0) {
                buffer[bytes_read] = '\0';
                m_queueMsg.push(buffer);
                service_log.push(LEVEL_VERBOSE, "Message received: %s", buffer);
            }
            close(client_fd);
        }
        else {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                continue;
            }
            service_log.push(LEVEL_WARN, "Failed to accept socket connection");
        }
    }
}

bool send_message(const std::string& msg) {
    int sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock == -1) {
        service_log.push(LEVEL_ERROR, "Failed to create socket");
        return false;
    }

    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (connect(sock, (sockaddr*)&addr, sizeof(addr)) == -1) {
        service_log.push(LEVEL_ERROR, "Connect failed");
        close(sock);
        return false;
    }

    if (write(sock, msg.c_str(), msg.size()) == -1) {
        service_log.push(LEVEL_ERROR, "Message send failed: %s", msg.c_str());
        close(sock);
        return false;
    }
    else {
        service_log.push(LEVEL_VERBOSE, "Message sent success: %s", msg.c_str());
        close(sock);
        return true;
    }
}

bool register_service() {
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGTERM, &sa, nullptr);
    sigaction(SIGINT, &sa, nullptr);  // ≤∂ªÒCtrl+C£®ø…—°£©

    thread_bind = std::thread(bind_thread_main);

    return true;
}

bool clean_service(){
    if (thread_bind.joinable())
        thread_bind.join();
    unlink(SOCKET_PATH);
    return true;
}
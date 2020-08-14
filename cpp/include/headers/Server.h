#pragma once
#include <arpa/inet.h>
#include <cstring>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <atomic>
#include <mutex>
#include <set>
#include <thread>

#include "Field.h"
#include "Game.h"
#include "JSON.h"
#include "Logger.h"
#include "Player.h"

#include <iostream>

class Server {
public:
    Server(std::shared_ptr<Logger> logger);

    ~Server();

    void Start(uint16_t port, uint32_t n_workers);

    void Stop(bool);

    void Join();

private:
    void OnRun();

    void Worker(int client_socket);

    std::shared_ptr<Logger> logger;

    std::atomic<bool> running;

    uint32_t _free_workers_count;

    std::set<int> _client_sockets;

    std::mutex _client_sockets_mutex;

    std::thread _thread;

    int _server_socket;
};

class BufferIO {
public:
    BufferIO(size_t buff_size);

    ~BufferIO();

    std::string Read(int socket);

    void Write(std::string, int socket);

private:
    char *buffer;
    size_t size;
    int socket;
    std::string message;
};

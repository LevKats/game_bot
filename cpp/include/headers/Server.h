#pragma once
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <thread>

#include <iostream>

class Server {
public:
    Server();

    ~Server();

    void Start();

    void Stop();

    void Join();

private:
    void OnRun(int socket);
};

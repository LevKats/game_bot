#pragma once
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <condition_variable>
#include <mutex>
#include <thread>

#include "Field.h"
#include "Game.h"
#include "JSON.h"
#include "Logger.h"
#include "Player.h"

#include <iostream>

class Server {
public:
    Server();

    ~Server();

    void Start();

    void Stop(bool);

    void Join();

private:
    void OnRun();

    void OnGame(int socket);

    Logger log;

    std::thread tr;
};

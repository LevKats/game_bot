#include "Server.h"
//#include <iostream>

BufferIO::BufferIO(size_t buff_size) : size(buff_size) {
    buffer = new char[buff_size];
}

BufferIO::~BufferIO() { delete[] buffer; }

std::string BufferIO::Read(int socket) {
    auto find = message.find('\n');
    if (find != std::string::npos) {
        auto res = message.substr(0, find + 1);
        message = message.substr(find + 1);
        return res;
    }
    while (true) {
        int count = read(socket, buffer, size);
        if (count == -1) {
            throw std::runtime_error("no read bytes");
        }
        for (int i = 0; i < count; ++i) {
            message += buffer[i];
        }
        auto find = message.find('\n');
        if (find != std::string::npos) {
            auto res = message.substr(0, find + 1);
            message = message.substr(find + 1);
            return res;
        }
    }
}

void BufferIO::Write(std::string s, int socket) {
    if (s.find('\n') != std::string::npos) {
        throw std::runtime_error("incorrect message");
    }
    s += '\n';
    while (s.length() > 0) {
        strncpy(buffer, s.c_str(), size);
        int count = write(socket, buffer, std::min(s.length(), size));
        if (count == -1) {
            throw std::runtime_error("no written bytes");
        }
        s = s.substr(count);
    }
}

Server::Server(std::shared_ptr<Logger> logger) : logger(logger) {}

Server::~Server() {}

void Server::Start(uint16_t port, uint32_t n_workers) {
    logger->log("Start network service");

    sigset_t sig_mask;
    sigemptyset(&sig_mask);
    sigaddset(&sig_mask, SIGPIPE);
    if (pthread_sigmask(SIG_BLOCK, &sig_mask, NULL) != 0) {
        throw std::runtime_error("Unable to mask SIGPIPE");
    }

    struct sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;         // IPv4
    server_addr.sin_port = htons(port);       // TCP port number
    server_addr.sin_addr.s_addr = INADDR_ANY; // Bind to any address

    _server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (_server_socket == -1) {
        throw std::runtime_error("Failed to open socket");
    }

    int opts = 1;
    if (setsockopt(_server_socket, SOL_SOCKET, SO_REUSEADDR, &opts,
                   sizeof(opts)) == -1) {
        close(_server_socket);
        throw std::runtime_error("Socket setsockopt() failed");
    }

    if (bind(_server_socket, (struct sockaddr *)&server_addr,
             sizeof(server_addr)) == -1) {
        close(_server_socket);
        throw std::runtime_error("Socket bind() failed");
    }

    if (listen(_server_socket, 5) == -1) {
        close(_server_socket);
        throw std::runtime_error("Socket listen() failed");
    }

    {
        std::unique_lock<std::mutex> lock(_client_sockets_mutex);
        // sorry for default use .store, but
        // we don't need something more complex
        _free_workers_count = n_workers;
        running.store(true);
    }

    _thread = std::thread(&Server::OnRun, this);
}

void Server::Stop(bool join) {
    running.store(false);
    shutdown(_server_socket, SHUT_RDWR);
    {
        std::unique_lock<std::mutex> lock(_client_sockets_mutex);
        for (auto socket : _client_sockets) {
            shutdown(socket, SHUT_RDWR);
        }
    }
    if (join) {
        Join();
    } else {
        _thread.detach();
    }
}

void Server::Join() {
    assert(_thread.joinable());
    _thread.join();
}

void Server::OnRun() {
    while (running.load()) {
        logger->log("waiting for connection...");

        // The call to accept() blocks until the incoming connection arrives
        int client_socket;
        struct sockaddr client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        if ((client_socket =
                 accept(_server_socket, (struct sockaddr *)&client_addr,
                        &client_addr_len)) == -1) {
            continue;
        }

        // Got new connection
        logger->log("Accepted connection on socket " +
                    std::to_string(client_socket));

        // Configure read timeout
        {
            struct timeval tv;
            tv.tv_sec = 3600;
            tv.tv_usec = 0;
            setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO,
                       (const char *)&tv, sizeof tv);
        }

        // TODO: Start new thread and process data from/to connection
        {
            std::unique_lock<std::mutex> lock(_client_sockets_mutex);
            if (!running.load() || !_free_workers_count) {
                logger->log("Closed connection on socket " +
                            std::to_string(client_socket) + " server to busy");
                close(client_socket);
            } else {
                --_free_workers_count;
                _client_sockets.insert(client_socket);
                auto worker =
                    std::thread(&Server::Worker, this, client_socket);
                worker.detach();
            }
        }
    }

    logger->log("Network stopped");
}

void Server::Worker(int client_socket) {
    try {
        BufferIO io(4096);

        std::string describe_str = io.Read(client_socket);

        std::vector<Game::PlayerFullState> bears;
        std::vector<Game::PlayerFullState> humans;

        auto describe = JSONParser(describe_str).parse();

        uint32_t field_size = std::stoi((*describe)["FIELD"]["SIZE"].text());
        uint32_t holles = std::stoi((*describe)["FIELD"]["HOLLES"].text());
        Field field(field_size, holles);

        auto &players = (*describe)["PLAYERS"];
        auto keys = players.keys();
        for (auto key : keys) {
            auto &obj = players[key];

            std::shared_ptr<Player> player(new Player(
                [client_socket, &io]() {
                    auto s = io.Read(client_socket);
                    // std::cout << s << '\n';
                    return s;
                },
                [client_socket, &io](std::string s) {
                    // std::cout << s << '\n';
                    io.Write(s, client_socket);
                },
                obj["NAME"].text()));

            if (obj["TYPE"].text() == "BEAR") {
                bears.push_back(
                    {player, {rand() % field_size, rand() % field_size}, {}});
            } else if (obj["TYPE"].text() == "HUMAN") {
                humans.push_back(
                    {player, {rand() % field_size, rand() % field_size}, {}});
            } else {
                throw std::runtime_error("strange type");
            }
        }

        Game g(bears, humans, field, logger, {3, 1, 2, 3, 3});
        logger->log("The game begins on socket " +
                    std::to_string(client_socket));

        while (g.is_running()) {
            g.step();
        }
    } catch (std::runtime_error &e) {
        logger->log("Error on connection " + std::to_string(client_socket) +
                    " std::runtime_error(" + '"' + e.what() + '"' + ")\n" +
                    "connection closed");
    }
    {
        std::unique_lock<std::mutex> lock(_client_sockets_mutex);

        _client_sockets.erase(client_socket);
        close(client_socket);
        ++_free_workers_count;
    }
}

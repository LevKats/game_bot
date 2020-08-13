#include "Server.h"

Server::Server();

Server::~Server();

void Server::Start();

void Server::Stop(bool join);

void Server::Join();

void Server::OnRun();

void Server::OnGame(int socket);

#include "include/server.hpp"
#include "include/tintin_reporter.hpp"
#include "include/config.hpp"
#include "include/signals.hpp"

#include <stdexcept>
#include <algorithm>
#include <cerrno>
#include <cstring>

#include <fcntl.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

namespace
{
    constexpr int kPort = md::kPort;
    constexpr size_t kMaxClients = 3;
}

Server::Server() {}
Server::~Server() { closeAll(); }

void Server::setup()
{
    Tintin_reporter::instance().info("Creating server.");
    _serverFd = ::socket(AF_INET, SOCK_STREAM, 0);
    int on = 1;
    ::setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(kPort);
    if (::bind(_serverFd, (sockaddr *)&addr, sizeof(addr)) < 0)
    {
        Tintin_reporter::instance().error(std::string("bind failed: ") + std::strerror(errno));
        throw std::runtime_error("bind failed");
    }
    if (::listen(_serverFd, 10) < 0)
    {
        Tintin_reporter::instance().error(std::string("listen failed: ") + std::strerror(errno));
        throw std::runtime_error("listen failed");
    }
    ::fcntl(_serverFd, F_SETFL, O_NONBLOCK);

    if (::pipe(_selfPipe) < 0)
    {
        Tintin_reporter::instance().error(std::string("pipe failed: ") + std::strerror(errno));
        throw std::runtime_error("pipe failed");
    }
    ::fcntl(_selfPipe[0], F_SETFL, O_NONBLOCK);

    Tintin_reporter::instance().info("Server created.");
}

void Server::run()
{
    static bool initialized = false;
    if (!initialized)
    {
        setup();
        initialized = true;
    }
    loop();
}

void Server::loop()
{
    while (!_stop.load())
    {
        // Honor external termination signals
        if (g_signal_received)
        {
            _stop.store(true);
        }
        fd_set rfds;
        FD_ZERO(&rfds);
        int maxfd = _serverFd;
        FD_SET(_serverFd, &rfds);
        FD_SET(_selfPipe[0], &rfds);
        if (_selfPipe[0] > maxfd)
            maxfd = _selfPipe[0];
        for (int fd : _clients)
        {
            FD_SET(fd, &rfds);
            if (fd > maxfd)
                maxfd = fd;
        }
        // Use a short timeout so we can react to signals promptly
        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        int r = ::select(maxfd + 1, &rfds, nullptr, nullptr, &tv);
        if (r < 0)
        {
            if (errno == EINTR)
                continue;
            Tintin_reporter::instance().error(std::string("select failed: ") + std::strerror(errno));
            break;
        }
        if (r == 0)
        {
            // timeout
            continue;
        }
        if (FD_ISSET(_selfPipe[0], &rfds))
        {
            char buf[32];
            ssize_t rbytes = ::read(_selfPipe[0], buf, sizeof(buf));
            (void)rbytes;
            _stop.store(true);
        }
        if (FD_ISSET(_serverFd, &rfds))
            handleNewClient();
        for (size_t i = 0; i < _clients.size() && !_stop.load(); ++i)
        {
            int fd = _clients[i];
            if (FD_ISSET(fd, &rfds))
                handleClientData(i);
        }
    }
    for (int fd : _clients)
    {
        const char *bye = "server shutting down\n";
        ssize_t w = ::write(fd, bye, std::strlen(bye));
        (void)w;
        // Ensure EOF delivery and immediate disconnect
        ::shutdown(fd, SHUT_RDWR);
        ::close(fd);
    }
    _clients.clear();
    Tintin_reporter::instance().info("Quitting.");
}

void Server::handleNewClient()
{
    int c = ::accept(_serverFd, nullptr, nullptr);
    if (c < 0)
        return;

    if (_clients.size() >= kMaxClients)
    {
        const char *msg = "server full\n";
        ssize_t w = ::write(c, msg, std::strlen(msg));
        (void)w;
        // Ensure client exits immediately
        ::shutdown(c, SHUT_RDWR);
        ::close(c);
        return;
    }

    ::fcntl(c, F_SETFL, O_NONBLOCK);
    _clients.push_back(c);

    const char *welcome = "vous etes connectes\n";
    ssize_t w = ::write(c, welcome, std::strlen(welcome));
    (void)w;
}

void Server::handleClientData(size_t idx)
{
    int fd = _clients[idx];
    char buf[1024];
    ssize_t n = ::read(fd, buf, sizeof(buf));
    if (n <= 0)
    {
        ::shutdown(fd, SHUT_RDWR);
        ::close(fd);
        _clients.erase(_clients.begin() + idx);
        return;
    }
    static std::string acc[FD_SETSIZE];
    acc[fd].append(buf, buf + n);
    size_t pos;
    while ((pos = acc[fd].find('\n')) != std::string::npos)
    {
        std::string line = acc[fd].substr(0, pos);
        acc[fd].erase(0, pos + 1);
        if (line == "quit")
        {
            Tintin_reporter::instance().info("Request quit.");
            _stop.store(true);
            return;
        }
        else if (!line.empty())
        {
            Tintin_reporter::instance().user(std::string("User input: ") + line);
        }
    }
}

void Server::requestStop()
{
    _stop.store(true);
    const char x = 'x';
    ssize_t w = ::write(_selfPipe[1], &x, 1);
    (void)w;
}

void Server::closeAll()
{
    for (int fd : _clients)
    {
        ::shutdown(fd, SHUT_RDWR);
        ::close(fd);
    }
    _clients.clear();
    if (_serverFd != -1)
        ::close(_serverFd);
    if (_selfPipe[0] != -1)
        ::close(_selfPipe[0]);
    if (_selfPipe[1] != -1)
        ::close(_selfPipe[1]);
}

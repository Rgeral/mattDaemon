#ifndef SERVER_HPP
#define SERVER_HPP

#include <atomic>
#include <vector>

// Simple TCP server for Matt_daemon.
// - Listens on port 4242 (see md::kPort)
// - Accepts up to 3 simultaneous clients
// - Sends a welcome line on connect
// - A line "quit" from any client triggers graceful shutdown
class Server {
public:
    Server();
    ~Server();

    // Initialize and run the event loop. Blocks until shutdown requested.
    void run();

    // Trigger a shutdown from outside (e.g., signal handler).
    void requestStop();

private:
    int _serverFd{-1};
    int _selfPipe[2]{-1, -1};
    std::atomic<bool> _stop{false};
    std::vector<int> _clients;

    void setup();
    void loop();
    void handleNewClient();
    void handleClientData(size_t idx);
    void closeAll();
};

#endif // SERVER_HPP

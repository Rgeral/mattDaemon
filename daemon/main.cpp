#include "main.hpp"

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

#include "include/config.hpp"
#include "include/daemonize.hpp"
#include "include/server.hpp"
#include "include/signals.hpp"
#include "include/tintin_reporter.hpp"

bool log_directory_exist(const char* path) {
    if (mkdir(path, 0755) != 0 && errno != EEXIST) {
        std::cerr << "Failed to create log directory '" << path << "': " << std::strerror(errno)
                  << std::endl;
        return false;
    }
    return true;
}

int main(void) {
    // root check
    if (geteuid() != 0) {
        std::cerr << "Must be run as root" << std::endl;
        return 1;
    }

    // log directory check
    if (!log_directory_exist(md::kLogDir)) return 1;

    // single instance check
    int lockfd = open(md::kLockFile, O_CREAT | O_EXCL | O_WRONLY, 0644);
    if (lockfd < 0) {
        std::cerr << "Can't open lock file: " << md::kLockFile << std::endl;
        return 1;
    }
    close(lockfd);

    // Init logging
    Tintin_reporter::instance().info("Started.");
    Tintin_reporter::instance().info("Creating server.");

    // Daemonize now
    Tintin_reporter::instance().info("Entering Daemon mode.");
    daemonize();
    Tintin_reporter::instance().info(std::string("started. PID: ") + std::to_string(getpid()) +
                                     ".");

    // Signals
    register_signal_handlers();

    // Run server
    try {
        Server s;
        // Poll signal flag and request stop -> ensures all clients are closed
        while (true) {
            if (g_signal_received) {
                Tintin_reporter::instance().info("Signal handler.");
                s.requestStop();
                break;
            }
            s.run();
            break;  // run() returns on stop
        }
    } catch (...) {
        Tintin_reporter::instance().error("Fatal error in server loop.");
    }

    // Cleanup
    if (g_signal_received) {
        Tintin_reporter::instance().info("Exiting due to received signal.");
    }
    ::unlink(md::kLockFile);
    return 0;
}

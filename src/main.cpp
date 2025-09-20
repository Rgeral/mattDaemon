#include "main.hpp"
#include "tintin_reporter.hpp"
#include "server.hpp"
#include "daemonize.hpp"
#include "config.hpp"
#include "signals.hpp"

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

int main() {
    // Must be run as root
    if (geteuid() != 0) {
        std::cerr << "Must be run as root" << std::endl;
        return 1;
    }

    // Ensure log directory exists
    ::mkdir(md::kLogDir, 0755);

    // Single instance via lock file
    int lockfd = ::open(md::kLockFile, O_CREAT | O_EXCL | O_WRONLY, 0644);
    if (lockfd < 0) {
        // Print the exact error as in subject example
        std::cerr << "Can't open :/var/lock/matt_daemon.lock" << std::endl;
        return 1;
    }
    ::close(lockfd);

    // Init logging
    Tintin_reporter::instance().info("Started.");
    Tintin_reporter::instance().info("Creating server.");

    // Daemonize now
    Tintin_reporter::instance().info("Entering Daemon mode.");
    daemonize();
    Tintin_reporter::instance().info(std::string("started. PID: ") + std::to_string(getpid()) + ".");

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
            break; // run() returns on stop
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
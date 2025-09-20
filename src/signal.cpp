#include "signals.hpp"

#include <csignal>
#include <cstdio>
#include <unistd.h>

volatile sig_atomic_t g_signal_received = 0;

void cleanup() {
    // Flush all libc buffers
    fflush(nullptr);
}

static void shutdown_handler(int /*signum*/) {
    g_signal_received = 1;
}

void register_signal_handlers(void) {
    struct sigaction sa{};
    sa.sa_handler = shutdown_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa, nullptr);
    sigaction(SIGTERM, &sa, nullptr);
    sigaction(SIGHUP, &sa, nullptr);
    sigaction(SIGQUIT, &sa, nullptr);

    struct sigaction sa_ign{};
    sa_ign.sa_handler = SIG_IGN;
    sigemptyset(&sa_ign.sa_mask);
    sa_ign.sa_flags = SA_RESTART | SA_NOCLDWAIT;
    sigaction(SIGCHLD, &sa_ign, nullptr);
    sigaction(SIGPIPE, &sa_ign, nullptr);
}
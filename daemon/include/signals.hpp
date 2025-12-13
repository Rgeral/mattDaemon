#pragma once

#include <csignal>

// Global signal state used to request graceful shutdown.
extern volatile sig_atomic_t g_signal_received;

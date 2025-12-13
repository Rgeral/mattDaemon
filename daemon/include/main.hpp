#pragma once

// High-level declarations for Matt_daemon.
// Keep this header minimal to avoid unnecessary recompilations.

// Install UNIX signal handlers required by the project.
void register_signal_handlers();

// Flush buffered IO and perform any final cleanup steps.
void cleanup();

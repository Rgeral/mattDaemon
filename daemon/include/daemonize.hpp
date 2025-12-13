#ifndef DAEMONIZE_HPP
#define DAEMONIZE_HPP

// Detach the current process and run as a background daemon using the
// standard double-fork + setsid pattern.
void daemonize();

#endif  // DAEMONIZE_HPP

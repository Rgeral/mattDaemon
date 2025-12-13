#ifndef MD_CONFIG_HPP
#define MD_CONFIG_HPP

namespace md {
// Global constants for Matt_daemon
inline constexpr const char* kLockFile = "/var/lock/matt_daemon.lock";
inline constexpr const char* kLogDir   = "/var/log/matt_daemon";
inline constexpr const char* kLogFile  = "/var/log/matt_daemon/matt_daemon.log";
inline constexpr int         kPort     = 4242;
}  // namespace md

#endif  // MD_CONFIG_HPP

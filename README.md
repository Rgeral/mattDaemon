Matt_daemon
===========

Overview
- Real UNIX daemon (double-fork, setsid, chdir("/"), stdio -> /dev/null, umask 027)
- Root-only execution; single instance via /var/lock/matt_daemon.lock
- TCP server on port 4242, max 3 simultaneous clients
- Logging via Tintin_reporter to /var/log/matt_daemon/matt_daemon.log
- Clean shutdown on "quit" or SIGINT/SIGTERM/SIGHUP/SIGQUIT; broadcasts "server shutting down" to clients

Build
- Requires g++ (C++17).
- Targets: all, clean, fclean, re, logs, client

Usage
- Build: make
- Run: sudo ./Matt_daemon
- View logs: make logs
- Connect client: make client

Behavior
- On connect: "vous etes connectes".
- If full: "server full" and connection closes immediately.
- On shutdown: "server shutting down" to all clients, then disconnects.

Notes
- Lock file: /var/lock/matt_daemon.lock prevents multiple instances.
- Logs: /var/log/matt_daemon/matt_daemon.log
- Port: 4242

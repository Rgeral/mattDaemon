#include "daemonize.hpp"

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdlib>

// Daemonize
// ---------
// Detaches the current process using the canonical double-fork sequence,
// creates a new session (setsid), changes the working directory to "/",
// closes stdio file descriptors and redirects them to /dev/null, and sets
// a restrictive umask.
void daemonize() {
    pid_t pid;

    // First fork: let the parent exit so the child is orphaned from the shell
    pid = fork();
    if (pid < 0) {
        _exit(EXIT_FAILURE);
    }
    if (pid > 0) _exit(0);

    // Become session leader to lose controlling terminal
    if (setsid() < 0) {
        _exit(EXIT_FAILURE);
    }

    // Second fork: ensure we cannot acquire a controlling terminal
    pid = fork();
    if (pid < 0) {
        _exit(EXIT_FAILURE);
    }
    if (pid > 0) _exit(0);

    // Change working directory to root
    int cd = chdir("/");
    (void)cd;

    // Close standard fds and redirect to /dev/null
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    int devnull = open("/dev/null", O_RDWR);
    if (devnull >= 0) {
        dup2(devnull, STDIN_FILENO);
        dup2(devnull, STDOUT_FILENO);
        dup2(devnull, STDERR_FILENO);
        if (devnull > 2) close(devnull);
    }

    // Restrictive default permissions for created files
    umask(027);
}
#include "tintin_reporter.hpp"
#include "config.hpp"

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <ctime>
#include <iomanip>
#include <sstream>

Tintin_reporter& Tintin_reporter::instance() {
    static Tintin_reporter inst;
    return inst;
}

Tintin_reporter::Tintin_reporter() {}
Tintin_reporter::~Tintin_reporter() {
    if (_fd >= 0) close(_fd);
}

std::string Tintin_reporter::timestamp() const {
    std::time_t t = std::time(nullptr);
    std::tm tmv;
    localtime_r(&t, &tmv);
    std::ostringstream oss;
    oss << '[' << std::setfill('0') << std::setw(2) << tmv.tm_mday << '/'
        << std::setw(2) << (tmv.tm_mon + 1) << '/' << (tmv.tm_year + 1900)
        << '-' << std::setw(2) << tmv.tm_hour << ':' << std::setw(2) << tmv.tm_min
        << ':' << std::setw(2) << tmv.tm_sec << "] ";
    return oss.str();
}

void Tintin_reporter::ensureLogReady() {
    if (_ready) return;
    ::mkdir(md::kLogDir, 0755);
    _fd = ::open(md::kLogFile, O_CREAT | O_WRONLY | O_APPEND, 0644);
    _ready = (_fd >= 0);
}

void Tintin_reporter::log(Level level, const std::string& message) {
    static const char* L[] = {"[ INFO ]", "[ LOG ]", "[ ERROR ]"};
    std::lock_guard<std::mutex> g(_mtx);
    ensureLogReady();
    if (_fd < 0) return; // Failing silently per subject simplicity
    std::string line = timestamp();
    line += L[static_cast<int>(level)];
    line += " - Matt_daemon: ";
    line += message;
    line += '\n';
    ssize_t w = ::write(_fd, line.c_str(), line.size());
    (void)w;
}

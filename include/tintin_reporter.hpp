#ifndef TINTIN_REPORTER_HPP
#define TINTIN_REPORTER_HPP

#include <mutex>
#include <string>

// Tintin_reporter
// ---------------
// Minimal reusable logger writing to a file with the format:
// [DD/MM/YYYY-HH:MM:SS] [ LEVEL ] - Matt_daemon: message
class Tintin_reporter {
public:
    enum class Level { INFO, LOG, ERROR };

    static Tintin_reporter& instance();

    void log(Level level, const std::string& message);

    inline void info(const std::string& message) { log(Level::INFO, message); }
    inline void user(const std::string& message) { log(Level::LOG, message); }
    inline void error(const std::string& message) { log(Level::ERROR, message); }

private:
    Tintin_reporter();
    ~Tintin_reporter();
    Tintin_reporter(const Tintin_reporter&) = delete;
    Tintin_reporter& operator=(const Tintin_reporter&) = delete;

    std::string timestamp() const;
    void ensureLogReady();

    int  _fd{-1};
    bool _ready{false};
    std::mutex _mtx;
};

#endif // TINTIN_REPORTER_HPP

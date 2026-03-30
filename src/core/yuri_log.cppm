/*
 * @Author: love-yuri yuri2078170658@gmail.com
 * @Date: 2023-09-28 08:49:03
 * @LastEditTime: 2025-11-02 21:21:54
 * @Description: 高性能的日志库基于c++11，支持更多类型和更美观的输出，单文件 13万条/s 单控制台 5万/s
 */
module;
#ifdef _WIN32
#include <windows.h>
#endif
#include <ctime>
export module yuri_log;

import std;

export namespace yuri {

constexpr std::uint8_t WriteInConsole = 0x0001;                              // 仅写入控制台
constexpr std::uint8_t WriteInFile = 0x0002;                                 // 仅写入文件
constexpr std::uint8_t WriteInConsoleAndFile = WriteInConsole | WriteInFile; // 同时写入控制台和文件

class Log final {
public:
  enum class LogLevel {
    Info,
    Debug,
    Warning,
    Error
  };

  constexpr static std::array<const char *, 4> levelStrings = {
    "INFO",
    "DEBUG",
    "WARN",
    "ERROR",
  };

  /**
   * 控制设备的写入模式
   * @return 写入模式引用
   */
  static std::atomic<std::uint32_t> &logLevelFilter() {
    static std::atomic<std::uint32_t> level_filter = std::to_underlying(LogLevel::Info);
    return level_filter;
  }

  /**
   * 控制设备的写入模式
   * @return 写入模式引用
   */
  static std::atomic<std::uint32_t> &writeMode() {
    static std::atomic<std::uint32_t> write_mode = WriteInConsole;
    return write_mode;
  }

  /**
   * 写入文件时的文件路径
   * @return 设置文件路径
   */
  static std::string &filePath() {
    static std::string path = "log.txt";
    return path;
  }

  /**
   * @brief 静态成员函数，用于控制是否使用std::cerr输出错误日志
   */
  static std::atomic_bool &useStdError() {
    static std::atomic_bool use_std_cerr = false;
    return use_std_cerr;
  }

private:
  std::ostringstream ost{};
  using stringRef = const std::string &;
  LogLevel level = LogLevel::Info;
  static constexpr int INDENT_SIZE = 2;

  static std::mutex &getMutex() {
    static std::mutex mutex;
    return mutex;
  }

  /**
   * 格式化日志消息
   * @return 格式化后的消息
   */
  std::string formatMessage() {
    // 获取当前时间（含毫秒）
    const auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    const auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    // 转换为本地时间
    std::tm localTimeData{};
#ifdef _WIN32
    localtime_s(&localTimeData, &now_time_t);
#else
    localtime_r(&now_time_t, &localTimeData);
#endif

    // 格式化时间（HH:MM:SS）
    char formattedTime[9]; // HH:MM:SS 固定8字符 + 终止符
    std::strftime(formattedTime, sizeof(formattedTime), "%H:%M:%S", &localTimeData);

    char buf[26];
    std::snprintf(
      buf,
      sizeof(buf),
#ifdef _WIN32
      "[%02d:%02d:%02d.%03lld %s] ",
#else
      "[%02d:%02d:%02d.%03ld %s] ",
#endif
      localTimeData.tm_hour,
      localTimeData.tm_min,
      localTimeData.tm_sec,
      now_ms.count(),
      levelStrings[static_cast<unsigned>(level)]);

    // 启用Windows控制台的ANSI转义序列支持
    static std::once_flag once_flag;
    std::call_once(once_flag, [] {
#ifdef _WIN32
      // 设置编码
      SetConsoleOutputCP(CP_UTF8);
      SetConsoleCP(CP_UTF8);
      const auto hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
      DWORD mode;
      if (GetConsoleMode(hConsole, &mode)) {
        SetConsoleMode(hConsole, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
      }
#endif
    });

    return std::string{buf};
  }

  // 格式化容器的辅助函数
  template <typename Container>
  void formatContainer(const Container &container, const std::string &open = "[", const std::string &close = "]") {
    ost << open;
    bool first = true;
    for (const auto &item : container) {
      if (!first) {
        ost << ", ";
      }
      first = false;
      *this << item;
    }
    ost << close;
  }

  // 格式化map类型容器的辅助函数
  template <typename MapContainer>
  void formatMapContainer(const MapContainer &container) {
    ost << "{\n";
    bool first = true;
    for (const auto &pair : container) {
      if (!first) {
        ost << "\n";
      }
      first = false;
      ost << std::string(INDENT_SIZE, ' ') << pair.first << ": ";
      *this << pair.second;
    }
    if (!container.empty()) {
      ost << "\n";
    }
    ost << "}";
  }

public:
  Log(const std::string &func, const int line, LogLevel level = LogLevel::Info) :
    level(level) {
    ost << func << ":" << line << " -> ";
  }

  explicit Log(const LogLevel level = LogLevel::Info) :
    level(level) {
  }

  ~Log() {
    if (static_cast<std::uint32_t>(level) < logLevelFilter()) {
      return;
    }

    const std::string prefix = formatMessage();

    std::lock_guard lock(getMutex());

    if (writeMode() & WriteInConsole) {
      std::ostream &ostream = useStdError() && level == LogLevel::Error ? std::cerr : std::cout;
      switch (level) {
        case LogLevel::Info:
          ostream << "\x1b[38;5;41m";
          break;
        case LogLevel::Warning:
          ostream << "\x1b[33m";
          break;
        case LogLevel::Debug:
          break;
        case LogLevel::Error:
          ostream << "\x1b[31m";
          break;
      }
      ostream << prefix;
      (level != LogLevel::Error ? ostream << "\x1b[0m" << ost.str() : ostream << ost.str() << "\x1b[0m") << std::endl;
    }

    if (writeMode() & WriteInFile) {
      try {
        std::ofstream file(filePath(), std::ios::app);
        if (file.is_open()) {
          file << prefix << ost.str() << '\n';
          file.flush();
        }
      } catch (const std::exception &e) {
        std::cerr << "Log file error: " << e.what() << std::endl;
      }
    }
  }

  // 基础类型输出
  template <typename T>
  Log &operator<<(const T &val) {
    ost << val;
    return *this;
  }

  // std::pair 特化
  template <typename K, typename V>
  Log &operator<<(const std::pair<K, V> &pair) {
    ost << "{" << pair.first << ": " << pair.second << "}";
    return *this;
  }

  // std::vector 特化
  template <typename T, typename Alloc>
  Log &operator<<(const std::vector<T, Alloc> &vec) {
    formatContainer(vec);
    return *this;
  }

  // std::list 特化
  template <typename T, typename Alloc>
  Log &operator<<(const std::list<T, Alloc> &list) {
    formatContainer(list);
    return *this;
  }

  // std::deque 特化
  template <typename T, typename Alloc>
  Log &operator<<(const std::deque<T, Alloc> &deq) {
    formatContainer(deq);
    return *this;
  }

  // std::set 特化
  template <typename T, typename Compare, typename Alloc>
  Log &operator<<(const std::set<T, Compare, Alloc> &set) {
    formatContainer(set, "{", "}");
    return *this;
  }

  // std::unordered_set 特化
  template <typename T, typename Hash, typename KeyEqual, typename Alloc>
  Log &operator<<(const std::unordered_set<T, Hash, KeyEqual, Alloc> &set) {
    formatContainer(set, "{", "}");
    return *this;
  }

  // std::map 特化
  template <typename K, typename V, typename Compare, typename Alloc>
  Log &operator<<(const std::map<K, V, Compare, Alloc> &map) {
    formatMapContainer(map);
    return *this;
  }

  // std::unordered_map 特化
  template <typename K, typename V, typename Hash, typename KeyEqual, typename Alloc>
  Log &operator<<(const std::unordered_map<K, V, Hash, KeyEqual, Alloc> &map) {
    formatMapContainer(map);
    return *this;
  }

  // std::array 特化
  template <typename T, std::size_t N>
  Log &operator<<(const std::array<T, N> &arr) {
    formatContainer(arr);
    return *this;
  }

  // bool 类型特化
  Log &operator<<(const bool v) {
    ost << std::boolalpha << v;
    return *this;
  }
};

/**
 * 扩展函数
 */
template <typename... Args>
void info(const std::format_string<Args...> fmt, Args&&... args) {
  Log(Log::LogLevel::Info) << std::format(fmt, std::forward<Args>(args)...);
}

template <typename T>
void info(const T && t) {
  Log(Log::LogLevel::Info) << t;
}

template <typename... Args>
void error(const std::format_string<Args...> fmt, Args&&... args) {
  Log(Log::LogLevel::Error) << std::format(fmt, std::forward<Args>(args)...);
}

template <typename T>
void error(const T && t) {
  Log(Log::LogLevel::Error) << t;
}

// 明确区分「运行时字符串」和「编译时格式化」
void error(const std::string_view msg) {
  Log(Log::LogLevel::Error) << msg;
}

template <typename... Args>
void warn(const std::format_string<Args...> fmt, Args&&... args) {
  Log(Log::LogLevel::Warning) << std::format(fmt, std::forward<Args>(args)...);
}

template <typename T>
void warn(const T && t) {
  Log(Log::LogLevel::Warning) << t;
}

template <typename... Args>
void debug(const std::format_string<Args...> fmt, Args&&... args) {
  Log(Log::LogLevel::Debug) << std::format(fmt, std::forward<Args>(args)...);
}

template <typename T>
void debug(const T && t) {
  Log(Log::LogLevel::Debug) << t;
}

} // namespace yuri

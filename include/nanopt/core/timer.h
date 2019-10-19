#pragma once

#include <cmath>
#include <string>
#include <chrono>
#include <sstream>
#include <iomanip>

namespace nanopt {

inline std::string timeString(double time, bool precise = false) {
  if (std::isnan(time) || std::isinf(time)) return "inf";
  auto suffix = "ms";
  if (time > 1000) {
    time /= 1000; suffix = "s";
    if (time > 60) {
      time /= 60; suffix = "m";
      if (time > 60) {
        time /= 60; suffix = "h";
        if (time > 12) {
          time /= 12; suffix = "d";
        }
      }
    }
  }
  std::ostringstream os;
  os << std::setprecision(precise ? 4 : 1)
     << std::fixed << time << suffix;
  return os.str();
}

inline std::string memString(size_t size, bool precise = false) {
  auto value = (double)size;
  const char* suffixes[] = { "B", "KiB", "MiB", "GiB", "TiB", "PiB" };
  auto suffix = 0;
  while (suffix < 5 && value > 1024.0f) {
    value /= 1024.0f;
    ++suffix;
  }
  std::ostringstream os;
  os << std::setprecision(suffix == 0 ? 0 : (precise ? 4 : 1))
     << std::fixed << value << " " << suffixes[suffix];
  return os.str();
}

class Timer {
public:
  Timer() noexcept {
    reset();
  }

  void reset() {
    start = std::chrono::system_clock::now();
  }

  /// Return the number of milliseconds
  double elapsed() const {
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
    return (double)duration.count();
  }

  /// Like \ref elapsed(), but return a human-readable string
  std::string elapsedString(bool precise = false) const {
    return timeString(elapsed(), precise);
  }

  /// Return the number of milliseconds elapsed since the timer was last reset and then reset it
  double lap() {
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - start);
    start = now;
    return (double)duration.count();
  }

  /// Link \ref lap(), but return a human-readable string
  std::string lapString(bool precise = false) {
    return timeString(lap(), precise);
  }

private:
  std::chrono::system_clock::time_point start;
};

}

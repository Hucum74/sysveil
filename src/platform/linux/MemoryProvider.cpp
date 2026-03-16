#include "MemoryProvider.h"

#include <QDebug>

#include <fstream>
#include <sstream>
#include <string>

struct MemoryProvider::Impl {

  static bool readProcMeminfo(qint64 &totalPhysical, qint64 &availablePhysical,
                              qint64 &totalSwap, qint64 &freeSwap) {
    std::ifstream file("/proc/meminfo");
    if (!file.is_open()) {
      qWarning() << "MemoryProvider: cannot open /proc/meminfo";
      return false;
    }

    totalPhysical = 0;
    availablePhysical = 0;
    totalSwap = 0;
    freeSwap = 0;

    std::string line;
    int found = 0;

    while (std::getline(file, line) && found < 4) {
      std::istringstream ss(line);
      std::string key;
      long long value{0};
      std::string unit;
      ss >> key >> value >> unit;

      const qint64 bytes = value * 1024LL;

      if (key == "MemTotal:") {
        totalPhysical = bytes;
        ++found;
      } else if (key == "MemAvailable:") {
        availablePhysical = bytes;
        ++found;
      } else if (key == "SwapTotal:") {
        totalSwap = bytes;
        ++found;
      } else if (key == "SwapFree:") {
        freeSwap = bytes;
        ++found;
      }
    }

    return true;
  }
};

MemoryProvider::MemoryProvider(QObject *parent)
    : QObject(parent), d(new Impl{}) {}

MemoryProvider::~MemoryProvider() { delete d; }

void MemoryProvider::initialize() { qDebug() << "MemoryProvider: initialized"; }

void MemoryProvider::poll() {
  qint64 totalPhysical{0};
  qint64 availablePhysical{0};
  qint64 totalSwap{0};
  qint64 freeSwap{0};

  if (!Impl::readProcMeminfo(totalPhysical, availablePhysical, totalSwap,
                             freeSwap))
    return;

  const qint64 usedPhysical = totalPhysical - availablePhysical;
  const qint64 usedSwap = totalSwap - freeSwap;

  emit memoryDataReady(totalPhysical, usedPhysical, totalSwap, usedSwap);
}
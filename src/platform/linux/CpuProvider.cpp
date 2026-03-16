#include "CpuProvider.h"

#include <QDebug>

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

// -- Internal types --

struct CpuTicks {
  long long user{0};
  long long nice{0};
  long long system{0};
  long long idle{0};
  long long iowait{0};
  long long irq{0};
  long long softirq{0};
  long long steal{0};

  long long active() const {
    return user + nice + system + irq + softirq + steal;
  }

  long long total() const { return active() + idle + iowait; }
};

// -- Impl --

struct CpuProvider::Impl {
  std::vector<CpuTicks> previousTicks;
  int coreCount{0};

  static std::vector<CpuTicks> readProcStat() {
    std::ifstream file("/proc/stat");
    std::vector<CpuTicks> result;

    if (!file.is_open()) {
      qWarning() << "CpuProvider: cannot open /proc/stat";
      return result;
    }

    std::string line;
    while (std::getline(file, line)) {
      if (line.rfind("cpu", 0) != 0)
        break;

      std::istringstream ss(line);
      std::string label;
      CpuTicks t;
      ss >> label >> t.user >> t.nice >> t.system >> t.idle >> t.iowait >>
          t.irq >> t.softirq >> t.steal;

      result.push_back(t);
    }

    return result;
  }

  static double usageBetween(const CpuTicks &prev, const CpuTicks &curr) {
    const long long deltaTotal = curr.total() - prev.total();
    const long long deltaActive = curr.active() - prev.active();

    if (deltaTotal <= 0)
      return 0.0;

    return (static_cast<double>(deltaActive) /
            static_cast<double>(deltaTotal)) *
           100.0;
  }
};

// Compute usage % between two snapshots of singel core/aggregate entry.
static double usageBetween(const CpuTicks &prev, const CpuTicks &curr) {
  const long long deltaTotal = curr.total() - prev.total();
  const long long deltaActive = curr.active() - prev.active();

  if (deltaTotal == 0)
    return 0.0;

  return (static_cast<double>(deltaActive) / static_cast<double>(deltaTotal)) *
         100.0;
}

// -- Cpu Provider --

CpuProvider::CpuProvider(QObject *parent) : QObject(parent), d(new Impl{}) {}

CpuProvider::~CpuProvider() { delete d; }

void CpuProvider::initialize() {
  // Take an initial snapshot so the first poll() has a previous reading
  // to diff against. Without this the first poll always return 0%.
  d->previousTicks = Impl::readProcStat();

  // Index 0 is the aggregate line; core start at index 1
  d->coreCount = static_cast<int>(d->previousTicks.size()) - 1;

  qDebug() << "CpuProvider: initialized," << d->coreCount
           << "logical cores detected";
}

int CpuProvider::coreCount() const { return d->coreCount; }

void CpuProvider::poll() {
  if (d->previousTicks.empty()) {
    qWarning() << "CpuProvider: poll() called before initialize()";
    return;
  }

  const auto currentTicks = Impl::readProcStat();

  if (currentTicks.size() != d->previousTicks.size()) {
    qWarning()
        << "CpuProvider: core count changed during poll, re-initializing";
    d->previousTicks = currentTicks;
    return;
  }

  // Index 0 = aggregate
  const double overall =
      Impl::usageBetween(d->previousTicks[0], currentTicks[0]);

  // Indices 1..N = per core
  QVector<double> perCore;
  perCore.reserve(d->coreCount);
  for (int i = 1; i <= d->coreCount; ++i)
    perCore.append(Impl::usageBetween(d->previousTicks[i], currentTicks[i]));

  d->previousTicks = currentTicks;

  emit cpuDataReady(overall, perCore);
}
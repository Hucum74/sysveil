#include "ProcessProvider.h"

#include <QDebug>
#include <QDir>

#include <pwd.h>
#include <signal.h>
#include <unistd.h>

#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>

// ── Internal types
// ────────────────────────────────────────────────────────────

struct ProcTicks {
  long long utime{0};
  long long stime{0};
  long long cutime{0};
  long long cstime{0};

  long long total() const { return utime + stime; }
};

// ── Impl
// ──────────────────────────────────────────────────────────────────────

struct ProcessProvider::Impl {
  std::unordered_map<int, ProcTicks> previousTicks;
  long long previousUptime{0};
  long long totalMemKb{0};
  long clkTck{0};

  static long long readUptime() {
    std::ifstream f("/proc/uptime");
    double up = 0;
    f >> up;
    return static_cast<long long>(up * 100);
  }

  static long long readTotalMemKb() {
    std::ifstream f("/proc/meminfo");
    std::string line;
    while (std::getline(f, line)) {
      if (line.rfind("MemTotal:", 0) == 0) {
        std::istringstream ss(line);
        std::string key;
        long long val;
        ss >> key >> val;
        return val;
      }
    }
    return 1;
  }

  static bool readStat(int pid, std::string &name, ProcTicks &ticks,
                       long long &rssPages) {
    std::string path = "/proc/" + std::to_string(pid) + "/stat";
    std::ifstream f(path);
    if (!f.is_open())
      return false;

    std::string line;
    std::getline(f, line);

    auto nameStart = line.find('(');
    auto nameEnd = line.rfind(')');
    if (nameStart == std::string::npos || nameEnd == std::string::npos)
      return false;

    name = line.substr(nameStart + 1, nameEnd - nameStart - 1);

    std::istringstream ss(line.substr(nameEnd + 2));
    std::string state;
    int ppid, pgrp, session, ttyNr, tpgid;
    unsigned int flags;
    long long minflt, cminflt, majflt, cmajflt;

    ss >> state >> ppid >> pgrp >> session >> ttyNr >> tpgid >> flags >>
        minflt >> cminflt >> majflt >> cmajflt >> ticks.utime >> ticks.stime >>
        ticks.cutime >> ticks.cstime;

    long long priority, nice, numThreads, itrealvalue, starttime, vsize;
    ss >> priority >> nice >> numThreads >> itrealvalue >> starttime >> vsize >>
        rssPages;

    return true;
  }

  static QString readUser(int pid) {
    std::string path = "/proc/" + std::to_string(pid) + "/status";
    std::ifstream f(path);
    std::string line;
    while (std::getline(f, line)) {
      if (line.rfind("Uid:", 0) == 0) {
        std::istringstream ss(line);
        std::string key;
        uid_t uid;
        ss >> key >> uid;
        struct passwd *pw = getpwuid(uid);
        if (pw)
          return QString::fromUtf8(pw->pw_name);
        return QString::number(uid);
      }
    }
    return "?";
  }
};

// ── ProcessProvider
// ───────────────────────────────────────────────────────────

ProcessProvider::ProcessProvider(QObject *parent)
    : QObject(parent), d(new Impl{}) {}

ProcessProvider::~ProcessProvider() { delete d; }

void ProcessProvider::initialize() {
  d->clkTck = sysconf(_SC_CLK_TCK);
  d->totalMemKb = Impl::readTotalMemKb();
  d->previousUptime = Impl::readUptime();

  QDir procDir("/proc");
  const auto entries = procDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
  for (const QString &entry : entries) {
    bool ok;
    int pid = entry.toInt(&ok);
    if (!ok)
      continue;

    std::string name;
    ProcTicks ticks;
    long long rss;
    if (Impl::readStat(pid, name, ticks, rss))
      d->previousTicks[pid] = ticks;
  }

  qDebug() << "ProcessProvider: initialized," << d->previousTicks.size()
           << "processes primed,"
           << "clkTck:" << d->clkTck;
}

void ProcessProvider::poll() {
  const long long currentUptime = Impl::readUptime();
  const long long deltaUptime = currentUptime - d->previousUptime;
  if (deltaUptime <= 0)
    return;

  QDir procDir("/proc");
  const auto entries = procDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

  QVector<ProcessInfo> result;
  result.reserve(entries.size());

  std::unordered_map<int, ProcTicks> currentTicks;

  const long long pageSize = sysconf(_SC_PAGESIZE);

  for (const QString &entry : entries) {
    bool ok;
    int pid = entry.toInt(&ok);
    if (!ok)
      continue;

    std::string name;
    ProcTicks ticks;
    long long rssPages;

    if (!Impl::readStat(pid, name, ticks, rssPages))
      continue;

    currentTicks[pid] = ticks;

    ProcessInfo info;
    info.pid = pid;
    info.name = QString::fromStdString(name);

    auto prevIt = d->previousTicks.find(pid);
    if (prevIt != d->previousTicks.end()) {
      const long long deltaTicks = ticks.total() - prevIt->second.total();
      info.cpuUsage =
          (static_cast<double>(deltaTicks) / static_cast<double>(deltaUptime)) *
          100.0;
      info.cpuUsage = qBound(0.0, info.cpuUsage, 100.0);
    }

    const long long rssKb = (rssPages * pageSize) / 1024LL;
    info.memoryMiB = rssKb / 1024;
    info.memoryPerc = d->totalMemKb > 0 ? (static_cast<double>(rssKb) /
                                           static_cast<double>(d->totalMemKb)) *
                                              100.0
                                        : 0.0;

    info.user = Impl::readUser(pid);

    result.append(info);
  }

  d->previousTicks = currentTicks;
  d->previousUptime = currentUptime;

  std::sort(result.begin(), result.end(),
            [](const ProcessInfo &a, const ProcessInfo &b) {
              return a.cpuUsage > b.cpuUsage;
            });

  emit processDataReady(result);
}

bool ProcessProvider::killProcess(int pid, bool graceful) {
  if (pid <= 1) {
    qWarning() << "ProcessProvider: refusing to kill PID" << pid;
    return false;
  }

  if (graceful) {
    if (::kill(pid, SIGTERM) != 0) {
      qWarning() << "ProcessProvider: SIGTERM failed for PID" << pid;
      return false;
    }
    qDebug() << "ProcessProvider: SIGTERM sent to PID" << pid;
  } else {
    if (::kill(pid, SIGKILL) != 0) {
      qWarning() << "ProcessProvider: SIGKILL failed for PID" << pid;
      return false;
    }
    qDebug() << "ProcessProvider: SIGKILL sent to PID" << pid;
  }
  return true;
}
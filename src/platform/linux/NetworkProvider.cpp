#include "NetworkProvider.h"

#include <QDebug>

#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>

struct NetworkProvider::Impl {

  struct IfaceTicks {
    long long rxBytes{0};
    long long txBytes{0};
  };

  std::unordered_map<std::string, IfaceTicks> previousTicks;

  // /proc/net/dev format (after two header lines):
  // iface: rxBytes rxPackets rxErrs rxDrop ... txBytes txPackets ...
  // col 0 = interface name (with colon)
  // col 1 = rx bytes
  // col 9 = tx bytes
  static std::unordered_map<std::string, IfaceTicks> readProcNetDev() {
    std::unordered_map<std::string, IfaceTicks> result;
    std::ifstream file("/proc/net/dev");

    if (!file.is_open()) {
      qWarning() << "NetworkProvider: cannot open /proc/net/dev";
      return result;
    }

    std::string line;

    // Skip two header lines
    std::getline(file, line);
    std::getline(file, line);

    while (std::getline(file, line)) {
      std::istringstream ss(line);
      std::string name;
      long long rxBytes, rxPackets, rxErrs, rxDrop, rxFifo, rxFrame,
          rxCompressed, rxMulticast;
      long long txBytes;

      ss >> name >> rxBytes >> rxPackets >> rxErrs >> rxDrop >> rxFifo >>
          rxFrame >> rxCompressed >> rxMulticast >> txBytes;

      // Strip trailing colon from interface name
      if (!name.empty() && name.back() == ':')
        name.pop_back();

      // Skip loopback
      if (name == "lo")
        continue;

      IfaceTicks t;
      t.rxBytes = rxBytes;
      t.txBytes = txBytes;
      result[name] = t;
    }

    return result;
  }
};

NetworkProvider::NetworkProvider(QObject *parent)
    : QObject(parent), d(new Impl{}) {}

NetworkProvider::~NetworkProvider() { delete d; }

void NetworkProvider::initialize() {
  d->previousTicks = Impl::readProcNetDev();
  qDebug() << "NetworkProvider: initialized," << d->previousTicks.size()
           << "interfaces detected";
}

void NetworkProvider::poll() {
  if (d->previousTicks.empty()) {
    qWarning() << "NetworkProvider: poll() called before initialize()";
    return;
  }

  const auto current = Impl::readProcNetDev();

  QVector<NetworkInterfaceStats> result;

  for (const auto &[name, currTicks] : current) {
    auto prevIt = d->previousTicks.find(name);
    if (prevIt == d->previousTicks.end())
      continue;

    NetworkInterfaceStats s;
    s.name = QString::fromStdString(name);
    s.rxBytesPerSec = std::max(0LL, currTicks.rxBytes - prevIt->second.rxBytes);
    s.txBytesPerSec = std::max(0LL, currTicks.txBytes - prevIt->second.txBytes);

    result.append(s);
  }

  d->previousTicks = current;

  emit networkDataReady(result);
}
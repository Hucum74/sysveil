#include <QDebug>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QTimer>

#include "CpuProvider.h"
#include "DiskProvider.h"
#include "MemoryProvider.h"
#include "NetworkProvider.h"

int main(int argc, char *argv[]) {
  QGuiApplication app(argc, argv);

  app.setApplicationName("SysVeil");
  app.setApplicationVersion("0.1.0");
  app.setOrganizationName("Talha");
  app.setOrganizationDomain("com.talha.sysveil");

  QQuickStyle::setStyle("Material");

  // -- CPU verification --
  CpuProvider cpu;
  cpu.initialize();

  QObject::connect(&cpu, &CpuProvider::cpuDataReady,
                   [](double overall, QVector<double> perCore) {
                     qDebug() << "CPU overall:"
                              << QString::number(overall, 'f', 1) + "%";
                     for (int i = 0; i < perCore.size(); ++i)
                       qDebug() << "  Core" << i
                                << QString::number(perCore[i], 'f', 1) + "%";
                   });

  // ── Memory ────────────────────────────────────────────────────────────
  MemoryProvider mem;
  mem.initialize();

  QObject::connect(
      &mem, &MemoryProvider::memoryDataReady,
      [](qint64 totalPhys, qint64 usedPhys, qint64 totalSwap, qint64 usedSwap) {
        auto toMiB = [](qint64 bytes) {
          return QString::number(bytes / (1024.0 * 1024.0), 'f', 1) + " MiB";
        };
        qDebug() << "RAM :" << toMiB(usedPhys) << "/" << toMiB(totalPhys);
        qDebug() << "Swap:" << toMiB(usedSwap) << "/" << toMiB(totalSwap);
      });

  // ── Disk ──────────────────────────────────────────────────────────────
  DiskProvider disk;
  disk.initialize();

  QObject::connect(
      &disk, &DiskProvider::diskDataReady, [](QVector<DiskStats> disks) {
        auto toMiB = [](qint64 bytes) {
          return QString::number(bytes / (1024.0 * 1024.0), 'f', 1) + " MiB";
        };
        auto toKiB = [](qint64 bytes) {
          return QString::number(bytes / 1024.0, 'f', 1) + " KiB/s";
        };
        for (const auto &d : disks) {
          qDebug() << "Disk" << d.mountPoint
                   << "used:" << toMiB(d.usedBytes) + "/" + toMiB(d.totalBytes)
                   << "R:" << toKiB(d.readBytesPerSec)
                   << "W:" << toKiB(d.writeBytesPerSec);
        }
      });

  // ── Network ───────────────────────────────────────────────────────────
  NetworkProvider net;
  net.initialize();

  QObject::connect(&net, &NetworkProvider::networkDataReady,
                   [](QVector<NetworkInterfaceStats> ifaces) {
                     auto toKiB = [](qint64 bytes) {
                       return QString::number(bytes / 1024.0, 'f', 1) +
                              " KiB/s";
                     };
                     for (const auto &iface : ifaces) {
                       qDebug() << "Net" << iface.name
                                << "rx:" << toKiB(iface.rxBytesPerSec)
                                << "tx:" << toKiB(iface.txBytesPerSec);
                     }
                   });

  // ── Poll timer (shared) ───────────────────────────────────────────────
  QTimer pollTimer;
  QObject::connect(&pollTimer, &QTimer::timeout, &cpu, &CpuProvider::poll);
  QObject::connect(&pollTimer, &QTimer::timeout, &mem, &MemoryProvider::poll);
  QObject::connect(&pollTimer, &QTimer::timeout, &disk, &DiskProvider::poll);
  QObject::connect(&pollTimer, &QTimer::timeout, &net, &NetworkProvider::poll);
  pollTimer.start(1000);

  // ── QML window ──
  QQmlApplicationEngine engine;

  using namespace Qt::StringLiterals;
  const QUrl url(u"qrc:/SysVeil/qml/main.qml"_s);

  QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
      []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);

  engine.load(url);

  return app.exec();
}
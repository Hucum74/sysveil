#include <QDebug>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>

#include "SystemMonitor.h"

int main(int argc, char *argv[]) {
  QGuiApplication app(argc, argv);

  app.setApplicationName("SysVeil");
  app.setApplicationVersion("0.1.0");
  app.setOrganizationName("YourName");
  app.setOrganizationDomain("com.yourname.sysveil");

  QQuickStyle::setStyle("Material");

  // ── SystemMonitor ─────────────────────────────────────────────────────
  SystemMonitor monitor;

  QObject::connect(&monitor, &SystemMonitor::cpuDataReady,
                   [](double overall, QVector<double> perCore) {
                     qDebug()
                         << "CPU:" << QString::number(overall, 'f', 1) + "%";
                   });

  QObject::connect(
      &monitor, &SystemMonitor::memoryDataReady,
      [](qint64 totalPhys, qint64 usedPhys, qint64 totalSwap, qint64 usedSwap) {
        auto toMiB = [](qint64 b) {
          return QString::number(b / (1024.0 * 1024.0), 'f', 1) + " MiB";
        };
        qDebug() << "RAM:" << toMiB(usedPhys) << "/" << toMiB(totalPhys);
      });

  QObject::connect(
      &monitor, &SystemMonitor::diskDataReady, [](QVector<DiskStats> disks) {
        for (const auto &d : disks)
          qDebug() << "Disk" << d.mountPoint
                   << QString::number(d.usedBytes / (1024.0 * 1024.0), 'f', 0) +
                          "/" +
                          QString::number(d.totalBytes / (1024.0 * 1024.0), 'f',
                                          0) +
                          " MiB";
      });

  QObject::connect(&monitor, &SystemMonitor::networkDataReady,
                   [](QVector<NetworkInterfaceStats> ifaces) {
                     for (const auto &i : ifaces) {
                       if (i.rxBytesPerSec > 0 || i.txBytesPerSec > 0)
                         qDebug() << "Net" << i.name
                                  << "rx:" << i.rxBytesPerSec / 1024 << "KiB/s"
                                  << "tx:" << i.txBytesPerSec / 1024 << "KiB/s";
                     }
                   });

  monitor.start(1000);

  // ── QML window ────────────────────────────────────────────────────────
  QQmlApplicationEngine engine;

  using namespace Qt::StringLiterals;
  const QUrl url(u"qrc:/SysVeil/qml/main.qml"_s);

  QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
      []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);

  engine.load(url);

  return app.exec();
}
#include <QDebug>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>

#include "SystemMonitor.h"
#include "qml/MonitorBridge.h"

int main(int argc, char *argv[]) {
  QGuiApplication app(argc, argv);

  app.setApplicationName("SysVeil");
  app.setApplicationVersion("0.1.0");
  app.setOrganizationName("YourName");
  app.setOrganizationDomain("com.yourname.sysveil");

  QQuickStyle::setStyle("Material");

  // ── Backend ───────────────────────────────────────────────────────────
  SystemMonitor monitor;
  MonitorBridge bridge(&monitor);

  monitor.start(1000);

  // ── QML engine ────────────────────────────────────────────────────────
  QQmlApplicationEngine engine;

  QQmlContext *ctx = engine.rootContext();

  // Current values bridge
  ctx->setContextProperty("bridge", &bridge);

  // History models — exposed individually so QML charts can bind directly
  ctx->setContextProperty("cpuHistory", bridge.cpuHistory());
  ctx->setContextProperty("memoryHistory", bridge.memoryHistory());
  ctx->setContextProperty("diskHistory", bridge.diskHistory());
  ctx->setContextProperty("networkHistory", bridge.networkHistory());

  using namespace Qt::StringLiterals;
  const QUrl url(u"qrc:/SysVeil/qml/main.qml"_s);

  QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
      []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);

  engine.load(url);

  return app.exec();
}
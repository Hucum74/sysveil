#include <QDebug>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
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

  // Expose backend
  ctx->setContextProperty("bridge", &bridge);
  ctx->setContextProperty("cpuHistory", bridge.cpuHistory());
  ctx->setContextProperty("memoryHistory", bridge.memoryHistory());
  ctx->setContextProperty("diskHistory", bridge.diskHistory());
  ctx->setContextProperty("networkHistory", bridge.networkHistory());

  // Instantiate Theme from QML and expose as context property
  // so every QML file can use Theme.xxx without any import statement
  using namespace Qt::StringLiterals;
  QQmlComponent themeComponent(&engine,
                               QUrl(u"qrc:/SysVeil/qml/theme/Theme.qml"_s));
  QObject *theme = themeComponent.create();
  if (!theme) {
    qWarning() << "Failed to create Theme:" << themeComponent.errorString();
  } else {
    ctx->setContextProperty("Theme", theme);
    theme->setParent(&engine);
  }

  const QUrl url(u"qrc:/SysVeil/qml/main.qml"_s);

  QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
      []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);

  engine.load(url);

  return app.exec();
}
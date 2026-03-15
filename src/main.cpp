#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>

int main(int argc, char *argv[]) {
  QGuiApplication app(argc, argv);

  app.setApplicationName("SysVeil");
  app.setApplicationVersion("0.1.0");
  app.setOrganizationName("Talha");
  app.setOrganizationDomain("com.talha.sysveil");

  QQuickStyle::setStyle("Material");

  QQmlApplicationEngine engine;

  using namespace Qt::StringLiterals;
  const QUrl url(u"qrc:/SysVeil/qml/main.qml"_s);

  QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
      []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);

  engine.load(url);

  return app.exec();
}
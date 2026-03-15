#pragma once

#include <QObject>
#include <QString>
#include <QVector>

struct NetworkInterfaceStats {
  QString name;
  qint64 rxBytesPerSec{0};
  qint64 txBytesPerSec{0};
};

class NetworkProvider : public QObject {
  Q_OBJECT

public:
  explicit NetworkProvider(QObject *parent = nullptr);
  ~NetworkProvider() override;

  void initialize();

public slots:
  void poll();

signals:
  void networkDataReady(QVector<NetworkInterfaceStats> interfaces);

private:
  struct Impl;
  Impl *d{nullptr};
};
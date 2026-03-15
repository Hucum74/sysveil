#pragma once

#include <QObject>
#include <QString>
#include <QVector>

struct DiskStats {
  QString mountPoint;
  qint64 totalBytes{0};
  qint64 usedBytes{0};
  qint64 readBytesPerSec{0};
  qint64 writeBytesPerSec{0};
};

class DiskProvider : public QObject {
  Q_OBJECT

public:
  explicit DiskProvider(QObject *parent = nullptr);
  ~DiskProvider() override;

  void initialize();

public slots:
  void poll();

signals:
  void diskDataReady(QVector<DiskStats> disks);

private:
  struct Impl;
  Impl *d{nullptr};
};
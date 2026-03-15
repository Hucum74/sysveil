#pragma once

#include <QObject>

class MemoryProvider : public QObject {
  Q_OBJECT

public:
  explicit MemoryProvider(QObject *parent = nullptr);
  ~MemoryProvider() override;

  void initialize();

public slots:
  void poll();

signals:
  void memoryDataReady(qint64 totalPhysical, qint64 usedPhysical,
                       qint64 totalSwap, qint64 usedSwap);

private:
  struct Impl;
  Impl *d{nullptr};
};
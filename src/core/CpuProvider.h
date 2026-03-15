#pragma once

#include <QObject>
#include <QVector>

class CpuProvider : public QObject {
  Q_OBJECT

public:
  explicit CpuProvider(QObject *parent = nullptr);
  ~CpuProvider() override;

  void initialize();

  [[nodiscard]] int coreCount() const;

public slots:
  void poll();

signals:
  void cpuDataReady(double overallUsage, QVector<double> perCoreUsage);

private:
  struct Impl;
  Impl *d{nullptr};
};
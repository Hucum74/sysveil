#include "SystemMonitor.h"

#include <QDebug>
#include <QThread>
#include <QTimer>

SystemMonitor::SystemMonitor(QObject *parent) : QObject(parent) {
  m_thread = new QThread(this);
  m_thread->setObjectName("SysVeilWorker");

  // Create providers and timer with no parent so moveToThread works
  m_cpu = new CpuProvider;
  m_memory = new MemoryProvider;
  m_disk = new DiskProvider;
  m_network = new NetworkProvider;
  m_timer = new QTimer;
  m_timer->setObjectName("PollTimer");

  // Move everything to the worker thread
  m_cpu->moveToThread(m_thread);
  m_memory->moveToThread(m_thread);
  m_disk->moveToThread(m_thread);
  m_network->moveToThread(m_thread);
  m_timer->moveToThread(m_thread);

  // ── Wire timer → poll slots ───────────────────────────────────────────
  connect(m_timer, &QTimer::timeout, m_cpu, &CpuProvider::poll);
  connect(m_timer, &QTimer::timeout, m_memory, &MemoryProvider::poll);
  connect(m_timer, &QTimer::timeout, m_disk, &DiskProvider::poll);
  connect(m_timer, &QTimer::timeout, m_network, &NetworkProvider::poll);

  // ── Re-emit provider signals from SystemMonitor ───────────────────────
  connect(m_cpu, &CpuProvider::cpuDataReady, this,
          &SystemMonitor::cpuDataReady);
  connect(m_memory, &MemoryProvider::memoryDataReady, this,
          &SystemMonitor::memoryDataReady);
  connect(m_disk, &DiskProvider::diskDataReady, this,
          &SystemMonitor::diskDataReady);
  connect(m_network, &NetworkProvider::networkDataReady, this,
          &SystemMonitor::networkDataReady);

  // ── Initialize providers on the worker thread ─────────────────────────
  // Qt::DirectConnection here means the lambda runs on m_thread itself
  // (the thread that emitted started()), not on the thread that owns
  // the SystemMonitor. This is what puts initialize() on the worker.
  connect(
      m_thread, &QThread::started, this,
      [this]() {
        m_cpu->initialize();
        m_memory->initialize();
        m_disk->initialize();
        m_network->initialize();
        qDebug() << "SystemMonitor: all providers initialized on"
                 << QThread::currentThread()->objectName();
        // Start the timer on the worker thread — must be called from
        // the thread the timer lives on
        m_timer->start(m_intervalMs);
      },
      Qt::DirectConnection);
}

SystemMonitor::~SystemMonitor() { stop(); }

void SystemMonitor::start(int intervalMs) {
  if (m_thread->isRunning())
    return;
  m_intervalMs = intervalMs;
  m_thread->start();
  qDebug() << "SystemMonitor: started, polling every" << intervalMs << "ms";
}

void SystemMonitor::stop() {
  if (!m_thread->isRunning())
    return;
  QMetaObject::invokeMethod(m_timer, &QTimer::stop, Qt::QueuedConnection);
  m_thread->quit();
  m_thread->wait();
  qDebug() << "SystemMonitor: stopped";
}
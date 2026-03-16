#include "SystemMonitor.h"

#include <QDebug>
#include <QThread>
#include <QTimer>

SystemMonitor::SystemMonitor(QObject *parent) : QObject(parent) {
  // Create the worker thread — providers will live on this thread
  m_thread = new QThread(this);
  m_thread->setObjectName("SysVeilWorker");

  // Create all providers with no parent — we will move them to m_thread
  m_cpu = new CpuProvider;
  m_memory = new MemoryProvider;
  m_disk = new DiskProvider;
  m_network = new NetworkProvider;

  // Create timer with no parent — also moves to worker thread
  m_timer = new QTimer;
  m_timer->setObjectName("PollTimer");

  // Move everything to the worker thread
  m_cpu->moveToThread(m_thread);
  m_memory->moveToThread(m_thread);
  m_disk->moveToThread(m_thread);
  m_network->moveToThread(m_thread);
  m_timer->moveToThread(m_thread);

  // ── Wire timer → poll slots ───────────────────────────────────────────
  // These connections are queued automatically because timer and providers
  // are on the same thread — direct connections within a thread are fine.
  connect(m_timer, &QTimer::timeout, m_cpu, &CpuProvider::poll);
  connect(m_timer, &QTimer::timeout, m_memory, &MemoryProvider::poll);
  connect(m_timer, &QTimer::timeout, m_disk, &DiskProvider::poll);
  connect(m_timer, &QTimer::timeout, m_network, &NetworkProvider::poll);

  // ── Re-emit provider signals from SystemMonitor ───────────────────────
  // These cross the thread boundary (worker → main thread) so Qt
  // automatically makes them queued connections.
  connect(m_cpu, &CpuProvider::cpuDataReady, this,
          &SystemMonitor::cpuDataReady);
  connect(m_memory, &MemoryProvider::memoryDataReady, this,
          &SystemMonitor::memoryDataReady);
  connect(m_disk, &DiskProvider::diskDataReady, this,
          &SystemMonitor::diskDataReady);
  connect(m_network, &NetworkProvider::networkDataReady, this,
          &SystemMonitor::networkDataReady);

  // ── Initialize providers when thread starts ───────────────────────────
  // QMetaObject::invokeMethod queues the call onto the worker thread's
  // event loop — safe because the thread isn't running yet here.
  connect(m_thread, &QThread::started, this, [this]() {
    m_cpu->initialize();
    m_memory->initialize();
    m_disk->initialize();
    m_network->initialize();
    qDebug() << "SystemMonitor: all providers initialized on"
             << QThread::currentThread()->objectName();
  });
}

SystemMonitor::~SystemMonitor() { stop(); }

void SystemMonitor::start(int intervalMs) {
  if (m_thread->isRunning())
    return;

  // Start the thread — this triggers the QThread::started signal above,
  // which initializes all providers, then we start the timer.
  m_thread->start();

  // Start the timer on the worker thread via invokeMethod —
  // QTimer::start() must be called from the thread it lives on.
  QMetaObject::invokeMethod(
      m_timer, [this, intervalMs]() { m_timer->start(intervalMs); },
      Qt::QueuedConnection);

  qDebug() << "SystemMonitor: started, polling every" << intervalMs << "ms";
}

void SystemMonitor::stop() {
  if (!m_thread->isRunning())
    return;

  // Stop the timer on its own thread, then quit the event loop
  QMetaObject::invokeMethod(m_timer, &QTimer::stop, Qt::QueuedConnection);
  m_thread->quit();
  m_thread->wait();

  qDebug() << "SystemMonitor: stopped";
}
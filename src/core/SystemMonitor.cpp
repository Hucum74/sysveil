#include "SystemMonitor.h"

#include <QDebug>
#include <QThread>
#include <QTimer>

SystemMonitor::SystemMonitor(QObject *parent) : QObject(parent) {
  m_thread = new QThread(this);
  m_thread->setObjectName("SysVeilWorker");

  m_cpu = new CpuProvider;
  m_memory = new MemoryProvider;
  m_disk = new DiskProvider;
  m_network = new NetworkProvider;
  m_process = new ProcessProvider;

  m_timer = new QTimer;
  m_processTimer = new QTimer;
  m_timer->setObjectName("PollTimer");
  m_processTimer->setObjectName("ProcessPollTimer");

  m_cpu->moveToThread(m_thread);
  m_memory->moveToThread(m_thread);
  m_disk->moveToThread(m_thread);
  m_network->moveToThread(m_thread);
  m_process->moveToThread(m_thread);
  m_timer->moveToThread(m_thread);
  m_processTimer->moveToThread(m_thread);

  // ── Timer → poll ──────────────────────────────────────────────────────
  connect(m_timer, &QTimer::timeout, m_cpu, &CpuProvider::poll);
  connect(m_timer, &QTimer::timeout, m_memory, &MemoryProvider::poll);
  connect(m_timer, &QTimer::timeout, m_disk, &DiskProvider::poll);
  connect(m_timer, &QTimer::timeout, m_network, &NetworkProvider::poll);

  // Process polls on its own slower timer
  connect(m_processTimer, &QTimer::timeout, m_process, &ProcessProvider::poll);

  // ── Re-emit signals ───────────────────────────────────────────────────
  connect(m_cpu, &CpuProvider::cpuDataReady, this,
          &SystemMonitor::cpuDataReady);
  connect(m_memory, &MemoryProvider::memoryDataReady, this,
          &SystemMonitor::memoryDataReady);
  connect(m_disk, &DiskProvider::diskDataReady, this,
          &SystemMonitor::diskDataReady);
  connect(m_network, &NetworkProvider::networkDataReady, this,
          &SystemMonitor::networkDataReady);
  connect(m_process, &ProcessProvider::processDataReady, this,
          &SystemMonitor::processDataReady);

  // ── Initialize on worker thread ───────────────────────────────────────
  connect(
      m_thread, &QThread::started, this,
      [this]() {
        m_cpu->initialize();
        m_memory->initialize();
        m_disk->initialize();
        m_network->initialize();
        m_process->initialize();
        qDebug() << "SystemMonitor: all providers initialized on"
                 << QThread::currentThread()->objectName();
        m_timer->start(m_intervalMs);
        m_processTimer->start(3000);
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
  QMetaObject::invokeMethod(m_processTimer, &QTimer::stop,
                            Qt::QueuedConnection);
  m_thread->quit();
  m_thread->wait();
  qDebug() << "SystemMonitor: stopped";
}

bool SystemMonitor::killProcess(int pid, bool graceful) {
  if (!m_process)
    return false;
  return m_process->killProcess(pid, graceful);
}
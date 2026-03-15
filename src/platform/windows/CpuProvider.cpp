#include "core/CpuProvider.h"

struct CpuProvider::Impl {};

CpuProvider::CpuProvider(QObject *parent) : QObject(parent), d(new Impl{}) {}
CpuProvider::~CpuProvider() { delete d; }
void CpuProvider::initialize() {}
int CpuProvider::coreCount() const { return 1; }
void CpuProvider::poll() { emit cpuDataReady(0.0, {0.0}); }
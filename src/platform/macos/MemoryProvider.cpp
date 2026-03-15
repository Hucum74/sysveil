#include "core/MemoryProvider.h"

struct MemoryProvider::Impl {};

MemoryProvider::MemoryProvider(QObject *parent)
    : QObject(parent), d(new Impl{}) {}
MemoryProvider::~MemoryProvider() { delete d; }
void MemoryProvider::initialize() {}
void MemoryProvider::poll() { emit memoryDataReady(0, 0, 0, 0); }
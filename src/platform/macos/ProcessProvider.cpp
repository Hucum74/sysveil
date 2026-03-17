#include "ProcessProvider.h"

struct ProcessProvider::Impl {};

ProcessProvider::ProcessProvider(QObject *parent)
    : QObject(parent), d(new Impl{}) {}
ProcessProvider::~ProcessProvider() { delete d; }
void ProcessProvider::initialize() {}
void ProcessProvider::poll() { emit processDataReady({}); }
bool ProcessProvider::killProcess(int, bool) { return false; }
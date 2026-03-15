#include "core/DiskProvider.h"

struct DiskProvider::Impl {};

DiskProvider::DiskProvider(QObject *parent) : QObject(parent), d(new Impl{}) {}
DiskProvider::~DiskProvider() { delete d; }
void DiskProvider::initialize() {}
void DiskProvider::poll() { emit diskDataReady({}); }
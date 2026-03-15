#include "core/NetworkProvider.h"

struct NetworkProvider::Impl {};

NetworkProvider::NetworkProvider(QObject *parent)
    : QObject(parent), d(new Impl{}) {}
NetworkProvider::~NetworkProvider() { delete d; }
void NetworkProvider::initialize() {}
void NetworkProvider::poll() { emit networkDataReady({}); }
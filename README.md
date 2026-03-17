# SysVeil

> A lightweight, open-source, real-time system resource monitor built with C++ and Qt Quick.

[![CI](https://github.com/Hucum74/sysveil/actions/workflows/build.yml/badge.svg)](https://github.com/Hucum74/sysveil/actions/workflows/build.yml)
[![Release](https://github.com/Hucum74/sysveil/actions/workflows/release.yml/badge.svg)](https://github.com/Hucum74/sysveil/actions/workflows/release.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-teal.svg)](LICENSE)

![SysVeil Overview](assets/screen1.png)
![SysVeil CPU](assets/screen2.png)

## Features

- **Real-time monitoring** — CPU per-core, RAM, swap, disk I/O, network rx/tx updated every second
- **Process manager** — full process list sortable by CPU, memory, user; kill with SIGTERM → SIGKILL fallback
- **Animated UI** — smooth number transitions, threshold color shifts (green → amber → red), pulse on update
- **Cross-platform** — runs on Linux, Windows, and macOS from a single codebase
- **Dark theme** — clean, modern Qt Quick interface with sidebar navigation

## Screenshots

| Overview | CPU |
|----------|-----------|
| ![Overview](assets/screen1.png) | ![CPU](assets/screen2.png) |

## Platform support

| Platform | Build | Packaging |
|----------|-------|-----------|
| Linux    | ✅    | `.tar.gz` |
| Windows  | ✅    | `.exe` (NSIS installer) |
| macOS    | ✅    | `.dmg` |

## Build instructions

### Prerequisites
- CMake 3.21+
- Qt 6.2+ with modules: `qtcharts`, `qtdeclarative`
- C++20 compiler (GCC 12+, Clang 14+, MSVC 2022+)

### Linux
```bash
sudo pacman -S qt6-base qt6-declarative qt6-charts cmake ninja gcc  # Arch/CachyOS
# or
sudo apt install qt6-base-dev qt6-declarative-dev qt6-charts-dev    # Ubuntu

cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
./build/src/sysveil
```

### Windows
```bash
cmake -B build -DCMAKE_PREFIX_PATH=C:\Qt\6.7.0\msvc2019_64
cmake --build build --config Release --parallel
```

### macOS
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
open build/src/sysveil.app
```

## Architecture
```
Platform APIs (/proc, PDH, sysctl)
        │
        ▼
CpuProvider / MemoryProvider / DiskProvider / NetworkProvider / ProcessProvider
        │  (QThread worker, polls every 1–3s)
        ▼
SystemMonitor  (aggregator, owns all providers)
        │  (Qt signals, queued cross-thread)
        ▼
MonitorBridge  (QObject registered as QML context property)
        │  (Q_PROPERTY + NOTIFY, QAbstractListModel subclasses)
        ▼
QML UI  (binds directly, animates on change)
```

## Tech stack

| Concern | Choice |
|---|---|
| Language | C++20 |
| UI framework | Qt 6 Quick / QML |
| Build system | CMake 3.21+ |
| Threading | QThread + QTimer per provider |
| Data binding | Q_PROPERTY + QAbstractListModel |
| Packaging | CPack (NSIS / DragNDrop / TGZ) |
| CI/CD | GitHub Actions matrix (Linux, Windows, macOS) |

## Roadmap

- [x] Phase 1 — CMake scaffold, CI, Qt Quick skeleton
- [x] Phase 2 — CPU, memory, disk, network data providers (Linux)
- [x] Phase 3 — QML bridge and history models
- [x] Phase 4 — Animated UI with metric cards and detail pages
- [x] Phase 5 — Process table with sort, filter, and graceful kill
- [x] Phase 6 — Cross-platform packaging and GitHub releases
- [ ] GPU monitoring (NVML / Metal Performance)
- [ ] Windows and macOS provider implementations
- [ ] Qt Charts integration (pending Qt 6.10 stability fix)
- [ ] Settings persistence via QSettings
- [ ] System tray with mini popup

## License

MIT — see [LICENSE](LICENSE)
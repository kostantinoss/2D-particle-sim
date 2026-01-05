# 2D Particle Simulation (SFML + CMake)

This repository started from the **CMake SFML Project Template**: [SFML/cmake-sfml-project](https://github.com/SFML/cmake-sfml-project).

It’s a **learning project** to build a 2D particle simulation (currently rendering particles and a simple visual boundary).  
**Work in progress:** under active development and not a final/complete version.

## Build & run

Prereqs: CMake + a C++17 compiler. SFML is fetched/built via CMake (FetchContent).

```bash
cmake -S . -B build
cmake --build build
./build/bin/main
```

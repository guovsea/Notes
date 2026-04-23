---
tags:
  - tools/cmake
  - type/reference
mastery: 5
---
Usual build configurations are Debug, Release, RelWithDebInfo and MinSizeRel. For single configuration generators like `make` and Ninja run:

```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

or multi-configuration generators like Visual Studio or Xcode, the configuration can be set at the building stage:

```
cmake ..
cmake --build . --config Release
```

To install targets, after building the project, run CMake like

```bash
cmake --install . --prefix "/home/myuser/installdir " --config Release
```

Note that `--config Release` only works for multi-configuration generators. For a single configuration generator, you already set the CMAKE_BUILD_TYPE before building the project.
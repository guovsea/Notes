---
tags:
  - tools/cmake
  - type/reference
mastery: 5
---

# CMake Presets

> CMake Presets（`CMakePresets.json`）用于标准化项目的配置、构建和测试参数，方便团队协作和 CI/CD 集成。

```json
{
  "version": 4,
  "cmakeMinimumRequired": {
    "major": 3,
    "minor": 21,
    "patch": 0
  },
  "configurePresets": [
    {
      "name": "default",
      "hidden": true,
      "generator": "Ninja",
      "binaryDir": "${sourceDir}/${presetName}",
      "cacheVariables": {
        "CMAKE_INSTALL_PREFIX": "${sourceDir}/install",
        "CMAKE_BUILD_TYPE": "Release"
      }
    },
    {
      "name": "debug",
      "inherits": "default",
      "cacheVariables": {
        "CMAKE_BUILD_TYPE": "Debug"
      }
    },
    {
      "name": "release",
      "inherits": "default",
      "cacheVariables": {
        "CMAKE_BUILD_TYPE": "Release"
      }
    }
  ]
}

```
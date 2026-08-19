# CMake & Submodules Integration

`sip2json` provides full CMake target support with exported target `sip2json::sip2json`.

---

## Modern CMake Integration via CPM

[CPM.cmake](https://github.com/cpm-cmake/CPM.cmake) is the recommended package manager for C++ CMake projects:

```cmake
cmake_minimum_required(VERSION 3.23)
project(MySipApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/CPM.cmake)

CPMAddPackage("gh:SiddiqSoft/sip2json#v1.17.0")

add_executable(MySipApp main.cpp)
target_link_libraries(MySipApp PRIVATE sip2json::sip2json)
```

`CPMAddPackage` automatically resolves dependent packages (`nlohmann_json` and `ctre`).

---

## FetchContent Integration

If you prefer CMake's built-in `FetchContent` module:

```cmake
include(FetchContent)

FetchContent_Declare(
    sip2json
    GIT_REPOSITORY https://github.com/SiddiqSoft/sip2json.git
    GIT_TAG        v1.17.0
)
FetchContent_MakeAvailable(sip2json)

target_link_libraries(your_target PRIVATE sip2json::sip2json)
```

---

## Build Options

| Option | Default | Description |
| :--- | :--- | :--- |
| `sip2json_BUILD_TESTS` | `OFF` | Build unit tests (requires GoogleTest) |
| `sip2json_BUILD_BENCHMARKS` | `OFF` | Build performance benchmark suite |

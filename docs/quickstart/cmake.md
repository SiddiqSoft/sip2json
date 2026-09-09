# CMake Integration

Target: `sip2json::sip2json` (header-only, C++20).

## CPM.cmake (Recommended)

```cmake
include(cmake/CPM.cmake)
CPMAddPackage("gh:SiddiqSoft/sip2json#{ tag_version }")
target_link_libraries(your_target PRIVATE sip2json::sip2json)
```

## CMake FetchContent

```cmake
include(FetchContent)
FetchContent_Declare(
    sip2json
    GIT_REPOSITORY https://github.com/SiddiqSoft/sip2json.git
    GIT_TAG        { tag_version }
)
FetchContent_MakeAvailable(sip2json)
target_link_libraries(your_target PRIVATE sip2json::sip2json)
```

## Build Options

| Option | Default | Description |
| :--- | :--- | :--- |
| `sip2json_BUILD_TESTS` | `OFF` | Build unit tests (requires GoogleTest). |
| `sip2json_BUILD_BENCHMARKS` | `OFF` | Build benchmark suite. |

## Building with Presets

```bash
cmake --preset Darwin-Clang-Release
cmake --build --preset Darwin-Clang-Release
ctest --preset Darwin-Clang-Release -j 4
```

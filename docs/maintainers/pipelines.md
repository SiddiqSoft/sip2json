# Azure Pipelines CI/CD

The CI pipeline is defined in `azure-pipelines.yml` and uses the self-hosted `Default` agent pool.

## Build stages

The default matrix includes these platform stages:

| Stage | Agents | Compilers | CMake presets | Template |
| --- | --- | --- | --- | --- |
| Windows | x64 and arm64 | MSVC | `Windows-${arch}-${buildType}` | `.azure/az-build-windows.yml` |
| Linux | x64 and arm64 | GCC and Clang | `Linux-${compiler}-${buildType}` | `.azure/az-build-unix.yml` |
| Darwin | x64 and arm64 | Clang | `Darwin-Clang-${buildType}` | `.azure/az-build-unix.yml` |

The Linux and Darwin stages share `.azure/az-build-unix.yml`. The template selects the agent OS, CMake preset prefix, architecture matrix, compiler matrix, and build type through template parameters.

Each build configures and builds with CMake and Ninja, optionally runs the CTest suite, publishes build artifacts, and optionally runs release benchmarks. Debug Linux builds also collect code coverage when the agent architecture is x64.

## Self-hosted agent requirements

All build agents must be registered in the `Default` pool and expose the matching `Agent.OS` demand:

- Linux agents: `Agent.OS -equals Linux`, CMake 3.29+, Ninja, Python 3.10+, and GCC or Clang.
- Darwin agents: `Agent.OS -equals Darwin`, macOS on x64 or arm64, CMake 3.29+, Ninja, Python 3.10+, and Apple Clang or Homebrew LLVM.
- Windows agents: `Agent.OS -equals Windows_NT`, Visual Studio/MSVC, Windows SDK, CMake, and Ninja.

The agent process needs read/write access to `$(Agent.HomeDirectory)/.cpmcache`. Darwin builds use the `Darwin-Clang-Debug` and `Darwin-Clang-Release` presets from `CMakePresets.json`.

## Publishing stages

`PublishGitHub` and `PublishDocs` depend on the Windows, Linux, and Darwin stages. On `main` and `master`, publication is enabled automatically and remains gated by manual approval. Benchmark artifacts from the build matrix are available to the publication jobs.

## Pipeline parameters

The pipeline supports selecting:

- `Platforms`: `Windows`, `Linux`, and `Darwin`.
- `Architectures`: `x64` and `arm64`.
- `Compilers_Windows`: `MSVC`.
- `Compilers_Linux`: `Clang` and `GCC`.
- `Compilers_Darwin`: `Clang`.
- `BuildTypes`: `Release` and `Debug`.
- `RunTests`, `RunBenchmarks`, and `Cleanup`.

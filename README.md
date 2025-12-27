# TLA+ Visualiser

A cross-platform desktop application for visualizing TLA+ specifications. Import specs from GitHub URLs, run model checking, and explore state spaces with interactive visualizations.

## Features

- **GitHub Import**: Import TLA+ specifications directly from GitHub URLs
  - File URLs: `https://github.com/owner/repo/blob/branch/file.tla`
  - Raw URLs: `https://raw.githubusercontent.com/owner/repo/branch/file.tla`
  - Repository URLs: `https://github.com/owner/repo`

- **Interactive Visualizations**:
  - State/transition graph with circular layout
  - Step-by-step trace viewer
  - Invariant/property dashboard

- **Model Checking Integration**:
  - Run TLC model checker
  - Parse and display results
  - Save/load run results

- **Export Capabilities**:
  - Export graphs as SVG/PNG
  - Export traces as JSON/Markdown
  - Save model checking runs

## Tech Stack

- **Language**: C++20
- **UI Framework**: Qt 6 + QML
- **Networking**: libcurl
- **Build System**: CMake + Ninja
- **Package Manager**: Conan
- **CI/CD**: GitHub Actions with QEMU for multi-arch builds

## Prerequisites

### All Platforms
- CMake 3.22 or later
- Ninja build system
- Conan package manager
- Qt 6.5 or later
- C++20 compatible compiler

### Platform-Specific

**Linux:**
```bash
sudo apt-get install qt6-base-dev qt6-declarative-dev libcurl4-openssl-dev cmake ninja-build
pip install conan
```

**macOS:**
```bash
brew install qt@6 curl cmake ninja
pip3 install conan
```

**Windows:**
- Install Qt 6 from https://www.qt.io/download
- Install CMake and Ninja via Chocolatey or directly
- Install Conan via pip

## Building

### 1. Clone the Repository
```bash
git clone https://github.com/johndoe6345789/tla_visualiser.git
cd tla_visualiser
```

### 2. Install Dependencies with Conan
```bash
conan profile detect
conan install . --output-folder=build --build=missing
```

### 3. Configure with CMake
```bash
cmake -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_TOOLCHAIN_FILE=build/conan_toolchain.cmake
```

### 4. Build
```bash
cmake --build build --config Release
```

### 5. Run Tests
```bash
cd build
ctest --output-on-failure
```

### 6. Run the Application
```bash
./build/tla_visualiser
```

## Usage

### Importing a Specification

1. Launch the application
2. Click **File → Import from GitHub...**
3. Enter a GitHub URL:
   - File: `https://github.com/tlaplus/Examples/blob/master/specifications/ewd998/EWD998.tla`
   - Repository: `https://github.com/tlaplus/Examples`
4. Click **Import**

### Running Model Checker

1. After importing a spec, configure any options
2. Optionally specify a `.cfg` configuration file
3. Click **Run Model Checker**
4. View results in the different tabs

### Exploring Results

- **Graph View**: Visual representation of state space
- **Trace View**: Step-by-step execution traces
- **Invariants**: Status of invariants and properties

### Exporting

- **Graph**: File → Export Graph... (SVG/PNG)
- **Trace**: File → Export Trace... (JSON/Markdown)

## Project Structure

```
tla_visualiser/
├── CMakeLists.txt          # Main build configuration
├── conanfile.txt           # Conan dependencies
├── src/                    # C++ source files
│   ├── main.cpp
│   ├── github_importer.cpp
│   ├── tlc_runner.cpp
│   ├── state_graph_model.cpp
│   └── trace_viewer_model.cpp
├── include/                # Header files
│   ├── github_importer.h
│   ├── tlc_runner.h
│   ├── state_graph_model.h
│   └── trace_viewer_model.h
├── qml/                    # QML UI files
│   ├── main.qml
│   ├── ImportView.qml
│   ├── GraphView.qml
│   ├── TraceView.qml
│   └── InvariantView.qml
├── tests/                  # Unit tests
│   ├── CMakeLists.txt
│   ├── test_github_importer.cpp
│   └── test_tlc_runner.cpp
└── .github/workflows/      # CI/CD workflows
    └── build.yml
```

## Architecture

### Components

1. **GitHubImporter**: Handles fetching specs from GitHub
   - URL parsing and validation
   - HTTP requests via libcurl
   - Local caching

2. **TLCRunner**: Manages TLC model checker execution
   - Process management
   - Output parsing
   - Result serialization

3. **StateGraphModel**: Qt model for state graph visualization
   - State/transition data
   - Layout calculation
   - QML integration

4. **TraceViewerModel**: Qt model for trace inspection
   - Step-by-step navigation
   - Variable display
   - Export functions

### Design Patterns

- **PIMPL**: Used for implementation hiding and ABI stability
- **Model-View**: Qt's model/view architecture for UI
- **Observer**: Callbacks for async operations

## Testing

The project includes unit tests for core components:

```bash
cd build
ctest --verbose
```

Individual tests:
```bash
./build/tests/test_github_importer
./build/tests/test_tlc_runner
```

## CI/CD

GitHub Actions workflows automatically:
- Build on Linux (x64, arm64 with QEMU), macOS, and Windows
- Run all tests
- Upload build artifacts

### Gated Tree Workflow

The project uses a **gated tree workflow** pattern to ensure code quality:

1. **Lint Job**: Fast pre-build checks (formatting, TODOs, CMake syntax)
2. **Build Jobs**: Platform-specific builds (Linux, macOS, Windows) that depend on lint
3. **Gate Job**: Final check that all required jobs passed

This ensures that:
- No code can be merged if lint checks fail
- All platforms must build successfully
- All tests must pass
- The gate provides a single "merge-ready" indicator

### Workflow Simulation

Test the CI workflow locally before pushing:

```bash
# Run full workflow simulation
./simulate_workflow.sh

# Skip tests (faster iteration)
./simulate_workflow.sh --no-tests

# Debug build
./simulate_workflow.sh --debug

# Get help
./simulate_workflow.sh --help
```

### Manual Workflow Triggers

You can manually trigger the workflow from GitHub Actions tab with custom parameters:
- Select specific platforms to build (linux, macos, windows)
- Toggle test execution
- Useful for testing workflow changes

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Run tests
5. Submit a pull request

## License

See [LICENSE](LICENSE) file for details.

## Roadmap

### Version 1.0 (Current)
- ✅ Basic GitHub import
- ✅ TLC integration stub
- ✅ State graph visualization
- ✅ Trace viewer
- ✅ Cross-platform support

### Future Versions
- Enhanced graph layouts (force-directed, hierarchical)
- Real-time TLC output streaming
- Full JSON parsing for GitHub API
- Spec editing capabilities
- Custom themes and styling
- Plugin system

## Known Limitations

- TLC must be installed separately and in PATH
- GitHub API calls are not authenticated (rate-limited)
- Large state spaces may impact performance
- Limited TLC output parsing (basic implementation)

## Support

For issues, feature requests, or questions:
- GitHub Issues: https://github.com/johndoe6345789/tla_visualiser/issues
- Documentation: See `docs/` directory

## Acknowledgments

- TLA+ and TLC: Leslie Lamport and Microsoft Research
- Qt Framework: The Qt Company
- libcurl: Daniel Stenberg and contributors

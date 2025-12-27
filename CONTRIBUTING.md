# Contributing to TLA+ Visualiser

Thank you for your interest in contributing! This document provides guidelines and instructions for contributing to the project.

## Code of Conduct

- Be respectful and inclusive
- Welcome newcomers and help them learn
- Focus on constructive feedback
- Respect different viewpoints and experiences

## Getting Started

1. **Fork the repository** on GitHub
2. **Clone your fork** locally:
   ```bash
   git clone https://github.com/YOUR_USERNAME/tla_visualiser.git
   cd tla_visualiser
   ```
3. **Set up development environment** (see [BUILDING.md](docs/BUILDING.md))
4. **Create a branch** for your work:
   ```bash
   git checkout -b feature/my-new-feature
   ```

## Development Workflow

### 1. Making Changes

- Keep changes focused and atomic
- Follow existing code style
- Add tests for new functionality
- Update documentation as needed

### 2. Code Style

#### C++
- Follow C++20 best practices
- Use meaningful variable names
- Prefer `const` where applicable
- Use smart pointers over raw pointers
- RAII for resource management

**Example:**
```cpp
// Good
auto importer = std::make_unique<GitHubImporter>();
const auto url_info = importer->parseUrl(url);

// Avoid
GitHubImporter* importer = new GitHubImporter();
auto url_info = importer->parseUrl(url);
delete importer;
```

#### QML
- Use consistent indentation (4 spaces)
- Group properties logically
- Use property bindings where appropriate
- Add comments for complex logic

**Example:**
```qml
Rectangle {
    id: root
    
    // Size properties
    width: 200
    height: 100
    
    // Visual properties
    color: "#f0f0f0"
    border.color: "#cccccc"
    border.width: 1
    
    // Content
    Text {
        anchors.centerIn: parent
        text: "Hello"
    }
}
```

### 3. Commits

Write clear, descriptive commit messages:

```
Short summary (50 chars or less)

More detailed explanation if needed. Wrap at 72 characters.
Explain what changes were made and why.

- Bullet points are okay
- Use imperative mood: "Add feature" not "Added feature"
```

### 4. Testing

Always test your changes:

```bash
# Quick: Simulate full CI workflow locally
./simulate_workflow.sh

# Or manually:
# Build
cmake --build build --config Release

# Run tests
cd build
ctest --output-on-failure

# Run specific test
./tests/test_github_importer
```

**Workflow Simulation Options:**
```bash
./simulate_workflow.sh              # Full simulation
./simulate_workflow.sh --no-tests   # Skip tests (faster)
./simulate_workflow.sh --debug      # Debug build
./simulate_workflow.sh --no-lint    # Skip lint checks
```

The simulation script mirrors the gated tree workflow used in CI, ensuring your changes will pass before pushing.

#### Writing Tests

Add tests for new functionality:

```cpp
#include <QtTest/QtTest>
#include "my_class.h"

class TestMyClass : public QObject
{
    Q_OBJECT

private slots:
    void testFeature();
};

void TestMyClass::testFeature()
{
    MyClass obj;
    QCOMPARE(obj.getValue(), 42);
}

QTEST_MAIN(TestMyClass)
#include "test_my_class.moc"
```

### 5. Documentation

Update documentation for:
- New features
- API changes
- Build requirements
- Usage instructions

Documentation locations:
- `README.md`: Overview and quick start
- `docs/BUILDING.md`: Build instructions
- `docs/ARCHITECTURE.md`: Architecture details
- Code comments: Public API and complex logic

## Pull Request Process

### 1. Before Submitting

- [ ] Code builds successfully
- [ ] All tests pass
- [ ] New tests added for new features
- [ ] Documentation updated
- [ ] Code follows style guidelines
- [ ] Commit messages are clear
- [ ] Workflow simulation passes: `./simulate_workflow.sh`

**CI Gated Tree Workflow:**
Your PR must pass through the gated tree workflow:
1. **Lint checks** - Code quality and formatting
2. **Build jobs** - All platforms (Linux, macOS, Windows)
3. **Test jobs** - All tests must pass
4. **Gate check** - Final verification that all jobs succeeded

The gate will block merging if any check fails.

### 2. Submitting PR

1. Push your branch to your fork:
   ```bash
   git push origin feature/my-new-feature
   ```

2. Open a pull request on GitHub

3. Fill in the PR template:
   - Describe the changes
   - Reference related issues
   - List testing performed
   - Add screenshots for UI changes

### 3. Review Process

- Maintainers will review your PR
- Address feedback by pushing new commits
- Once approved, maintainer will merge

### 4. After Merge

- Delete your feature branch
- Pull latest changes:
   ```bash
   git checkout main
   git pull upstream main
   ```

## Types of Contributions

### Bug Fixes

1. Check if issue already exists
2. If not, create an issue describing the bug
3. Reference the issue in your PR

### New Features

1. Discuss in an issue first (for large features)
2. Implement the feature
3. Add tests and documentation
4. Submit PR

### Documentation

Documentation improvements are always welcome:
- Fix typos
- Clarify instructions
- Add examples
- Improve organization

### Testing

Help improve test coverage:
- Add missing tests
- Improve existing tests
- Add integration tests

## Project Structure

```
tla_visualiser/
├── CMakeLists.txt          # Build configuration
├── conanfile.txt           # Dependencies
├── src/                    # C++ implementation
├── include/                # Public headers
├── qml/                    # QML UI files
├── tests/                  # Unit tests
├── examples/               # Example TLA+ specs
├── docs/                   # Documentation
└── .github/workflows/      # CI/CD
```

## Development Tips

### Building in Debug Mode

```bash
cmake -B build-debug -G Ninja \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_TOOLCHAIN_FILE=build-debug/conan_toolchain.cmake
cmake --build build-debug
```

### Running with Debugging

```bash
# GDB
gdb ./build-debug/tla_visualiser

# LLDB
lldb ./build-debug/tla_visualiser
```

### Qt Creator

Open `CMakeLists.txt` in Qt Creator for integrated development:
- Syntax highlighting
- Code completion
- Visual debugging
- QML preview

### Verbose Build

```bash
cmake --build build --config Release --verbose
```

### CMake Reconfigure

```bash
cmake --build build --target rebuild_cache
```

## Getting Help

- **GitHub Issues**: Report bugs or request features
- **Discussions**: Ask questions or discuss ideas
- **Documentation**: Check `docs/` directory

## Recognition

Contributors will be:
- Listed in release notes
- Credited in documentation
- Acknowledged in the project

Thank you for contributing to TLA+ Visualiser!

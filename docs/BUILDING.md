# Building from Source

This guide provides detailed instructions for building TLA+ Visualiser from source on different platforms.

## Prerequisites

### Required Tools

- **CMake** 3.22 or later
- **Ninja** build system
- **Conan** 1.x or 2.x package manager
- **C++ Compiler** with C++20 support:
  - GCC 10+ (Linux)
  - Clang 12+ (macOS)
  - MSVC 2019+ (Windows)
- **Qt 6.5+** with QML modules

### Installing Prerequisites

#### Ubuntu/Debian
```bash
# System packages
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    libcurl4-openssl-dev \
    qt6-base-dev \
    qt6-declarative-dev \
    libqt6quick6 \
    python3-pip

# Conan
pip3 install conan
```

#### macOS
```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install qt@6 curl cmake ninja python3

# Conan
pip3 install conan
```

#### Windows
```powershell
# Install Chocolatey if not already installed
Set-ExecutionPolicy Bypass -Scope Process -Force
[System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072
iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))

# Install build tools
choco install cmake ninja git python3

# Install Qt from official installer
# Download from: https://www.qt.io/download

# Install Conan
pip install conan
```

## Build Steps

### 1. Clone Repository
```bash
git clone https://github.com/johndoe6345789/tla_visualiser.git
cd tla_visualiser
```

### 2. Configure Conan Profile
```bash
# Detect default profile
conan profile detect

# View profile (optional)
conan profile show default
```

### 3. Install Dependencies
```bash
conan install . --output-folder=build --build=missing
```

This will download and build libcurl and any other Conan dependencies.

### 4. Configure Build

#### Linux/macOS
```bash
cmake -B build -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_TOOLCHAIN_FILE=build/conan_toolchain.cmake
```

#### Windows (PowerShell)
```powershell
cmake -B build -G Ninja `
    -DCMAKE_BUILD_TYPE=Release `
    -DCMAKE_TOOLCHAIN_FILE=build/conan_toolchain.cmake
```

### 5. Build
```bash
cmake --build build --config Release
```

Build with multiple cores:
```bash
cmake --build build --config Release --parallel 4
```

### 6. Run Tests
```bash
cd build
ctest --output-on-failure
```

### 7. Install (Optional)
```bash
cmake --install build --prefix /usr/local
```

## Troubleshooting

### Qt Not Found

If CMake cannot find Qt, specify the Qt installation directory:

```bash
cmake -B build -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_TOOLCHAIN_FILE=build/conan_toolchain.cmake \
    -DQt6_DIR=/path/to/Qt/6.5.0/gcc_64/lib/cmake/Qt6
```

On macOS with Homebrew:
```bash
export Qt6_DIR=$(brew --prefix qt@6)/lib/cmake/Qt6
```

### Conan Errors

If Conan fails to install dependencies:

1. Check your Conan version:
   ```bash
   conan --version
   ```

2. Update Conan:
   ```bash
   pip install --upgrade conan
   ```

3. Clean Conan cache:
   ```bash
   conan remove "*" -c
   ```

### Build Failures

Clean build directory and retry:
```bash
rm -rf build
conan install . --output-folder=build --build=missing
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=build/conan_toolchain.cmake
cmake --build build --config Release
```

### CURL Not Found

Ensure libcurl is installed:

- **Linux**: `sudo apt-get install libcurl4-openssl-dev`
- **macOS**: `brew install curl`
- **Windows**: Should be provided by Conan

## Build Options

### Debug Build
```bash
cmake -B build -G Ninja \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_TOOLCHAIN_FILE=build/conan_toolchain.cmake
cmake --build build --config Debug
```

### Verbose Build Output
```bash
cmake --build build --config Release --verbose
```

### Clean Build
```bash
cmake --build build --target clean
```

## Advanced Configuration

### Custom Compiler
```bash
export CC=clang
export CXX=clang++
cmake -B build -G Ninja ...
```

### Static Linking (Linux)
```bash
cmake -B build -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_TOOLCHAIN_FILE=build/conan_toolchain.cmake \
    -DBUILD_SHARED_LIBS=OFF
```

### Cross-Compilation

For ARM64 on x86_64 Linux with QEMU:
```bash
# Install cross-compilation tools
sudo apt-get install g++-aarch64-linux-gnu qemu-user-static

# Configure Conan for cross-compilation
conan install . --output-folder=build-arm64 \
    --build=missing \
    -s arch=armv8 \
    -s os=Linux

# Build
cmake -B build-arm64 -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_TOOLCHAIN_FILE=build-arm64/conan_toolchain.cmake \
    -DCMAKE_C_COMPILER=aarch64-linux-gnu-gcc \
    -DCMAKE_CXX_COMPILER=aarch64-linux-gnu-g++

cmake --build build-arm64
```

## Platform-Specific Notes

### Linux
- Requires X11 or Wayland for GUI
- Qt platform plugin may need explicit selection: `export QT_QPA_PLATFORM=xcb`

### macOS
- Builds create a `.app` bundle
- May need to sign for distribution: `codesign -s - build/tla_visualiser.app`

### Windows
- Requires Visual Studio or MinGW compiler
- Qt DLLs must be in PATH or copied to exe directory
- Use `windeployqt` to gather dependencies:
  ```powershell
  windeployqt build/tla_visualiser.exe
  ```

## Next Steps

After successful build:
1. See [USAGE.md](USAGE.md) for application usage
2. See [CONTRIBUTING.md](CONTRIBUTING.md) for development guidelines
3. Run the application: `./build/tla_visualiser`

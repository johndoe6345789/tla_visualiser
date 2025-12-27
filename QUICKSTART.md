# Quick Start Guide

Get up and running with TLA+ Visualiser in 5 minutes!

## Prerequisites

Make sure you have:
- CMake 3.22+
- Qt 6.5+
- A C++20 compiler
- Conan package manager

See [BUILDING.md](docs/BUILDING.md) for detailed installation instructions.

## Installation

### Step 1: Clone and Install Dependencies

```bash
# Clone repository
git clone https://github.com/johndoe6345789/tla_visualiser.git
cd tla_visualiser

# Install dependencies
pip install conan
conan profile detect
conan install . --output-folder=build --build=missing
```

### Step 2: Build

```bash
# Configure
cmake -B build -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_TOOLCHAIN_FILE=build/conan_toolchain.cmake

# Build
cmake --build build --config Release
```

### Step 3: Run

```bash
./build/tla_visualiser
```

## First Use

### 1. Import a Specification

**Option A: From Examples**
- Open the application
- Go to: File → Import from GitHub
- Paste: `https://github.com/tlaplus/Examples/blob/master/specifications/ewd998/EWD998.tla`
- Click Import

**Option B: Local Example**
- Use the included example: `examples/SimpleCounter.tla`
- Import it directly through the Import tab

### 2. Run Model Checker

- After importing, click "Run Model Checker"
- Wait for TLC to complete
- Results will appear automatically

### 3. Explore Results

**Graph View:**
- Click the "Graph View" tab
- See the state space visualized as a graph
- Click nodes to inspect states

**Trace View:**
- Click the "Trace View" tab
- Navigate through execution traces step-by-step
- View variable values at each step

**Invariants:**
- Click the "Invariants" tab
- See which invariants passed/failed

### 4. Export

Export your results:
- **Graph**: File → Export Graph → SVG or PNG
- **Trace**: File → Export Trace → JSON or Markdown

## Common Workflows

### Workflow 1: Explore Public TLA+ Specs

```
1. Find a TLA+ spec on GitHub
2. Copy the URL (file or repository)
3. Import via File → Import from GitHub
4. Run model checker
5. Visualize and explore
```

### Workflow 2: Analyze Your Own Specs

```
1. Create your .tla file locally
2. Optionally create a .cfg file
3. In TLA+ Visualiser, use Import tab
4. Browse to your local file
5. Run and analyze
```

### Workflow 3: Compare Runs

```
1. Run model checker on a spec
2. File → Save Run Results
3. Modify spec or config
4. Run again
5. File → Load Run Results to compare
```

## Tips & Tricks

### Keyboard Shortcuts

- `Ctrl+I` (Cmd+I on Mac): Import dialog
- `Ctrl+R` (Cmd+R): Run model checker
- `Ctrl+E` (Cmd+E): Export current view
- `Ctrl+Q` (Cmd+Q): Quit

### Performance

For large state spaces:
- Start with small models
- Use state constraints in TLC config
- Increase Java heap size: `export JAVA_OPTS="-Xmx4g"`

### Caching

Imported specs are cached in:
- Linux/Mac: `/tmp/tla_visualiser_cache/`
- Windows: `%TEMP%\tla_visualiser_cache\`

Clear cache to force re-download:
```bash
rm -rf /tmp/tla_visualiser_cache/
```

## Troubleshooting

### TLC Not Found

Make sure TLC (tla2tools.jar) is:
- Downloaded from https://github.com/tlaplus/tlaplus/releases
- In your PATH or current directory
- Accessible via `java -jar tla2tools.jar`

### Qt Not Found During Build

Specify Qt location:
```bash
export Qt6_DIR=/path/to/Qt/6.5.0/gcc_64/lib/cmake/Qt6
cmake -B build ...
```

### Import Fails

- Check internet connection
- Verify GitHub URL is correct
- Check GitHub rate limits (60 requests/hour without auth)

### Build Errors

Clean and rebuild:
```bash
rm -rf build
conan install . --output-folder=build --build=missing
cmake -B build ...
cmake --build build
```

## Next Steps

- Read the full [README](README.md)
- Check [ARCHITECTURE](docs/ARCHITECTURE.md) for design details
- See [CONTRIBUTING](CONTRIBUTING.md) to contribute
- Try example specs in `examples/`

## Getting Help

- **Documentation**: Check `docs/` directory
- **Issues**: https://github.com/johndoe6345789/tla_visualiser/issues
- **Examples**: See `examples/` directory

## Example Session

```bash
# Start the app
./build/tla_visualiser

# In the application:
# 1. File → Import from GitHub
# 2. Enter: https://github.com/tlaplus/Examples/blob/master/specifications/ewd998/EWD998.tla
# 3. Click Import
# 4. Click "Run Model Checker"
# 5. Switch to "Graph View" tab
# 6. Explore the state space!
```

Happy visualizing! 🚀

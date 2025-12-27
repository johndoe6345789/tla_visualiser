# Architecture Overview

This document describes the architecture and design of TLA+ Visualiser.

## High-Level Architecture

```
┌─────────────────────────────────────────────────────────┐
│                     QML UI Layer                        │
│  (ImportView, GraphView, TraceView, InvariantView)      │
└────────────────────┬────────────────────────────────────┘
                     │ Qt Signals/Slots
┌────────────────────▼────────────────────────────────────┐
│                Qt Models (QAbstractListModel)           │
│      StateGraphModel      TraceViewerModel              │
└──────────┬──────────────────────┬───────────────────────┘
           │                      │
┌──────────▼──────────┐  ┌────────▼─────────────────────┐
│  GitHubImporter     │  │      TLCRunner               │
│  - URL parsing      │  │  - Process management        │
│  - HTTP requests    │  │  - Output parsing            │
│  - Local caching    │  │  - Result serialization      │
└─────────────────────┘  └──────────────────────────────┘
```

## Component Details

### 1. UI Layer (QML)

**Purpose**: Provides the user interface using Qt Quick/QML.

**Components**:
- `main.qml`: Application window, menu bar, tab navigation
- `ImportView.qml`: GitHub URL input and spec preview
- `GraphView.qml`: State/transition graph visualization
- `TraceView.qml`: Step-by-step trace inspection
- `InvariantView.qml`: Invariant status dashboard

**Communication**: Uses Qt's property binding and signal/slot mechanism to interact with C++ models.

### 2. Model Layer (Qt Models)

#### StateGraphModel
**Responsibility**: Provides data for state graph visualization.

**Key Methods**:
- `loadFromResults()`: Populate from TLC results
- `getTransitions()`: Return transition edges
- `getStateDetails()`: Get details for specific state

**Data**:
- States with positions (x, y coordinates)
- Transitions (edges between states)
- Layout calculated using circular algorithm

#### TraceViewerModel
**Responsibility**: Provides data for trace/counterexample viewing.

**Key Methods**:
- `loadTrace()`: Load a counterexample trace
- `exportToJson()`: Export trace as JSON
- `exportToMarkdown()`: Export trace as Markdown

**Data**:
- Sequence of trace steps
- Current step index
- Variable values at each step

### 3. Business Logic Layer

#### GitHubImporter
**Responsibility**: Fetch TLA+ specs from GitHub.

**Features**:
- **URL Parsing**: Regex-based parsing of GitHub URLs
  - File URLs: `github.com/owner/repo/blob/branch/path`
  - Raw URLs: `raw.githubusercontent.com/owner/repo/branch/path`
  - Repo URLs: `github.com/owner/repo`

- **HTTP Communication**: Uses libcurl for HTTP requests
  - Follows redirects
  - Sets User-Agent header
  - Error handling

- **Local Caching**: Saves fetched content to temp directory
  - Cache key: `owner_repo_branch_filepath`
  - Automatic cache lookup before fetching

**Design Pattern**: PIMPL (Pointer to Implementation)
- Public interface in header
- Private implementation in .cpp
- Hides libcurl details from API users

#### TLCRunner
**Responsibility**: Execute TLC model checker and parse results.

**Features**:
- **Process Management**: Runs TLC as external Java process
  - Async execution in separate thread
  - Cancellation support
  - Status callbacks

- **Output Parsing**: Parses TLC text output
  - State count extraction
  - Error message collection
  - Timing information

- **Result Persistence**: Save/load results
  - Text-based format (upgradable to JSON)
  - Deterministic runs

**States**:
- NotStarted
- Running
- Completed
- Failed
- Cancelled

**Design Pattern**: PIMPL + Observer (callbacks)

## Data Flow

### Import Flow
```
User enters URL
    ↓
ImportView captures input
    ↓
GitHubImporter.parseUrl()
    ↓
GitHubImporter.fetchFile()
    ├─→ Check cache
    └─→ HTTP request (libcurl)
    ↓
Content displayed in ImportView
```

### Model Check Flow
```
User clicks "Run Model Checker"
    ↓
TLCRunner.startModelCheck()
    ↓
Spawn Java process (TLC)
    ↓
Parse output in background thread
    ↓
Update status via callback
    ↓
TLCRunner.getResults()
    ↓
StateGraphModel.loadFromResults()
TraceViewerModel.loadTrace()
    ↓
UI updates via Qt property bindings
```

### Visualization Flow
```
User switches to Graph View
    ↓
StateGraphModel provides data
    ├─→ Node positions (calculated)
    ├─→ Edge list
    └─→ State details
    ↓
QML Canvas or GraphicsView renders
    ↓
User clicks node
    ↓
Model provides details for selected node
```

## Design Patterns

### PIMPL (Pointer to Implementation)
Used in: `GitHubImporter`, `TLCRunner`, `StateGraphModel`, `TraceViewerModel`

**Benefits**:
- Binary compatibility (ABI stability)
- Faster compilation (hidden dependencies)
- Implementation flexibility

**Structure**:
```cpp
class MyClass {
public:
    MyClass();
    ~MyClass();
    void publicMethod();
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};
```

### Observer Pattern
Used in: `TLCRunner` callbacks

**Benefits**:
- Async notification
- Decoupling

**Usage**:
```cpp
runner.setStatusCallback([](Status s) {
    // Handle status change
});
```

### Model-View (Qt)
Used throughout for UI

**Benefits**:
- Separation of concerns
- Automatic updates
- Testable models

### Factory Pattern (Future)
Could be used for:
- Different graph layout algorithms
- Export format handlers

## Threading Model

### Main Thread
- Qt event loop
- UI updates
- User interaction
- Model data access

### Background Threads
- TLC execution (via `std::thread`)
- HTTP requests (via libcurl)

**Synchronization**: Qt signals/slots (thread-safe when queued)

## Memory Management

- **Smart Pointers**: `std::unique_ptr` for PIMPL idiom
- **Qt Parent-Child**: QObjects use Qt's parent-child ownership
- **RAII**: Resources cleaned up in destructors
- **No Manual new/delete**: Prefer stack allocation or smart pointers

## Error Handling

### Current Strategy
- Return values (bool for success/failure)
- Empty returns for errors (empty string, empty vector)
- Error messages stored in result structures

### Future Improvements
- Exception-based error handling
- Error codes enumeration
- Detailed error context

## Dependencies

### External Libraries
- **Qt 6**: UI framework, models, networking
- **libcurl**: HTTP requests
- **C++ Standard Library**: Containers, threading, filesystem

### Dependency Management
- **Conan**: Package manager for libcurl
- **CMake**: Build system integration
- **Find Modules**: Qt6 via CMake's built-in finder

## Extensibility Points

### Adding New Visualizations
1. Create new QML view (e.g., `TimelineView.qml`)
2. Create corresponding Qt model (e.g., `TimelineModel`)
3. Add tab in `main.qml`
4. Load data from `TLCRunner::RunResults`

### Adding New Import Sources
1. Create new importer class (e.g., `GitLabImporter`)
2. Implement common interface
3. Add UI option to select source
4. Integrate in `ImportView`

### Adding New Export Formats
1. Add export method to model (e.g., `exportToXML()`)
2. Add UI button/menu item
3. Implement serialization logic

## Testing Strategy

### Unit Tests
- **GitHubImporter**: URL parsing, cache operations
- **TLCRunner**: Status management, result saving
- **Models**: Data loading, transformations

### Integration Tests (Future)
- End-to-end import flow
- TLC execution with sample specs
- Export functionality

### UI Tests (Future)
- QML test framework
- Automated UI interaction

## Performance Considerations

### Current Optimizations
- Circular layout O(n) complexity
- Local caching to avoid redundant downloads
- Async TLC execution

### Future Optimizations
- Lazy loading for large graphs
- Level-of-detail rendering
- Progressive result display
- Worker threads for layout calculation

## Security Considerations

### Current Measures
- No credential storage
- Read-only spec access
- Local cache in temp directory

### Future Improvements
- GitHub token authentication (optional)
- HTTPS certificate validation
- Input sanitization for TLC invocation
- Sandboxed TLC execution

## Build System

### CMake Configuration
- Minimum version: 3.22
- C++20 standard requirement
- Qt6 modules: Core, Gui, Quick, Widgets
- Conan integration via toolchain file

### Platform Support
- **Linux**: Native builds, ARM64 via QEMU
- **macOS**: Native builds (x86_64, arm64)
- **Windows**: MSVC and MinGW support

### CI/CD
- GitHub Actions workflows
- Multi-platform builds
- Automated testing
- Artifact uploads

## Future Architecture Improvements

1. **Plugin System**: Allow third-party visualizations
2. **Async I/O**: libuv or Boost.Asio for better performance
3. **Database**: SQLite for result persistence
4. **Language Server**: TLA+ LSP integration
5. **Web Backend**: Optional web UI via REST API

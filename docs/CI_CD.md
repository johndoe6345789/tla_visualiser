# CI/CD and Gated Tree Workflow

This document describes the Continuous Integration/Continuous Deployment (CI/CD) setup and the gated tree workflow pattern used in the TLA+ Visualiser project.

## Overview

The project uses a **gated tree workflow** pattern to ensure code quality and prevent broken code from being merged into protected branches. This pattern provides:

- **Fast feedback** through early lint checks
- **Parallel execution** of platform-specific builds
- **Clear merge criteria** via a final gate check
- **Simulation capability** for local testing

## Workflow Architecture

### Trigger Events

The workflow runs on:
- **Push** to `main` or `develop` branches
- **Pull Request** targeting `main` or `develop` branches
- **Manual trigger** (`workflow_dispatch`) with customizable parameters

### Job Dependency Graph

```
┌─────────────┐
│    Lint     │ ← Fast pre-build checks
└──────┬──────┘
       │
       ├─────────────────┬─────────────────┐
       │                 │                 │
       ▼                 ▼                 ▼
┌─────────────┐   ┌─────────────┐   ┌─────────────┐
│Build Linux  │   │Build macOS  │   │Build Windows│
│  (x64+arm64)│   │             │   │             │
└──────┬──────┘   └──────┬──────┘   └──────┬──────┘
       │                 │                 │
       └────────┬────────┴─────────────────┘
                │
                ▼
         ┌─────────────┐
         │    Gate     │ ← Final verification
         └─────────────┘
```

## Workflow Jobs

### 1. Lint Job

**Purpose:** Fast, early checks to catch common issues before expensive builds.

**Checks performed:**
- File formatting (trailing whitespace)
- TODO/FIXME tracking (warns if no issue reference)
- CMake syntax validation

**Why first?**
- Fails fast (saves CI resources)
- Takes seconds vs. minutes for builds
- No dependencies required

**Example output:**
```
✅ No trailing whitespace found
⚠️  Warning: Found TODOs without issue references
✅ CMake found: cmake version 3.22.0
```

### 2. Build Jobs

**Purpose:** Compile the application on all supported platforms.

**Platforms:**
- **Linux x64** - Standard Linux build
- **Linux ARM64** - ARM build using QEMU emulation
- **macOS** - Apple platform (x86_64/ARM64 universal)
- **Windows** - MSVC build

**Dependencies:**
- Requires lint job to succeed
- Can be selectively run via `workflow_dispatch`

**Each build job:**
1. Sets up platform-specific dependencies (Qt6, Conan, compilers)
2. Installs Conan dependencies
3. Configures CMake with appropriate toolchain
4. Builds in Release mode
5. Runs tests via CTest
6. Uploads build artifacts

**Conditional execution:**
```yaml
if: |
  always() &&
  (needs.lint.result == 'success') &&
  (github.event_name != 'workflow_dispatch' || 
   contains(github.event.inputs.platforms, 'linux'))
```

### 3. Gate Job

**Purpose:** Final verification that all required checks passed - the "merge-ready" indicator.

**Function:**
- Runs after all build jobs complete (using `needs:` and `if: always()`)
- Checks status of each required job
- Fails if:
  - Lint check failed
  - Any build job failed
  - No builds were executed
- Generates summary report

**Gate logic:**
```bash
# Must pass
- Lint: success

# Must be success OR skipped (for workflow_dispatch)
- Build Linux: success/skipped
- Build macOS: success/skipped
- Build Windows: success/skipped

# Constraint: At least one build must run
```

**Summary output:**
```markdown
### Gated Tree Workflow Results

| Check          | Status  |
|----------------|---------|
| Lint           | success |
| Linux Build    | success |
| macOS Build    | success |
| Windows Build  | success |
```

## Workflow Simulation

### Local Testing

The `simulate_workflow.sh` script allows developers to test the entire CI workflow locally before pushing.

**Basic usage:**
```bash
# Full simulation
./simulate_workflow.sh

# Skip tests (faster iteration)
./simulate_workflow.sh --no-tests

# Debug build
./simulate_workflow.sh --debug

# Skip lint (when you know lint will fail)
./simulate_workflow.sh --no-lint

# View help
./simulate_workflow.sh --help
```

**What it does:**
1. Runs lint checks (same as CI)
2. Configures and builds the project
3. Runs tests with CTest
4. Reports gate status

**Output format:**
- Color-coded results (✅ green, ❌ red, ⚠️ yellow)
- Summary table
- Clear pass/fail indication

**Benefits:**
- Test changes before pushing
- Save CI resources
- Faster feedback loop
- Identical to CI behavior

### Manual Workflow Triggers

You can manually trigger workflows from the GitHub Actions tab with custom parameters:

**Parameters:**
- `run_tests` (boolean) - Whether to run tests
- `platforms` (string) - Comma-separated platform list: `linux,macos,windows`

**Use cases:**
- Test specific platform builds
- Quick build without tests
- Workflow development and testing

**Example:**
```
Platforms: linux
Run tests: false
```
This would only build Linux, skip macOS and Windows, and not run tests.

## Branch Protection

To enforce the gated tree pattern, configure branch protection rules:

### Recommended Settings for `main` branch:

1. **Require pull request reviews**
   - At least 1 approval

2. **Require status checks to pass**
   - Required checks:
     - `Lint and Code Quality`
     - `Gated Tree Check`
   - These ensure no code can be merged if it fails lint or the gate

3. **Require branches to be up to date**
   - Ensures PRs are tested against latest main

4. **Include administrators**
   - Apply rules to everyone (recommended)

### Configuration in GitHub:

```
Settings → Branches → Branch protection rules → Add rule

Branch name pattern: main

☑ Require a pull request before merging
  ☑ Require approvals: 1

☑ Require status checks to pass before merging
  ☑ Require branches to be up to date before merging
  Status checks:
    - Lint and Code Quality
    - Gated Tree Check

☑ Include administrators
```

## Workflow Development

### Testing Workflow Changes

1. **Create feature branch:**
   ```bash
   git checkout -b workflow/test-changes
   ```

2. **Modify workflow file:**
   ```bash
   vim .github/workflows/build.yml
   ```

3. **Test locally:**
   ```bash
   ./simulate_workflow.sh
   ```

4. **Push and create PR:**
   ```bash
   git push origin workflow/test-changes
   # Create PR on GitHub
   ```

5. **Use workflow_dispatch:**
   - Go to Actions tab
   - Select "Build and Test" workflow
   - Click "Run workflow"
   - Select your branch
   - Customize parameters
   - Run workflow

### Debugging Failed Workflows

1. **View workflow run:**
   - Go to Actions tab
   - Click on failed run
   - Expand failed job
   - Review logs

2. **Download artifacts:**
   - Failed builds still upload artifacts when possible
   - Download from workflow run page

3. **Reproduce locally:**
   ```bash
   # Same environment as CI
   ./simulate_workflow.sh
   
   # Or specific step
   cmake --build build --config Release
   ```

4. **Check gate job:**
   - Gate job shows status of all jobs
   - Summary indicates which job(s) failed

## CI Performance Optimization

### Current Optimizations

1. **Lint runs first** - Fails fast for formatting issues
2. **Parallel builds** - All platforms build simultaneously
3. **Conditional execution** - Skip unnecessary jobs with workflow_dispatch
4. **Artifact caching** - Conan packages cached between runs
5. **QEMU for ARM** - Build ARM without native hardware

### Future Improvements

- [ ] Cache Conan dependencies between runs
- [ ] Parallel test execution
- [ ] Incremental builds
- [ ] Docker-based builds for consistency
- [ ] Matrix expansion for more platforms

## Security Considerations

### Workflow Security

- **No secrets in logs** - Workflow doesn't use secrets
- **Pinned actions versions** - `actions/checkout@v3` (should update to SHA)
- **Limited permissions** - Default GITHUB_TOKEN permissions
- **No untrusted input** - workflow_dispatch inputs are sanitized

### Recommended Actions

1. **Pin action versions to SHA:**
   ```yaml
   - uses: actions/checkout@8e5e7e5ab8b370d6c329ec480221332ada57f0ab # v3
   ```

2. **Review third-party actions:**
   - `docker/setup-qemu-action@v2`
   - `jurplel/install-qt-action@v3`

3. **Enable Dependabot:**
   ```yaml
   # .github/dependabot.yml
   version: 2
   updates:
     - package-ecosystem: "github-actions"
       directory: "/"
       schedule:
         interval: "weekly"
   ```

## Metrics and Monitoring

### Key Metrics

- **Workflow run time** - Target: < 20 minutes
- **Success rate** - Target: > 90%
- **Time to feedback** - Lint: < 2 min, Full: < 20 min
- **Resource usage** - Monitor CI minute consumption

### Monitoring

1. **GitHub Actions dashboard:**
   - View run history
   - Success/failure rates
   - Duration trends

2. **Workflow run badges:**
   ```markdown
   ![Build Status](https://github.com/johndoe6345789/tla_visualiser/workflows/Build%20and%20Test/badge.svg)
   ```

3. **Notification settings:**
   - Configure in repository settings
   - Email on workflow failure
   - GitHub notifications

## Troubleshooting

### Common Issues

**Problem:** Gate fails even though all jobs succeeded
```
Solution: Check if any job was skipped unexpectedly
- Review job conditions
- Ensure at least one build runs
```

**Problem:** Simulation passes locally but CI fails
```
Solution: Environment differences
- Check dependency versions
- Verify CMake/Conan versions match CI
- Review platform-specific issues
```

**Problem:** ARM64 build fails
```
Solution: QEMU issues
- Check QEMU setup step
- Verify arm64 platform configured
- May need to update docker/setup-qemu-action
```

**Problem:** Qt6 not found
```
Solution: Installation issues
- Check Qt6 installation step logs
- Verify apt/brew packages installed
- Check Qt6_DIR environment variable
```

## Best Practices

### For Contributors

1. **Always run simulation before pushing:**
   ```bash
   ./simulate_workflow.sh
   ```

2. **Fix lint issues early:**
   - Address warnings
   - Don't bypass checks

3. **Test on your platform:**
   - At minimum, verify your platform builds
   - Cross-platform issues caught in CI

4. **Keep PRs focused:**
   - Smaller PRs = faster CI
   - Easier to diagnose failures

### For Maintainers

1. **Monitor CI health:**
   - Review failed runs
   - Update dependencies regularly
   - Optimize slow jobs

2. **Keep workflows simple:**
   - Clear job names
   - Good error messages
   - Maintainable conditions

3. **Document changes:**
   - Update this file
   - Note in CHANGELOG
   - Communicate to team

4. **Review gate failures:**
   - Don't override without reason
   - Investigate root cause
   - Fix issues, don't mask them

## Resources

### Documentation
- [GitHub Actions Documentation](https://docs.github.com/en/actions)
- [Workflow Syntax](https://docs.github.com/en/actions/reference/workflow-syntax-for-github-actions)
- [GitHub Actions Status Checks](https://docs.github.com/en/pull-requests/collaborating-with-pull-requests/collaborating-on-repositories-with-code-quality-features/about-status-checks)

### Tools
- [act](https://github.com/nektos/act) - Run GitHub Actions locally
- [actionlint](https://github.com/rhysd/actionlint) - Lint workflow files

### Related Files
- `.github/workflows/build.yml` - Workflow definition
- `simulate_workflow.sh` - Local simulation script
- `CONTRIBUTING.md` - Contribution guidelines
- `README.md` - Project overview

---

**Last Updated:** December 2024
**Workflow Version:** 2.0 (Gated Tree)

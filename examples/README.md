# Example TLA+ Specifications

This directory contains example TLA+ specifications for testing and demonstrating TLA+ Visualiser.

## SimpleCounter.tla

A basic counter specification that demonstrates:
- Simple state variables
- Increment/Decrement actions
- Type invariants

### Usage

1. Import the spec in TLA+ Visualiser
2. Run the model checker
3. Explore the state graph (will show counter incrementing/decrementing)

### With Command Line TLC

If you have TLC installed:

```bash
java -jar tla2tools.jar SimpleCounter.tla -config SimpleCounter.cfg
```

## Adding More Examples

Feel free to add more example specs to demonstrate various TLA+ features:
- Concurrent algorithms
- Distributed systems
- Safety/liveness properties
- Deadlock detection

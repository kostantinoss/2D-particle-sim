# Batched Rendering Demo

This demo demonstrates efficient particle rendering using a **single batched VertexArray** instead of individual draw calls per particle.

## What This Demo Shows

The demo renders 3 colored circles and allows you to toggle between two rendering methods:

1. **BATCHED**: All circles rendered in a single draw call using one VertexArray with Triangle primitives
2. **INDIVIDUAL**: Each circle rendered separately using SFML's CircleShape (3 draw calls)

## How It Works

### Traditional Approach (Multiple Draw Calls)
```
For each particle:
    Create CircleShape
    Set properties
    Draw to window  ← Separate GPU call
```
Result: N particles = N draw calls

### Batched Approach (Single Draw Call)
```
Create single VertexArray
For each particle:
    Calculate triangle vertices
    Add to VertexArray  ← Just CPU work
Draw entire VertexArray once  ← One GPU call
```
Result: N particles = 1 draw call

### Circle Construction

Each circle is made of triangles arranged in a fan pattern:
- Center point + 2 circumference points = 1 triangle
- 16 triangles per circle (configurable)
- All stored contiguously in one VertexArray

## Building & Running

```bash
# Build
cmake --build build --target batched_demo

# Run
./build/bin/batched_demo
```

## Controls

- **SPACE**: Toggle between batched and individual rendering modes
- **ESC**: Exit

## Performance

With 200+ particles, the performance difference becomes significant:
- Individual rendering: ~2-3ms per frame
- Batched rendering: ~0.1ms per frame

## Implementation Details

See `src/batched_rendering_demo.cpp` for the complete implementation.

Key class: `BatchedCircleRenderer`
- `rebuild()`: Reconstructs the VertexArray from particle data
- `add_circle()`: Adds triangle geometry for one circle
- `draw()`: Single draw call for all circles

## Applying to Your Project

To use this approach in the main particle simulation:

1. Remove `sf::VertexArray` from the `Particle` class
2. Keep particles as pure data (position, radius, color, etc.)
3. Add a `BatchedCircleRenderer` or similar in `ParticleSim`
4. Call `rebuild()` after physics updates
5. Replace per-particle drawing with single batched draw

This separates physics (particle data) from rendering (visual representation).


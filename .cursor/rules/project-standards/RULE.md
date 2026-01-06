---
description: "Project coding standards and development guidelines for the 2D particle simulation"
alwaysApply: true
---

# Project Standards

## Development Principles

1. **Don't overengineer things**
   - Keep solutions simple and straightforward
   - Avoid unnecessary abstractions or design patterns
   - Only add complexity when there's a clear, justified need

2. **Use modern C++ features (C++17 and later)**
   - Leverage C++17, C++20, and later standard features
   - Prefer modern alternatives to legacy patterns


3. **Code generation policy**
   - **Do NOT generate full implementations unless explicitly requested**
   - When showing structure or architecture:
     - Use template/placeholder functions with clear signatures
     - Include comments explaining what the function should do
     - Show the general structure and flow
     - Indicate where logic should go with `// TODO` or `// Implementation here`
   - Example of acceptable placeholder:
     ```cpp
     // Calculate force between two particles
     Vector2D calculateForce(const Particle& p1, const Particle& p2) {
         // TODO: Implement force calculation
         // Return force vector based on particle properties
         return Vector2D{0, 0};
     }
     ```
   - Only provide complete implementations when the user explicitly asks for them

4. **Communication and explanations**
   - Provide detailed explanations of what you're doing and why
   - Give technical answers with context
   - Explain architectural decisions and trade-offs
   - Include relevant technical details about:
     - Why a particular approach was chosen
     - Performance considerations
     - Potential alternatives and their trade-offs
     - How the code fits into the larger system
   - Be thorough but concise in technical explanations

## Additional Guidelines

- Follow RAII principles for resource management
- Use const correctness wherever possible
- Prefer smart pointers over raw pointers
- Use meaningful variable names related to physics/particles
- Document and explain complex algorithms and non-obvious code


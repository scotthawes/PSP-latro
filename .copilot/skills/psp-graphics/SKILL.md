---
name: psp-graphics
description: "Best practices for PSP GU/GUM rendering, textures, batching, and cache management."
---

# PSP Graphics Skill

Use this skill when modifying `src/graphics.c`, `src/draw.c`, textures, batching code, or when diagnosing render glitches and performance problems.

Core guidelines
- Vertex data must be 16-byte aligned for DMA and GU; use `__attribute__((aligned(16)))`.
- Minimize full dcache operations; prefer `sceKernelDcacheWritebackRange(ptr, size)` for per-batch writes.
- Keep `MAX_QUADS` tuned to your vertex buffer (avoid overly-frequent flushes).
- Texture formats: prefer `GU_PSM_565` for 16-bit RGB or `GU_PSM_4444` for alpha; swizzle textures into VRAM at load time.

Debugging tips
- Broken geometry: verify vertex stride and attribute order (u,v,color,x,y,z).
- Missing textures: ensure swizzled data is uploaded and VRAM pointers are valid.
- Overdraw: batch quads where possible, reduce state changes (texture bindings).

Performance
- Reduce calls to `sceGuFinish()` and `sceGuSync()` in tight loops; group draw calls.
- Avoid `sceKernelDcacheWritebackInvalidateAll()` per-frame — reserve for load-time only.

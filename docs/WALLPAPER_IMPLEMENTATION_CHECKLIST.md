# Wallpaper Implementation Checklist

This checklist is updated using PSPDev and PSPSDK documentation gathered during research.

## Documentation References

- PSPDev basic examples (Drawing Sprites + stb_image flow):
	https://pspdev.github.io/basic_programs.html
- PSPSDK Graphics Utility API reference:
	https://pspdev.github.io/pspsdk/group__GU.html
- PSPSDK sprite sample (texture state + draw flow):
	https://raw.githubusercontent.com/pspdev/pspsdk/master/src/samples/gu/sprite/sprite.c
- PSPSDK blit sample (swizzle and texture cache behavior):
	https://raw.githubusercontent.com/pspdev/pspsdk/master/src/samples/gu/blit/blit.c
- PSPSDK GU sample index:
	https://github.com/pspdev/pspsdk/tree/master/src/samples/gu

## Key Constraints Confirmed

1. GU texture upload expects power-of-two dimensions in sceGuTexImage.
2. Texture data should be 16-byte aligned for safe/fast uploads.
3. Swizzled textures are preferred for performance.
4. Texture cache coherency matters after CPU writes (flush/invalidate before use).
5. Full-screen PSP view is 480x272, but texture storage commonly uses 512 width (or 512x512 texture atlas space).

## Implementation Steps (Project-Specific)

1. Prepare wallpaper files in a format your loader already supports (prefer matching current archive/16-bit pipeline).
2. Store wallpaper assets in a dedicated location (for example: assets/wallpapers/) and register them in the archive/build process.
3. In graphics loader code, decode wallpaper source into a PSP-ready texture buffer.
4. Ensure texture dimensions are power-of-two for GU upload.
5. If the source is 480x272, upload into a power-of-two backing texture and render only the visible UV region.
6. Apply data cache writeback/invalidate after CPU-side pixel writes.
7. Set texture mode and upload explicitly before menu draw (format, swizzle choice, filter mode).
8. Draw wallpaper first in menu render order, then draw menu decorations/text on top.
9. Keep texture state transitions explicit when switching between wallpaper, cards, and non-textured quads.
10. Add clean fallback behavior if wallpaper load fails (solid color or procedural background).

## Current Status (12 May 2026)

### Implementation Steps

- [x] 1. Prepare wallpaper files in a supported format.
- [x] 2. Store wallpaper assets in a dedicated location and register them in archive/build.
	- Dedicated files are now present under assets/wallpapers and menu variants resolve from that folder first.
- [x] 3. Decode wallpaper source into PSP-ready texture buffer.
- [x] 4. Ensure texture dimensions are power-of-two for GU upload.
- [x] 5. Render only visible UV region for 480x272 over power-of-two backing.
- [x] 6. Apply data cache writeback/invalidate after CPU-side writes.
- [x] 7. Set texture mode/upload before menu draw (existing texture pipeline).
- [x] 8. Draw wallpaper first, then menu decorations/text.
- [x] 9. Keep texture state transitions explicit.
- [x] 10. Add fallback behavior on load failure.

### Verification Steps

- [ ] 1. Confirm wallpaper appears on title/main every frame (manual runtime check still needed).
- [ ] 2. Confirm card rendering still works after wallpaper draw (manual regression check still needed).
- [ ] 3. Validate no distortion/aspect/UV alignment at 480x272 (manual visual check still needed).
- [ ] 4. Compare swizzled vs unswizzled performance and keep faster path (not yet benchmarked).
- [ ] 5. Test on PPSSPP and device-targeted settings for compatibility (PPSSPP smoke run done; dedicated wallpaper compatibility pass still needed).
- [ ] 6. Test repeated scene transitions for leaks/stale pointers (not yet stress-tested).

### Optional Enhancements

- [x] 1. Add multiple wallpapers with deterministic setting-based selection.
- [ ] 2. Add metadata table (name, texture id, source path, dimensions, format).
- [x] 3. Add runtime toggle in Options for wallpaper variant.

## Verification Steps

1. Confirm wallpaper appears on title and main menu screens every frame.
2. Confirm card rendering still works after wallpaper draw (no texture state contamination).
3. Validate no distortion: aspect, UV crop, and alignment are correct at 480x272.
4. Compare swizzled vs unswizzled performance and keep the faster path.
5. Test on PPSSPP and device-targeted settings to confirm compatibility.
6. Test repeated scene transitions for leaks or stale texture pointers.

## Optional Next Enhancements

1. Add multiple wallpapers with deterministic selection (theme/seed/settings).
2. Add tiny metadata table (name, texture id, source path, dimensions, format).
3. Add runtime toggle in Options for wallpaper pack/variant.
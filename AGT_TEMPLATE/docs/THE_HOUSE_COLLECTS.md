# The House Collects — placeholder prototype

A single terrace heist built on the supplied C++17/OpenGL game template. Steal
$600 in chips and return to the EXIT pad within 120 seconds. Four guards can
catch you; three successful tags end the round. Ordinary chips alone supply
$600. The $200 gold chip is an optional shortcut that triggers hard mode.

## Open and run on Windows

1. Use the complete university template / this repository's `AGT_TEMPLATE`.
   If applying the small patch ZIP, merge its `AGT_TEMPLATE` directory into
   your repository. Back up any local changes first. It replaces only
   `game/src/example_layer.h` and `game/src/example_layer.cpp`; other files
   in the patch are new. It does not contain the engine or its dependencies.
2. Open `AGT_TEMPLATE/engine.sln` in **Visual Studio 2026** (v145 toolset).
3. Select **Debug / x64** and `game` as the startup project. Leave the bundled
   dependency projects unloaded, as described in the university README.
4. Press F5. If assets cannot be found, set the `game` project's debugging
   working directory to `$(ProjectDir)` (the directory containing `assets`).
5. Press Enter on the intro screen. No additional downloads or Python execution
   are needed; all generated placeholders are included.

The added game modules are header-only, included by `example_layer.cpp`, which
is already in the solution. No project regeneration is required. Use Show All
Files to see them in Visual Studio, or add the headers to the project if desired.

**Build status:** the complete Windows executable has not been built or run in
this Linux environment. Headless rules and geometry tests pass. A Linux syntax
probe of the engine adapter encounters the template's existing MSVC-specific
headers; it is not a Windows build. Rendering, model import, audio playback,
controls and difficulty balance still require the Windows checks below.

## Controls and pickups

| Control | Action |
|---|---|
| Enter | Start; retry after an outcome |
| WASD | Move relative to the camera |
| Space | Jump once per key press; cross low crates or dodge a guard tag |
| Q / E | Orbit the follow camera |
| P | Pause / resume (freezes countdown, guards and power-up timers) |
| R | Restart the whole round |
| M | Mute / unmute background audio and future effects |
| Escape | Quit |

| Pickup | Effect |
|---|---|
| Red chip | $50, collected once |
| Gold chip | $200 and alarm: greater guard detection range and chase speed |
| Blue pass | Clears alarm, sends guards back to their routes, grants 2 seconds of protection |
| Green pyramid | 8-second shield |

A guard's red body marks its attack wind-up. Walk or jump away before the tag.
The guards use patrol, chase, attack and return states, with local obstacle
avoidance. This is deliberately a small open arena, not a maze with pathfinding.
Imported props sit outside the playable walls, so replacing scenery cannot
silently change collision geometry. The four tables and two low crates are solid.

## Where the code lives

- `game/src/casino_game.h`: deterministic game rules, layout, collision, simple
  physics (acceleration, friction/damping, gravity), pickups, guard state machine.
- `game/src/casino_geometry.h`: custom cube, cylinder and pyramid triangle meshes
  with UVs, outward winding and normals; repeating floor geometry.
- `game/src/example_layer.cpp`: university engine adapter, scene, fixed 120 Hz
  simulation, orbit/follow camera, HUD, sound events, lighting and effects.
- `game/src/casino_assets.h`: all replacement asset paths and model sizing.
- `tools/generate_placeholders.py`: reproducible original placeholder generation
  using only Python's standard library. The outputs are already included.
- `tests/casino_tests.cpp`: headless rules and geometry tests.

Tune `target_score`, `round_seconds`, pickup placement and the guard speeds in
`casino_game.h`. Keep enough obtainable chip value to reach the target. Adjust
obstacle dimensions there; the rendered tables/crates use those same dimensions.

## Replace the placeholders

Edit **`game/src/casino_assets.h`**. Paths are relative to `game/`, use forward
slashes, and must travel with your submitted project.

| Slot | Included file(s) | Replacement guidance |
|---|---|---|
| Chair | `assets/casino/models/chair.obj` + `.mtl` | Static chair model; repeated at different scales/rotations |
| Lamp | `assets/casino/models/lamp.obj` + `.mtl` | Static lamp; visual only, not an automatically added light |
| Planter | `assets/casino/models/planter.obj` + `.mtl` | Static plant/planter; repeated around the terrace |
| Thief / guard | Empty paths select block characters | Optional static OBJ/FBX model; all guards share one asset |
| Ground | `textures/paving.png` | Seamless image; UVs repeat at one square texture per 2 metres |
| Objects | `felt.png`, `metal.png`, `chip.png`, `crate.png` | Small RGB/RGBA PNG images |
| Signs | `casino.png`, `exit.png` | Wide PNG images with text baked in |
| Sky | `sky_front/right/back/left/top/bottom.png` | Six matching cubemap faces in that order, not one equirectangular HDR |
| Effects | Six WAV paths in `event_sounds` | Collection, alarm, power-up, damage, win and lose respectively |
| Background | `audio/ambience.wav` | Loopable WAV; MP3 also supported by FMOD when path is changed |

Prefer Y-up, static OBJ with an MTL and diffuse images. Keep MTL-relative texture
paths intact. Models are centred horizontally, placed on the floor and scaled
to the slot's `height`. Use `yaw_degrees` to correct facing direction. Collisions
are independent of visual model dimensions, so use similarly sized replacements.
Skeletons and animated FBX playback are not wired into these character slots.

Missing textures use white; missing/empty prop models use a box; empty character
paths use the block figures. Invalid model files are logged by the importer.
These fallbacks keep development moving, but inspect the console and fix them
before submission. A material without a diffuse image receives a white texture;
that is a fallback, not finished artwork.

The supplied sky is intentionally plain dark blue. All six faces match exactly;
replace them with a coherent skybox if you want visual detail. Image resolution
and filenames can change through the asset header. Avoid overwriting placeholder
files with final assets if you intend to regenerate the placeholders.

To regenerate elsewhere: `python tools/generate_placeholders.py --output <folder>`.
The script refuses to overwrite existing images unless `--force` is passed.

## Coursework coverage and remaining work

| Criterion | Implementation / remaining verification |
|---|---|
| Intro and controls | Intro screen and persistent controls strip |
| Primitive shapes | Cube tables/crates, cylindrical chips, pyramid shields; UVs and normals tested |
| Transformations | Repeated props and procedural objects, rotation and varied scaling |
| Sky and terrain | New unlit seamless placeholder sky, tiled paving |
| Camera | Smoothed player-follow camera, keyboard orbit, camera-relative movement |
| Imported meshes | Three distinct original OBJ placeholders loaded through the engine; replace/polish as desired |
| Theme | Small casino terrace: felt tables, chips, sign, exit carpet, thief and security |
| Audio | Six original event cues and new looping background cue; playback needs Windows verification |
| HUD | Score/target, health, countdown, alarm status, shield duration |
| Lighting | Directional moonlight and alarm point light |
| Two effects | Camera shake and alpha-blended damage flash |
| Physics | Inertia/acceleration, friction/damping, gravity/jumping; fixed update step |
| Four NPCs / AI | Four guards, each with patrol/chase/attack/return states |
| Gameplay additions | Countdown timer and shield power-up, plus security passes |
| Difficulty | Gold chip activates hard mode; blue pass restores easy mode |
| Documentation | This is an implementation guide, not the required assessed reports |

This is a placeholder implementation, not a claim of a particular mark. Before
Milestone 1, visually verify the normals, texture placement, model import,
transforms and camera, then write the maximum-two-page interim report from what
actually works. The final report (7–15 pages), asset/source ledger, AI appendix
and in-class presentation remain your work to complete. Record skipped or
partial features honestly.

## Windows playtest checklist

- Build Debug/x64 and launch with no missing asset errors. Confirm the intro,
  all three imported prop types, readable signs, paving, sky and lit primitives.
- Walk into each table and wall; jump over a low crate; release movement and
  check it decelerates. Check Q/E orbit and camera-relative WASD.
- Collect red and gold chips; verify scores only increase once, hard mode appears,
  guards speed up, and a blue pass returns to easy mode.
- Confirm each guard patrols, detects, chases, telegraphs a tag and returns;
  inspect avoidance around tables. Tune if guards get stuck or rounds are unfair.
- Check shield duration/protection, hit feedback, three-hit loss, timeout and
  a successful $600 escape. The ordinary chips alone must allow a win.
- Pause while a shield is active: countdown, guards and shield must freeze.
  Restart after each outcome: pickups, health, time, alarm and guards must reset.
- Hear the distinct cues and looping ambience. Toggle mute. Resize the window
  and check the HUD and camera. Keep the required FMOD DLL supplied by the template.
- Before submission, follow the brief's cleaning/packaging instructions and test
  F5 on a university machine. Release requires the template's Release libraries;
  they are not installed or built by this change.

## Headless checks

From `AGT_TEMPLATE`, on Linux with g++:

```sh
g++ -std=c++17 -Wall -Wextra -Werror -pedantic -Igame/src -Iengine/external/glm tests/casino_tests.cpp -o /tmp/casino-tests
/tmp/casino-tests
python tests/validate_assets.py
```

These do not validate OpenGL output, FMOD or Windows linking. They exercise
scoring, alarms, protection, physics/collisions, guard transitions, pause/restart,
win/loss rules and primitive index/normal/UV/winding correctness.

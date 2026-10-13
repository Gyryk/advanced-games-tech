# AGT engine — IN3026/INM710 Advanced Games Technology

C++/OpenGL games engine used for the labs and coursework of Advanced Games
Technology at City St George's. This folder is one download from Moodle;
`LESSON.md` says which one, what it is for, and what it adds.

@LESSON.md

## Getting started

1. **Extract the whole ZIP first.** Opening `engine.sln` from inside the ZIP
   fails: Windows quietly unpacks to a temporary folder and the build cannot
   find the engine.
2. Open `engine.sln` in **Visual Studio 2026**. The projects use platform
   toolset v145, which Visual Studio 2022 does not have.
3. `game` is the start-up project. Build and run with F5.

The external library projects (Bullet, GLFW, FreeType, ...) show as
*unloaded*. That is deliberate: they ship prebuilt, so you never wait for them
to compile. Leave them unloaded.

## Layout

```
game/src/              your code - most lab and coursework work goes here
game/assets/           models, textures, audio
engine/src/engine/     core, entities, events, renderer, utils
engine/src/platform/   opengl/, windows/
engine/external/       third-party libraries (prebuilt)
premake5.lua           build definition that generates the project files
```

`game/src/game.cpp` includes the single header `engine/src/engine.h`, which
pulls in the whole engine. Start reading from `game/src/example_layer.cpp`:
the constructor builds the scene, `on_update` moves things each frame,
`on_render` draws them and `on_event` handles key presses.

Only change `engine/src/` when a labsheet asks you to (for example
`projector.h` in Lab 5, or `bounding_box` in Lab 7).

## Adding files

New `.h`/`.cpp` files under `game/src/` can be added through Visual Studio
(*Add > New Item*). Alternatively, create them on disk and run
`generate_solution_vs.bat`, which rebuilds the project files from
`premake5.lua` and picks up everything under `game/src/`. That script
overwrites `engine.sln` and the `.vcxproj` files, so any project settings you
changed by hand in Visual Studio are lost when you run it.

## Conventions

- `snake_case` for types, functions and files, matching the existing code.
- Use lowercase filenames. `Player.cpp` next to `player.h` works on Windows
  but breaks on case-sensitive systems.
- The engine predates C++20 idioms in places; match the surrounding style.

## glm returns, it does not modify

`glm::translate`, `glm::rotate` and `glm::scale` **return** a new matrix and
leave their argument untouched. Always assign the result:

```cpp
transform = glm::translate(transform, position);
transform = glm::rotate(transform, angle, glm::vec3(0.f, 1.f, 0.f));
```

A bare `glm::rotate(transform, ...);` does nothing, and the compiler warns
about the discarded result.

## Mouse input

`input::mouse_position()` is the absolute cursor position.
`input::mouse_delta()` is the movement since the last frame, and
`input::cursor_captured()` says whether the cursor is captured. Camera code
uses the delta, and only while the cursor is captured.

## Models and textures

If a model renders with the wrong textures, check `mesh_materials` first.

Each mesh names its own textures through its assimp material index, and
`model::mesh_materials()` exposes that mapping, parallel to `meshes()`. Pass it
into `game_object_properties::mesh_materials` — as `example_layer` does for the
cow and tree — and the renderer binds diffuse to unit 0 and specular to unit 1
per mesh.

Without it the renderer pairs texture *i* with mesh *i* only when the two
counts happen to be equal, and otherwise binds every texture in turn so the
last one wins. That fallback is for objects built by hand from procedural
shapes, where you set textures in mesh order yourself.

Texture paths inside model files are written however the authoring tool chose:
bare filenames, sub-directories, either slash, sometimes absolute paths from
the artist's machine. `model::resolve_texture` tries the sensible candidates
and **logs an error naming the paths it tried** when it fails, so check the
console before assuming a black mesh is a lighting problem.

`mesh::vertex` carries a tangent for normal mapping. Procedural shapes leave it
zero and the shader falls back to the vertex normal, so a zero tangent is
expected. If you add a vertex attribute, update **both** `mesh::vertex` and the
`buffer_layout` in `gl_mesh.cpp`, and keep the shader's
`layout(location = ...)` in step.

## Build output

A fresh download builds with no warnings at all, so any warning you see
comes from code you added and is worth reading.

## Gotchas

- `engine.h` includes the audio manager, so FMOD is needed by every lesson,
  not just the audio ones. Do not remove `fmod.dll` or `fmod_vc.lib`.
- Build output goes to `bin/` and `inter/`. Delete them to force a clean
  rebuild; leave them out when you zip your work for submission.

## Third-party libraries

The libraries the engine uses, with their licences, are listed in
`THIRD-PARTY-NOTICES.md`. Most are open source. **FMOD is not**: it is
licensed for your use on this module only, so do not publish a project that
contains the FMOD files, for example in a public GitHub repository.

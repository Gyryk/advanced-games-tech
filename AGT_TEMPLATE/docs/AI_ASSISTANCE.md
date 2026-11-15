# AI assistance record — working notes, not the full assessed appendix

The coursework brief requires the sequences of prompts and outputs, and how
those outputs were used. Preserve/export the complete conversation, including
the preceding concept discussions. This note does not replace that transcript.

Implementation request, verbatim:

> can you write the code for the game with placeholders? i will source assets to replace placeholders with

Context supplied by the student: a simple thief-in-a-casino game using the
university C++/OpenGL template; both milestone requirements supplied; building
and playtesting are deferred to the student on Windows. The chosen setting is
a small outdoor casino terrace. The student will source replacement assets.

AI-generated output in this task:

- Replacement `game/src/example_layer.h` and `.cpp` integrating the prototype.
- `casino_game.h`, `casino_geometry.h` and `casino_assets.h`.
- Original placeholder models, textures and audio plus their Python generator.
- Headless test code, asset validation and implementation/replacement notes.

The game uses the university template's rendering, model importer, camera,
text and FMOD interfaces. Its game simulation is separate C++ code; it does not
use the template's Bullet world. Do not describe it as a Bullet implementation.

Complete these sections as you work:

1. Preserve the prompts and outputs (including code/diffs) from this conversation.
2. Explain which generated files/ideas you retained, modified or rejected.
3. Record your own implementation changes and the reasoning you can explain.
4. Record Windows build results and playtesting, including failures and fixes.
5. Record replacement assets and their provenance in `game/assets/casino/ASSETS.md`.

Do not claim that the AI-generated implementation was independently written by
you, or that it has been built/playtested where it has not.

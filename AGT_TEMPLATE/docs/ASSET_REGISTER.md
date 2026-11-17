# Asset and licence register

Prepared 17 November 2026. This register describes the active casino prototype,
not every unused sample asset that remains in the university template.

## Casino-specific assets: generated locally

These assets were generated from scratch for the project by
[`tools/generate_placeholders.py`](../tools/generate_placeholders.py), using
Python's standard library during AI-assisted development on 14 November 2026.
No asset-store downloads, sampled recordings, external fonts for the signs,
or copied image/model files were used in that generator.

**Source:** the local generator and generated files committed with this project.
**External source URL / download date:** not applicable; generated locally.
**Third-party asset licence:** not applicable to these original generated outputs;
no downloaded-asset licence or CC0 claim is attached. This does not remove the
coursework's requirement to acknowledge AI assistance. The template's MIT notice
must not be misrepresented as the origin/licence of third-party fonts or middleware.

All paths below are relative to `game/assets/casino/`.

| Files | Creation method / software | Use |
|---|---|---|
| `models/chair.obj`, `chair.mtl` | Python: seat, back and four legs assembled from cuboids; triangulated OBJ with normals, UVs and diffuse MTL | Repeated imported seating |
| `models/lamp.obj`, `lamp.mtl` | Python: cuboid base, pole, lantern and cap; OBJ/MTL export | Repeated imported lamps |
| `models/planter.obj`, `planter.mtl` | Python: pot/rim and geometric foliage; OBJ/MTL export | Repeated imported planters |
| `textures/paving.png` | Python: alternating paving cells and grout; RGB PNG written using zlib | Tiled ground |
| `textures/felt.png` | Python: green values with deterministic pseudo-random variation | Table surfaces / carpet tint |
| `textures/metal.png` | Python: repeated grey-blue stripes | Metal props and figures |
| `textures/wood.png` | Python: brown stripe pattern | Chair diffuse texture |
| `textures/stone.png` | Python: grey stripe pattern | Planter diffuse texture |
| `textures/crate.png` | Python: brown panels and dark bands | Table bases, barriers and boxes |
| `textures/chip.png` | Python: radial rings and alternating angular segments | Procedural chips and marker surfaces |
| `textures/casino.png`, `textures/exit.png` | Python: original 5x7 bitmap letter patterns, coloured borders and background | Scene signs; no font file used |
| `textures/sky_front.png`, `sky_right.png`, `sky_back.png`, `sky_left.png`, `sky_top.png`, `sky_bottom.png` | Python: six identical solid RGB colours | Unlit seamless placeholder sky |
| `audio/collect.wav` | Python: 660/990 Hz synthesised tones | Chip collection |
| `audio/alarm.wav` | Python: alternating 330/660 Hz tones | Alarm activation |
| `audio/powerup.wav` | Python: ascending 440/660/880 Hz tones | Shield/pass collection |
| `audio/damage.wav` | Python: descending 150/90 Hz tones | Guard tag |
| `audio/win.wav` | Python: ascending 523/659/784/1046 Hz tones | Escape |
| `audio/lose.wav` | Python: descending 330/220/110 Hz tones | Capture/timeout |
| `audio/ambience.wav` | Python: quiet four-note synthesised loop | Background audio |

WAV output is mono PCM16 at 22050 Hz. Tone envelopes and a second harmonic are
computed by the generator. PNGs are generated directly, and OBJ materials refer
to the included images. The geometry and files can be regenerated and inspected;
this supplies evidence of how the assets were made.

Runtime chips, tables, boxes, pyramids and the block characters use geometry
from `game/src/casino_geometry.h` and composition in `example_layer.cpp`. These
are also AI-assisted project code, not imported marketplace models. Their
textures are listed above.

Suitable report wording:

> The casino-specific models, textures and sounds were generated from scratch
> in Python using code developed with ChatGPT assistance. Their creation methods
> and source script are included with the project. The university engine, HUD
> font and middleware are pre-existing components and are credited separately.

Do not replace that with an unqualified statement that all assets/code were
independently made by the student.

## Inherited active assets and software

The inherited components came from the supplied university template; no new
versions were downloaded during casino implementation. Preserve their original
notices. The references below identify the evidence present in this checkout.

| Component | Source / licence evidence | Licence or status |
|---|---|---|
| AGT engine, including mesh/text shaders | Supplied module template; [`LICENSE.MD`](../LICENSE.MD), shader comments | MIT; copyright 2019 Pietro Lazzarotto; retain notice and shader references |
| `game/assets/fonts/arial.ttf` (HUD text) | Supplied template; embedded font name/copyright/licence records | Arial, The Monotype Corporation, copyright 2017. Proprietary Microsoft-supplied font; **not** an original casino asset or an open-source font |
| Assimp 6.0.5 | `engine/external/assimp/LICENSE` | BSD-3-Clause |
| zlib inside Assimp | `engine/external/assimp/contrib/zlib/LICENSE` | zlib |
| Bullet 3.25 | `engine/external/bullet/LICENSE.txt` | zlib; retained with template, not used for the casino's custom physics |
| FreeType 2.14.3 | `engine/external/freetype/FTL.TXT` | FreeType License / GPLv2 alternative; retain supplied notices |
| stb_image 2.30 | `engine/external/stb_image/stb_image.h`, template third-party notices | MIT / Unlicense alternative |
| Glad / Khronos headers | Template third-party notices and generated header comments | MIT loader; retain Khronos header notices (MIT/Apache-2.0 as applicable) |
| GLFW 3.5.1 | `engine/external/GLFW/LICENSE.md` | zlib/libpng |
| GLM 1.0.3 | `engine/external/glm/copying.txt` | MIT / Happy Bunny alternatives |
| spdlog 1.17.0 | `engine/external/spdlog/LICENSE` | MIT; retain bundled fmt notices too |
| premake5 | `vendor/premake/LICENSE.txt` | BSD-3-Clause; supplied build tool |
| FMOD Engine 2.04.00 | `engine/external/FMOD/LICENSE.TXT` | Proprietary; template supplied for module use. Educational-use conditions and attribution are in the EULA |

Full inherited notices: [`THIRD-PARTY-NOTICES.md`](../THIRD-PARTY-NOTICES.md).
FMOD reference: <https://www.fmod.com/attribution>. The intro now includes
“FMOD Studio by Firelight Technologies Pty Ltd.” as required by the supplied
notices; this does not replace retaining the EULA and notices.

### Font evidence and a remaining packaging check

The embedded Arial licence says it is a Microsoft-supplied font, usable to
create/display/print content as permitted by the relevant Microsoft product
terms, with restrictions on embedding and other use. Its separate MIT paragraph
covers Biblical Hebrew layout logic, **not the whole font**. No standalone
permission to redistribute the font was established here. For the course's
supplied-template submission, check the module's permitted packaging; before
any wider distribution, obtain appropriate permission or replace the font with
one carrying an explicit redistribution licence. This register does not invent
an open licence for the bundled file.

### Unused template samples

The original cow, tree and animated warrior, their textures, the original sky,
`bounce.wav` and `DST-impuretechnology.mp3` are not loaded by the casino layer.
They remain in the template checkout. They are not casino-generated assets and
must not be described as original project work; preserve the template notices
if retaining them in an archive.

## Later replacements

No replacement download is needed for Milestone 1 solely to make the graphics
more complex. If an asset is replaced, add its creator, exact URL, download date,
licence text/attribution and modifications here, and retain the licence file.

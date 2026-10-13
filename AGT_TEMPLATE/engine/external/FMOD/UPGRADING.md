# FMOD version history

Currently vendored: **FMOD Engine 2.04.00** (`FMOD_VERSION 0x00020400`),
upgraded from 2.00.04 on 29 September 2026.

`LICENSE.TXT` in this folder came from that same download, so the EULA matches
the binaries it governs.

## What the upgrade involved

Less than expected. The engine's FMOD surface is small and confined to six
files under `engine/src/engine/utils/`, using only Core API basics -
`System_Create`, `init`, `update`, `createSound`, `createStream`, `playSound`,
channel volume/pause/mode, 3D listener and source attributes, and two DSP
filters. **No source changes were needed** across four minor versions.

The risk we watched for: `FMOD_DSP_TYPE_LOWPASS` and `FMOD_DSP_TYPE_HIGHPASS`
are marked in the docs as *"Deprecated and will be removed in a future
release"*, and `audio_manager.cpp` uses both. They are still present in 2.04.00,
so nothing broke - but that cannot be relied on indefinitely.

## If they are removed in a later version

`audio_manager.cpp` will fail to build at these lines:

| Line | Call |
|---|---|
| 282 | `createDSPByType(FMOD_DSP_TYPE_HIGHPASS, ...)` |
| 295 | `setParameterFloat(FMOD_DSP_HIGHPASS_CUTOFF, freq)` |
| 306 | `createDSPByType(FMOD_DSP_TYPE_LOWPASS, ...)` |
| 319 | `setParameterFloat(FMOD_DSP_LOWPASS_CUTOFF, freq)` |

Replacement is `FMOD_DSP_TYPE_MULTIBAND_EQ` with band A configured as a filter
(`FMOD_DSP_MULTIBAND_EQ_FILTER_HIGHPASS_12DB` and its lowpass counterpart).
Cutoff maps across directly; resonance becomes the band's Q value. Worth
listening to afterwards rather than trusting a clean compile.

## Doing it again

1. Sign in at <https://www.fmod.com/download> - the download is gated, so this
   needs a person with an FMOD account. Free for educational use.
2. **FMOD Engine**, Windows. Not FMOD Studio, which is the authoring tool.
3. It is an installer `.exe`, not an archive. Run it; the default location is
   `C:\Program Files (x86)\FMOD SoundSystem\FMOD Studio API Windows`.
4. Copy from the install:

   | From | To |
   |---|---|
   | `api/core/inc/*` | `engine/external/FMOD/include/` |
   | `api/core/lib/x64/*` | `engine/external/FMOD/lib/x64/` |
   | `doc/LICENSE.TXT` | `engine/external/FMOD/LICENSE.TXT` |

   x86 is not kept - the engine is x64-only.
5. No `premake5.lua` change: it already points at these paths and links
   `fmod_vc.lib` by name.
6. Build Debug and Release, then **run it**. A clean compile is not proof the
   audio works; watch for `[sound] Creating sound` in the log and listen.
7. `maintain.py --propagate --push`, then regenerate the lesson ZIPs.

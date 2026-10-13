# Third-party notices

This project redistributes the following third-party libraries,
under their respective licences. Most are built from vendored source;
the Debug static libs under each lib/Debug/ are also redistributed.
Notices below are taken from the vendored sources in
`engine/external/`, so they correspond to the versions shipped here.

> **FMOD is bundled here for educational use**, which its EULA
> permits. It is proprietary middleware, not open source. Any game
> you ship using it must credit FMOD Studio and Firelight
> Technologies Pty Ltd in-game (EULA clause 3). See
> tools/agt-analysis/FMOD-DECISION.md for the licensing position.

## FMOD Studio 2.04.00 (Firelight Technologies)

**Licence:** Proprietary - bundled for educational use; see below

```
FMOD Studio by Firelight Technologies Pty Ltd.

FMOD is commercial middleware. It is NOT open source.

It is included here for educational use, which the FMOD EULA permits (clause
1.2 covers educational use by students and teachers, at no fee). Delivery is
internal: students receive the material through the university's own
authenticated platform, and this repository is private.

Note clause 1.2.ii, which states that the Studio Engine is not to be
distributed as part of a game engine or tool set. The position taken is that
internal course delivery to enrolled students is not that. The full reasoning,
and the circumstances that would change it - chiefly making this repository
public - is recorded in tools/agt-analysis/FMOD-DECISION.md.

Version: FMOD Engine 2.04.00 (FMOD_VERSION 0x00020400). The full EULA ships
beside the binaries at engine/external/FMOD/LICENSE.TXT, taken from the same
download, so the licence matches the version it governs.

ATTRIBUTION (EULA clause 3): any product shipped using FMOD requires an
in-game credit line including the words "FMOD Studio" and "Firelight
Technologies Pty Ltd". See https://www.fmod.com/attribution for approved
wording. This applies to student games built on this template.
```

Full text: `engine/external/FMOD/LICENSE.TXT`

## Open Asset Import Library (assimp) 6.0.5

**Licence:** BSD-3-Clause

Full text: `engine/external/assimp/LICENSE`

## zlib (bundled inside assimp)

**Licence:** zlib licence

Full text: `engine/external/assimp/contrib/zlib/LICENSE`

## Bullet Physics 3.25

**Licence:** zlib licence

Full text: `engine/external/bullet/LICENSE.txt`

## FreeType 2.14.3

**Licence:** FreeType License (FTL) / GPLv2

Full text: `engine/external/freetype/FTL.TXT`

## stb_image 2.30

**Licence:** Dual: MIT or Public Domain (Unlicense), at your option

```
This software is available under 2 licenses -- choose whichever you prefer.
ALTERNATIVE A - MIT License
Copyright (c) 2017 Sean Barrett
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
ALTERNATIVE B - Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
```

## Glad

**Licence:** MIT (generated loader); Khronos headers under MIT/Apache-2.0

```
* Copyright (c) 2008-2018 The Khronos Group Inc.
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and/or associated documentation files (the
* "Materials"), to deal in the Materials without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Materials, and to
* permit persons to whom the Materials are furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Materials.
*
* THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
```

## GLFW 3.5.1

**Licence:** zlib/libpng

Full text: `engine/external/GLFW/LICENSE.md`

## OpenGL Mathematics (glm) 1.0.3

**Licence:** MIT / Happy Bunny

Full text: `engine/external/glm/copying.txt`

## spdlog 1.17.0

**Licence:** MIT

Full text: `engine/external/spdlog/LICENSE`

## premake5

**Licence:** BSD-3-Clause

```
premake5 is redistributed here as a prebuilt binary at
vendor/premake/premake5.exe.

Copyright (c) 2002-2022 Jason Perkins and individual contributors.
Licensed under the BSD 3-Clause License. Full text:
vendor/premake/LICENSE.txt (shipped in this repo)
```

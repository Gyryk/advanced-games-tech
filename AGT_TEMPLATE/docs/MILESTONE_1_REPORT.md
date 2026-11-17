# The House Collects
IN3026 Advanced Games Technology — Milestone 1 interim report
Draft for review • 17 November 2026 • Student name / ID: [complete]

## Game concept
The House Collects is a small 3D casino heist for players aged 12 and above. The player is a thief collecting chips on an outdoor casino terrace, aiming to escape with $600 before security catches them. The single arena keeps the project manageable within the supplied C++/OpenGL engine. Stylised geometry, green gaming tables, chip stacks and a casino sign establish the setting without a detailed interior or playable casino games.

## Intro screen (10%)
The opening screen presents the title, objective, controls and pickup meanings. Enter starts the game; WASD moves the thief, Space jumps and Q/E orbits the camera. The instructions explain that the gold chip triggers an alarm and that the blue pass clears it. A control strip remains visible during play. The screen is functional; more polished layout and typography are planned for Milestone 2.

## Primitive object and transformations (20%)
Casino chips are constructed in code as cylinders using indexed OpenGL triangles, rather than loaded models. The cylinder has 24 side segments and separate cap vertices. Side normals point radially outward, while cap normals point up or down. Separate vertices allow the caps and sides to have appropriate texture coordinates and lighting. A generated chip texture is mapped around the rim and across the caps.

Multiple chips appear at different positions, rotate about the vertical axis and use different scales for ordinary pickups, the jackpot and table decorations. Translation, rotation and scaling matrices place these instances in the scene. Automated geometry checks verify valid triangle indices, unit normals, finite UVs and agreement between triangle winding and normals. Boxes and pyramids provide additional procedural scenery and pickups.

## Skybox and terrain (10%)
The template sky textures are replaced with six matching dark-blue images. The skybox follows the camera and is rendered unlit. Its identical face colours avoid visible edge seams, giving a deliberately simple night setting. The ground uses a new paving texture repeated 12 times across 24 metres and 10 times across 20 metres. Equal world-space tile sizes preserve square paving instead of stretching one image across the terrace.

## Camera technique (15%)
The template's free camera is replaced by an elevated third-person follow camera. Its desired position is offset from the thief, with exponential smoothing as the player moves. Q/E rotates the offset around the player, and the view remains directed towards the thief. WASD movement is transformed relative to the camera angle. This couples the camera to gameplay and substantially changes the template's independent movement controls.

<!-- pagebreak -->

## Imported meshes and placement (10%)
Three distinct placeholder models—a chair, lamp and planter—are generated offline as triangulated OBJ files and then loaded through the engine's Assimp importer. They include normals, UV coordinates and MTL references to diffuse textures. This import route is separate from the cylinder geometry constructed at runtime. The objects are lit and positioned around the terrace. Chairs in particular are repeated with different translations, opposite rotations and varied scales. The models are normalised by height and aligned to the ground, allowing later replacement through a central asset configuration file.

## Game theme coherence (10%)
The scene uses a consistent geometric style: a casino façade and sign, green felt tables, red carpet, chips and terrace furniture. The thief and security figures use the same block-based visual language. A restrained dark palette with gold, red and green accents helps distinguish the setting and interactive objects. The simple graphics are intentional; the milestone focus is coherent presentation and correct rendering rather than asset complexity.

## Source design and organisation (10%)
Game rules are separated from the university engine adapter. casino_game.h contains state, movement, collisions, pickups and guard behaviour. casino_geometry.h contains mesh construction, while casino_assets.h centralises replacement paths and model sizing. example_layer.cpp connects these modules to rendering, input, camera, text and sound. Comments explain non-obvious calculations and state changes. Headless tests exercise rules and geometry without requiring Windows or an OpenGL context; the existing engine and third-party libraries remain the foundation.

## Validation and final-game plan
The first Windows playtest with Visual Studio 2026 reported that the intro, gameplay, visuals and audio ran as expected. It also identified excessive time, protection and guard reaction delays. A modest balance revision shortens the round to 60 seconds and the shield to 3 seconds, speeds up tags and pursuit, and strengthens the jump. The revision passes headless tests but requires another Windows playtest; its difficulty is not yet validated.

The final game will refine the existing prototype into a clear, balanced heist. Planned work includes improving the intro/HUD, making jumping and guard interactions more useful, and testing the risk of collecting a high-value chip that activates harder security. The existing scoring, timer, power-ups, physics, four-state guards, lights, effects and audio provide a starting point for Milestone 2; their presence is not a claim that they are finished. Further playtesting and the final report will document the design, results and limitations.

## Sources and assistance
The university template is used under its supplied notices (Lazzarotto, 2019). Casino-specific models, textures and sounds were generated for this project with Python during AI-assisted development; they were not downloaded marketplace assets. The asset register records their creation methods and distinguishes the inherited font and libraries. ChatGPT assisted with code, assets and this report draft; the required appendix must record prompts, outputs and how they were used.

References: Lazzarotto, P. (2019) AGT engine, supplied university template, LICENSE.MD. Child, C. (n.d.) IN3026 Coursework Project and Milestone 1 Marking Scheme, supplied module documents.

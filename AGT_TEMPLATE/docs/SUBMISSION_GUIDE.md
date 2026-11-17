# Milestone 1: exact source and documentation submission guide

This follows the **Deliverables** section of `IN3026.Coursework.docx`, the
Milestone 1 requirements, and the supplied template's README/CLAUDE instructions.
The coursework specifies a ZIP with two top-level folders, a Word/PDF report,
and source that opens and builds/runs with F5 on the university machines.

**Do not submit only the changes ZIP, a GitHub link, `game/src`, or an executable.**
The examiner needs the complete template, solution, assets and dependencies.
Use your Windows working copy after the final playtest, because it contains the
Visual Studio state file that preserves the unloaded dependency projects.

## 1. Finish the documentation

Use `docs/MILESTONE_1_REPORT.docx` to edit, or edit
`docs/MILESTONE_1_REPORT.md` and regenerate with `tools/build_milestone_report.py`.
The supplied PDF is verified as two pages; Word may repaginate on another PC.

- Fill in your name/ID and check the text reflects the build you will submit.
- Update any pending test statement only after doing that test. The initial
  prototype was confirmed working by you; the revised balance needs a retest.
- Save the final report as `interim_report.pdf` or `.docx`, maximum **two pages**.
  The report includes the game concept, implemented M1 features and final-game plan.
- Complete the AI-use appendix. The brief explicitly permits an unlimited-length
  appendix recording prompts, outputs and how they were used. Export the relevant
  conversation/code outputs, explain retained/modified work and your contribution,
  and save it as `ai_appendix.pdf` or `.docx`. `AI_ASSISTANCE.md` is preparation
  material, not a complete transcript or a substitute for that appendix.
- The asset register can accompany the report as supporting attribution material;
  the detailed final-report asset inventory is a Milestone 2 requirement. Keep
  original library licence files in the source folder in either case.

Check the actual Moodle submission area for the deadline and upload size limit;
no exact deadline or size limit is stated in the supplied brief.

## 2. Verify the source on Windows

Use the `work` branch of `Gyryk/advanced-games-tech`. Pull current changes only
when your own local edits are safely saved/committed; do not discard local work.

1. Open `AGT_TEMPLATE/engine.sln` in **Visual Studio 2026**, using **Debug / x64**.
2. Set `game` as the startup project. Keep the external library projects unloaded
   as the template describes; their Debug libraries are already supplied.
3. Build/run with **F5**. Check the intro, chip texturing/lighting, imported props,
   ground, skybox, Q/E orbit and movement. Complete one short gameplay test.
4. Exit the game. Do not regenerate the solution just to add the new headers:
   they are included from `example_layer.cpp` and already compile with the project.

The template's Debug configuration is the tested starting point. The request
below to **clean Release** does not mean you must compile a new Release build.
The checkout does not ship all Release dependency libraries; do not switch your
final startup configuration to Release unless you have prepared and tested it.

## 3. Clean both configurations, then close Visual Studio

The coursework explicitly says:

> Perform a clean in both Debug and Release modes.

1. Select **Debug / x64** → **Build → Clean Solution**.
2. Select **Release / x64** → **Build → Clean Solution**.
3. Return to **Debug / x64** and verify `game` is the startup project.
4. Close Visual Studio so that it finishes saving its `.suo` state.

Leave the external library projects unloaded while cleaning. Do not delete
prebuilt libraries inside `engine/external/**/lib/Debug` or FMOD files. These
are dependencies needed for the next F5 build, not disposable game build output.
If they have been removed accidentally, restore them from the supplied template
and repeat the fresh-extraction test before submitting.

## 4. Make a separate submission copy

Create a new folder outside the repository, for example
`IN3026_Milestone1_YourStudentID`. Add these two directories:

```text
IN3026_Milestone1_YourStudentID/
├── documentation/
│   ├── interim_report.pdf          # maximum two pages
│   ├── ai_appendix.pdf             # required AI disclosure; separate appendix
│   └── asset_register.md           # optional supporting attribution register
└── source/
    └── AGT_TEMPLATE/
        ├── engine.sln
        ├── premake5.lua
        ├── generate_solution_vs.bat
        ├── README.md, CLAUDE.md, LESSON.md
        ├── LICENSE.MD, THIRD-PARTY-NOTICES.md
        ├── engine/                 # source, projects, external headers/libs/DLLs
        ├── game/                   # project, source, assets, font, shaders
        ├── vendor/                 # supplied build tooling and notices
        ├── docs/, tests/, tools/    # project documentation/tests/generators
        └── .vs/engine/v18/.suo      # preserve this Windows state file
```

Copy **your entire `AGT_TEMPLATE` folder** into `source/`, including hidden files.
Do not also copy `AGT_lab_01`, the repository's `.git` directory, the uploaded
lecture files or unrelated work. Keep the relative layout inside `AGT_TEMPLATE`.
Keep all `.sln`, `.vcxproj` and supplied `.vcxproj.filters` files.

## 5. Remove only generated clutter from that copy

In File Explorer, enable **View → Show → Hidden items**. In the submission copy:

- Remove `AGT_TEMPLATE/bin/` and `AGT_TEMPLATE/inter/` if they remain after Clean.
  The game will regenerate them when built.
- Follow the brief exactly: remove everything under `.vs` **except**
  `.vs/engine/v18/.suo` and the directories needed to contain it. For example,
  remove `.vs/engine/v18/Browse.VC.db`, `.vs/ipch`, etc., if present; retain `.suo`.
- Exclude Python `__pycache__` directories, unrelated exports and old submission ZIPs.
- Keep the dependency `.lib` and DLL files inside `engine/external`, including
  FMOD's runtime DLL and import library. Do not use a blanket “delete all DLLs,
  libraries or hidden files” cleanup.

The cloud checkout currently has no `.vs/.../.suo` file. Do **not** create an
empty substitute. Use the genuine file from your Windows working solution after
closing Visual Studio. The expected path is `.vs/engine/v18/.suo`; if your IDE
saved it elsewhere, investigate that actual location/version before packaging.

Do not rely solely on `git archive`/Download ZIP: local `.suo` state and any other
required untracked build dependencies may be absent. This is why the submission
copy should come from the Windows folder you have just tested.

## 6. ZIP the two folders, then test the ZIP

1. Open `IN3026_Milestone1_YourStudentID` in File Explorer.
2. Select **both** `documentation` and `source`, then **Compress to ZIP file**
   (or **Send to → Compressed (zipped) folder**, depending on Windows version).
3. Name it `IN3026_Milestone1_YourStudentID.zip`.
4. Open the ZIP and check that its top level contains exactly `documentation/`
   and `source/`, with the `.suo` and dependency libraries present inside source.
5. Extract the ZIP into a different folder. **Do not open the solution inside
   the ZIP.** Open the extracted `source/AGT_TEMPLATE/engine.sln` and press F5.
6. Verify assets/audio are found and the game starts without copying in extra
   files. Prefer doing this final check on a university machine with VS 2026.

The `.suo` should preserve startup/unloaded state and Debug/x64. If the fresh
copy needs manual repairs, fix the submission copy and make/test a new ZIP.
For an asset working-directory problem, `game` must run with its project folder
(containing `assets`) as the working directory. Do not leave a machine-specific
absolute path as a hidden prerequisite; verify the extracted copy works.

Testing the extracted copy regenerates build files in that **test folder**. Keep
the original clean ZIP for upload; do not zip the now-built test folder again.

## 7. Upload to Moodle

Upload the tested ZIP to the **Milestone 1** submission area. Confirm the upload
completed and inspect the receipt/status provided by Moodle. The coursework
requires a buildable source submission, not a replacement GitHub URL. Submit the
FMOD-containing template through the module's permitted route, not a public
repository or public download link.

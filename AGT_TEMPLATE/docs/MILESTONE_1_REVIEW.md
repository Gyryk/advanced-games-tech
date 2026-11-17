# Milestone 1 review and scope

The interim build is 25% of the module. The report is at most two pages. The
first priority is demonstrating the existing implementation against the eight
criteria, rather than adding more gameplay systems. This checklist is for review;
it is not a guarantee of a mark and is not part of the two-page report.

| Criterion | Weight | Current evidence | Final visual/review check |
|---|---:|---|---|
| Intro | 10% | Title, objective, pickup explanations and controls; user confirms it works | Legibility at the submission machine's resolution; retain current layout for now |
| Procedural object | 20% | Code-generated cylindrical chip with cap/side UVs and normals, repeated transforms | Inspect side/cap texture and lighting; show rotation, translation and scale differences |
| Sky and terrain | 10% | New matching unlit sky faces; square paving repeats | Rotate camera to inspect seams; check paving remains square |
| Camera | 15% | Smoothed follow and Q/E orbit; camera-relative movement | Demonstrate coupling and orbit, compared with template free movement |
| Imported object | 10% | OBJ chair, lamp and planter with textures/normals; repeated placement | Show chairs facing different directions and at different scales; check no white fallback/missing files |
| Theme | 10% | Casino sign, felt tables, chip motifs, carpet and coherent block style | Keep palette/style consistent; no photorealistic asset requirement |
| Report | 15% | Two-page DOCX/PDF draft and Markdown source | Fill name/ID; verify wording against final build; attach required AI-use appendix |
| Code | 10% | Separate game rules, geometry, asset configuration and engine adapter; comments/tests | Review code and be able to explain the implementation and assistance |

## What the first Windows playtest establishes

On 17 November 2026 the student reported that the intro works and the game runs
as expected, liked the current graphics/audio, and found the game too easy.
Specific feedback: jumping lacked impact; shields, guard wind-up and the timer
were too generous. This is a qualitative first playtest, not a timed user study.
No measured completion times or screenshots have been supplied.

## Small response to that feedback

| Setting | Previous | Revised first pass |
|---|---:|---:|
| Round timer | 120 s | 60 s |
| Shield | 8 s | 3 s |
| Tag wind-up | 0.40 s | 0.18 s |
| Protection after damage | 1.8 s | 0.9 s |
| Protection after security pass | 2 s | 1 s |
| Chase speed: easy / alarm | 2.5 / 3.8 units/s | 4.2 / 5.5 units/s |
| Detection radius: easy / alarm | 4.4 / 9 units | 5.5 / 10 units |
| Return-state pause / reacquire | 2 / 3 s | 0.65 / 0.9 s |
| Jump impulse / gravity | 6 / 18 | 6.8 / 20 |

The player reaches roughly 5.1 units/s, making an alarm guard faster than the
player on a straight path. The jump is slightly higher (about 1.16 metres in the
continuous formula), clears low crates and can dodge a tag. No new jump system,
level or GUI redesign is introduced. Retest these values on Windows; shorter
reaction windows and multiple guards may need adjustment for fairness.

## Milestone 2 scope

Retain current features instead of deleting working code to manufacture later
progress. Existing gameplay, effects and audio are prototypes. The final phase
can improve them through evidence from playtesting:

- Redesign intro/HUD hierarchy, spacing and feedback.
- Improve jump usefulness and feel, guard pursuit/avoidance and readable attacks.
- Balance timer, gold-chip risk, alarm behaviour, protection and scoring.
- Verify the complete final criteria: three different primitive shapes, three
  imported mesh types, two lights, two effects, three physics techniques, four
  NPCs with qualifying AI, audio and at least two gameplay additions.
- Finish the final 7–15-page report, screenshots, diagrams, attribution and demo.

The two-page interim report must describe what exists and distinguish future
refinement. Do not present already implemented gameplay as entirely future work.

## Before submitting Milestone 1

1. Rebuild on Windows after the balance/credit changes and complete a short
   round: ordinary chips, gold alarm, pass, shield, hit, escape and timeout.
2. Check the visual criteria above; a couple of screenshots are useful evidence
   even if not included in the two-page report.
3. Add name/ID, review the report, and export/retain a two-page PDF. Record actual
   retest results rather than changing a pending check to passed without evidence.
4. Complete the AI appendix with prompts, outputs and how they were used. Include
   source/licence notes and retain the supplied library notices.
5. Follow the coursework's source cleaning and ZIP layout instructions; perform
   a final F5 check from the submitted folder on a university machine.

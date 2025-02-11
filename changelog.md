# Eclipse Menu

## v1.1.0
**Additions**:
* Added <cg>Confirm Hacks</c> (Confirm Practice, Restart, etc.) (#140)
* Added <cg>Show Taps</c>
* Added <cg>Reset BG & SFX Volume</c> shortcuts
* Added <cg>Quick Chests</c>
* Added <cg>Ball Rotation Bug</c>
* Added <cg>Ignore Inputs</c> for <co>Bot</c>
* <co>Bot</c> now uses <cg>GDR2</c> format (improved file size and read/write speed)
* <co>Noclip</c> will trigger safe mode only if you die (allows 100% accuracy runs)

**Fixes**:
* <cg>Fixed</c> a crash on <cl>Android</c> with <co>color picker popups</c>
* <cg>Fixed</c> translation issues with <co>Accurate Percentage</c>
* <cg>Fixed</c> issues related to changing <co>texture quality</c> or switching <co>fullscreen</c> mode
* <cg>Optimized</c> few things with <co>Physics Bypass</c>
* <cg>Fixed</c> a rare crash with <co>Physics Bypass</c> if you die in editor with second player
* <cg>Fixed</c> translations getting duplicated in the settings
* <cg>Fixed</c> crashes with <co>Practice Fix</c> on <cl>Android</c>
* <cg>Fixed</c> <co>Shortcuts</c> tab alignment issues on <cl>Android</c>
* <cg>Fixed</c> <co>Labels</c> not showing up in <co>Editor</c>
* Rebinding keys to mouse click on <cl>Android</c> no longer works

## v1.0.0
**Additions**:
* <cg>Android</c> support featuring <cl>custom made UI</c> for mobile devices
* <cg>Labels</c> revamp:
  * <cg>Emojis support</c>
  * <cg>Added</c> keybind support for <co>Labels</c>
  * <cg>Events</c> to customize colors/fonts/etc. based on certain conditions
  * <cg>Added</c> **a lot** of new RIFT variables and functions
  * <cg>Added</c> font alignment options
  * <cg>Added</c> absolute positioning for labels
  * <cg>Added</c> Import/Export options for labels, so you can share them with others.
  * <cg>Added</c> "Only Cheating" option for cheat indicator, which will hide the indicator if no cheats are active
* <cg>Internal Recorder</c> revamp:
  * <co>FFmpeg API</c> is not a requirement to run Eclipse anymore (you still need it to use the recorder)
  * <cg>Improved performance</c> during recording
  * <cg>Implemented</c> "one-pass" audio rendering (no need to play the level twice to record audio)
  * <cg>Added</c> "Hide Preview" option to not render the game twice during recording
* <cg>Added</c> Translations (check the [translations repository](https://github.com/EclipseMenu/translations) on how to contribute)
* <cg>Added</c> search bar for hacks
* <cg>Added</c> customization settings for menu open/close animations (tabbed layout)
* Added <cg>Catppuccin Macchiato</c> theme as one of the default ones (thanks to [arvFlash](https://github.com/arvFlash))
* Added <cg>No Level Kick</c>
* Added <cg>Checkpoint Delay</c>
* Added <cg>All Passable</c>
* Added <cg>Unlock Paths</c>
* Added <cg>Physics Bypass</c> for <cl>macOS</c>
* Added <cg>Custom Modules API</c> for other mods to use Eclipse's cheat indicator system

**Fixes:**
* <cg>Fixed</c> a crash in editor when using <co>Physics Bypass</c>
* <cg>Fixed</c> percentage calculation with <co>Physics Bypass</c>
* <cg>Fixed</c> slowed down animations with <co>Physics Bypass</c>
* <cg>Fixed</c> <co>"Formatted Timer"</c> in <co>Accurate Percentage</c> not accounting for checkpoints.
* <cg>Fixed</c> a memory leak with <co>Show Trajectory</c>
* <cg>Fixed</c> "Show Labels" toggle not realigning labels when switched
* Greatly <cg>improved performance</c> of Labels
* <cg>Fixed</c> a random crash on <cb>ARM macOS</c> when entering any level
* <cg>Fixed</c> some <co>Practice Fix</c> crashes
* <cg>Fixed</c> <co>Audio Speed</c>/<co>Sync with Speedhack</c> not working sometimes
* <cg>Optimized</c> <co>Show Trajectory</c> a bit
* <cg>Fixed</c> a crash that could happen when using <co>Blur</c>
* <cg>Fixed</c> <co>Click Between Frames</c> toggle resetting on game launch
* <cg>Resolved</c> some issues with <co>replays</c> when using <co>Physics Bypass</c> (some old macros might stop working due to this)
* General performance improvements

## v1.0.0-beta.4
**<cg>Geometry Dash v2.2074</c> support**  
No Android support yet, but we're working on it!

**Fixes:**
* <cg>Fixed</c> radio buttons keybinds always setting the first option
* <cg>Fixed</c> random crash with combo boxes
* <cg>Fixed</c> some <co>Slider Limit</c> issues
* <cg>Fixed</c> shader related issues with <co>Internal Recorder</c>
* <cg>Fixed</c> rare crash when having an invalid UI renderer mode set
* <cg>Fixed</c> few issues with <co>Instant Complete</c> (#60)
* <cg>Fixed</c> issues with <co>Mute Level Audio On Death</c> (#60)
* <cg>Fixed</c> keyboard shortcuts (Ctrl+C, Ctrl+V, etc.) not working in the UI
* <cg>Fixed</c> keybind deletion crash
* <cg>Fixed</c> random crashes with <co>All Modes in Platformer</c>, <co>Practice Coins</c> and <co>Show Trajectory</c>
* <cg>Fixed</c> <co>Frame Stepper</c> bugging out with <co>Speedhack</c>
* <cg>Fixed</c> an issue with slashes in level name when saving a render
* <co>StartPos Switcher</c> keybinds now trigger instantly
* <co>Practice Music Sync</c> now auto-toggles the option in settings
* Replay does not trigger <co>Auto Safe Mode</c> unless you have a loaded macro
* Added some mod descriptions (#19, #70)

**Additions**:
* <cy>New</c> Internal Recorder backend (crossplatform support, no need to install ffmpeg)
* Added <cg>No Trail</c>
* Added <cg>Always Show Trail</c>
* Added <cg>Discord RPC</c>
* Added <cg>Show In Editor</c> option to <co>Show Hitboxes</c>
* Replaced <co>TPS Bypass</c> with <cg>Physics Bypass</c>
* Added <co>Place Checkpoint</c> and <co>Remove Checkpoint</c> shortcuts
* Added custom keybinds for <co>P1/P2 jump</c>
* Added <cg>Endscreen Cheat Indicator</c>
* <co>Auto Safe Mode</c> now stores your attempt state (whether any cheats were active at any time)
* <cg>Click Between Frames</c> toggle if you have the mod installed
* <cy>New</c> label variables: 
`isDualMode`, `cubeIcon`, `shipIcon`, `ballIcon`, `ufoIcon`, `waveIcon`, 
`robotIcon`, `spiderIcon`, `swingIcon`, `gamemode`, `playerIcon`, `levelDescription`, 
`frameReal`, `tps`, `runStart`
* <cy>Updated</c> <co>RIFT</c> scripting with more features:
    * Added <cg>cfg</c> function to get values from settings
    * Improved overall performance and stability
    * Fixed some expressions not working as expected
    * Added <cg>int</c>, <cg>float</c> and <cg>str</c> functions, to convert variable types
    * Added <cg>duration</c> function to convert seconds into a formatted time string
    * Added <cg>$</c> operator to evaluate sub-expressions
* Added <cg>Eclipse API</c> for developers to interact with the mod (BetterInfo users might notice a new tab :D)

## v1.0.0-beta.3
**Fixes:**
* <cg>Fixed</c> <co>Pause Countdown</c> taking 2.5 seconds instead of 3 seconds
* <cg>Fixed</c> <co>Noclip Deaths</c> not working properly
* <cg>Fixed</c> <co>Respawn Delay</c> not being able to added as a keybind
* Moved <co>Copy Bypass</c> and <co>Verify Bypass</c> to the Creator tab
* Fixed several issues with label parser

**Additions**:
* Added <cg>0% Practice Completion</c>
* Added <cg>No Mirror</c>
* Added <cg>Instant Mirror</c>
* Added <cg>No Object Glow</c>
* Added <cg>Level Uncomplete Shortcut</c>
* Labels can now be reordered
* Macros can now be deleted in the menu itself

## v1.0.0-beta.2
**Fixes:**
* <cg>Fixed</c> <co>Force Platformer</c> not working in online levels
* <cg>Fixed</c> the invisible interface tab when using the panel layout
* <cg>Fixed</c> the immediate change of font size
* <cg>Fixed</c> the awkward left window size in Sidebar layout
* <cg>Fixed</c> <co>Smart Startpos</c> on <cb>macOS</c>
* <cg>Fixed</c> crash if theme file was corrupted

**Additions**:
* Added <cg>Solid Wave Trail</c>
* Added <cg>Custom Wave Trail</c>
* Added <cg>Stop Triggers on Death</c>
* Added <cg>Noclip Death Limit</c> and <cg>Noclip Accuracy Limit</c>
* Keybinds will now not activate if you are typing in a GD text box
* Added a hint message for adding a new keybind
* Automatically disable <cr>Click Between Frames</cr> when botting
* Added <cg>new variables</c> for labels: `framestepper`, `monthName`, `lastDeath`

## v1.0.0-beta.1
* Initial Release!
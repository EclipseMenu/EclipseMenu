# Eclipse Menu

## v1.5.0
**Additions**:
* Added <cg>Freeze Best Run</c> for <co>Safe Mode</c>
* Added <cg>Show Graphics Settings</c> shortcut
* Added <cg>Shipcopter</c> (#248 by @Beat)
* Added <cg>No End Shake</c> (#247 by @slideglide)
* Added toggle to switch between <cg>ImGui</c> and <cg>Cocos</c> UI modes in mod settings
* Made <cg>Open DevTools</c> shortcut available on all platforms

**Fixes**:
* <cg>Fixed</c> <co>Checkpoint Delay</c> not being applied when starting recording mid-attempt
* <cg>Fixed</c> <co>Bot</c> reporting an error when creating a macro with missing directory
* <cg>Fixed</c> <co>Discord RPC</c> not connecting for some users
* <cg>Improved</c> support for <cl>DevTools</c> mouse cursor
* <cr>Removed</c> <co>Comment History Bypass</c> due to server-side patch
* Small performance improvements in <co>Show Hitboxes</c>

## v1.4.0
**Additions**:
* Added <cg>Physics Bypass</c> for <cy>JIT-less</c> iOS
* Added <cg>No Wave Trail Behind</c>
* Added <cg>Show Level Password</c> shortcut
* Added <cy>Globed</c> label variables: `globed.enabled`, `globed.isConnected`, `globed.ping`, `globed.tps`, `globed.playersOnline`, `globed.playersOnLevel`
* <co>Discord RPC</c> now supports <cy>Wine</c> (only Wine Staging 10.2+ is supported for now)
* Added <cy>API</c> for loading macros
* <cy>Menu Arrows</c> now change colors based on the current theme (on <cl>mobile</c> UI)

**Fixes**:
* <cg>Fixed</c> a bunch of issues related to <cl>Windows</c> paths and added more error handling
* <cg>Fixed</c> random startup crashes on <cl>Android</c> due to <co>Physics Bypass</c> patching
* <cg>Fixed</c> a few issues with <co>Practice Fix</c> (hopefully)
* <cg>Fixed</c> <co>Solid Wave Trail</c> compatibility with other mods and made it toggleable at any time
* <cg>Fixed</c> problems with <co>Discord RPC</c> not updating status properly
* <cg>Fixed</c> <co>Uncomplete Level</c> corrupting editor levels

## v1.3.2
**Fixes**:
* <cg>Fixed</c> rotated danger hitboxes being incorrect
* <cg>Fixed</c> hitboxes not updating when toggled if game is paused
* <cg>Fixed</c> <co>Practice Fix</c> locking attempts count 

## v1.3.1
**Fixes**:
* <cg>Fixed</c> cheat indicator not showing when <co>Show Hitboxes</c> was enabled
* <cg>Fixed</c> a regression in <co>Show Hitboxes</c> that caused duplicate hitboxes and occasional crashes
* <cg>Fixed</c> a crash in Discord RPC when button fields were left empty
* <cg>Fixed</c> ghost label bug when text becomes empty

## v1.3.0
**Additions**:
* <cg>JIT-less</c> support for <cl>iOS</c> (Physics Bypass is not supported on JIT-less due to technical limitations)
* Added <cg>two new themes</c> <cg>Gruvbox</c> (thanks fleeym) and <cg>OpenHack</c>
* Removed no longer required dependencies to reduce mod size
* Added <cg>DPI Scaling</c> setting to toggle scaling UI with screen resolution
* Added delay setting for <co>StartPos Switcher</c>
* <cy>New</c> label variable: `levelRating`
* Added ability to <cg>change menu keybind</c> from Geode mod settings

**Fixes**:
* <cg>Improved</c> <co>Physics Bypass</c> performance and accuracy
* Reworked <co>Show Hitboxes</c>, fixing bugs and improving performance
* <cg>Fixed</c> <co>Accurate Percentage</c> rounding to nearest, instead of down
* <co>P1/P2 Jump Keybinds</c> can no longer be bound to default jump keys to prevent issues
* <cg>Improved</c> replay bot recording accuracy
* <cg>Fixed</c> crash that could happen with <co>Only Show Death Object</c>
* <cg>Fixed</c> crash with <co>Legacy Trail</c> after changing resolution
* <cg>Fixed</c> menu popping up for few frames on launch
* <cg>Fixed</c> crash when using <co>Ghost Trail</c> hacks with editors
* <cg>Fixed</c> <co>Solid Wave Trail</c> incompatibility with some mods
* <cg>Fixed</c> camera related issues with <co>StartPos Switcher</c> (#205 by @UserIsntAvailable)
* <cg>Fixed</c> inverted Delete/Backspace keys on <cl>macOS</c>

## v1.2.2
**Additions**:
* Added <cg>Click Between Frames</c> toggle for <co>all platforms</c> (If you have the CBF mod installed)

**Fixes**:
* <cg>Fixed</c> <co>Hitbox Trails</c> not clearing in Editor
* <cg>Disabled</c> <co>Always Show Coins</c> on Mac OS to prevent crashing
* <cg>Fixed</c> <co>Startpos Switcher</c> crashing on Mac OS
* <cg>Fixed</c> menu sometimes going unstable for Windows & Mac OS
* <cg>Fixed</c> bronze coins not being removed when uncompleting a level

## v1.2.1
**Fixes**:
* <cg>Fixed</c> Mac OS crashing on startup
* <cg>Fixed</c> duals not working with <co>Trail Outline</c>
* <cg>Fixed</c> certain settings not showing up on Android

## v1.2.0
**Additions**:
* <cg>iOS</c> support. All platforms are now supported!
* Added <cg>Layout Mode</c>
* Added <cg>Trail Outline</c> to allow your wave trail to have an outline! Customizable too!
* Added <cg>Only Show Death Object</c>, for seeing which object you exactly died to!
* Added <cg>Force Ghost Trail</c>
* Added the <cg>Ball Variance</c> setting for <cg>Ball Rotation Bug</c>
* Added settings for <cg>Legacy Pulse</c>
* Added settings for <cg>Trail Cutting</c>
* <co>Force Platformer</c> is now considered a <cr>cheat</c>

**Fixes**:
* <cg>Improved</c> Mac OS keybinds
* <cg>Fixed</c> <co>Internal Recorder</c> crashes on iMac
* <cg>Fixed</c> ball rotation issues on Mac OS
* <cg>Fixed</c> label presets causing issues

## v1.1.1
**Additions**:
* Added <cg>Recount Secret Coins</c> for restoring any lost secret coins from Uncomplete Level.

**Fixes**:
* <cg>Fixed</c> uncomplete level from <cr>removing secret coins</c> rather than user coins if a rated level contains it.

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

# Eclipse Menu

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
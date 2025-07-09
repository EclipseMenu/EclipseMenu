# Eclipse Menu Label System
This document explains the Eclipse Menu label system, covering its usage, variables, and functions.

## Contents
1. [General information](#1-general-information)
2. [Formatting](#2-formatting)
3. [Using functions](#3-using-functions)
4. [Variables list](#4-variables-list)

## 1. General information

Eclipse Menu offers a unique status label system, giving users extensive freedom to create custom labels and complex use cases.

Instead of having hardcoded labels like in most mods you've seen before, you can create custom ones, make them personalized or even make complex scenarios.

This works by utilizing an in-house library designed specifically for Eclipse Menu, which formats the label content, based on values provided by the menu.

## 2. Formatting

### 1. Basic syntax

RIFT (Runtime Interpreted FormaTter) - has a very basic syntax, yet can still be powerful if used to its full extent. Let's look at this example:  
```
2 + 2 = { 2 + 2 }
```

If you could guess, this will result in "2 + 2 = 4".

Anything, written in curly brackets `{ ... }`, will be evaluated as an expression, and converted to string afterwards. Everything that is not enclosed by brackets, is considered plain text, and will always stay the same.

Let's now take a look at a more complex example:  
```
Progress: { precision(progress, 5) }%
```

Here, `progress` is a variable defined by Eclipse and will return your current progress in a level (see "4. Variables list" for more info). `precision` is a built-in function, which allows you to choose how many decimal places you want to print.  
With this information, we can say that this expression will result in your current progress being printed with 5 decimal places like this: `Progress: 12.34567%`.

Given this, you can now use different variables to show various information on the screen.

### 2. Types

If you have already scrolled to the variables list, you might have noticed there is a column named "Type", which has 4 different values. These are considered primary types, that are supported by RIFT. These types include:
- **Integer** - a whole number without decimal places (e.g., `1`, `987`);
- **Float** - a number with decimal places, representing real numbers (e.g., `1.0`, `3.14`)
- **String** - contains text (`"Hello"`, `'Single quotes also work'`, etc.);
- **Boolean** - type that is either `true` or `false`. Booleans can be used in conditions where true is treated as 1 and false as 0.
- **Array** - a collection of values, which can be of any type. You can access elements in an array using an index, like `array[0]` for the first element.
- **Dictionary** - a collection of key-value pairs, where each key is unique. You can access values using their keys, like `dict["key"]` or `dict.key`.

### 3. Comparisons

RIFT allows for a range of comparisons between variables, letting you create dynamic labels. Here’s a simple example:

```
{ progress >= 50 ? "Halfway there!" : "Keep going!" }
```

In this case, if your progress is 50% or more, the label will display "Halfway there!", otherwise it will show "Keep going!". This syntax uses a ternary operator, which allows you to choose between two values based on a condition. It is common in many programming languages.

You can use comparison operators like:
- `>` (greater than)
- `<` (less than)
- `>=` (greater than or equal to)
- `<=` (less than or equal to)
- `==` (equal to)
- `!=` (not equal to)

These can be used with integers, floats, and booleans to tailor labels based on real-time data.

### 4. Operations between types

RIFT supports a range of operations between different types, allowing you to combine, manipulate, and format data flexibly.

#### String and number addition

You can add a number to a string or concatenate two strings easily:

- **String + Number**: If you add a number to a string, the number will be automatically converted to a string and concatenated. Example:

```
{ "Epic number: " + 100 }
```

This results in `Epic number: 100`.

- **String + String**: Adding two strings together works the same way as concatenation. Example:

```
{ "Hello, " + "World" }!
```

This results in `Hello, World!`.

Similar to Python, you can multiply a string by a number to repeat it:

- **String * Number**: This repeats the string the specified number of times. Example:

```
{ "Hello!" * 3 }
```

This results in `Hello!Hello!Hello!`.

You can perform calculations inside a string context, and the result will automatically be formatted:

- **Number + Number**: Standard arithmetic works within strings. Example:

```
{ "2 + 2 = " + (2 + 2) }
```

This results in `2 + 2 = 4`.


### 5. Implicit type conversion

RIFT handles type conversion automatically when you mix types. For example, numbers are automatically converted to strings when combined with text:

- Numbers are converted to strings when added to strings.

- Strings cannot be directly used in mathematical operations (except for multiplication), so ensure you only perform calculations on number types when necessary.


These flexible operations make it easy to create complex, dynamic labels without needing to worry too much about explicit type conversions.


## 3. Using functions

RIFT includes many built-in functions for manipulating strings, numbers, and performing advanced operations. These functions can be used within expressions in your labels to create dynamic and informative outputs.

### 1. String Functions

- **len(str)**: Returns the length of the string. Example:

```
{ len("Hello") } // 5
```

- **substr(str, start, end)**: Extracts a portion of a string between the start and end indices (end is exclusive). Example:

```
{ substr("Hello World", 0, 5) } // "Hello"
```

- **toUpper(str)**: Converts all characters in the string to uppercase. Example:

```
{ toUpper("hello") } // "HELLO"
```

- **toLower(str)**: Converts all characters in the string to lowercase. Example:

```
{ toLower("HELLO") } // "hello"
```

- **trim(str)**: Removes leading and trailing whitespace from the string. Example:

```
{ trim("  hello  ") } // "hello"
```

- **replace(str, from, to)**: Replaces all occurrences of from with to in the string. Example:

```
{ replace("cats are cute", "cats", "dogs") } // "dogs are cute"
```


### 2. Random and Rounding Functions

- **random(min, max)**: Generates a random float between min and max. Example:

```
{ random(1, 10) } // A random value between 1 and 10
```

- **round(x)**: Rounds a float to the nearest integer. Example:

```
{ round(3.14159) } // 3
```

- **floor(x)**: Returns the largest integer less than or equal to x. Example:

```
{ floor(3.9) } // 3
```

- **ceil(x)**: Returns the smallest integer greater than or equal to x. Example:

```
{ ceil(3.1) } // 4
```


### 3. Mathematical Functions

- **abs(x)**: Returns the absolute value of x. Example:

```
{ abs(-5) } // 5
```

- **min(a, b, ...)**: Returns the smallest value from a list of numbers. Example:

```
{ min(1, 3, 5) } // 1
```

- **max(a, b, ...)**: Returns the largest value from a list of numbers. Example:

```
{ max(1, 3, 5) } // 5
```

- **sum(a, b, ...)**: Returns the sum of a list of numbers. Example:

```
{ sum(1, 2, 3) } // 6
```

- **avg(a, b, ...)**: Returns the average of a list of numbers. Example:

```
{ avg(1, 2, 3) } // 2
```

- **sqrt(x)**: Returns the square root of x. Example:

```
{ sqrt(16) } // 4
```

- **pow(base, exp)**: Raises the base to the power of exp. Example:

```
{ pow(2, 3) } // 8
```

- **sin(x)**, **cos(x)**, **tan(x)**: Trigonometric functions that return the sine, cosine, or tangent of x (in radians). Example:

```
{ sin(3.14159 / 2) } // 1
```

- **precision(x, digits)**: Rounds x to the specified number of decimal places. Example:

```
{ precision(3.14159, 2) } // 3.14
```


### 4. Padding Functions

- **leftPad(str, len, char=' ')**: Pads the left side of str with char until it reaches the specified length. Third parameter can be omitted, which will use a space character. Example:

```
{ leftPad("123", 5, '0') } // "00123"
```

- **rightPad(str, len, char=' ')**: Pads the right side of str with char until it reaches the specified length. Example:

```
{ rightPad("123", 5, '0') } // "12300"
```

- **middlePad(str, len, char=' ')**: Pads both the left and right sides of str with char to center the string, until it reaches the specified length. Example:

```
{ middlePad("1", 5, '-') } // "--1--"
```

In addition to these functions, Eclipse Menu also provides a function to access built-in config properties:
```
{ cfg("creator.leveledit") }
```
In this example, the function will return the value of the `creator.leveledit` property from the config file, which represents whether "Level Edit" hack is enabled or not.

There's no full list of config properties, as it would be too long and hard to maintain. However, you can always check the source code or read your config file to see what properties are available.


### 5. Precedence Rules

RIFT follows general mathematical precedence, ensuring that expressions are evaluated in a logical order. Operators are applied based on their priority, unless overridden by parentheses. The precedence rules allow for more complex expressions while ensuring correct evaluation.

**Basic precedence rules:**

- Multiplication (`*`), division (`/`) and modulo (`%`) have higher precedence than addition (`+`) and subtraction (`-`). This means they are evaluated first.

- Addition (`+`) and subtraction (`-`) come after multiplication and division.

- Parentheses (`(...)`) can be used to alter the normal precedence and enforce a different evaluation order.


**Example:**

Without parentheses:

```
{ 2 + 3 * 4 }
```

This will result in 14 because multiplication happens before addition.

With parentheses:

```
{ (2 + 3) * 4 }
```

This will result in 20 because the parentheses force the addition to occur first.

RIFT respects common mathematical conventions, ensuring consistency and predictability in label expressions.

## 4. Variables list

This list will be split into categories, each having a separate table.

### General values

| Name           | Description                             | Type    | Remarks                           |
|----------------|-----------------------------------------|---------|-----------------------------------|
| `modVersion`   | Current Eclipse Menu version.           | String  | `v.1.0.0-alpha.1`                 |
| `platform`     | Platform you're currently running on.   | String  | `Windows`/`macOS`/`Android`/`iOS` |
| `geodeVersion` | Current Geode version.                  | String  | `4.6.3`                           |
| `gameVersion`  | Current Geometry Dash version.          | String  | `2.206`                           |
| `totalMods`    | Total count of Geode mods installed.    | Integer |                                   |
| `enabledMods`  | Count of only enabled mods.             | Integer |                                   |
| `fps`          | Current average FPS.                    | Float   |                                   |
| `realFps`      | FPS based on time taken for last frame. | Float   |                                   |

### Emojis
> Note: Some emojis look different in-game than the ones shown on this page.

| Name                         | Value | Name                          | Value |
|------------------------------|-------|-------------------------------|-------|
| `starEmoji`                  | ⭐    | `snowflakeEmoji`              | ❄️    |
| `moonEmoji`                  | 🌙    | `catEmoji`                    | 🐱    |
| `heartEmoji`                 | ❤️    | `dogEmoji`                    | 🐶    |
| `checkEmoji`                 | ✅    | `speakingHeadEmoji`           | 🗣️    |
| `crossEmoji`                 | ❌    | `robotEmoji`                  | 🤖    |
| `exclamationEmoji`           | ❗    | `alienEmoji`                  | 👽    |
| `questionEmoji`              | ❓    | `ghostEmoji`                  | 👻    |
| `fireEmoji`                  | 🔥    | `skullEmoji`                  | 💀    |
| `brokenHeartEmoji`           | 💔    | `babyEmoji`                   | 👶    |
| `likeEmoji`                  | 👍    | `dislikeEmoji`                | 👎    |
| `personInSteamyRoomEmoji`    | 🧖    | `sleepingAccommodationEmoji`  | 🛌    |
| `eclipseEmoji`               | 🌗    | `nerdEmoji`                   | 🤓    |
| `sobEmoji`                   | 😭    | `explodingHeadEmoji`          | 🤯    |
| `diamondEmoji`               | 💎    | `diamondShardsEmoji`          | 💠    |
| `geodeEmoji`                 | <img src="resources/Emojis/1faa9.png" width=16 alt="geodeEmoji">    | `orbsEmoji`                   | 🔹    |
| `emoji100`                   | 💯    | `shushingFaceEmoji`           | 🤫    |
| `bearEmoji`                  | 🐻    | `foxEmoji`                    | 🦊    |
| `faceWithRaisedEyebrowEmoji` | 🤨    | `gunEmoji`                    | 🔫    |
| `moyaiEmoji`                 | 🗿    | `screamEmoji`                 | 😱    |
| `sunglassesEmoji`            | 😎    | `heartEyesEmoji`              | 😍    |
| `crownEmoji`                 | 👑    | `smilingImpEmoji`             | 😈    |
| `clownEmoji`                 | 🤡    | `fishEmoji`                   | 🐟    |
| `sparklesEmoji`              | ✨    | `partyPopperEmoji`            | 🎉    |
| `pigEmoji`                   | 🐷    | `moonWithFaceEmoji`           | 🌚    |
| `fullMoonFaceEmoji`          | 🌝    | `sunWithFaceEmoji`            | 🌞    |
| `monkeyEmoji`                | 🐵    | `seeNoEvilMonkey`             | 🙈    |
| `speakNoEvilMonkey`          | 🙊    | `prayEmoji`                   | 🙏    |

### Special Emojis
> Some can be arrays or values. If they are arrays, use [index] to select one of the emojis.
> Example usage: `emojis.userCoin[1]` or `emojis.practice`

| Name                | Values                                                                                                                                                |
|---------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------|
| `emojis.userCoin`   | <img src="resources/Emojis/1f6de.png" width=24 alt="emojis.userCoin[0]">,<img src="resources/Emojis/1f535.png" width=24 alt="emojis.userCoin[1]">     |
| `emojis.secretCoin` | <img src="resources/Emojis/1f6de.png" width=24 alt="emojis.secretCoin[0]">,<img src="resources/Emojis/1f7e1.png" width=24 alt="emojis.secretCoin[1]"> |
| `emojis.startPos`   | <img src="resources/Emojis/1f9ff.png" width=24 alt="emojis.startPos">                                                                                 |
| `emojis.practice`   | <img src="resources/Emojis/2666.png" width=24 alt="emojis.practice">                                                                                  |

### Regional Indicators 
These are stored as a dictionary called `ri`, where the key is the letter.
Numbers are stored in `ri.num` dictionary, where the key is the number.

Example:
```
{ ri['h'] + ri['e'] + ri['l'] + ri['l'] + ri['o'] }
{ ri.num[0] + ri.num[1] + ri.num[2] + ri.num[3] + ri.num[4] + ri.num[5] }
```
Would result in `🇭🇪🇱🇱🇴` and `0️⃣1️⃣2️⃣3️⃣4️⃣5️⃣`


### Time & Date

| Name        | Description                      | Type    |
|-------------|----------------------------------|---------|
| `hour`      | Current hour (24-hour format)    | Integer |
| `minute`    | Current minute                   | Integer |
| `second`    | Current second                   | Integer |
| `day`       | Current date                     | Integer |
| `month`     | Current month                    | Integer |
| `monthName` | Current month name               | String  |
| `year`      | Current year                     | Integer |
| `clock`     | Time in format: `HH:MM:SS`       | String  |
| `clock12`   | Time in format: `hh:MM:SS AM/PM` | String  |

### Hacks

| Name             | Description                                         | Type    |
|------------------|-----------------------------------------------------|---------|
| `isCheating`     | Has the current state of the cheat indicator.       | Boolean |
| `noclip`         | Check whether noclip is enabled.                    | Boolean |
| `speedhack`      | Check whether speedhack is enabled.                 | Boolean |
| `speedhackSpeed` | Selected speedhack speed.                           | Float   |
| `framestepper`   | Check whether frame stepper is active.              | Boolean |
| `cbf`            | Check whether CBF (Click Between Frames) is active. | Boolean |

### Player information

| Name          | Description                                     | Type       | Remarks                                         |
|---------------|-------------------------------------------------|------------|-------------------------------------------------|
| `username`    | Your current account name.                      | String     |                                                 |
| `cubeIcon`    | Current cube icon ID.                           | Integer    |                                                 |
| `shipIcon`    | Current ship icon ID.                           | Integer    |                                                 |
| `ballIcon`    | Current ball icon ID.                           | Integer    |                                                 |
| `ufoIcon`     | Current ufo icon ID.                            | Integer    |                                                 |
| `waveIcon`    | Current wave icon ID.                           | Integer    |                                                 |
| `robotIcon`   | Current robot icon ID.                          | Integer    |                                                 |
| `spiderIcon`  | Current spider icon ID.                         | Integer    |                                                 |
| `swingIcon`   | Current swing icon ID.                          | Integer    |                                                 |
| `paths`       | Dictionary containing progress of each path.    | Dictionary | Key is path name, value is amount of stars.     |
| `shards`      | Dictionary containing the amount of each shard. | Dictionary | Key is shard name, value is amount of shards.   |
| `currentPath` | The name of the currently selected path.        | String     | Value can be used as key for `paths` dictionary |

> Note: `paths` and `shards` share the same key names:
> - `fire`
> - `ice`
> - `poison`
> - `shadow`
> - `lava`
> - `earth`
> - `blood`
> - `metal`
> - `light`
> - `soul`

In addition, there's also a `stats` dictionary which contains various player statistics:

| Name                    | Description                                                 | Type    |
|-------------------------|-------------------------------------------------------------|---------|
| `totalJumps`            | Total amount of jumps between all levels.                   | Integer |
| `totalAttempts`         | Total amount of attempts between all levels.                | Integer |
| `completedLevels`       | Total amount of completed official levels.                  | Integer |
| `completedOnlineLevels` | Total amount of completed online levels.                    | Integer |
| `demons`                | Total amount of demons completed.                           | Integer |
| `stars`                 | Total amount of stars collected.                            | Integer |
| `completedMapPacks`     | Total amount of completed map packs.                        | Integer |
| `goldCoins`             | Total amount of secret (gold) coins collected.              | Integer |
| `playersDestroyed`      | Total amount of players killed on the main menu background. | Integer |
| `likedLevels`           | Total amount of levels you have liked.                      | Integer |
| `ratedLevels`           | Total amount of levels you have rated.                      | Integer |
| `userCoins`             | Total amount of user coins collected.                       | Integer |
| `diamonds`              | Total amount of diamonds collected.                         | Integer |
| `orbs`                  | Current amount of orbs.                                     | Integer |
| `dailies`               | Total amount of daily levels completed.                     | Integer |
| `keys`                  | Current amount keys.                                        | Integer |
| `totalOrbs`             | Total amount of orbs collected.                             | Integer |
| `moons`                 | Total amount of moons collected.                            | Integer |
| `diamondShards`         | Amount of spendable diamonds available.                     | Integer |
| `gauntlets`             | Total amount of gauntlets completed.                        | Integer |
| `lists`                 | Total amount of lists you have completed.                   | Integer |


### Level information
> Note: these values are only available when playing/editing the level

| Name              | Description                                                                                    | Type             | Remarks                             |
|-------------------|------------------------------------------------------------------------------------------------|------------------|-------------------------------------|
| `levelID`         | Current level ID.                                                                              | Integer          |                                     |
| `levelName`       | Current level name.                                                                            | String           |                                     |
| `author`          | Level author username.                                                                         | String           |                                     |
| `isRobtopLevel`   | Check whether current level is an official one.                                                | Boolean          |                                     |
| `levelAttempts`   | Total level attempts count.                                                                    | Integer          |                                     |
| `levelStars`      | Level stars/moons count.                                                                       | Integer          |                                     |
| `levelRating`     | Rating given to the level                                                                      | String           | "Unrated", "Featured", "Epic", etc. |
| `difficulty`      | Difficulty name.                                                                               | String           | "Easy", "Hard Demon", "N/A", etc.   |
| `difficultyKey`   | Difficulty name, but in URL safe format.                                                       | String           | Used for Discord RPC icons.         |
| `practicePercent` | Practice mode record percent.                                                                  | Integer          |                                     |
| `bestPercent`     | Normal mode record percent.                                                                    | Integer          |                                     |
| `bestTime`        | Best level completion time.                                                                    | Float            |                                     |
| `best`            | In platformer mode: formatted best time (HH:MM:SS.mmm). In normal mode: same as `bestPercent`. | String / Integer |                                     |
| `totalLevelOrbs`  | Maximum amount of orbs you can collect from this level.                                        | Integer          |                                     |
| `levelOrbs`       | Amount of orbs you have collected in this level.                                               | Integer          |                                     |

### Gameplay
> Note: these values are only available when playing/editing the level

| Name                     | Description                                                                                               | Type    |
|--------------------------|-----------------------------------------------------------------------------------------------------------|---------|
| `playerX`                | Current player X position.                                                                                | Float   |
| `playerY`                | Current player Y position.                                                                                | Float   |
| `player2X`               | Second player X position.                                                                                 | Float   |
| `player2Y`               | Second player Y position.                                                                                 | Float   |
| `attempt`                | Current session attempt.                                                                                  | Integer |
| `isTestMode`             | Whether playing from a StartPos object.                                                                   | Boolean |
| `isPracticeMode`         | Whether currently in practice mode.                                                                       | Boolean |
| `isPlatformer`           | Whether the player is in platformer mode.                                                                 | Boolean |
| `isDualMode`             | Whether the player is in dual mode.                                                                       | Boolean |
| `levelTime`              | Current attempt time in seconds.                                                                          | Float   |
| `levelLength`            | Level length in block units.                                                                              | Float   |
| `levelDuration`          | Level duration in seconds.                                                                                | Float   |
| `time`                   | Formatted attempt time (HH:MM:SS.mmm).                                                                    | String  |
| `frame`                  | Current frame number.                                                                                     | Integer |
| `isDead`                 | Whether the player is currently dead.                                                                     | Boolean |
| `noclipDeaths`           | How many times you have "died" in noclip.                                                                 | Integer |
| `noclipAccuracy`         | Displays a ratio of alive/dead frames in noclip.                                                          | Float   |
| `progress`               | Current progress in the level.                                                                            | Float   |
| `realProgress`           | Original level progress, without 0% startpos bug fix.                                                     | Float   |
| `editorMode`             | Whether you're currently in editor mode.                                                                  | Boolean |
| `objects`                | Total objects count in the level.                                                                         | Integer |
| `runFrom`                | Percentage at which your best run started.                                                                | Float   |
| `bestRun`                | Percentage at which your best run ended.                                                                  | Float   |
| `lastDeath`              | Progress of your last attempt (null on first attempt).                                                    | Float   |
| `tps`                    | Current ticks per second.                                                                                 | Float   |
| `timeWarp`               | Current time warp multiplier.                                                                             | Float   |
| `gravity`                | Current gravity multiplier.                                                                               | Float   |
| `activeObjects`          | Amount of currently active objects in the level.                                                          | Integer |
| `gradients`              | Amount of activated gradient triggers in the level.                                                       | Integer |
| `particleCount`          | Amount of particles in the level.                                                                         | Integer |
| `songsCount`             | Amount of currently playing songs in the level.                                                           | Integer |
| `sfxCount`               | Amount of currently playing sound effects in the level.                                                   | Integer |
| `moveTriggerCount`       | Amount of currently active move triggers in the level.                                                    | Integer |
| `rotateTriggerCount`     | Amount of currently active rotate triggers in the level.                                                  | Integer |
| `scaleTriggerCount`      | Amount of currently active scale triggers in the level.                                                   | Integer |
| `followTriggerCount`     | Amount of currently active follow triggers in the level.                                                  | Integer |
| `areaMoveTrigger`        | Amount of currently active area move triggers in the level.                                               | Integer |
| `areaMoveTriggerTotal`   | Total amount of area move triggers in the level.                                                          | Integer |
| `areaRotateTrigger`      | Amount of currently active area rotate triggers in the level.                                             | Integer |
| `areaRotateTriggerTotal` | Total amount of area rotate triggers in the level.                                                        | Integer |
| `areaScaleTrigger`       | Amount of currently active area scale triggers in the level.                                              | Integer |
| `areaScaleTriggerTotal`  | Total amount of area scale triggers in the level.                                                         | Integer |
| `areaColOpTrigger`       | Amount of currently active area color/opacity triggers in the level.                                      | Integer |
| `areaColOpTriggerTotal`  | Total amount of area color/opacity triggers in the level.                                                 | Integer |
| `coins`                  | Array with states of coins in the level. 0 - uncollected, 1 - collected on current attempt, 2 - collected | Array   |
| `gamemode`               | Current game mode.                                                                                        | String  |
| `playerIcon`             | Current player icon ID (for current game mode).                                                           | Integer |

### Clicks information
> Note: these values are only available when playing/editing the level

| Name        | Description                                       | Type    |
|-------------|---------------------------------------------------|---------|
| `cps1`      | Clicks per second for the jump button             | Integer |
| `cps2`      | Clicks per second for the left button             | Integer |
| `cps3`      | Clicks per second for the right button            | Integer |
| `clicks1`   | Total clicks for the jump button                  | Integer |
| `clicks2`   | Total clicks for the left button                  | Integer |
| `clicks3`   | Total clicks for the right button                 | Integer |
| `maxCps1`   | Maximum CPS for the jump button                   | Integer |
| `maxCps2`   | Maximum CPS for the left button                   | Integer |
| `maxCps3`   | Maximum CPS for the right button                  | Integer |
| `cps`       | Total clicks per second                           | Integer |
| `clicks`    | Total clicks                                      | Integer |
| `maxCps`    | Total maximum CPS                                 | Integer |
| `cps1P2`    | Clicks per second for the jump button (player 2)  | Integer |
| `cps2P2`    | Clicks per second for the left button (player 2)  | Integer |
| `cps3P2`    | Clicks per second for the right button (player 2) | Integer |
| `clicks1P2` | Total clicks for the jump button (player 2)       | Integer |
| `clicks2P2` | Total clicks for the left button (player 2)       | Integer |
| `clicks3P2` | Total clicks for the right button (player 2)      | Integer |
| `maxCps1P2` | Maximum CPS for the jump button (player 2)        | Integer |
| `maxCps2P2` | Maximum CPS for the left button (player 2)        | Integer |
| `maxCps3P2` | Maximum CPS for the right button (player 2)       | Integer |
| `cpsP2`     | Total clicks per second (player 2)                | Integer |
| `clicksP2`  | Total clicks (player 2)                           | Integer |
| `maxCpsP2`  | Total maximum CPS (player 2)                      | Integer |

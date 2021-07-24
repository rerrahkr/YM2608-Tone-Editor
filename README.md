# YM2608 Tone Editor
[![GitHub release](https://img.shields.io/badge/release-v0.4.0-brightgreen.svg)](https://github.com/rerrahkr/YM2608-Tone-Editor/releases)
![Platform: windows](https://img.shields.io/badge/platform-windows-lightgrey.svg)
[![LICENSE](https://img.shields.io/github/license/rerrahkr/YM2608-Tone-Editor.svg)](./LICENSE)

This is a tone editor of YM2608 FM synthesis chip (OPNA).  
It enables to load and save tone data, read tone parameters from text and convert parameters to text.

It will create two settings files automatically at the first run.
* settings.ini - application settings
* formats.ini - specify text format to convert tone parameters

### Keyboard
There are two rows of a piano keyboard:

```
   High octave
 2 3  5 6 7  9 0
Q W ER T Y UI O P

   Low octave
 S D  G H J  L ;
Z X CV B N M, . /
```

F1, F2 is octave up/down, and F3 toggles FM/PSG.  
If sound doesn't stop despite releasing key, try pressing F12 which is audio stream reset key.

### Import/Export
YM2608 Tone Editor supports importing/exporting a tone/bank/song.

- .tone/.bank (Default FM tone/bank format)
- .bnk (GEMS bank) (import only)
- .bti/.btb (BambooTracker instrument/bank)
- .dat (MUCOM88 voice data)
- .dmp (Deflemask preset)
- .ff (PMD bank)
- .gyb (GYB bank) (import only)
- .ins (MVSTracker instrument)
- .m/.m2 (PMD song file) (import only)
- .opi/.ovi/.ozi/.m26/.m86 (FMP song file) (import only)
- .opni/.wopn (OPN2BankEditor imstrument/bank)
- .s98 (S98 file) (import only)
- .tfi (TFM Music Maker instrument)
- .vgi (VGM Music Maker instrument)
- .vgm (Video Game Music) (import only)
- .y12 (Gens KMod dump) (import only)

To export tones as a bank, you need to select them in the tone list which you want to do.

### Convert to text
It enables to convert tone data to text formatted by "Setup" -> "Output". It is a text file containing some parameter macros.  
This app parses it and replaces macros with tone parameters. It means that you can customize text output by editing format.  
To copy tone parameters text, open "Convert to text" dialog ("File" -> "Convert To Text").

#### Macro syntax
All Macros are described as `%{param name:param format}` except tone name macro `%{NAME}`.  
`param name` specifies parameter to replace macro.

| `param name` | tone parameter                             |
| ------------ | ------------------------------------------ |
| NO           | Tone number (always 0).                    |
| AL           | Algorithm.                                 |
| FB           | Feedback.                                  |
| ARn          | Attack rate. n is operator number (1-4).   |
| DRn          | Decay rate. n is operator number (1-4).    |
| SRn          | Sustain rate. n is operator number (1-4).  |
| RRn          | Release rate. n is operator number (1-4).  |
| SLn          | Sustain level. n is operator number (1-4). |
| TLn          | Total level. n is operator number (1-4).   |
| KSn          | Key scale. n is operator number (1-4).     |
| MLn          | Multiple. n is operator number (1-4).      |
| DTn          | Detune. n is operator number (1-4).        |
| AMn          | AMS enable. n is operator number (1-4).    |
| SSGEGn       | SSG-EG. n is operator number (1-4).        |

`param format` specifies printing style of paramer value. There are two options for formatting value:
* Zero flag. When you write 0 at the beginning of option, it pads 0 to the left of parameter value. When skip this flag, it pads whitespace instead of 0.
* Field width. In text, the value is printed in the space of specified width.

For example, you write format like this:
```
'@ FA  %{NO:03}	   ; %{NAME}
'@ %{AR1:3},%{DR1:3},%{SR1:3},%{RR1:3},%{SL1:3},%{TL1:3},%{KS1:3},%{ML1:3},%{DT1:3},%{AM1:3}
'@ %{AR2:3},%{DR2:3},%{SR2:3},%{RR2:3},%{SL2:3},%{TL2:3},%{KS2:3},%{ML2:3},%{DT2:3},%{AM2:3}
'@ %{AR3:3},%{DR3:3},%{SR3:3},%{RR3:3},%{SL3:3},%{TL3:3},%{KS3:3},%{ML3:3},%{DT3:3},%{AM3:3}
'@ %{AR4:3},%{DR4:3},%{SR4:3},%{RR4:3},%{SL4:3},%{TL4:3},%{KS4:3},%{ML4:3},%{DT4:3},%{AM4:3}
'@ %{AL:3},%{FB:3}
```
The app converts tone data to text like this:
```
'@ FA  000	   ; piano
'@  31,  8,  0,  7, 12, 20,  0,  2,  7,  0
'@  31,  5,  2,  7,  6,  4,  0,  2,  3,  0
'@  31,  7,  4,  7, 10, 18,  0,  2,  3,  0
'@  31,  0,  4,  7,  0,  0,  0,  2,  0,  0
'@   4,  5
```

### Read from text
It also enable to read tone data from text.  
When you paste the tone data text in "Read text" dialog ("File" -> "Read Text"), this app reads digits from the text and set to tone parameters in the order specified by "Setup" -> "Input".  
Note: This app reads digits in the order of appearance. This means that it also reads digits included in comments like tone name. Please remove the comments before reading.

## Changelog
*See [CHANGELOG.md](./CHANGELOG.md).*

## License
This program and its source code are licensed under the GNU General License Version 2 or later.

*See [LICENSE](./LICENSE) and [license_list.md](./license_list.md) of libraries.*

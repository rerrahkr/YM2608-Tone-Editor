# Changelog

## Unreleased

### Fixed

- [#21] - Fix incorrect .vgi import/export (thanks [@bryc])

[@bryc]: https://github.com/bryc

[#21]: https://github.com/rerrahkr/YM2608-Tone-Editor/issues/21

## v0.4.0 (2021-07-24)

### Added

- [#18], [#19] - Support 4 new format import
  - .m/.m2 (PMD song file) (thanks [@OPNA2608])
  - .opi/.ovi/.ozi/.m26/.m86 (FMP song file)
  - .s98 (S98 file)
  - .vgm (Video Game Music)
- [#13] - Add `make install` rules ([#12]; thanks [@freq-mod])

### Changed

- Update MAME emulation
- Apply GPL v2+ to the project

### Fixed

- [#14] - Fix the parameter order of the default MUCOM88 output preset (thanks [@freq-mod])
- [#15] - Fix the default MMLDRV output format (thank [@freq-mod])

[@OPNA2608]: https://github.com/OPNA2608

[#12]: https://github.com/rerrahkr/YM2608-Tone-Editor/issues/12
[#13]: https://github.com/rerrahkr/YM2608-Tone-Editor/pull/13
[#14]: https://github.com/rerrahkr/YM2608-Tone-Editor/issues/14
[#15]: https://github.com/rerrahkr/YM2608-Tone-Editor/issues/15
[#18]: https://github.com/rerrahkr/YM2608-Tone-Editor/issues/18
[#19]: https://github.com/rerrahkr/YM2608-Tone-Editor/issues/19

## v0.3.0 (2020-09-05)

### Added

- [#6] - Support SSG-EG edit (thanks [@freq-mod])
- [#9] - Support LFO edit (thanks [@freq-mod])
- Bank support
- FM/PSG switch key
- [#3], [#5], [#7] - Support 13 new format import/export (thanks [@freq-mod])
  - .bnk (GEMS bank) (import only)
  - .bti/.btb (BambooTracker instrument/bank)
  - .dat (MUCOM88 voice data)
  - .dmp (Deflemask preset)
  - .ff (PMD bank)
  - .gyb (GYB bank) (import only)
  - .ins (MVSTracker instrument)
  - .opni/.wopn (OPN2BankEditor imstrument/bank)
  - .tfi (TFM Music Maker instrument)
  - .vgi (VGM Music Maker instrument)
  - .y12 (Gens KMod dump) (import only)
- [#6] - Add Nuked OPN-Mod emulation (thanks [@freq-mod])
- [#8] - Tone search box (thanks [@freq-mod])

### Changed

- [#2] - Changed the maximum audio rate from 110933Hz to 55466Hz (thanks [@freq-mod])
- Enable to save multiple text input/output formats
- Integrate FM/PSG octave keys
- Accept dragging and dropping file
- [#4] - Change keyboard input to 2 rows - 2 octaves (thanks [@freq-mod])
- [#4] - Change minimum octave to 0 (thanks [@freq-mod])

### Fixed

- [#2] - Fixed internal FM sample rate (thanks [@freq-mod])
- Fix text output bug that ignored no option parameter

[@freq-mod]: https://github.com/freq-mod

[#2]: https://github.com/rerrahkr/YM2608-Tone-Editor/pull/2
[#3]: https://github.com/rerrahkr/YM2608-Tone-Editor/issues/3
[#4]: https://github.com/rerrahkr/YM2608-Tone-Editor/issues/4
[#5]: https://github.com/rerrahkr/YM2608-Tone-Editor/issues/5
[#6]: https://github.com/rerrahkr/YM2608-Tone-Editor/issues/6
[#7]: https://github.com/rerrahkr/YM2608-Tone-Editor/issues/7
[#8]: https://github.com/rerrahkr/YM2608-Tone-Editor/issues/8
[#9]: https://github.com/rerrahkr/YM2608-Tone-Editor/issues/9

## v0.2.0 (2018-09-03)

### Added

- [#1] - Add tone data reading from text (thanks [@KiON-GiON])
- Add tone-text conversion settings form

[@KiON-GiON]: https://github.com/KiON-GiON

[#1]: https://github.com/rerrahkr/YM2608-Tone-Editor/issues/1

## v0.1.1 (2018-05-10)

### Fixed

- Fix integer types
- Fix tone file name in save dialog
- Fix form design

## v0.1.0 (2018-03-27)

### Added

- Initial release

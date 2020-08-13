# Changelog

## Unreleased
### Added
- [#6] - Support SSG-EG edit (thanks [@freq-mod])
- Bank support
- FM/PSG switch key
- [#3], [#5] - Support 3 new format import/export (thanks [@freq-mod])
  - .bti/.btb (BambooTracker instrument/bank)
  - .ff (PMD FF)
- [#6] - Add Nuked OPN-Mod emulation (thanks [@freq-mod])

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

## v0.2.0 (2018/09/03)
### Added
- [#1] - Add tone data reading from text (thanks [@KiON-GiON])
- Add tone-text conversion settings form

[@KiON-GiON]: https://github.com/KiON-GiON

[#1]: https://github.com/rerrahkr/YM2608-Tone-Editor/issues/1

## v0.1.1 (2018/05/10)
### Fixed
- Fix integer types
- Fix tone file name in save dialog
- Fix form design

## v0.1.0 (2018/03/27)
### Added
- Initial release


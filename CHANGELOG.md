# Changelog

All notable changes to this project will be documented in this file.
The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

## [Version 2.6.0] - 2023-05-05

### Added

- Allow timeouts in output expressions.
- Allow scan codes in configuration.
- Added MetaLeft/MetaRight aliases OSLeft/OSRight.

## [Version 2.5.0] - 2023-03-09

### Added

- Showing notifications on Linux.

### Fixed

- Fixed hanging keys when using Windows remote desktop.
- Removed limit of 127 keys per sequence.
- Enabled visual styles for Windows about dialog.

## [Version 2.4.1] - 2022-11-27

### Fixed

- Keys triggered by timeout no longer released immediately.

## [Version 2.4.0] - 2022-11-26

### Added

- Allow to add timeouts to input sequences.
- Added about dialog on Windows.

### Fixed

- Cancel output-on-release when focus changed on Windows.
- Fixed releasing focused window detection on Linux.
- Fixed releasing virtual device on Linux.
- Fixed displaying version.

## [Version 2.3.0] - 2022-10-21

### Added

- Allow the device context filter to match multiple devices.

### Fixed

- Grab devices regardless of bus type.
- Linking filesystem library when building with older gcc/clang

## [Version 2.2.0] - 2022-09-17

### Added

- Handling wheel/slider functionality of some keyboards (Linux).

### Changed

- Grabbing keyboard/mouse devices despite unhandled axes (Linux).

## [Version 2.1.5] - 2022-07-29

### Added

- Added command line parameter to hide tray icon (Windows).

### Fixed

- Further improved Pause/NumLock key handling (Windows).

## [Version 2.1.4] - 2022-06-24

### Fixed

- Improved Pause/NumLock key handling (Windows).

### Changed

- Showing notification when config was updated (Windows).
- Renamed Break key to Cancel.

## [Version 2.1.3] - 2022-06-16

### Fixed

- Fixed loading configuration from '\$HOME/.config/' (Linux).

## [Version 2.1.2] - 2022-06-10

### Added

- Added Windows installer.

## [Version 2.1.1] - 2022-06-06

### Added

- Added tray icon for Windows client.
- Showing errors in notifications on Windows.

### Fixed

- Fail when config file cannot be read.
- Fixed output-on-release for terminal commands.
- Starting terminal commands in foreground.

## [Version 2.1.0] - 2022-05-23

### Added

- Added mouse button support.
- Added device context filter (only supported on Linux).

### Changed

- Split Windows version in client/server.
- Removed Interception mode on Windows.
- Removed colored error messages.

## [Version 1.10.0] - 2022-05-03

### Added

- Exiting on special key sequence Shift-Escape-K.
- Any in output expressions outputs the matched input.
- Added Break key.

### Fixed

- Improved sending of Ctrl-key sequences on Windows.
- Restored order of logical key substitution.

## [Version 1.9.0] - 2022-03-13

### Added

- Added D-Bus context update support.
- Added gnome-shell extension for updating focused window on Wayland.
- Added wlroots context update support on Wayland.

## [Version 1.8.3] - 2022-03-07

### Fixed

- Better handling of unknown version by build script.

## [Version 1.8.2] - 2022-02-20

### Changed

- Improved Not in output behavior.
- Statically linking runtime under Windows.

### Fixed

- Fixed right-modifier / mouse drag under Windows.
- Setting initial context on Windows.

## [Version 1.8.1] - 2022-01-23

### Changed

- No longer grabbing combined keyboard/mouse devices.
- CMake defaults to build type "Release".

## [Version 1.8.0] - 2021-12-20

### Added

- Multiple contexts can be active at once.
- [default] can be used to return to the default context.
- Logical keys can be defined.
- Allow to define common modifiers for context blocks.

### Fixed

- Updating active context on configuration update.
- AltRight is no longer implicitly forwarded.

## [Version 1.7.0] - 2021-12-13

### Changed

- Removed implicitly mapping of modifier keys.
- Matching begin of sequence when might-match failed.
- Completely releasing sequences and modifier groups.

### Added

- Documented importance of mapping order.

### Fixed

- Made Linux keyboard initialization more robust.

## [Version 1.6.0] - 2021-08-29

### Added

- Added terminal command output.
- Allow mapping of sequences in context blocks.
- Add option to check the validity of the config file, then exit.
- Added icon and metadata to Windows executable.

### Fixed

- Fixed bug with more than one virtual key in output.
- Fixed regular expressions containing [ ].

## [Version 1.5.0] - 2021-05-10

### Added

- Allow to define output on key release.

### Fixed

- Stricter validation of configuration to reduce undefined behavior.

## [Version 1.4.0] - 2021-03-29

### Changed

- Releasing triggered input in reverse order.

### Fixed

- Prevent hanging key.
- Validating state on Windows after session change.

## [Version 1.3.0] - 2021-01-26

### Added

- Optional verbose output.
- Regular expression support for context definition.

### Changed

- Updating context when window title changes.
- Applying system context independent of window context.

## [Version 1.2.0] - 2021-01-22

### Added

- Support of multiple keyboards under Linux.
- Improved device hot-plugging.
- Added some missing keys.

### Changed

- Simplified context block definition.
- Made X11 dependency optional.

### Fixed

- Proper output of Any key.
- Might match when key is hold.

## [Version 1.1.5] - 2020-05-09

[version 2.6.0]: https://github.com/houmain/keymapper/compare/2.5.0...2.6.0
[version 2.5.0]: https://github.com/houmain/keymapper/compare/2.4.1...2.5.0
[version 2.4.1]: https://github.com/houmain/keymapper/compare/2.4.0...2.4.1
[version 2.4.0]: https://github.com/houmain/keymapper/compare/2.3.0...2.4.0
[version 2.3.0]: https://github.com/houmain/keymapper/compare/2.2.0...2.3.0
[version 2.2.0]: https://github.com/houmain/keymapper/compare/2.1.5...2.2.0
[version 2.1.5]: https://github.com/houmain/keymapper/compare/2.1.4...2.1.5
[version 2.1.4]: https://github.com/houmain/keymapper/compare/2.1.3...2.1.4
[version 2.1.3]: https://github.com/houmain/keymapper/compare/2.1.2...2.1.3
[version 2.1.2]: https://github.com/houmain/keymapper/compare/2.1.1...2.1.2
[version 2.1.1]: https://github.com/houmain/keymapper/compare/2.1.0...2.1.1
[version 2.1.0]: https://github.com/houmain/keymapper/compare/1.10.0...2.1.0
[version 1.10.0]: https://github.com/houmain/keymapper/compare/1.9.0...1.10.0
[version 1.9.0]: https://github.com/houmain/keymapper/compare/1.8.2...1.9.0
[version 1.8.3]: https://github.com/houmain/keymapper/compare/1.8.2...1.8.3
[version 1.8.2]: https://github.com/houmain/keymapper/compare/1.8.1...1.8.2
[version 1.8.1]: https://github.com/houmain/keymapper/compare/1.8.0...1.8.1
[version 1.8.0]: https://github.com/houmain/keymapper/compare/1.7.0...1.8.0
[version 1.7.0]: https://github.com/houmain/keymapper/compare/1.6.0...1.7.0
[version 1.6.0]: https://github.com/houmain/keymapper/compare/1.5.0...1.6.0
[version 1.5.0]: https://github.com/houmain/keymapper/compare/1.4.0...1.5.0
[version 1.4.0]: https://github.com/houmain/keymapper/compare/1.3.0...1.4.0
[version 1.3.0]: https://github.com/houmain/keymapper/compare/1.2.0...1.3.0
[version 1.2.0]: https://github.com/houmain/keymapper/compare/1.1.5...1.2.0
[version 1.1.5]: https://github.com/houmain/keymapper/releases/tag/1.1.5

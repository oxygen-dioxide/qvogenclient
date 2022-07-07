# QVogenClient

---

## Intro

Vogen editor using **QSynthesis** framework.

![Display](./docs/images/vogen-plus.png)

## Supported Platforms

+ Microsoft Windows
+ Apple MacOS
+ Linux

## How To Use

### System Related

#### Windows

See [Windows Instructions](./docs/usage-win.md).

#### Mac

See [Mac Instructions](./docs/usage-mac.md).

#### Linux

See [Linux Instructions](./docs/usage-linux.md).

### User Interface

See [User Interface](./docs/usage-common.md).


## Build from source

The whole QSynthesis source code is written in C++ with Qt library. In order to facilitate code reuse and reduce compilation troubles, the project uses as few other libraries as possible.

### Requirements

| Component    | Requirement | Detailed                               |
| :----:       | :----:      | :----:                                 |
| Qt           | >=5.15      | Core, Gui, Svg, Network, Multimedia    |
| Compiler     | >=C++17     | MSVC 2019, MinGW 0.8.1, GCC, Clang     |
| CMake        | >=3.17      | >=3.20 is recommended                  |

On Windows, compiling with Microsoft Visual C++ is strongly recommended.

For detailed information, see [Build Instructions](./docs/build-insturctions.md).

## Open-source softwares used

#### Qt 5.15.2
+ Home: https://www.qt.io/
+ License: LGPL v3
+ Owner: Qt Company
+ Usage: Build QVogenClient interface

#### Frameless Helper
+ Source: https://github.com/wangwenx190/framelesshelper
+ License: MIT License
+ Owner: Yuhang Zhao
+ Usage: Create frameless window for QVogenClient

#### QMidi
+ Source: https://github.com/waddlesplash/QMidi
+ License: MIT License
+ Owner: Augustin Cavalier
+ Usage: Parse midi file for QVogenClient

#### QuaZip
+ Source: https://github.com/stachenov/quazip
+ License: LGPL v2.1
+ Owner: Sergey A. Tachenov
+ Usage: Parse ZIP format files for QVogenClient

#### Zlib
+ Home: http://www.zlib.net/
+ License: zlib License
+ Owner: Jean-loup Gailly and Mark Adler
+ Usage: For QuaZip to call

#### Wave
+ Source: https://github.com/audionamix/wave
+ License: MIT license
+ Owner: audionamix
+ Usage: Export Wave Audio

Most libraries and their source code are unmodified and are called as dynamic links.

## Related Pages

### Vogen Official

+ Gitee: https://gitee.com/aqtq314/Vogen.Client
+ GitHub: https://github.com/aqtq314/Vogen.Client

### QVogenServer

+ Gitee: https://gitee.com/functioner/qvogenserver

---

## License

QVogenClient is licensed under Apache 2.0 License.
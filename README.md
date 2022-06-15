# QVogenClient

## Intro

Vogen editor using **QSynthesis** framework.

## Build from source

The whole QSynthesis source code is written in C++ with Qt library. In order to facilitate code reuse and reduce compilation troubles, the project uses as few other libraries as possible.

### Requirements

| Component  | Requirement | Detailed |
| :----:     | :----:      | :----:   |
| Qt         | >=5.15    | Core, Gui, Svg, Network, Multimedia    |
| Compiler      | >=C++17       | MSVC 2019, MinGW 0.8.1, GCC, Clang   |
| CMake      |  >=3.5      | >=3.20 is recommended  |

On Windows, compiling with Microsoft Visual C++ is strongly recommended.

For detailed information, see [Build Instructions](./docs/build-insturctions.md).

## Open-source softwares used

#### Qt 5.15.2
+ Home: https://www.qt.io/
+ License: LGPL v3
+ Owner: Qt Company

#### Frameless Helper
+ Source: https://github.com/wangwenx190/framelesshelper
+ License: MIT License
+ Owner: Yuhang Zhao

#### QMidi
+ Source: https://github.com/waddlesplash/QMidi
+ License: MIT License
+ Owner: Augustin Cavalier

#### QuaZip
+ Source: https://github.com/stachenov/quazip
+ License: LGPL v2.1
+ Owner: Sergey A. Tachenov

## Libraries built for QSynthesis

#### QScrollableTabWidget

+ Source: https://github.com/SineStriker/QScrollableTabWidget
+ License: Apache 2.0 License

## License

QVogenClient is licensed under Apache 2.0 License.
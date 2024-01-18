# C.Basic - 针对fx-9860及fx-CG系列图形计算器开发的卡西欧BASIC集成开发环境插件

[English](README.md) | [Français](README_fr.md) | [日本語](README_ja.md) | [中文(简体)](README_zh.md)

C.Basic（读音 "c-点-basic"）是以C/C++语言实现的卡西欧BASIC解释器。*FX版*以卡西欧官方SDK编写，而*CG版*以社区miniSDK编写。

如果你想随时随地为计算器上的BASIC程序添加SDK专属功能，而又不想面对易错的C/C++调试环境，那C.Basic这个插件非常适合你。

C.Basic在目前阶段只是一个解释器。未来C.Basic将会拥有编译器，从而进化为C:Basic（"c-冒号-basic"）。

## Features

* Achieve add-in like performance with little to no modifications to existing Casio Basic programs
* Access and edit programs directly in SMEM and SD card
* Robust editor with features from modern IDE — syntax highlighting, indentation, variable/matrix lookup and debugging
* Extended commands with Casio Basic style syntax
* Emulate G1M runtime environment in fx-CG calculators

## Things to know

* If you are an fx-CG10 or fx-CG20 user, C.Basic will ask whether you want it to overclock your calculator to match the performance of an fx-CG50.
    * The mechanic is based on [Ptune2](https://pm.matrix.jp/ftune2e.html), an overclocking add-in made by the same author of C.Basic. Upon agreement, C.Basic will increase your processor core clock (IFC) from 58.98 MHz to 117.96 MHz.
    * While this overclocking preset is proven to be stable in our test cases, please keep in mind that damage caused by overclocking (and using this add-in) will not be covered by Casio's warranty.
    * You can disable this prompt in the settings at anytime.
* As C.Basic is written in C/C++, it uses floating point to express numbers instead of binary-coded decimal that Casio Basic uses. As such, handling non-integers may cause numerical errors. You are advised not to use C.Basic for applications involving serious calculations.
* Please note that the file manager of C.Basic is not yet optimized with calculators featuring Fugue filesystem. See [#1](https://gitea.planet-casio.com/CalcLoverHK/C.Basic/issues/1) for more details.

## Guidelines

Find a bug during use? Feel like there are rooms for improvement? Please provide these feedbacks by opening an issue ticket or pull request. We will credit your work in our version release if they are approved!

### Bug report

Describe how the bug behaves and provide the steps to reproduce it in your report. Screenshots of the bug are welcomed. If capable, please indicate which parts of code that need to be changed to fix the bug in your pull request.

### Feature request

List the reasons to support your proposal. **Once your proposal is approved**, you can provide your codes for our references.

### Documentation

Open a pull request to commit your documentation changes.

## License

C.Basic is licensed under the GNU General Public License, version 2. See [LICENSE.md](LICENSE.md) for the full license text.

## Credit

See [CREDIT.md](CREDIT.md) for the credit list.

## Wiki
Check out the official C.Basic Wiki by clicking the `Wiki` tab in the navigation bar.

Currently the Wiki is hosted on Fandom. However, we have decided to move from it to Casio Universal Wiki once the majority contents of the Wiki are ready.
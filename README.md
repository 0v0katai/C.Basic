# C.Basic - Casio Basic IDE add-in for fx-9860 and fx-CG graphing calculators

C.Basic (pronounce "c-dot-basic") is the reference implementation of Casio Basic, written in Casio SDK for fx-9860 and miniSDK for fx-CG calculators.

The add-in lets you incorporate SDK-exclusive features to your Casio Basic programs in calculator, without having to worry about the cumbersome debugging environment of C/C++.

C.Basic has only implemented the interpreter part in the current phase. In the future, C.Basic will evolve into C:Basic ("c-colon-basic") which includes a compiler.

## Supported calculators

* *FX Edition*
    * fx-9860 series
    * fx-9750GIII
    * Graph 75/85/95 series
    * Graph 35+E II
* *CG Edition*
    * fx-CG series
    * Graph 90+E

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

Find a bug during use? Feel like there are rooms for improvement? Please provide these feedbacks by opening an issue ticket or pull request. We will consider any feedback and credit your work in our version release.

### Bug report

Describe how the bug behaves and provide the steps to reproduce it in your report. If capable, please indicate the part of code that need to be changed as well to fix it in your pull request.

### Feature request

List the reasons to support your proposal, working principles and usages of the new feature.

### Documentation

Open a pull request to commit your documentation changes.

## License

C.Basic is licensed under the GNU General Public License, version 2. See [LICENSE.md](LICENSE.md) for the full license text.

## Credit

See [CREDIT.md](CREDIT.md) for the credit list.

## Wiki
Check out the official C.Basic Wiki by clicking the `Wiki` tab in the navigation bar. The Wiki is hosted on Fandom, but we have decided to move from it to Casio Universal Wiki once the majority contents of the Wiki are ready.
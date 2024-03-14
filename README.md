# C.Basic - CASIO Basic IDE for fx-9860 and fx-CG calculators

C.Basic ("c-dot-basic") is the reference implementation of CASIO Basic, written in CASIO SDK for fx-9860 and miniSDK for fx-CG calculators.

The add-in lets you incorporate SDK-exclusive features to your CASIO Basic programs in calculator, without having to worry about the cumbersome debugging environment of C/C++.

At this moment, only the interpreter part is implemented (hence the dot in the add-in name). In the future, C.Basic will evolve into C:Basic ("c-colon-basic") which includes a compiler.

## Supported calculators

_This is a simplified list of supported calculators. For a detailed calculator models, click [here](https://cbasic.fandom.com/wiki/Tutorials/Installation)._

Edition | Calculators
---     | ---
FX      | fx-9750GIII/9860, Graph 35+EII/75/85/95
CG      | fx-CG10/20/50, Graph 90+E

## Features

* Achieve add-in like performance with little to no modifications to existing CASIO Basic programs
* Access and edit programs directly in SMEM and SD card
* Robust editor with features from modern IDE — syntax highlighting, indentation, variable/matrix lookup and debugging
* Extended commands with CASIO Basic style syntax
* Emulate G1M runtime environment in fx-CG calculators

## Things to know

* If you are an fx-CG10 or fx-CG20 user, C.Basic will ask you if you wish to overclock your calculator to achieve the performance of an fx-CG50.
    * The overclocking mechanism is based on [Ptune2](https://git.planet-casio.com/CalcLoverHK/Ftune-Ptune), an overclocking add-in developed by the same author of C.Basic. Upon agreement, C.Basic will apply the [F3](https://git.planet-casio.com/CalcLoverHK/Ftune-Ptune/wiki/Ptune2#presets) preset of Ptune2.
    * Please keep in mind that damage caused by overclocking (and using this add-in) will not be covered by CASIO's warranty, and the author is not responsible for any loss caused by user's improper operations.
    * You can disable the `Auto OverClk` setting to prevent this prompt from being displayed at each startup.
* As C.Basic uses floating point to express numbers instead of binary-coded decimal that CASIO Basic uses, handling non-integers calculations may cause approximation errors. You are advised not to use C.Basic for applications involving serious calculations.
* Please note that the file manager of C.Basic is not yet optimized with calculators featuring the Fugue filesystem. See [#1](https://git.planet-casio.com/CalcLoverHK/C.Basic/issues/1) for more details.

## License

C.Basic is licensed under the GNU General Public License, version 2.  
See [LICENSE.md](LICENSE.md) for the full license text.

## Credit

See [CREDIT.md](CREDIT.md) for the credit list.

## Wiki

Check out the official C.Basic Wiki by clicking the `Wiki` tab in the navigation bar.  
The Wiki is hosted on Fandom, but we have decided to migrate to other wiki host once the majority content of it are ready.
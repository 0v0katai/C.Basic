# C.Basic - CASIO Basic IDE for fx-9860 and fx-CG calculators

🌍 **English** | Français | 日本語 | [简体中文](README_zh.md)

C.Basic ("c-dot-basic") is the reference implementation of CASIO Basic programming language in C.

In addition to speeding up your CASIO Basic programs, the add-in also lets you execute SDK functions on-the-go, without opening your PC and compile a full-fledged add-in.

## Supported calculators

_This is a simplified list of supported calculators. For an exact calculator model list, click [here](https://cbasic.fandom.com/wiki/Tutorials/Installation)._

Edition | Calculators
---     | ---
FX      | fx-9750G III, fx-9860, Graph 35+E II/75/85/95
CG      | fx-CG10/20/50, Graph 90+E

## Features

* Achieve add-in like performance with little to no modifications to existing CASIO Basic programs
* Access and edit programs directly in SMEM and SD cards
* Robust editor with features from modern IDE:
  * Syntax highlighting
  * Indentation
  * Variable inspection
  * Just-In-Time debugging
* Extended commands with CASIO Basic style syntax
* Emulate G1M runtime environment in fx-CG calculators

## Compiling C.Basic

You must install the full gint/fxSDK toolchain before compiling C.Basic.

### FX Edition

```bash
# Compile and send to fx-9860 (except G-III) via Cahute
fxsdk build-fx -s

# Compile and send to G-III via UDisk2
fxsdk build-fx && fxlink -sw CBASIC.g1a
```

### CG Edition

```bash
# Compile and send to fx-CG via UDisk2
fxsdk build-cg -s

# Compile and load directly into Add-in Push on fx-CG50
fxsdk build-cg-push -s
```

## Things to know

* Please keep in mind that we bear no responsibility to damages caused by using this add-in. **Use it at your own risk!**
* If you are an fx-CG10/20 user, C.Basic will ask whether you would like to overclock your calculator to achieve a similar performance to an fx-CG50.
  * The mechanism is based on [Ptune2](https://git.planet-casio.com/CalcLoverHK/Ftune-Ptune), an overclocking add-in developed by the same author of C.Basic. Upon agreement, C.Basic will apply the [F3](https://git.planet-casio.com/CalcLoverHK/Ftune-Ptune/wiki/Ptune2#presets) preset of Ptune2.
  * You can disable the `Auto OverClk` setting to skip this prompt at startup.
* As C.Basic uses floating point for decimal representation instead of binary-coded decimal (BCD) that CASIO Basic uses, non-integer calculations may have approximation errors. You are advised not to use C.Basic for purposes like finance that require precise results. Currently there is no plan for adding BCD arithmetic to C.Basic.
* Please note that the file manager of C.Basic is not yet optimized with calculators featuring the Fugue filesystem. See [#1](https://git.planet-casio.com/CalcLoverHK/C.Basic/issues/1) for more details.

## Wiki

Check out the official C.Basic Wiki by clicking the `Wiki` tab in the navigation bar. The Wiki is hosted on Fandom, but we have decided to migrate to other wiki host once the majority content of it are ready.

# C.Basic - fx-9860及fx-CG系列图形计算器的CASIO Basic集成开发环境

🌍 [English](README.md) | Français | 日本語 | **简体中文**

C.Basic是以C语言实现的CASIO Basic解释器。

该插件除了能加快CASIO Basic程序的运行速度，还能让你在计算器上使用SDK函数，无需打开电脑并编译完整插件。

## 适用机型

_此列表为适用机型的摘要，点击[此处](https://cbasic.fandom.com/wiki/Tutorials/Installation)以获取更详细的型号列表。_

版本 | 机型
--- | ---
FX  | fx-9750G III, fx-9860, Graph 35+E II/75/85/95
CG  | fx-CG10/20/50, Graph 90+E
CW  | fx-CG100, fx-1AU Graph, Graph Math+

## 特色

* 几乎无需任何修改，即可实现类似插件的性能
* 可直接存取和编辑在SMEM和SD卡（仅限配备SD卡槽的机型）里的程序
* 具有主流集成开发环境特性的编辑器
  * 语法高亮
  * 缩进
  * 检查变量
  * JIT调试器
* 承继CASIO Basic语法风格的扩展指令
* 在fx-CG/CW系列计算器上模拟G1M运行环境

## 编译C.Basic

在编译C.Basic前，你必须安装完整gint/fxSDK编译工具。

### FX版本

```bash
# 编译插件，然后使用Cahute传送插件至fx-9860机型（除G-III机型）
fxsdk build-fx -s

# 编译插件，然后使用UDisk2传送插件至G-III机型
fxsdk build-fx
fxlink -sw ./CBASIC.g1a
```

### CG/CW版本

若需要编译CW版本，请先在CMakeList.txt里取消注释`set(MPM 1)`这一行。

```bash
# 编译插件，然后使用UDisk2传送插件至fx-CG/CW机型
fxsdk build-cg -s

# 编译插件，然后传送插件至fx-CG50上的Add-in Push插件
fxsdk build-cg-push -s

# 编译插件，然后传送插件至fx-CG100上MPM的Add-in Push模块
fxsdk build-cg && fxlink -pw build-cg/CBASIC.bin
```

## 注意事项

* 请注意：我们对不当使用该插件所造成的损失概不负责，**风险自负！**
* 如果您是fx-CG10/20用户，C.Basic会询问你是否启用超频功能，使你的计算器达到类似于fx-CG50的性能。
  * 超频原理基于[Ptune2](https://git.planet-casio.com/CalcLoverHK/Ftune-Ptune)插件，由C.Basic的原作者制作。一经同意，C.Basic将会套用Ptune2的[F3](https://git.planet-casio.com/CalcLoverHK/Ftune-Ptune/wiki/Ptune2#presets)预设。
  * 您可以随时在设置中禁用此提示。
* 由于C.Basic使用浮点表示小数，而不是卡西欧Basic使用的二进码十进数（BCD码），处理非整数计算可能会导致误差。我们建议你不要将C.Basic用于金融等需要精确结果的用途上。目前尚无计划为C.Basic添加BCD运算模式。
* C.Basic的文件管理器尚未针对采用Fugue文件系统的计算器进行优化，详情请参见[#1](https://git.planet-casio.com/CalcLoverHK/C.Basic/issues/1)。

## 百科

点击上方导航栏中的`Wiki`标签，以进入C.Basic百科。目前百科被托管在Fandom上，不过我们已决定在其大部分内容准备就绪后，将其转移到其他托管网站。

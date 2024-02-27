# C.Basic - fx-9860及fx-CG系列图形计算器的CASIO Basic集成开发环境

C.Basic是以C/C++语言实现的卡西欧Basic解释器。FX版以卡西欧官方SDK编写，而CG版以社区miniSDK编写。

该插件能让你随时随地在计算器上将SDK专属功能整合到你的卡西欧Basic程序，而不用面对繁琐的C/C++调试环境。

C.Basic在当前阶段只实现了解释器部分。未来C.Basic将包含编译器，并改名为C:Basic。

## 适用机型

_此列表为适用机型的摘要，点击[此处](https://cbasic.fandom.com/wiki/Tutorials/Installation)以知悉更详细的受支持机型。_

版本 | 机型
---  | ---
FX版 | fx-9750GIII/9860, Graph 35+EII/75/85/95
CG版 | fx-CG10/20/50, Graph 90+E

## 特色

* 几乎无需任何修改，即可实现类似插件的性能
* 可直接存取和编辑在SMEM和SD卡里的程序
* 具有主流集成开发环境特性的编辑器 — 语法高亮、缩进、变量/矩阵查找和调试
* 延续卡西欧Basic语法风格的扩展指令
* 在fx-CG系列计算器上模拟G1M运行环境

## 注意事项

* 如果您是fx-CG10或fx-CG20用户，C.Basic会询问你是否同意让它超频，使你的计算器达到fx-CG50的性能。
    * 超频原理基于[Ptune2](https://gitea.planet-casio.com/CalcLoverHK/Ftune-Ptune)插件，由C.Basic的原作者制作。一经同意，C.Basic将会套用Ptune2的[F3](https://gitea.planet-casio.com/CalcLoverHK/Ftune-Ptune/wiki/Ptune2#presets)模式。
    * 超频（以及使用该插件）所造成的损坏不在卡西欧的保修范围之内，作者对任何因用户操作不当造成的损失概不负责。
    * 您可以随时在设置中禁用此提示。
* 由于C.Basic使用浮点表示数字，而不是卡西欧Basic使用的二进码十进数（BCD码），处理非整数计算可能会导致误差。我们建议不要将C.Basic用于涉及严谨计算的用途上。
* C.Basic的文件管理器尚未针对采用Fugue文件系统的计算器进行优化，详情请参见[#1](https://gitea.planet-casio.com/CalcLoverHK/C.Basic/issues/1)。

## 许可证

C.Basic采用GNU通用公共许可证第2版（GPLv2）。  
请参阅[LICENSE.md](LICENSE.md)，以查看完整的许可证文本。

## 鸣谢

请参阅[CREDIT.md](CREDIT.md)，以查看鸣谢名单。

## 百科

点击上方导航栏中的"Wiki"标签，以进入C.Basic百科。  
目前它托管在Fandom上，不过我们已决定在其大部分内容准备就绪后，将其转移到其他Wiki托管。
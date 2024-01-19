# C.Basic - 针对fx-9860及fx-CG系列图形计算器开发的Casio Basic集成开发环境插件

[English](README.md) | [Français](README_fr.md) | [日本語](README_ja.md) | [中文(简体)](README_zh.md)

C.Basic是以C/C++语言实现的Casio Basic解释器。*FX版*以卡西欧官方SDK编写，而*CG版*以社区miniSDK编写。

C.Basic能让你随时随地在计算器上的程序添加SDK专属功能，而不用面对繁琐的C/C++调试环境。

C.Basic在当前阶段只是一个解释器。未来C.Basic将包含编译器，并改名为C:Basic。

## 特色

* 几乎无需任何修改，即可实现类似插件的性能
* 可直接存取和编辑在SMEM和SD卡里的程序
* 具有主流集成开发环境特性的编辑器 — 语法高亮、缩进、变量/矩阵查找和调试
* Casio Basic语法风格的扩展指令
* 在fx-CG系列计算器上模拟G1M运行环境

## 注意事项

* 如果您是fx-CG10或fx-CG20用户，C.Basic会询问你是否同意让它超频，使你的计算器达到 fx-CG50的性能。
    * 超频原理基于[Ptune2](https://pm.matrix.jp/ftune2e.html)插件，由C.Basic的同一作者制作。一经同意，C.Basic将会把处理器核心时钟（IFC）从58.98 MHz提高到117.96 MHz。
    * 超频（以及使用该插件）所造成的损坏不在卡西欧的保修范围之内。
    * 您可以随时在设置中禁用此提示。
* 由于C.Basic使用浮点表示数字，而不是Casio Basic使用的二进码十进数（BCD码），处理非整数计算可能会导致误差。我们建议不要将C.Basic用于涉及严谨计算的用途上。
* C.Basic的文件管理器尚未针对采用Fugue文件系统的计算器进行优化。详情请参见[#1](https://gitea.planet-casio.com/CalcLoverHK/C.Basic/issues/1)。

## 指引

在使用过程中发现了漏洞？觉得有需要改进的地方？请开启工单或合并请求并依据以下指引提交。我们会考虑每一个反馈，并在我们的版本发布中注明你的贡献以示感谢。

### 漏洞报告

描述此漏洞并提供重现步骤。如果可以的话，也请在合并请求中指出需要更改代码的哪些部分以修复该漏洞。

### 功能提议

列出支持提议的理由、新功能的工作原理和使用方法。

### 文档

打开合并请求以提交你的文档更改。

### 许可证

C.Basic采用GNU通用公共许可证第2版（GPLv2）。请参阅[LICENSE.md](LICENSE.md)，以查看完整的许可证文本。

## 鸣谢

参阅[CREDIT.md](CREDIT.md)，以查看鸣谢名单。

### 维基

点击上方导航栏中的"Wiki"标签，以进入C.Basic百科全书。

目前C.Basic百科全书托管在Fandom上。不过我们已决定在百科全书的大部分内容准备就绪后，将其转移到Casio Universal Wiki。
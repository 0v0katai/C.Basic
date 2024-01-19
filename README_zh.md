# C.Basic - 针对fx-9860及fx-CG系列图形计算器开发的Casio Basic集成开发环境插件

[English](README.md) | [Français](README_fr.md) | [日本語](README_ja.md) | [中文(简体)](README_zh.md)

C.Basic是以C/C++语言实现的Casio Basic解释器。*FX版*以卡西欧官方SDK编写，而*CG版*以社区miniSDK编写。

C.Basic能让你随时随地在计算器上的程序添加SDK专属功能，而不用面对繁琐的C/C++调试环境。

C.Basic在当前阶段只是一个解释器。未来C.Basic将包含编译器，并改名为C:Basic。

## 功能

* 几乎无需任何修改，即可实现类似插件的性能
* 可直接存取和编辑在SMEM和SD卡里的程序
* 具有主流集成开发环境特性的编辑器 — 语法高亮、缩进、变量/矩阵查找和调试
* Casio Basic语法风格的扩展指令
* 在fx-CG系列计算器上模拟G1M运行环境

## 注意事项

* 如果您是fx-CG10或fx-CG20用户，C.Basic会询问你是否同意让它超频，使你的计算器达到 fx-CG50的性能。
    * 超频原理基于[Ptune2](https://pm.matrix.jp/ftune2e.html)插件，由C.Basic的同一作者制作。一经同意，C.Basic将会把处理器核心时钟（IFC）从58.98 MHz提高到117.96 MHz。
    * 在我们的测试案例中，该超频预设值被证明是稳定的。但请注意，超频（以及使用该插件）所造成的损坏不在卡西欧的保修范围之内。
    * 您可以随时在设置中禁用此提示。
* 由于C.Basic以C/C++编写，它使用浮点表示数字，而不是Casio Basic使用的二进制编码的十进制（binary-coded decimal）。因此，处理非整数计算可能会导致误差。我们建议不要将C.Basic用于涉及严谨计算的用途上。
* 请注意，C.Basic的文件管理器尚未针对采用Fugue文件系统的计算器进行优化。详情请参见 [#1](https://gitea.planet-casio.com/CalcLoverHK/C.Basic/issues/1)。

## 指引

在使用过程中发现了漏洞？觉得有需要改进的地方？请开启工单或合并请求。我们会考虑每一个反馈。

### 错误报告

描述错误并提供重现错误的步骤。欢迎提供错误的截图。如果可以，请在您的拉取请求中指出需要更改代码的哪些部分以修复该错误。

### 功能请求

列出支持您提议的理由。**一旦您的建议获得批准**，您可以提供代码供我们参考。

### 文档

打开一个拉取请求，提交您的文档更改。

### 许可证

C.Basic 采用 GNU 通用公共许可证第 2 版。请参阅 [LICENSE.md](LICENSE.md)，了解完整的许可证文本。

## Credit

参见 [CREDIT.md](CREDIT.md)，查看授权列表。

### 维基
点击导航栏中的 "Wiki "标签，查看官方 C.Basic 维基。

目前维基托管在 Fandom 上。不过，我们已决定在维基的大部分内容准备就绪后，将其转移到卡西欧通用维基。

通过www.DeepL.com/Translator（免费版）翻译

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
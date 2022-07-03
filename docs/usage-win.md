# QVogenClient

---

## 前言

Vogen 编辑器，使用**QSynthesis**的框架实现。

Copyright 2020-2022, Sine Striker.

## 构建信息

#### 版本
QVogenClient version 0.2.21


#### 平台
Windows / x86_64

#### 构建

Microsoft Visual C++ 2019

#### 共享库

+ Qt 5.15.2
+ FramelessHelper
+ QMidi
+ QuaZip + zlib
+ QUtauStd
+ wave

#### 运行时

+ VCRuntime 14.29.30139 (按需执行 `vc_redist.x64.msi`)

## 使用的资源

#### 运行时数据
````
%TEMP%\QVogenTemp
````
+ 默认路径
  `C:\Users\<username>\AppData\Local\Temp\QVogenTemp`


卸载时请删除本目录。

#### 应用数据
````
%APPDATA%\QVogenClient
````
+ 默认路径
  `C:\Users\<username>\AppData\Roaming\QVogenClient`

卸载时请删除本目录。

## 本地服务端

### 注意事项

+ 使用客户端前，必须先打开服务端`resources/server/CSRenderHost.exe`（或使用`CSRenderHost.bat`脚本启动），确保使用本地`14251`端口没有被占用。

+ 不宜打开超过一个客户端进程。

### 系统需求

+ [.Net 5 x64 Desktop Runtime](https://dotnet.microsoft.com/zh-cn/download/dotnet/5.0/runtime)

## 使用方法

### 钢琴卷帘

#### 滚轮移动

+ 横向: Shift + 滚轮
+ 纵向: 滚轮

#### 拖拽移动

+ Ctrl + Shift + 鼠标拖放

#### 滚轮缩放

+ 横向: Ctrl + Shift + 滚轮
+ 纵向: Ctrl + 滚轮

#### 拖拽缩放

+ Ctrl + Alt + 鼠标单击

#### 选择/不选

+ Ctrl + 鼠标单击

#### 连续选择

+ Shift + 鼠标单击

#### 框选

+ 选择模式: 鼠标拖放
+ 绘制模式: Ctrl + 鼠标拖放

#### 音符组

+ 鼠标双击空白处切换到主音符组
+ 鼠标双击音符切换到音符所在音符组

#### 用于输入的命令选项板

+ Enter 确认
+ Esc 取消

#### 播放头

+ 鼠标单击标尺区域空白处即可调整播放头位置

#### 修改工程属性

+ 修改拍号：鼠标单击标尺区域拍号
+ 修改曲速：鼠标单击标尺区域曲速
+ 修改声库：鼠标单击钢琴卷帘下方音轨头部标记
+ 修改语种：鼠标右键单击钢琴卷帘下方音轨头部标记

## 更新日志

+ 2022.07.03
  + 修复了一些问题

+ 2022.07.02
  + 完成波形绘制、并行播放
  + 添加部分菜单功能

+ 2022.06.30
  + 初步完成C#服务端

+ 2022.06.28
  + 修复了一些问题

+ 2022.06.26
  + 音符组变更
  + 拍号、曲速变更
  + Midi、UST文件导入

+ 2022.06.25
  + 音符创建、删除
  + 复制、粘贴
  + 修改歌词

+ 2022.06.24
  + 撤销、重做
  + 音符移动、拉伸

+ 2022.06.23
  + 打开工程、音符移动

+ 2022.06.20
  + 初始化翻译

+ 2022.06.18
  + 初步完成钢琴卷帘

+ 2022.06.17
  + 打开文件

+ 2022.06.15
  + Initial Commit
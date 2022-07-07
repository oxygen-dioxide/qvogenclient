# QVogenClient for Windows

---

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

### 系统需求

+ [.Net 5 x64 Desktop Runtime](https://dotnet.microsoft.com/zh-cn/download/dotnet/5.0/runtime)

### 注意事项

+ 使用客户端前，必须先打开服务端`resources/synth/CSRenderHost.exe`（或使用`CSRenderHost.bat`脚本启动），确保使用本地`14251`端口没有被占用。

+ 不宜打开超过一个客户端进程。
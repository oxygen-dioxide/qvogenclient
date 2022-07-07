# QVogenClient for Mac

---

## 构建信息

#### 版本
QVogenClient version 0.2.22


#### 平台
Mac OSX / x86_64

#### 构建

Clang_64 (Apple clang) 12.0.5

#### 共享库

+ Qt 5.15.2
+ QMidi
+ QuaZip + zlib
+ QUtauStd
+ wave

## 使用的资源

#### 运行时数据

````
$TMPDIR/QVogenTemp
````

卸载时请删除本目录。

#### 应用数据
````
~/.config/QVogenClient
````

卸载时请删除本目录。

## 本地服务端

### 系统需求

+ [.Net 6 Runtime](https://docs.microsoft.com/zh-cn/dotnet/core/install/macos)

### 注意事项

+ 使用客户端前，必须先打开服务端`resources/server/CSRenderHost`（或使用`CSRenderHost.sh`脚本启动），当前会话需要`DOTNET_ROOT`环境变量（一般是`~/.dotnet`），确保使用本地`14251`端口没有被占用。

+ 不宜打开超过一个客户端进程。
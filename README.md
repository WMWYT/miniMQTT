# miniMQTT

## 介绍

一个小型的支持mqtt V3.1.1版本的服务器

## 编译

``` bash
git clone https://github.com/WMWYT/miniMQTT.git
cd miniMQTT
mkdir build
cd build
cmake ..
make
```

## 使用

在build目录下添加miniMQTT的配置文件config.ini:

``` ini
[info]
port = 1883;
[system]
dir = ./control/libsystemextend.so
[login]
anonymously = 0;
control_type = file;
[control]
dir = ./control/libfileextend.so;
```

设置之后就可以在build目录下启动miniMQTT

```bash
./miniMQTT
```

## 客户端链接

只要支持MQTT V3.1.1版本的客户端都可以链接此服务器。

## 结束

本仓库已停止开发，项目更名dauntless，并将项目目录结构优化迁移至新仓库https://github.com/WMWYT/dauntless.git

# NetToolbox

简体中文 | [English](README.md)

易大师网络工具箱

![capture.jpg](capture.jpg)

## 描述

易大师网络工具箱是一个开源小工具集合，集成了网络方面众多小功能，可以极大提高您的工作效率

源码结构讲解：[【易大师网络工具箱】项目解析](https://www.fawdlstty.com/archives/608.html)

所用界面库（MIT开源）：[DuiLib_Faw](https://github.com/fawdlstty/DuiLib_Faw)

> 本地网络

1. 本机信息：展示本机各类系统信息，包括配置信息与CPU ID、主板ID等信息
2. 本机网络：展示本机当前所有网络信息
3. 网络连接：展示本机当前有哪些进程访问网络，列出连接端口地址，右键菜单打开进程位置或结束进程

> 网络工具

1. 路由跟踪：跟踪从当前主机到目标地址所经过的链接数
2. HTTP请求：模拟实现HTTP(s)的GET/POST请求
3. QPS测试：测试指定Web服务所能承受的最大HTTP连接数

> 格式工具

1. 正则表达式：用于测试验证正则表达式执行效果
2. RSA生成：用于生成RSA公私钥对
3. 转码解码：支持4种方式的转码及解码操作

> 单机工具

1. GIF录制：用于录制GIF图片
2. 串口调试工具：用于调试串口
3. 窗口工具：用于屏幕取色、窗口信息获取
4. 文件工具：用于计算文件哈希值

## 编译步骤

1. 安装VS2017、vcpkg、7-zip与upx并设置环境变量，直到这几个命令能在命令行里直接使用
2. 执行命令：vcpkg install boost:x86-windows boost:x86-windows-static openssl:x86-windows openssl:x86-windows-static nlohmann-json:x86-windows nlohmann-json:x86-windows-static curl:x86-windows curl:x86-windows-static
3. 编译

## 许可证

第三方库 DuiLib_Faw 与 serial 按原项目许可协议进行许可

其他未明确标注的均为 GPL-3.0 许可协议进行许可

## 赞助

![poor.jpg](poor.jpg)

## 其他

技术交流：[点击链接加入群【网络工具箱交流群 943497747】](https://jq.qq.com/?_wv=1027&k=5vG1SGq)

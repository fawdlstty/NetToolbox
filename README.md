# NetToolbox

English | [中文](README-cn.md)

Net Toolbox for Master Yi

![capture.jpg](capture.jpg)

## Description

NetToolbox is a opensource mini tool collection, it's contains some tool for network, it's may improve your productivity.

Source structure explanation: [【易大师网络工具箱】项目解析](https://www.fawdlstty.com/archives/608.html)

UI Library（MIT License）: [DuiLib_Faw](https://github.com/fawdlstty/DuiLib_Faw)

> Local Net

1. Localhost Info: Display various system information of the machine, including configuration information, CPU ID, motherboard ID and other information
2. Localhost Net: Displays all network information on the machine
3. Net Connect: Shows which processes are currently accessing the network on the machine, lists the port address of the connection, right click to open the process location or end the process

> Net Tool

1. Tracert: Keeps track of the number of links from the current host to the destination address
2. HTTP Request: Simulate GET/POST requests that implement HTTP(s)
3. QPS Test: Test specifies the maximum number of HTTP connections the Web service can withstand

> Format Tool

1. Regex Tool: Used to test and validate regular expression execution
2. RSA Generate: Used to generate RSA public-private key pairs
3. Encode Decode: Supports 4 modes of transcoding and decoding operations

> Standalone Tool

1. GIF Record: Used to record GIF images
2. Serial Port: Used for debugging serial ports
3. Window Tool: Used for screen color, window information acquisition
4. File Tool: Used to calculate the file hash value

## Compile Steps

1. Install VS2017/vcpkg/7-zip/upx and set environment variables, Until these commands can be used directly from the command line
2. run: vcpkg install boost:x86-windows boost:x86-windows-static openssl:x86-windows openssl:x86-windows-static nlohmann-json:x86-windows nlohmann-json:x86-windows-static curl:x86-windows curl:x86-windows-static
3. compile

## Update Log

2019.03.29 Community 替换DuiLib_Faw实现

2018.12.18 Community 修复Combo控件显示不正常的bug；修复分析文件时偶发性崩溃

2018.12.09 Community 开源；修改描述信息

2018.11.29 Community 优化在启动时阻塞请求网络的问题；优化网页首页展示；新增Base64编码解码；完成http、qps两个页面的参数编辑优化

2018.11.26 Community 修复更新失败的问题；新增部分常用正则；修复升级正则所造成的路由跟踪bug

2018.11.24 Community 修复保存图片的bug；主机信息页面新增硬件信息展示；列表页优化；修复win7下崩溃的bug；修复本地连接为空时切换标签页无效的bug；修复存储图片无效的bug；优化文件存储失败提示

2018.11.21 Community 重构界面文件；新增取色指示框；文件覆盖提示；进程提权提示

2018.11.14 beta 修复网络连接列表在切换类型的地方没有及时更新的bug；主页内存数据实时更新

2018.11.14 alpha 本地连接列表更新优化；完成右键菜单的结束进程、打开文件所在位置功能

2018.11.13 alpha 修复颜色萃取器的位置缩放bug；网络连接拆开三种不同的连接方式；修复更新没有验证md5的bug

2018.11.12 alpha 涉及缩放功能的bug的修复；gif录屏窗口保持最前；新增颜色萃取选择框

2018.11.11 alpha 新增转码解码功能

2018.11.08 beta 优化gif录屏框的显示；修复路由跟踪时显示bug

2018.11.06 alpha gif录像完成鼠标绘制

2018.11.05 beta 修复列表bug；修复更新bug；优化服务器运行统计

2018.11.04 beta 修复gif窗口显示bug

2018.11.04 alpha 修复路由跟踪时可多次点击的bug；新增屏幕gif录制功能

2018.11.03 alpha 修复http下post提交没有指定data的bug

2018.11.02 beta 修复RichEdit无法复制bug;网络连接新增排序功能；关于页优化

## License

GPL-2.0

## Donate

![poor.jpg](poor.jpg)

## Other

technical exchange: [Click the link to join the group【网络工具箱交流群 943497747】](https://jq.qq.com/?_wv=1027&k=5vG1SGq)

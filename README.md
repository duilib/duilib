# Duilib

Duilib是一个Windows下免费开源的DirectUI界面库，由于简约易扩展的设计以及稳定高效的实现被各大互联网公司普遍接受，广泛应用于包括IM、视频客户端、股票行情软件、导航软件、手机辅助软件、安全软件等多个行业的众多pc客户端软件。Duilib还在不断的发展中，在文档、例子、动画、渲染引擎等多个方面将持续改进。

欢迎为Duilib提供功能改进、代码修补、bug反馈、使用文档和献计献策，让我们一起把Duilib做的更好！

# 快速安装

您可以使用[vcpkg](https://github.com/Microsoft/vcpkg)库管理器下载并安装duilib:

    git clone https://github.com/Microsoft/vcpkg.git
    cd vcpkg
    ./bootstrap-vcpkg.sh
    ./vcpkg integrate install
    ./vcpkg install duilib

vcpkg中的duilib库由Microsoft团队成员和社区贡献者保持最新状态。如果版本过时，请在vcpkg存储库上[创建问题或请求请求](https://github.com/Microsoft/vcpkg)。

# 基于Duilib的软件不完全列表
Duilib自2010年8月21日发布以来，获得了众多开发者和大公司的信赖，在各行各业的软件中得到了广泛应用，以下是一个作者知道不完全列表，欢迎大家补充指正
https://github.com/duilib/duilib/wiki/%E5%9F%BA%E4%BA%8EDuilib%E7%9A%84%E8%BD%AF%E4%BB%B6%E4%B8%8D%E5%AE%8C%E5%85%A8%E5%88%97%E8%A1%A8

下面两个修改版本大家可以参考一下

# 腾讯使用的的duilib修改版
https://github.com/tencentyun/TIMSDK/tree/master/cross-platform/Windows/IMApp/Basic/duilib

# 网易使用的duilib修改版
https://github.com/netease-im/NIM_Duilib_Framework/tree/master/duilib

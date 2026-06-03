# VirtualListDemo

基于 duilib 自实现的虚表（VirtualList）控件演示。

## 控件能力

- **数据 / UI 解耦**：数据通过 `IVirtualListProvider` 提供（`CreateElement` / `FillElement` / `GetElementCount`），列控件复用——10 万行只创建约"可见行 + 1"个真实 `CControlUI`。
- **表头列宽拖动**：复用 `CListHeaderUI` / `CListHeaderItemUI`，拖动分隔符时下方各行同步重排。
- **滚动条按需自动显示/隐藏**：水平 / 垂直方向都走 `CContainerUI::ProcessScrollBar`，内容超过视口才出现。
- **任意视口高度**：池大小按 `ceil(viewH / itemH) + 1` 计算，视口高度不是 `itemHeight` 的整数倍时最后一行会被正确裁剪显示，不会留白也不会越界。
- **选中态跨滚动持久化**：`CVirtualListUI::GetCurSelDataIndex()` 保留的是数据下标，控件池循环复用时按数据下标重新 mark / unmark。

## 工程文件

- `DuiLib/Control/UIVirtualList.h` / `UIVirtualList.cpp`：控件实现
- `VirtualListDemo/Main.cpp`：使用示例
- `VirtualListDemo/VirtualListDemo.vcxproj`：VS 工程
- `VirtualListDemo/CMakeLists.txt`：CMake 工程
- `bin/skin/VirtualListRes/skin.xml`：UI 描述

集成到主仓的修改：

- `DuiLib/UIlib.h` 增加 `#include "Control/UIVirtualList.h"`
- `DuiLib/Core/UIDlgBuilder.cpp` 增加 `VirtualList` 类工厂分支
- `DuiLib/DuiLib.vcxproj` / `DuiLib_Static.vcxproj` / `DuiLib.vcxproj.filters` 添加新文件
- `CMakeLists.txt` 主入口增加 `add_subdirectory(VirtualListDemo)`
- `DuiLib.sln` 增加 `VirtualListDemo` 项目

## 编译

### Visual Studio

打开 `DuiLib.sln`，先编译 `DuiLib` 项目，再编译 `VirtualListDemo`。可执行文件输出到 `bin/`。

> 配置 `Debug / Release` 输出 `VirtualListDemo_d.exe / VirtualListDemo.exe`，
> 配置 `UnicodeDebug / UnicodeRelease` 输出 `VirtualListDemo_ud.exe / VirtualListDemo_u.exe`。

### CMake

```bash
cmake -S . -B build -DDUILIB_BUILD_EXAMPLES=ON
cmake --build build --config Release
```

## 运行 & 自测要点

启动后会自动加载 10 万行数据。验证项：

| 自测项 | 操作 | 预期 |
| --- | --- | --- |
| 海量数据流畅性 | 滚动条上下拖动到任意位置 | 滚动顺滑、不卡顿；任意位置数据正确 |
| 虚拟化生效 | 调试器查看 `bigtable->GetList()->GetCount()` | 始终保持在十几个，绝不等于 100000 |
| 滚动条自动显隐 | 点 `清空` / 点 `加载50行(测试边界)` / 拉伸窗口让数据少于一屏 | 垂直滚动条按需出现/消失 |
| 水平滚动条自动显隐 | 缩窄窗口宽度 / 拖动列变宽超出视口 | 水平滚动条按需出现，且滚动时表头与表体同步移动 |
| 表头拖动同步 | 鼠标拖动列分隔符调整列宽 | 下方所有行的同列实时跟随 |
| 视口高度非整数倍 | 缩放窗口让 ListBody 高度不是 34 的整数倍（如显示 7.7 行） | 最后一行被正确裁剪；上下滚动到底部时正常显示，无空白闪烁 |
| 跳转 | 点 `跳到第一行` / `跳到88888行` | 滚动条立即定位、对应行高亮 |
| 选中持久 | 选中第 50 行后滚动到很远再返回 | 第 50 行仍处于选中态 |
| 行点击通知 | 点击任意行 | 顶部信息栏 `选中: <行号>` 实时更新 |

## 核心实现

虚表的关键是**控件池**和**位置计算**：

```cpp
int iTopIndex  = scrollPos / itemHeight;            // 视口顶部对应的数据下标
int yTop       = rcView.top + iDataIndex * itemH    // 该控件的虚拟 y
                 - scrollPos;                        // 减去滚动后的屏幕 y
```

池中每个控件 i 对应数据下标 `iTopIndex + i`。滚动时 y 值变了但池中控件不变，
只需触发一次 `DoArrange(false)` 重新摆放并填数据，复杂度 O(可见行)。

详见 `DuiLib/Control/UIVirtualList.cpp` 的 `CVirtualListBodyUI::DoArrange` 实现。

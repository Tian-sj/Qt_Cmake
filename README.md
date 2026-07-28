# Qt / C++ 跨平台工程模板

面向 Windows、macOS 和 Linux 的 C++20 工程模板，同一套构建基础支持四种交付物：

| 交付物 | CMake 形式 | 示例 |
|---|---|---|
| 提供给其他项目使用的功能模块 | `STATIC` 或 `SHARED` library | `libraries/precision_countdown` |
| 基于 Qt 的可复用库 | `STATIC` 或 `SHARED` Qt library | `libraries/qt_status` |
| 被宿主运行时加载的插件 | `MODULE` library + C ABI | `plugins/example` |
| Qt 桌面应用 | Qt Widgets executable | `apps/desktop` |

所有模块使用 CMake、Ninja、CMake Presets、CTest 和同一套警告/分析器配置。

## 快速开始

需要 CMake 3.25+、Ninja、对应平台的 C++ 编译器。构建桌面应用还需要 Qt 6.5+。

```bash
cmake -P scripts/setup-toolchain.cmake
cmake --preset local-dev
cmake --build --preset local-dev
ctest --preset local-dev
```

环境脚本会生成已被 Git 忽略的 `CMakeUserPresets.json`。重新探测环境：

```bash
cmake -DFORCE=ON -P scripts/setup-toolchain.cmake
```

工具安装在非常规位置时可以显式指定：

```bash
cmake -DFORCE=ON \
  -DQT_ROOT=<Qt前缀> \
  -DNINJA_PATH=<Ninja路径> \
  -DTHIRD_PARTY_ROOT=<共享库路径> \
  -P scripts/setup-toolchain.cmake
```

所有 `-D` 参数必须位于 `-P` 前。

## 项目结构

```text
Qt_Cmake/
├── apps/
│   └── desktop/                   # Qt 桌面应用的唯一开发根目录
│       ├── application/           # QApplication 生命周期和界面设置
│       ├── views/                 # 窗口、对话框和 Designer UI
│       ├── widgets/               # 应用私有控件
│       ├── resources/             # 样式与 Qt Resource
│       ├── assets/                # 字体和应用图标
│       ├── translations/          # Qt 翻译
│       ├── integrations/          # 应用专属第三方 SDK 适配
│       └── main.cpp               # 依赖组装和程序入口
├── libraries/
│   ├── app_core/                  # 无 Qt 的应用核心、配置和端口
│   │   ├── include/               # 对外公开头文件
│   │   ├── src/                   # 私有实现
│   │   └── tests/                 # 该库自己的测试
│   ├── precision_countdown/       # 独立纯 C++ 功能模块示例
│   ├── qt_status/                 # 可安装、可独立测试的 Qt 库示例
│   └── qt_third_party/            # Qt 三方源码与本地二进制缓存
├── plugins/
│   ├── api/                       # 稳定的宿主/插件 C ABI
│   └── example/                   # MODULE 插件及动态加载测试
├── tests/
│   ├── consumer/                  # 安装后 find_package 消费测试
│   ├── qt_consumer/               # 安装后的 Qt 库消费测试
│   └── test_support.hpp           # 轻量测试辅助
├── cmake/                         # 公共构建、测试和包配置
├── scripts/                       # 跨平台工具链发现
└── vendor/                        # 必须随项目分发的专有 SDK
```

目录按“交付物”划分，不按 manager、repository、platform 等技术层级无限拆分。

## 构建模式

| Preset | 内容 |
|---|---|
| `local-dev` | Qt Debug 应用、所有库和库测试 |
| `local-release` | Qt Release 应用、所有库和库测试 |
| `local-qt-libraries` | 只构建可复用 Qt 库，不构建桌面应用 |
| `local-core-only` | 本机纯 C++ 库、插件和全部测试 |
| `core-only` | 通用纯 C++ 库、插件和全部测试 |
| `sanitizers` | ASan + UBSan 的库和插件测试 |

各 Preset 的产物相互隔离：

```text
build/<preset>/
├── bin/                           # 应用和测试外的运行文件
├── lib/                           # 库与插件
├── symbols/                       # 调试符号
└── tests/bin/                     # 测试程序
```

不用 Qt 时：

```bash
cmake --preset core-only
cmake --build --preset core-only
ctest --preset core-only
```

## 开发独立功能模块

一个可复用模块包含自己的公开头文件、实现、测试和 CMake：

```text
libraries/my_module/
├── include/cppproject/my_module/
├── src/
├── tests/
└── CMakeLists.txt
```

参考 `libraries/precision_countdown` 创建模块，然后在根 `CMakeLists.txt` 中添加：

```cmake
add_subdirectory(libraries/my_module)
```

模块应提供命名 target：

```cmake
add_library(cppproject_my_module)
add_library(CppProject::my_module ALIAS cppproject_my_module)
```

只编译和测试单个模块：

```bash
cmake --preset core-only
cmake --build --preset core-only --target cppproject_precision_countdown_tests
ctest --test-dir build/core-only -L precision_countdown --output-on-failure
```

默认生成静态库。需要动态库时使用标准 CMake 开关：

```bash
cmake --preset core-only -DBUILD_SHARED_LIBS=ON
```

动态 C++ 库要求消费者使用兼容的编译器、标准库和运行库。需要跨工具链稳定接口时，
优先使用下一节的 C ABI 插件形式。

## 开发 Qt 库

Qt 库仍放在 `libraries/`，与 Qt 桌面应用分开。参考 `libraries/qt_status`：

```text
libraries/qt_status/
├── include/cppproject/qt_status/  # 会安装的 QObject API
├── src/
├── tests/
└── CMakeLists.txt
```

只构建和测试 Qt 库：

```bash
cmake --preset local-qt-libraries
cmake --build --preset local-qt-libraries
ctest --test-dir build/local-qt-libraries -L qt_status --output-on-failure
```

该示例支持 `QObject`、属性、signals/slots 和 `AUTOMOC`，并导出
`CppProject::qt_status`。公开头文件使用 Qt 类型，所以 `Qt6::Core` 是合理的公开依赖；
不使用 Qt 的业务逻辑仍应放在普通 C++ library 中。

### QCustomPlot、QXlsx 和 QWindowKit

仓库固定包含 QCustomPlot 2.1.1、QXlsx 1.5.1.1 和 QWindowKit 1.5.0 源码，
并提供稳定 target：

```cmake
target_link_libraries(
    my_qt_library
    PRIVATE
        CppProject::qcustomplot
        CppProject::qxlsx
        CppProject::qwindowkit
)
```

第一次按源码构建：

```bash
cmake --preset local-qt-libraries \
  -DCPPPROJECT_BUILD_QT_THIRD_PARTY_FROM_SOURCE=ON
cmake --build --preset local-qt-libraries
```

生成的库不会提交到 Git，而是保存在各自源码目录的 `prebuilt/` 中：

```text
vendor/<库>/prebuilt/
└── <系统-架构>/<编译器-版本>/<Qt版本>/<static|shared>/<配置>/
    ├── lib/
    ├── bin/                        # Windows 动态库
    └── symbols/
```

以后关闭源码编译，CMake 会从相同 ABI 路径查找库：

```bash
cmake --preset local-qt-libraries \
  -DCPPPROJECT_BUILD_QT_THIRD_PARTY_FROM_SOURCE=OFF
cmake --build --preset local-qt-libraries
```

Debug、Release、静态和动态模式必须分别预编译一次。修改第三方源码、编译器版本或
Qt 版本后，也需要重新生成对应缓存。普通的增量构建本身不会重复编译未修改源码。

QXlsx 使用 MIT 许可证，QWindowKit 使用 Apache-2.0。QCustomPlot 2.1.1 使用
GPLv3 或商业许可证；闭源分发前需要取得合适的商业授权。其官方稳定版标注支持到
Qt 6.4，在更新的 Qt 上可能出现弃用警告。

## 安装并提供给其他项目

库、公开头文件和 CMake Package 会一起安装：

```bash
cmake --preset release -DCPPPROJECT_BUILD_GUI=OFF
cmake --build --preset release
cmake --install build/release --prefix <安装目录>
```

其他项目可以直接消费：

```cmake
find_package(CppProjectTemplate CONFIG REQUIRED)

target_link_libraries(
    my_application
    PRIVATE
        CppProject::app_core
        CppProject::precision_countdown
        CppProject::plugin_api
        CppProject::qt_status # 仅安装了 Qt 库的构建中存在
)
```

`tests/consumer` 是一个真正独立的项目，用于验证安装后的头文件、库和
`find_package` 配置，而不是偷偷依赖源码目录。

## 开发运行时插件

插件使用 `MODULE` library，不要求宿主在链接阶段依赖插件本体：

```bash
cmake --preset core-only
cmake --build --preset core-only --target cppproject_example_plugin
ctest --test-dir build/core-only -R example_plugin --output-on-failure
```

公共协议位于 `plugins/api/include/cppproject/plugin/plugin_api.h`，提供：

- 固定宽度整数 ABI 版本。
- `extern "C"` 插件入口。
- 宿主回调表。
- 插件初始化和关闭函数。
- Windows/macOS/Linux 导出定义。

插件边界不要暴露 STL、Qt 类型或 C++ 异常。对象的创建与销毁应由同一动态库完成。
示例测试会实际使用 `LoadLibrary` 或 `dlopen` 加载插件、查找入口并验证 ABI 协商。

新增插件时复制 `plugins/example`，修改插件名称和实现即可。

## 开发 Qt 桌面应用

Qt 应用继续保持简单的前后端依赖：

```text
CppProject::desktop_ui  ──>  CppProject::app_core
        │
        └── apps/desktop/main.cpp 创建对象并启动应用
```

- 主窗口通过 `QWK::WidgetWindowAgent` 使用 QWindowKit 的跨平台无边框窗口实现。
- 非 UI、可测试的逻辑放入 `libraries/app_core` 或新的独立 library。
- 窗口、对话框、控件、主题和翻译放入 `apps/desktop`。
- `main.cpp` 只创建对象和连接依赖。
- 桌面 UI 不直接访问配置文件、数据库或第三方 SDK。

macOS 应用位于 `build/<preset>/bin/<应用名>.app`。Windows 会在构建后调用
`windeployqt` 部署所需的 Qt DLL 和插件。

## 常用选项

| 选项 | 默认值 | 作用 |
|---|---:|---|
| `BUILD_SHARED_LIBS` | `OFF` | 将可复用库构建为动态库 |
| `CPPPROJECT_BUILD_GUI` | `ON` | 构建 Qt 桌面应用 |
| `CPPPROJECT_BUILD_QT_LIBRARIES` | 跟随 GUI | 构建可复用 Qt 库 |
| `CPPPROJECT_ENABLE_QT_THIRD_PARTY` | 跟随 Qt 库 | 提供 QCustomPlot、QXlsx 和 QWindowKit target |
| `CPPPROJECT_BUILD_QT_THIRD_PARTY_FROM_SOURCE` | `ON` | 否则从每个库的本地二进制缓存加载 |
| `CPPPROJECT_BUILD_PLUGINS` | `OFF` | 构建示例运行时插件 |
| `CPPPROJECT_BUILD_TESTS` | 顶层工程时 `ON` | 构建模块和插件测试 |
| `CPPPROJECT_ENABLE_LICENSING` | `OFF` | 启用桌面应用注册码适配 |
| `CPPPROJECT_WARNINGS_AS_ERRORS` | `OFF` | 将警告视为错误 |
| `CPPPROJECT_ENABLE_CLANG_TIDY` | `OFF` | 编译时运行 clang-tidy |
| `CPPPROJECT_ENABLE_SANITIZERS` | `OFF` | 启用 ASan 和 UBSan |

## 第三方依赖和注册码

`vendor/` 保存必须随项目分发的专用 SDK。共享通用依赖可以放在仓库同级的
`../third_party`，由环境脚本写入本机 Preset。

注册码适配属于桌面应用，而不是公开 `app_core` library。Windows 和 macOS 可以启用：

```bash
cmake --preset local-dev -DCPPPROJECT_ENABLE_LICENSING=ON
cmake --build --preset local-dev
```

Linux 未附带对应 SDK，应保持关闭或替换 `apps/desktop/integrations` 中的实现。

## 版本、CI 与模板定制

版本从最近的 `vMAJOR.MINOR.PATCH` Git Tag 解析：

```bash
git tag v1.2.0
```

没有 Git 元数据或合法 Tag 时回退到 `1.0.0`。CI 会在 Windows、macOS 和 Linux
验证纯 C++ 库、Qt 库、独立模块、动态插件、外部消费者以及 Qt GUI。

开始新项目时：

1. 修改根 `CMakeLists.txt` 中的项目名和应用元数据。
2. 保留需要的 `apps/`、`libraries/`、`plugins/`，删除不用的示例。
3. 将 `CppProject`、`cppproject` 命名空间和 include 前缀替换为产品名称。
4. 替换应用图标、字体和可选专有 SDK。

架构取舍见 [docs/architecture.md](docs/architecture.md)，提交规范见
[CONTRIBUTING.md](CONTRIBUTING.md)。

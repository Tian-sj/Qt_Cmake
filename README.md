# Qt / C++ 跨平台工程模板

这是一个适合个人或 1～3 人团队的 C++20 工程模板。它不要求所有项目采用同一种
形态，而是让一个仓库按需开发：

- 独立的纯 C++ 功能模块。
- 提供给其他项目使用的 Qt 库。
- 可由宿主动态加载的 C ABI 插件。
- Qt Widgets 桌面程序。
- Qt Quick/QML 桌面程序。

Widgets 和 QML 是两个独立前端，共享无 Qt 的 `app_core`。不需要的部分可以直接
关闭，不必为简单项目保留一整套复杂架构。

## 1. 第一次使用

### 准备环境

需要：

- CMake 3.25 或更高版本。
- Ninja。
- 支持 C++20 的编译器：Windows 使用 MSVC 2022，Linux 使用 GCC/Clang，
  macOS 使用 AppleClang。
- 开发 Qt 库或桌面程序时安装 Qt 6.5 或更高版本，建议 Qt 6.8 LTS。

克隆仓库后先探测本机工具：

```bash
cmake -P scripts/setup-toolchain.cmake
```

脚本会生成不提交到 Git 的 `CMakeUserPresets.json`。Qt 或 Ninja 不在常规位置时：

```bash
cmake -DFORCE=ON -DQT_ROOT=<Qt安装目录> -DNINJA_PATH=<Ninja路径> -P scripts/setup-toolchain.cmake
```

所有 `-D` 参数必须写在 `-P` 前面。

### 选择要开发的项目

| 你要开发什么 | 使用的 Preset |
|---|---|
| Qt Widgets 程序 | `local-dev` |
| Qt Quick/QML 程序 | `local-qml-dev` |
| 只开发 Qt 库 | `local-qt-libraries` |
| 纯 C++ 库或插件，不使用 Qt | `local-core-only` |

例如开发 Widgets：

```bash
cmake --preset local-dev
cmake --build --preset local-dev
ctest --preset local-dev
```

开发 QML：

```bash
cmake --preset local-qml-dev
cmake --build --preset local-qml-dev
ctest --preset local-qml-dev
```

日常修改代码后通常只需要重复 `cmake --build` 和 `ctest`。新增文件、修改 CMake
选项或切换 Qt 后，再执行一次 `cmake --preset`。

## 2. 先理解目录

```text
Qt_Cmake/
├── apps/
│   ├── desktop_widgets/           # Qt Widgets 可执行程序
│   └── desktop_qml/               # Qt Quick/QML 可执行程序
├── libraries/
│   ├── app_core/                  # 两个桌面前端共享的纯 C++ 应用逻辑
│   ├── precision_countdown/       # 独立纯 C++ 模块示例
│   ├── qt_status/                 # 可复用 Qt 库示例
│   └── qt_third_party/            # Qt 三方源码和本机 ABI 缓存
├── plugins/
│   ├── api/                       # 稳定的宿主/插件 C ABI
│   └── example/                   # 动态加载插件示例
├── tests/
│   ├── consumer/                  # 安装后的纯 C++ 消费测试
│   └── qt_consumer/               # 安装后的 Qt 库消费测试
├── cmake/                         # 公共 CMake 配置
├── scripts/                       # 本机工具链探测
└── vendor/                        # 必须随产品分发的专有 SDK
```

判断代码放在哪里，只问一个问题：它最终交付成什么？

| 代码性质 | 放置位置 |
|---|---|
| 与界面无关、可能被复用和单测 | `libraries/<模块名>` |
| 两个桌面前端共享的应用逻辑 | `libraries/app_core` |
| QWidget、Designer UI、Widgets 交互 | `apps/desktop_widgets` |
| QML 页面、组件、Quick 交互 | `apps/desktop_qml` |
| 对外提供的 Qt API | `libraries/<Qt库名>` |
| 运行时加载的插件实现 | `plugins/<插件名>` |
| 产品专属 SDK | `vendor/` 或应用的 `integrations/` |

不要因为文件变多就增加 manager、repository、factory 等目录。只有代码真的需要独立
复用和独立测试时，才提取成 library。

## 3. 开发独立 C++ 模块

参考 `libraries/precision_countdown`：

```text
libraries/my_module/
├── include/cppproject/my_module/  # 对外公开 API
├── src/                           # 私有实现
├── tests/                         # 模块自己的测试
└── CMakeLists.txt
```

模块提供一个有命名空间的 CMake target：

```cmake
add_library(cppproject_my_module)
add_library(CppProject::my_module ALIAS cppproject_my_module)
```

然后在根 `CMakeLists.txt` 添加：

```cmake
add_subdirectory(libraries/my_module)
```

只构建并测试这个模块：

```bash
cmake --preset local-core-only
cmake --build --preset local-core-only --target cppproject_my_module_tests
ctest --test-dir build/local-core-only -L my_module --output-on-failure
```

默认生成静态库。需要动态库时：

```bash
cmake --preset local-core-only -DBUILD_SHARED_LIBS=ON
```

## 4. 开发 Qt 库

Qt 库仍放在 `libraries/`，不要放入某个桌面应用。参考 `libraries/qt_status`：

```text
libraries/my_qt_library/
├── include/cppproject/my_qt_library/
├── src/
├── tests/
└── CMakeLists.txt
```

只开发、测试 Qt 库：

```bash
cmake --preset local-qt-libraries
cmake --build --preset local-qt-libraries
ctest --preset local-qt-libraries
```

公开 API 使用 `QObject`、`QString` 等 Qt 类型时公开链接相应 Qt target；Qt 只在
实现内部使用时保持私有链接。每个 Qt 库自己启用 `AUTOMOC`，不要在全仓库全局开启。

## 5. 开发 Widgets 或 QML 应用

两个前端的关系是：

```text
apps/desktop_widgets ──┐
                      ├──> CppProject::app_core
apps/desktop_qml ──────┘
```

- `desktop_widgets` 负责 QWidget、Designer UI、主题和 Widgets 版 QWindowKit。
- `desktop_qml` 负责 QML 页面、Quick Controls 和 Quick 版 QWindowKit。
- 业务规则、配置读写接口等非 UI 逻辑进入 `app_core` 或新的独立 library。
- 两个前端不互相包含源码，可以单独构建，也可以同时开启。
- `main.cpp` 只负责创建对象和组装依赖。

只运行 QML 冒烟测试：

```bash
ctest --test-dir build/local-qml-dev -L qml --output-on-failure
```

运行 QML 静态检查：

```bash
cmake --build build/local-qml-dev --target cppproject_desktop_qml_qmllint
```

同时构建两个前端：

```bash
cmake --preset local-dev -DCPPPROJECT_BUILD_WIDGETS_APP=ON -DCPPPROJECT_BUILD_QML_APP=ON
cmake --build --preset local-dev
```

## 6. 使用 QCustomPlot、QXlsx 和 QWindowKit

仓库固定包含 QCustomPlot 2.1.1、QXlsx 1.5.1.1 和 QWindowKit 1.5.0 源码。
按需要链接，不要让普通业务库无条件依赖全部三方库：

```cmake
target_link_libraries(my_chart PRIVATE CppProject::qcustomplot)
target_link_libraries(my_excel_export PRIVATE CppProject::qxlsx)
target_link_libraries(my_widgets_window PRIVATE CppProject::qwindowkit_widgets)
target_link_libraries(my_qml_window PRIVATE CppProject::qwindowkit_quick)
```

第一次在某台机器、某种构建配置下使用时从源码编译：

```bash
cmake --preset local-qml-dev -DCPPPROJECT_BUILD_QT_THIRD_PARTY_FROM_SOURCE=ON
cmake --build --preset local-qml-dev
```

产物写入各依赖自己的：

```text
prebuilt/
└── <系统-架构>/<编译器-版本>/<Qt版本>/<static|shared>/<Debug|Release>/
    ├── include/
    ├── lib/
    ├── bin/                       # Windows DLL
    └── symbols/
```

以后可以关闭源码编译，直接读取匹配的 ABI 缓存：

```bash
cmake --preset local-qml-dev -DCPPPROJECT_BUILD_QT_THIRD_PARTY_FROM_SOURCE=OFF
cmake --build --preset local-qml-dev
```

缓存不能跨系统、架构、编译器、Qt 版本、静态/动态或 Debug/Release 混用。更换其中
任何一项，都先重新执行一次源码构建。

QXlsx 使用 MIT，QWindowKit 使用 Apache-2.0。QCustomPlot 2.1.1 使用 GPLv3 或商业
许可证，闭源分发前需要购买合适的商业授权。

## 7. 开发运行时插件

插件使用 `MODULE` library 和稳定 C ABI。参考 `plugins/example`：

```bash
cmake --preset local-core-only
cmake --build --preset local-core-only --target cppproject_example_plugin
ctest --test-dir build/local-core-only -R example_plugin --output-on-failure
```

插件边界只传递固定宽度整数、C struct、指针和函数表，不传递 STL、Qt 对象或 C++
异常。测试会在 Windows 使用 `LoadLibrary`，在 Linux/macOS 使用 `dlopen` 真实加载。

## 8. 安装并提供给其他项目

构建并安装库、公开头文件和 CMake Package：

```bash
cmake --preset release -DCPPPROJECT_BUILD_WIDGETS_APP=OFF
cmake --build --preset release
cmake --install build/release --prefix <安装目录>
```

外部项目通过标准 CMake Package 使用：

```cmake
find_package(CppProjectTemplate CONFIG REQUIRED)

target_link_libraries(
    my_application
    PRIVATE
        CppProject::app_core
        CppProject::precision_countdown
        CppProject::plugin_api
        CppProject::qt_status
)
```

`tests/consumer` 和 `tests/qt_consumer` 会从安装目录重新创建独立项目，防止公开头文件、
传递依赖或 `find_package` 配置遗漏。

## 9. Windows、Linux 和 macOS 验证

`.github/workflows/ci.yml` 会在真实的 GitHub 托管系统上运行：

| 系统 | 工具链 | 验证内容 |
|---|---|---|
| Windows Server 2022 | MSVC 2022 + Qt 6.8 LTS | C++、插件、Widgets、QML、Qt 库、安装消费 |
| Ubuntu 24.04 | GCC + Qt 6.8 LTS | C++、插件、Widgets、QML、Qt 库、安装消费 |
| macOS | AppleClang + Qt 6.8 LTS | C++、插件、Widgets、QML、Qt 库、安装消费 |

GUI 流程还会验证：

1. 第三方库从源码编译。
2. 关闭源码编译后，从当前平台 ABI 缓存重新配置和构建。
3. QML 冒烟测试与 `qmllint`。
4. 安装后的 Qt 库能被外部项目 `find_package`。

推送分支或创建 Pull Request 会自动触发。也可以在 GitHub 的 Actions 页面选择
`CI`，点击 `Run workflow` 手动运行。只有对应 Windows/Linux job 通过，才能说明
代码真正在该系统上编译和测试过；在 macOS 上交叉配置不能替代这一点。

## 10. 常用构建选项

| 选项 | 默认值 | 作用 |
|---|---:|---|
| `BUILD_SHARED_LIBS` | `OFF` | 将可复用库构建为动态库 |
| `CPPPROJECT_BUILD_WIDGETS_APP` | `ON` | 构建 Qt Widgets 应用 |
| `CPPPROJECT_BUILD_QML_APP` | `OFF` | 构建 Qt Quick/QML 应用 |
| `CPPPROJECT_BUILD_QT_LIBRARIES` | 跟随桌面应用 | 构建可复用 Qt 库 |
| `CPPPROJECT_ENABLE_QT_THIRD_PARTY` | 跟随 Qt 库 | 提供 Qt 三方 target |
| `CPPPROJECT_BUILD_QT_THIRD_PARTY_FROM_SOURCE` | `ON` | 否则读取本机 ABI 缓存 |
| `CPPPROJECT_BUILD_PLUGINS` | `OFF` | 构建示例运行时插件 |
| `CPPPROJECT_BUILD_TESTS` | 顶层工程时 `ON` | 构建自动化测试 |
| `CPPPROJECT_ENABLE_LICENSING` | `OFF` | 启用注册码 SDK 适配 |
| `CPPPROJECT_WARNINGS_AS_ERRORS` | `OFF` | 将编译警告视为错误 |
| `CPPPROJECT_ENABLE_CLANG_TIDY` | `OFF` | 编译时执行 clang-tidy |
| `CPPPROJECT_ENABLE_SANITIZERS` | `OFF` | 启用 ASan 和 UBSan |

需要严格检查时显式加入
`-DCPPPROJECT_WARNINGS_AS_ERRORS=ON`；日常 Preset 不默认开启，避免编译器版本变化
把新出现的警告误判成工程无法构建。

## 11. 创建新项目时修改什么

1. 修改根 `CMakeLists.txt` 的项目名、组织名、应用名和 Bundle ID。
2. 将 `CppProject`、`cppproject` 和公开 include 前缀替换成产品名称。
3. 保留需要的 `apps/`、`libraries/`、`plugins/`，删除不用的示例。
4. 替换图标、字体和可选的产品专属 SDK。
5. 首先保持 CI 全绿，再开始加入业务代码。

版本来自最近的 `vMAJOR.MINOR.PATCH` Git Tag：

```bash
git tag v1.2.0
```

更详细的边界说明见 [docs/architecture.md](docs/architecture.md)，提交规范见
[CONTRIBUTING.md](CONTRIBUTING.md)。

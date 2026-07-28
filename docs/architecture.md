# 工程边界

## 按交付物组织

模板不强制所有项目使用同一种架构。目录对应实际交付物：

| 目录 | 交付物 | 使用者 |
|---|---|---|
| `libraries/` | 纯 C++ 或 Qt 静态库/动态库 | 其他 CMake target 或外部项目 |
| `plugins/` | 运行时加载的 MODULE | 插件宿主 |
| `apps/` | 可执行程序和安装包 | 最终用户 |

只有代码确实被多个交付物共同使用时，才提取成新的 library。不要为了目录对称创建
空接口、manager、repository 或 factory。

## Library 边界

每个 library 自己拥有：

- `include/`：稳定公开 API。
- `src/`：私有实现。
- `tests/`：只验证该模块行为的测试。
- `CMakeLists.txt`：target、依赖、安装和测试声明。

公共头文件只能包含调用者真正需要的依赖。第三方 SDK、私有 include 路径、警告选项
和项目内部宏不能向消费者传播。

安装必须包含库、公开头文件和 CMake Package。`tests/consumer` 必须只通过安装目录
执行 `find_package`，用来发现缺失头文件、错误的传递依赖和不可重定位路径。

静态库适合源码团队内部和单一工具链。动态 C++ 库需要维护符号可见性、版本和 ABI，
并要求消费者使用兼容编译器与运行库。

## Qt Library 边界

Qt 库与普通 C++ 库使用相同的 `include/src/tests/CMakeLists.txt` 结构，但只在
`CPPPROJECT_BUILD_QT_LIBRARIES` 开启时参与构建：

```text
Qt application ──> Qt library ──> pure C++ library
external Qt app ──┘
```

- 公开 API 使用 `QObject`、`QString` 等类型时，公开链接 `Qt6::Core`。
- 仅实现内部使用 Qt 时，Qt target 应保持私有依赖。
- Qt 库可以依赖纯 C++ 库；纯 C++ 库不得反向依赖 Qt。
- 每个 Qt 库独立启用 `AUTOMOC`，不要全局开启自动生成。
- 安装包通过 `find_dependency(Qt6)` 向外部消费者恢复传递依赖。

QCustomPlot、QXlsx 和 QWindowKit 属于第三方 Qt 库，不进入任何业务 library 的源码
目录。源码模式把二进制写入各依赖自己的 `prebuilt/<ABI>/<配置>` 缓存；预编译模式
只创建 imported target。缓存键必须包含平台、架构、编译器、Qt 版本、静态/动态和
Debug/Release，不同键之间不得混用。Widgets 前端依赖
`CppProject::qwindowkit_widgets`，QML 前端依赖 `CppProject::qwindowkit_quick`，
纯 C++ library 不感知窗口框架。

## Plugin 边界

需要跨工具链或运行时发现实现时，使用 C ABI：

```text
host ── cppproject_plugin_get_api(abi_version) ──> plugin
     <──────── function table ───────────────
```

插件协议必须遵守：

- 入口使用 `extern "C"`。
- API 包含明确 ABI 版本。
- 边界只使用固定宽度整数、C struct、指针和函数指针。
- 不跨边界传递 STL、Qt 对象或异常。
- 分配和释放由同一个模块完成。
- 插件加载、符号查找、错误 ABI 和初始化必须有自动化测试。

如果宿主与插件严格使用同一个编译器、运行库和 Qt 版本，可以在 C ABI 之上约定更丰富
的 C++ 接口，但基础发现入口仍建议保持 C ABI。

## Qt 应用边界

Qt 桌面应用使用：

```text
apps/desktop_widgets ──┐
                      ├──> CppProject::app_core
apps/desktop_qml ──────┘
```

- `apps/desktop_widgets` 负责 QWidget、Designer UI、主题和 Widgets 用户交互。
- `apps/desktop_qml` 负责 Qt Quick 页面、QML 组件和 QML 用户交互。
- 两个前端是独立可执行程序，通过 CMake 选项单独启用，不互相包含源码。
- 非 UI 业务进入 `app_core` 或独立 library。
- 专属于应用的 SDK 适配放在 `apps/desktop_widgets/integrations`。
- 公共 library 不因桌面应用的私有 SDK 而引入 Qt。

## 并发与错误

- 后台线程不得直接操作 QWidget，应通过 queued signal/slot 返回 GUI 线程。
- 不在持锁状态执行外部回调。
- 回调连接必须有明确生命周期。
- 可恢复错误使用状态或结果返回；无法继续的启动错误传到入口统一处理。
- 不在 CMake 或源码中保存个人路径、密钥、令牌和服务器地址。

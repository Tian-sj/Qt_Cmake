# 架构与模块边界

## 依赖规则

依赖方向是本模板最重要的约束：

1. `application` 定义用例、数据结构和 ports，只依赖 C++ 标准库。
2. `platform` 实现 application ports，例如文件设置存储和平台目录解析。
3. `src/frontend/qt` 是完整的 Qt 表现层，只做控件、翻译、主题和用户交互，不直接访问文件、数据库或专有 SDK。
4. `src/backend/integrations/registration` 把专有注册码 SDK 的 `QString/QDateTime` 转成标准 C++ 类型。
5. `apps/desktop` 是 composition root，负责创建实现并注入 application/gui。
6. `precision_countdown` 是独立纯 C++ 模块，不依赖 application 或 Qt。

禁止出现以下依赖：

- application → platform/integrations/gui/Qt
- platform → gui
- GUI 直接读取数据库、配置文件或调用 third-party SDK
- 公共头文件暴露 private include 路径
- 用全局单例代替构造函数注入

## 目录职责

```text
apps/desktop/                              桌面程序入口与依赖组装
cmake/                                     可复用 CMake 模块和生成头模板
docs/                                      架构和开发文档
scripts/                                   环境、构建、部署辅助脚本
src/backend/application/                   纯 C++ 用例与抽象接口
src/backend/modules/precision_countdown/   独立纯 C++ 模块示例
src/backend/platform/                      文件、路径、时间等系统实现
src/backend/integrations/registration/     专有 SDK 防腐层
src/frontend/qt/application/               Qt 启动流程和界面偏好
src/frontend/qt/views/                     窗口的 .hpp/.cpp/.ui
src/frontend/qt/widgets/                   自定义 Qt 控件
src/frontend/qt/resources/                 qrc、样式和图片
src/frontend/qt/translations/              Qt 翻译源文件
src/frontend/qt/assets/                    字体和应用图标
third_party/registration_code/             第三方 SDK 头文件和二进制
tests/                                     按后端模块镜像组织的测试
```

## 新增业务模块

新增功能时按以下顺序处理：

1. 在 application 定义输入、输出和 port；不要引入 Qt 类型。
2. 为用例编写无 Qt 单元测试，使用 fake port 验证行为。
3. 在 platform 或独立 integration 中实现 port，并添加集成测试。
4. 在 `apps/desktop/main.cpp` 组装实现。
5. 最后在 GUI 中调用用例并显示结果。

独立、边界稳定的大功能可以使用 `src/backend/modules/<module>/include/qtcpp/<module>` 与 `src/backend/modules/<module>/src` 结构，并提供命名空间 target，例如 `QtCpp::<module>`。只有实际存在多个实现或测试替身需求时才创建接口，避免无意义的抽象层。

## 并发规则

- 后台工作不得直接操作 QWidget；通过 queued signal/slot 或 GUI 线程调度器回到主线程。
- 回调注册必须有明确的生命周期句柄。
- 不在持锁状态执行外部回调。
- application 的端口实现需明确线程安全承诺，默认不要假设线程安全。

`PrecisionCountdown` 的回调在内部工作线程执行，其 `Signal` 在发布前复制订阅者列表，因此不会持锁调用用户代码。

## 错误与配置

- 可恢复业务错误通过明确的状态/结果返回，例如 `ApplicationStatus` 和 `LicenseResult`。
- 无法继续的基础设施错误使用异常传到 composition root，由入口统一记录并退出。
- 业务配置由 application port 管理；字体、语言、主题属于 GUI 偏好，可留在 Qt 表现层。
- 不在 CMake 或源码中保存个人路径、密钥、令牌和服务器地址。

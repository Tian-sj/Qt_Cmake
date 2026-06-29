# 协作开发规范

## 本地检查

提交前运行：

```bash
cmake --preset local-dev
cmake --build --preset local-dev
ctest --preset local-dev
```

不修改 GUI 时仍应运行 `core-only`，它会在 `-Werror` 下验证纯 C++ 边界。改动 C++ 文件后使用仓库根目录的 `.clang-format` 格式化；需要更严格检查时启用 `QTCPP_ENABLE_CLANG_TIDY` 或 sanitizers 预设。

## 分支和评审

- 一个分支只解决一个主题，避免把格式化、重构和功能混在同一提交。
- Pull Request 必须说明行为变化、边界变化、测试证据和平台影响。
- 公共接口变更需要至少一名使用该接口的模块维护者评审。
- third-party 二进制更新必须说明来源、版本、支持平台和 ABI/Qt 版本。
- 禁止提交 `CMakeUserPresets.json`、build 目录、密钥和个人绝对路径。

## Definition of Done

- 新行为有自动化测试；缺陷修复包含能复现问题的测试。
- Windows MSVC、macOS 和 Linux CI 通过，三者都使用 Ninja。
- application/platform 公共接口没有 Qt 类型泄漏。
- 新增文本已更新英文/简体中文 TS 文件，文件编码为 UTF-8。
- CMake target 使用显式 source、target include 和 target link 声明。
- 文档与实际命令、选项和目录保持一致。

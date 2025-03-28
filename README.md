# 项目目录结构

```plaintext
FreqStation/
├── src/                      # 源代码目录
│   ├── gui/                  # GUI 相关代码
│   │   ├── main_window.h
│   │   ├── main_window.cpp
│   │   ├── resources.qrc     # Qt 资源文件
│   │   └── CMakeLists.txt
│   ├── database/             # 数据库相关代码
│   │   ├── db_manager.h
│   │   ├── db_manager.cpp
│   │   └── CMakeLists.txt
│   ├── communication/        # 通信模块
│   │   ├── network_manager.h
│   │   ├── network_manager.cpp
│   │   └── CMakeLists.txt
│   ├── core/                 # 核心逻辑（如业务规则）
│   │   ├── core_logic.h
│   │   ├── core_logic.cpp
│   │   └── CMakeLists.txt
│   ├── utils/                # 通用工具类
│   │   ├── xxx.h
│   │   ├── xxx.cpp
│   │   └── CMakeLists.txt
│   ├── RegistrationCode      # 注册码类
│   │   ├── xxx.h
│   │   ├── xxx.cpp
│   │   └── CMakeLists.txt
├── include/                  # 公共头文件
│   ├── common.h
│   └── config.h              # 配置文件
├── main/                     # 项目入口
│   ├── main.cpp
│   └── CMakeLists.txt
├── tests/                    # 测试代码
│   ├── gui_tests/
│   ├── db_tests/
│   ├── network_tests/
│   └── CMakeLists.txt
├── third_party/              # 第三方库
│   ├── Qt/                   # Qt 相关依赖
│   ├── sqlite/               # 数据库依赖
│   └── boost/                # Boost 库
├── scripts/                  # 构建或运行脚本
│   ├── build.sh
│   ├── run_tests.sh
│   └── deploy.sh
├── docs/                     # 项目文档
│   ├── design.md             # 设计文档
│   ├── api.md                # API 文档
│   └── README.md
├── build/                    # 构建输出目录（通常添加到 .gitignore）
├── CMakeLists.txt            # 项目根目录的 CMake 配置
├── LICENSE                   # 许可证文件
└── .gitignore                # Git 忽略规则
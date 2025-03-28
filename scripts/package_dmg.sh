#!/bin/bash

# ===== 检查参数 =====
if [ $# -ne 1 ]; then
    echo "❌ 用法：$0 /path/to/YourApp.app"
    exit 1
fi

# ===== 获取绝对路径、文件名、名字 =====
APP_PATH="$1"                                 # 完整路径
APP_FULLNAME=$(basename "$APP_PATH")          # e.g. CQ_RC.app
APP_NAME="${APP_FULLNAME%.app}"               # e.g. CQ_RC
APP_DIR=$(dirname "$APP_PATH")                # e.g. /Users/you/project/build/Release
VOLUME_NAME="${APP_NAME} Installer"
DMG_NAME="${APP_NAME}.dmg"
TMP_DIR="dmg_temp"
QT_PATH="/opt/Qt/6.8.2/macos/bin/"  # 修改成你自己的路径

# ===== 检查文件是否存在 =====
if [ ! -d "${APP_PATH}" ]; then
    echo "❌ 找不到 ${APP_PATH}"
    exit 1
fi

# ===== 执行 macdeployqt =====
echo "🚚 执行 macdeployqt..."
"${QT_PATH}/macdeployqt" "${APP_PATH}" -verbose=1

# ===== 清理旧文件 =====
echo "🧹 清理旧文件..."
rm -rf "${TMP_DIR}" "${DMG_NAME}"

# ===== 拷贝 .app 到临时目录 =====
echo "📦 拷贝 .app..."
mkdir -p "${TMP_DIR}"
cp -R "${APP_PATH}" "${TMP_DIR}/"

# ===== 生成 .dmg =====
echo "📀 打包为 ${DMG_NAME}..."
hdiutil create -volname "${VOLUME_NAME}" \
  -srcfolder "${TMP_DIR}" \
  -ov -format UDZO "${DMG_NAME}"

# ===== 清理临时目录 =====
rm -rf "${TMP_DIR}"
echo "✅ 打包完成：${DMG_NAME}"

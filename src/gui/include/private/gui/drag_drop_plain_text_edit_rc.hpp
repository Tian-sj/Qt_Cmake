#pragma once

#include <QPlainTextEdit>

/*!
 * @brief         拖拽支持的文本编辑控件（注册文件专用）
 *
 * Custom plain text editor supporting drag-and-drop for RC files
 *
 * @attention     该控件继承自 QPlainTextEdit，可接收拖拽的文件并加载其内容，仅接受合法文件。
 */
class DragDropPlainTextEditRC : public QPlainTextEdit {
    Q_OBJECT

public:
    /*!
     * @brief         构造函数
     *
     * @param         parent   父级 QWidget 指针（默认 nullptr）
     */
    DragDropPlainTextEditRC(QWidget *parent = nullptr);

protected:
    /*!
     * @brief         拖入事件处理
     *
     * 处理拖入事件，判断拖入数据是否可接受
     *
     * @param         event   拖拽进入事件对象
     */
    void dragEnterEvent(QDragEnterEvent *event) override;

    /*!
     * @brief         拖放释放事件处理
     *
     * 拖放释放后，尝试读取文件内容
     *
     * @param         event   拖拽释放事件对象
     */
    void dropEvent(QDropEvent *event) override;

private:
    /*!
     * @brief         加载并显示文件内容
     *
     * 读取指定文件并设置到编辑器中
     *
     * @param         file_path   文件路径
     */
    void loadFile(const QString &file_path);

    /*!
     * @brief         判断文件是否有效
     *
     * 检查拖拽文件是否符合要求（如扩展名）
     *
     * @param         file_path   文件路径
     * @return        true 表示合法文件，false 表示拒绝处理
     */
    bool isValidFile(const QString &file_path);
};

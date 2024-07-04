#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QFocusEvent>
#include <QTranslator>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <memory>
#include <QTimer>
#include <app/mainwindow.h>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

    /*!
     * @brief         Loading Language
     * 
     * @param         language  0-English， 1-Chinese
     */
    void loadLanguage(int language);

    std::unique_ptr<MainWindow> get_windows();

protected:
    void changeEvent(QEvent *event) override;

private slots:
    /*!
     * @brief         Login button click event
     */
    void on_btn_login_clicked();

    /*!
     * @brief         User name input event
     * 
     * @param         arg1(unused)
     */
    void on_username_textEdited(const QString &arg1);

    /*!
     * @brief         Password input event
     * 
     * @param         arg1(unused)
     */
    void on_password_textEdited(const QString &arg1);

    void on_login_successful();

    void on_load_successful();

private:

    /*!
     * @brief         Initialize UI
     */
    void initUI();

    void initUIText();

    void initConnect();

    /*!
     * @brief         Loading CSS styles
     * 
     * @param         cssFile    CSS file path
     */
    void loadCSS(const QString &cssFile);

private:
    Ui::LoginDialog *ui;

    QTranslator* translator;

    // test
    QString m_username;
    int m_role;

    QPropertyAnimation *animation_left;
    QPropertyAnimation *animation_right;

    QPropertyAnimation *animation_label;
    QGraphicsOpacityEffect *effect_label;

    std::unique_ptr<MainWindow> m_windows;

    QTimer* create_timer;
};

#endif // LOGINDIALOG_H

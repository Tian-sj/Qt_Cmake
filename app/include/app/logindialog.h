#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QFocusEvent>
#include <QTranslator>

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

private:

    /*!
     * @brief         Initialize UI
     */
    void initUI();

    void initUIText();

    /*!
     * @brief         Loading CSS styles
     * 
     * @param         cssFile    CSS file path
     */
    void loadCSS(const QString &cssFile);

private:
    Ui::LoginDialog *ui;

    QTranslator* translator;
};

#endif // LOGINDIALOG_H

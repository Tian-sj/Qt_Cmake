#include <app/logindialog.h>
#include "ui_logindialog.h"

#include <QFile>
#include <QListView>
#include <QLineEdit>

/*
LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    translator(new QTranslator),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);

    initUI();
    initUIText();
}
*/

LoginDialog::~LoginDialog()
{
    delete translator;
    delete ui;
}

LoginDialog::LoginDialog(int& role, QWidget *parent) :
    m_role(role),
    QDialog(parent),
    translator(new QTranslator),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);

    initUI();
    initUIText();
}

void LoginDialog::loadLanguage(int language)
{
    switch (language)
    {
    case 0: {
        qDebug() << translator->load("login_en.qm", ":/languages");
        break;
    }
    case 1: {
        qDebug() << translator->load("login_cn.qm", ":/languages");
        break;
    }
    default:
        break;
    }
    qApp->installTranslator(translator);
    ui->retranslateUi(this);
}

void LoginDialog::changeEvent(QEvent *event)
{
    if (NULL != event) {
        switch (event->type()) {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            initUIText();
            break;
        default:
            break;
        }
    }
    QDialog::changeEvent(event);
}

void LoginDialog::initUI()
{
    // 设置为不边框模式
    this->setWindowFlags(Qt::FramelessWindowHint);

    this->setAttribute(Qt::WA_TranslucentBackground);

    ui->role->setView(new QListView());

    // 加载css样式
    loadCSS(":/css/LoginDialog.css");

    // 设置错误提示不可见
    ui->lab_error_username->setVisible(false);
    ui->lab_error_password->setVisible(false);
}

void LoginDialog::initUIText()
{
    ui->lab_username->setText(tr("Username"));
    ui->lab_error_username->setText(tr("Username does not exist"));
    ui->lab_password->setText(tr("Password"));
    ui->lab_error_password->setText(tr("Password error"));
    ui->lab_role->setText(tr("Role"));
    ui->role->clear();
    ui->role->addItem(tr("SuperAdministrator"));
    ui->role->addItem(tr("Administrator"));
    ui->role->addItem(tr("Ordinary user"));
    ui->role->setCurrentIndex(2);
    ui->btn_login->setText(tr("Login"));
}

void LoginDialog::loadCSS(const QString &cssFile)
{
    QFile file(cssFile);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        return;
    }

    QTextStream stream(&file);
    QString styleSheet = stream.readAll();
    this->setStyleSheet(styleSheet);

    file.close();
}

void LoginDialog::on_btn_login_clicked()
{
    QString username = ui->username->text();
    QString password = ui->password->text();

    // 临时使用
    if (username != "Tian_sj") {
        ui->lab_error_username->setVisible(true);
        return;
    }

    if (password != "123456") {
        ui->lab_error_password->setVisible(true);
        return;
    }
    m_role = ui->role->currentIndex();
    return accept();
}

void LoginDialog::on_username_textEdited(const QString &arg1)
{
    Q_UNUSED(arg1);
    ui->lab_error_username->setVisible(false);
}

void LoginDialog::on_password_textEdited(const QString &arg1)
{
    Q_UNUSED(arg1);
    ui->lab_error_password->setVisible(false);
}

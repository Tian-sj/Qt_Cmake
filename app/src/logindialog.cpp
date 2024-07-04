#include <app/logindialog.h>
#include <QFile>
#include <QListView>
#include <QLineEdit>

#include "ui_logindialog.h"

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    translator(new QTranslator),
    m_windows(nullptr),
    create_timer(new QTimer),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);

    initUI();
    initUIText();
    initConnect();
}

LoginDialog::~LoginDialog()
{
    if (translator) {
        delete translator;
        translator = nullptr;
    }
    if (animation_left) {
        delete animation_left;
        animation_left = nullptr;
    }
    if (animation_right) {
        delete animation_right;
        animation_right = nullptr;
    }
    if (animation_label) {
        delete animation_label;
        animation_label = nullptr;
    }
    if (effect_label) {
        delete effect_label;
        effect_label = nullptr;
    }

    delete ui;
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

std::unique_ptr<MainWindow> LoginDialog::get_windows()
{
    return std::move(m_windows);
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

    animation_left = new QPropertyAnimation(ui->group_in_left, "geometry", ui->groupBackground);
    animation_left->setDuration(1000);
    animation_left->setStartValue(QRect(0, 0, ui->group_in_left->width(), ui->group_in_left->height()));
    animation_left->setEndValue(QRect(400, 0, ui->group_in_left->width(), ui->group_in_left->height()));

    animation_right = new QPropertyAnimation(ui->group_in_right, "geometry", ui->groupBackground);
    animation_right->setDuration(1000);
    animation_right->setStartValue(QRect(400, 0, ui->group_in_right->width(), ui->group_in_right->height()));
    animation_right->setEndValue(QRect(0, 0, ui->group_in_right->width(), ui->group_in_right->height()));

    effect_label = new QGraphicsOpacityEffect(ui->label);
    ui->label->setGraphicsEffect(effect_label);
    effect_label->setOpacity(0);

    animation_label = new QPropertyAnimation(effect_label, "opacity", ui->label);
    animation_label->setDuration(2000);
    animation_label->setStartValue(0.0);
    animation_label->setEndValue(1.0);
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
    ui->label->setText(tr("Hello World!"));
}

void LoginDialog::initConnect()
{
    connect(animation_left, &QPropertyAnimation::finished, this, &LoginDialog::on_login_successful);
    connect(animation_label, &QPropertyAnimation::finished, this, &LoginDialog::on_load_successful);
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
    QString m_username = ui->username->text();
    QString password = ui->password->text();
    m_role = ui->role->currentIndex();

    // 临时使用
    if (m_username != "Tian_sj") {
        ui->lab_error_username->setVisible(true);
        return;
    }

    if (password != "123456") {
        ui->lab_error_password->setVisible(true);
        return;
    }

    ui->btn_close->setVisible(false);
    animation_left->start();
    animation_right->start();
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

void LoginDialog::on_login_successful()
{
    animation_label->start();

    create_timer->setSingleShot(true);
    connect(create_timer, &QTimer::timeout, [=](){
        delete create_timer;
        create_timer = nullptr;
        m_windows = std::unique_ptr<MainWindow>(MainWindow::create(m_role, m_username));
    });
    create_timer->start(0);
}

void LoginDialog::on_load_successful() {
    
    return accept();
}

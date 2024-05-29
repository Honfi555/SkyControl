#include "authorization.h"
#include "ui_authorization.h"

#include "stylesheet.h"


Authorization::Authorization(QDialog *parent, QWidget *main_parent)
    : QDialog(parent)
    , ui_(new Ui::Authorization)
    , main_window_(new MainWindow(main_parent))
    , db_(DataBase())
{
    ui_->setupUi(this);
    this->setWindowTitle("Authorization");
    this->setWindowIcon(QIcon(":/resource/icons/icons_authorization.png"));

    ui_->frame_error->hide();
    ui_->pushButton_signUp->hide();

    this->apply_styleSheet();

    connect(this, SIGNAL(userAuthorized(User)), this->main_window_, SLOT(set_authorizedUser(User)));
}


Authorization::~Authorization()
{
    delete ui_;
}


void Authorization::on_pushButton_signIn_clicked()
{
    if (!db_.signIn(ui_->lineEdit_login->text(), ui_->lineEdit_password->text())) {
        ui_->label_error->setText(this->db_.lastError().text());
        ui_->frame_error->show();
        return;
    }

    this->close();
    this->main_window_->show();
    emit this->userAuthorized(this->db_.authorizedUser());
}


void Authorization::on_toolButton_hideError_clicked()
{
    ui_->frame_error->hide();
}


void Authorization::on_pushButton_signUp_clicked()
{
    if (!db_.SignUp(ui_->lineEdit_login->text(), ui_->lineEdit_password->text())) {
        ui_->label_error->setText(this->db_.lastError().text());
        ui_->frame_error->show();
        return;
    }

    this->close();
    this->main_window_->show();
    emit this->userAuthorized(this->db_.authorizedUser());
}


void Authorization::on_pushButton_createAccount_clicked()
{
    ui_->frame_createAccount->hide();
    ui_->pushButton_signIn->hide();
    ui_->pushButton_signUp->show();
}


void Authorization::apply_styleSheet()
{
    StyleSheet style_sheet;
    this->setStyleSheet(style_sheet.window_styleSheet());

    ui_->pushButton_createAccount->setStyleSheet(style_sheet.pushButton_styleSheet());
    ui_->pushButton_signIn->setStyleSheet(style_sheet.pushButton_styleSheet());
    ui_->pushButton_signUp->setStyleSheet(style_sheet.pushButton_styleSheet());

    ui_->lineEdit_login->setStyleSheet(style_sheet.lineEdit_styleSheet());
    ui_->lineEdit_password->setStyleSheet(style_sheet.lineEdit_styleSheet());

    ui_->label_welcome->setStyleSheet(style_sheet.label_styleSheet());
    ui_->label_error->setStyleSheet(style_sheet.label_styleSheet());
    ui_->label_createAccount->setStyleSheet(style_sheet.label_styleSheet());
    ui_->label_login->setStyleSheet(style_sheet.label_styleSheet());
    ui_->label_password->setStyleSheet(style_sheet.label_styleSheet());

    ui_->toolButton_hideError->setStyleSheet(style_sheet.toolButton_styleSheet());
}


#ifndef AUTHORIZATION_H
#define AUTHORIZATION_H

#include <QDialog>

#include "mainwindow.h"
#include "database.h"


namespace Ui {
class Authorization;
}

class Authorization : public QDialog
{
    Q_OBJECT

public:
    explicit Authorization(QDialog *parent = nullptr, QWidget *main_window = nullptr);
    ~Authorization();

signals:
    void userAuthorized(User);

private slots:
    void on_pushButton_signIn_clicked();

    void on_toolButton_hideError_clicked();

    void on_pushButton_signUp_clicked();

    void on_pushButton_createAccount_clicked();

private:
    void apply_styleSheet();

    Ui::Authorization *ui_;

    MainWindow *main_window_;

    DataBase db_;
};

#endif // AUTHORIZATION_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>

#include "user.h"
#include "pathOperations/pathdialog.h"
#include "authorization/database.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent=nullptr, QWidget *pathDialog=nullptr, DataBase db=DataBase());
    ~MainWindow();

private slots:
    void set_authorizedUser(User user);

    void on_pushButton_create_clicked();

    void add_path(Route);

    void edit_path(Route);

    void on_pushButton_delete_clicked();

    void on_pushButton_edit_clicked();

    void on_pushButton_check_clicked();

    void on_pushButton_verification_clicked();

    void on_pushButton_check1_clicked();

    void on_pushButton_completeTask_clicked();

    void on_listWidget_chats_itemClicked(QListWidgetItem *item);

    void on_toolButton_send_clicked();

    void on_action_technicalSupport_triggered();

    void on_actionCustomer_triggered();

    void on_actionAdministrator_triggered();

private:
    void update_tableViewPaths();
    void update_tableViewTasks();
    void update_listWidgetChats();
    void print_error(const QString &window_tittle, const QString &error, const QString &icon_path);

    [[nodiscard]] QPixmap drawRoute(QImage map, const Route &path) const;

    void apple_styleSheet();

    Ui::MainWindow *ui_;

    User authorized_user_;
    PathDialog *pathDialog_window_;

    DataBase db_;

    QString selected_field_;
};

#endif // MAINWINDOW_H

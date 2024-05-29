#ifndef PATHDIALOG_H
#define PATHDIALOG_H

#include <QWidget>

#include <user.h>


namespace Ui {
class PathDialog;
}


class PathDialog : public QWidget
{
    Q_OBJECT
public:
    enum PathDialogType {
        Add,
        Update
    };


    explicit PathDialog(QWidget *parent = nullptr);
    ~PathDialog();

    void setDialogType(PathDialogType type);

signals:
    void pathAddComplited(Route);
    void pathUpdateComplited(Route);

private slots:
    void on_pushButton_cancel_clicked();

    void on_pushButton_confirm_clicked();

    void on_pushButton_browseMap_clicked();

private:
    void apple_styleSheet();

    Ui::PathDialog *ui_;

    PathDialogType type_;
};

#endif // PATHDIALOG_H

#include "pathdialog.h"
#include "ui_pathdialog.h"

#include "stylesheet.h"

#include <QFileDialog>


PathDialog::PathDialog(QWidget *parent)
    : QWidget(parent)
    , ui_(new Ui::PathDialog)
{
    ui_->setupUi(this);
    this->setWindowTitle("Route operation");
    this->setWindowIcon(QIcon(":/resource/icons/icons_edit.png"));

    this->apple_styleSheet();
}


PathDialog::~PathDialog()
{
    delete ui_;
}


void PathDialog::setDialogType(PathDialogType type)
{
    this->type_ = type;

    ui_->label_operation->setText((type_ == PathDialogType::Add) ? "Create route" : "Update route");
}


void PathDialog::on_pushButton_cancel_clicked()
{
    this->close();
}


void PathDialog::on_pushButton_confirm_clicked()
{
    if (ui_->lineEdit_tittle->text().isEmpty() || ui_->lineEdit_map->text().isEmpty() || ui_->lineEdit_coordinates->text().isEmpty()) {
        return;
    }

    Route path(ui_->lineEdit_tittle->text(), ui_->lineEdit_map->text(), Status::Waiting);

    QVector<QPair<int, int> > coordinates;
    for (const QString &coordinate : ui_->lineEdit_coordinates->text().split(";")) {
        QStringList coordinates_list = coordinate.split(",");
        if (coordinates_list.size() % 2 == 1) coordinates_list.push_back(coordinates_list.last());

        coordinates.push_back(std::make_pair(coordinates_list.at(0).toInt(), coordinates_list.at(1).toInt()));
    }
    path.set_coordinates(coordinates);

    (type_ == PathDialogType::Add) ? emit this->pathAddComplited(path) : emit this->pathUpdateComplited(path);

    this->close();
}


void PathDialog::on_pushButton_browseMap_clicked()
{
    const QString FILE_PATH = QFileDialog::getOpenFileName(
        this,
        tr("Open Map File"),
        "D://repos//qt//SkyControl//resource",
        "Image Files (*.png);; Image Files (*.jpg);;All Files (*.*)");

    ui_->lineEdit_map->setText(FILE_PATH);
}


void PathDialog::apple_styleSheet()
{
    StyleSheet style_sheet;
    this->setStyleSheet(style_sheet.window_styleSheet());

    ui_->pushButton_browseMap->setStyleSheet(style_sheet.pushButton_styleSheet());
    ui_->pushButton_cancel->setStyleSheet(style_sheet.pushButton_styleSheet());
    ui_->pushButton_confirm->setStyleSheet(style_sheet.pushButton_styleSheet());


    ui_->lineEdit_coordinates->setStyleSheet(style_sheet.lineEdit_styleSheet());
    ui_->lineEdit_map->setStyleSheet(style_sheet.lineEdit_styleSheet());
    ui_->lineEdit_tittle->setStyleSheet(style_sheet.lineEdit_styleSheet());

    ui_->label_operation->setStyleSheet(style_sheet.label_styleSheet());
    ui_->label_coordinates->setStyleSheet(style_sheet.label_styleSheet());
    ui_->label_map->setStyleSheet(style_sheet.label_styleSheet());
    ui_->label_tittle->setStyleSheet(style_sheet.label_styleSheet());
}


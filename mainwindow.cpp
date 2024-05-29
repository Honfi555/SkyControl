#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "stylesheet.h"

#include <QStandardItemModel>
#include <QStandardItem>
#include <QMessageBox>
#include <QPixmap>
#include <QPainter>
#include <QTimer>


MainWindow::MainWindow(QWidget *parent, QWidget *pathDialog, DataBase db)
    : QMainWindow(parent)
    , ui_(new Ui::MainWindow)
    , pathDialog_window_(new PathDialog(pathDialog))
    , db_(db)
{
    ui_->setupUi(this);
    this->setWindowTitle("SkyControl");
    this->setWindowIcon(QIcon(":/resource/icons/icons_skycontrol.png"));

    this->apple_styleSheet();
}


MainWindow::~MainWindow()
{
    delete ui_;
}


void MainWindow::set_authorizedUser(User user)
{
    this->authorized_user_ = user;

    switch (user.privilage()) {
    case Privilage::Administrator:
        ui_->menu->addAction(ui_->action_technicalSupport);
        this->update_tableViewTasks();
        ui_->stackedWidget->setCurrentWidget(ui_->main_admin);
        break;
    case Privilage::Customer:
        this->update_tableViewPaths();
        ui_->menu->addAction(ui_->action_technicalSupport);
        ui_->stackedWidget->setCurrentWidget(ui_->main_customer);
        break;
    case Privilage::TechnicalSupport:
        ui_->menubar->hide();
        this->update_listWidgetChats();
        ui_->stackedWidget->setCurrentWidget(ui_->main_support);

        break;
    }

    this->db_ = DataBase();
}


void MainWindow::on_pushButton_create_clicked()
{
    this->pathDialog_window_->setDialogType(PathDialog::Add);
    connect(this->pathDialog_window_, SIGNAL(pathAddComplited(Route)), this, SLOT(add_path(Route)));

    this->pathDialog_window_->show();
}


void MainWindow::add_path(Route path)
{
    if (!db_.addRoute(this->authorized_user_.login(), path)) {
        this->print_error("Database error", db_.lastError().text(), ":/resource/icons/icons_error.png");
        return;
    }

    if (!db_.createTask(path.tittle(), Task(this->authorized_user_.login(), path))) {
        this->print_error("Database error", db_.lastError().text(), ":/resource/icons/icons_error.png");
        return;
    }

    authorized_user_.addRoute(path);

    this->update_tableViewPaths();
}


void MainWindow::edit_path(Route path)
{
    if (!db_.editRoute(this->authorized_user_.login(), this->selected_field_, path)) {
        this->print_error("Database error", db_.lastError().text(), ":/resource/icons/icons_error.png");
        return;
    }

    authorized_user_.editRoute(this->selected_field_, path);

    this->update_tableViewPaths();
}


void MainWindow::update_tableViewPaths()
{
    QStandardItemModel *model = new QStandardItemModel;
    model->setHorizontalHeaderItem(0, new QStandardItem("Tittle"));
    model->setHorizontalHeaderItem(1, new QStandardItem("Status"));

    int row = 0;
    for (const auto &path : this->authorized_user_.routes()) {
        model->setItem(row, 0, new QStandardItem(path.tittle()));
        model->setItem(row, 1, new QStandardItem(QString::number(path.status())));

        row++;
    }

    ui_->tableView_paths->setModel(model);

    ui_->tableView_paths->resizeColumnsToContents();
}


void MainWindow::update_tableViewTasks()
{
    QStandardItemModel *model = new QStandardItemModel;
    model->setHorizontalHeaderItem(0, new QStandardItem("User"));
    model->setHorizontalHeaderItem(1, new QStandardItem("Tittle"));

    int row = 0;
    for (const auto &path : this->authorized_user_.tasks()) {
        model->setItem(row, 0, new QStandardItem(path.login()));
        model->setItem(row, 1, new QStandardItem(path.routeTittle()));

        row++;
    }

    ui_->tableView_tasks->setModel(model);

    ui_->tableView_tasks->resizeColumnsToContents();
}


void MainWindow::update_listWidgetChats()
{
    for (const auto &chat : this->authorized_user_.chats()) {
        ui_->listWidget_chats->addItem((chat.users().first == authorized_user_.login())
                                           ? chat.users().second
                                           : chat.users().first);
    }
}


void MainWindow::print_error(const QString &window_tittle, const QString &error, const QString &icon_path)
{
    StyleSheet style_sheet;
    QPixmap icon(icon_path);
    QMessageBox message_box(this);

    message_box.setIconPixmap(icon.scaled(QSize(48, 48)));
    message_box.setWindowTitle(window_tittle);
    message_box.setText(error);
    message_box.setStandardButtons(QMessageBox::Ok);
    message_box.setStyleSheet(style_sheet.messageBox_styleSheet());
    message_box.exec();
}


QPixmap MainWindow::drawRoute(QImage map, const Route &path) const
{
    QVector<QPair<int, int>> coordinates = path.coordinates();

    if (coordinates.size() < 1)
        return QPixmap::fromImage(map);

    QPainter painter(&map);
    QPen pen;

    pen.setColor(Qt::red);
    pen.setWidth(2);
    painter.setPen(pen);

    for (int i = 1; i < coordinates.size() && coordinates.size() > 1; i++) {
        painter.drawLine(coordinates.at(i - 1).first
                         , coordinates.at(i - 1).second
                         , coordinates.at(i).first
                         , coordinates.at(i).second);
    }

    pen.setColor(Qt::darkRed);
    pen.setWidth(4);
    painter.setPen(pen);
    painter.drawPoint(coordinates.at(0).first, coordinates.at(0).second);
    painter.drawPoint(coordinates.last().first, coordinates.last().second);

    painter.end();

    return QPixmap::fromImage(map);
}


void MainWindow::apple_styleSheet()
{
    StyleSheet style_sheet;
    this->setStyleSheet(style_sheet.window_styleSheet());

    ui_->menubar->setStyleSheet(style_sheet.menuBar_styleSheet());
    ui_->menu->setStyleSheet(style_sheet.menu_styleSheet());

    ui_->pushButton_check->setStyleSheet(style_sheet.pushButton_styleSheet());
    ui_->pushButton_check1->setStyleSheet(style_sheet.pushButton_styleSheet());
    ui_->pushButton_completeTask->setStyleSheet(style_sheet.pushButton_styleSheet());
    ui_->pushButton_create->setStyleSheet(style_sheet.pushButton_styleSheet());
    ui_->pushButton_delete->setStyleSheet(style_sheet.pushButton_styleSheet());
    ui_->pushButton_edit->setStyleSheet(style_sheet.pushButton_styleSheet());
    ui_->pushButton_verification->setStyleSheet(style_sheet.pushButton_styleSheet());

    ui_->tableView_paths->setStyleSheet(style_sheet.table_styleSheet());
    ui_->tableView_tasks->setStyleSheet(style_sheet.table_styleSheet());

    ui_->lineEdit_send->setStyleSheet(style_sheet.lineEdit_styleSheet());

    ui_->label->setStyleSheet(style_sheet.label_styleSheet());

    ui_->toolButton_send->setStyleSheet(style_sheet.toolButton_styleSheet());

    ui_->listWidget_chats->setStyleSheet(style_sheet.listWidget_styleSheet());

    ui_->textEdit_chat->setStyleSheet(style_sheet.textEdit_styleSheet());
}


void MainWindow::on_pushButton_delete_clicked()
{
    QModelIndexList selected = ui_->tableView_paths->selectionModel()->selectedIndexes();

    if (selected.isEmpty()) {
        this->print_error("User manual", "Select row to deliting", ":/resource/icons/icons_information.png");
        return;
    }

    for (const auto &field : selected) {
        if (field.column() != 0) continue;

        QVariant tittle_field = ui_->tableView_paths->model()->data(field);
        if (!db_.removeRoute(this->authorized_user_.login(), tittle_field.toString())) {
            this->print_error("Database error", db_.lastError().text(), ":/resource/icons/icons_error.png");
            return;
        }

        authorized_user_.removRoute(tittle_field.toString());

        this->update_tableViewPaths();
        break;
    }
}


void MainWindow::on_pushButton_edit_clicked()
{
    QModelIndexList selected = ui_->tableView_paths->selectionModel()->selectedIndexes();

    if (selected.isEmpty()) {
        this->print_error("User manual", "Select row to update route", ":/resource/icons/icons_information.png");
        return;
    }

    for (const auto &field : selected) {
        if (field.column() != 0) continue;
        this->selected_field_ = ui_->tableView_paths->model()->data(field).toString();
        break;
    }

    this->pathDialog_window_->setDialogType(PathDialog::Update);
    connect(this->pathDialog_window_, SIGNAL(pathUpdateComplited(Route)), this, SLOT(edit_path(Route)));

    this->pathDialog_window_->show();
}


void MainWindow::on_pushButton_check_clicked()
{
    QModelIndexList selected = ui_->tableView_paths->selectionModel()->selectedIndexes();

    if (selected.isEmpty()) {
        this->print_error("User manual", "Select row to check route", ":/resource/icons/icons_information.png");
        return;
    }

    QModelIndex field_index = ui_->tableView_paths->model()->index(selected.at(0).row(), 0);
    this->selected_field_ = ui_->tableView_paths->model()->data(field_index).toString();

    Route route = authorized_user_.route(selected_field_);
    QImage map(route.map());
    map = map.scaled(1000, 1000, Qt::KeepAspectRatio);

    QMessageBox message_box(this);
    StyleSheet style_sheet;

    message_box.setWindowIcon(QIcon(":/resource/icons/icons_route.png"));
    message_box.setIconPixmap(this->drawRoute(map, route));
    message_box.setWindowTitle("Check route");
    message_box.setStyleSheet(style_sheet.messageBox_styleSheet());
    message_box.exec();
}


void MainWindow::on_pushButton_verification_clicked()
{
    QModelIndexList selected = ui_->tableView_tasks->selectionModel()->selectedIndexes();

    if (selected.isEmpty()) {
        this->print_error("User manual", "Select row to verification", ":/resource/icons/icons_information.png");
        return;
    }

    QTimer::singleShot(2000, this, SLOT(on_pushButton_completeTask_clicked()));
}


void MainWindow::on_pushButton_check1_clicked()
{
    QModelIndexList selected = ui_->tableView_tasks->selectionModel()->selectedIndexes();

    if (selected.isEmpty()) {
        this->print_error("User manual", "Select row to check route", ":/resource/icons/icons_information.png");
        return;
    }

    QModelIndex login_index = ui_->tableView_tasks->model()->index(selected.at(0).row(), 0);
    QString login = ui_->tableView_tasks->model()->data(login_index).toString();
    QModelIndex routeTittle_index = ui_->tableView_tasks->model()->index(selected.at(0).row(), 1);
    QString route_tittle = ui_->tableView_tasks->model()->data(routeTittle_index).toString();

    Task task = this->authorized_user_.task(login, route_tittle);
    QImage map(task.mapPath());
    map = map.scaled(1000, 1000, Qt::KeepAspectRatio);

    QMessageBox message_box;
    message_box.setIconPixmap(this->drawRoute(map, Route(task)));
    message_box.exec();
}


void MainWindow::on_pushButton_completeTask_clicked()
{
    QModelIndexList selected = ui_->tableView_tasks->selectionModel()->selectedIndexes();

    if (selected.isEmpty()) {
        this->print_error("User manual", "Select row to complete task", ":/resource/icons/icons_information.png");
        return;
    }

    QModelIndex login_index = ui_->tableView_tasks->model()->index(selected.at(0).row(), 0);
    QString login = ui_->tableView_tasks->model()->data(login_index).toString();
    QModelIndex routeTittle_index = ui_->tableView_tasks->model()->index(selected.at(0).row(), 1);
    QString route_tittle = ui_->tableView_tasks->model()->data(routeTittle_index).toString();

    Task task = this->authorized_user_.task(login, route_tittle);

    if (!db_.completeTask(task, Status::Confirmed)) {
        this->print_error("Database error", db_.lastError().text(), ":/resource/icons/icons_error.png");
        return;
    }

    this->authorized_user_.completeTask(task);

    this->update_tableViewTasks();
}


void MainWindow::on_listWidget_chats_itemClicked(QListWidgetItem *item)
{
    ui_->textEdit_chat->clear();

    for (const auto &message : this->authorized_user_.chat(item->text()).messages()) {
        if (message.sender() == authorized_user_.login())
            ui_->textEdit_chat->append("<font color=blue>" + message.text() + "</font>");
        else
            ui_->textEdit_chat->append("<font color=red>" + message.text() + "</font>");
    }
}


void MainWindow::on_toolButton_send_clicked()
{
    if (ui_->lineEdit_send->text().isEmpty())
        return;

    QString reciver;
    switch (authorized_user_.privilage()) {
    case Privilage::Customer:
        reciver = db_.techSupLogin();
        break;
    case Privilage::Administrator:
        reciver = db_.techSupLogin();
        break;
    case Privilage::TechnicalSupport:
        reciver = ui_->listWidget_chats->currentItem()->text();
        break;
    }
    if (!db_.sendMessage(reciver
                         , Message(authorized_user_.login(), ui_->lineEdit_send->text()))) {
        this->print_error("Database error", db_.lastError().text(), ":/resource/icons/icons_error.png");
        return;
    }

    authorized_user_.sendMessage(reciver, Message(authorized_user_.login(), ui_->lineEdit_send->text()));
    ui_->textEdit_chat->append("<font color=blue>" + ui_->lineEdit_send->text() + "</font>");
    ui_->lineEdit_send->clear();
}


void MainWindow::on_action_technicalSupport_triggered()
{
    if (!db_.createChat(authorized_user_.login(), db_.techSupLogin())) {
        this->print_error("Database error", db_.lastError().text(), ":/resource/icons/icons_error.png");
        return;
    }

    on_listWidget_chats_itemClicked(new QListWidgetItem(db_.techSupLogin()));

    switch (authorized_user_.privilage()) {
    case Privilage::Customer:
        ui_->listWidget_chats->hide();
        ui_->menu->addAction(ui_->actionCustomer);
        break;
    case Privilage::Administrator:
        ui_->listWidget_chats->hide();
        ui_->menu->addAction(ui_->actionAdministrator);
        break;
    case Privilage::TechnicalSupport:
        return;
    }

    ui_->menu->removeAction(ui_->action_technicalSupport);
    ui_->stackedWidget->setCurrentWidget(ui_->main_support);
}


void MainWindow::on_actionCustomer_triggered()
{
    ui_->menu->removeAction(ui_->actionCustomer);
    ui_->menu->addAction(ui_->action_technicalSupport);
    ui_->stackedWidget->setCurrentWidget(ui_->main_customer);
}


void MainWindow::on_actionAdministrator_triggered()
{
    if (authorized_user_.privilage() == Privilage::TechnicalSupport) {
        if (!db_.createChat(authorized_user_.login(), db_.techSupLogin())) {
            this->print_error("Database error", db_.lastError().text(), ":/resource/icons/icons_error.png");
            return;
        }
        else
            on_listWidget_chats_itemClicked(new QListWidgetItem(db_.adminLogin()));
    }

    ui_->menu->removeAction(ui_->actionAdministrator);
    ui_->menu->addAction(ui_->action_technicalSupport);
    ui_->stackedWidget->setCurrentWidget(ui_->main_admin);
}


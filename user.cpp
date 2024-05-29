#include "user.h"

#include <utility>

#include <QJsonArray>


User::User(const User &other)
{
    this->login_ = other.login_;
    this->password_ = other.password_;
    this->privilage_ = other.privilage_;
    this->routes_ = other.routes_;
    this->tasks_ = other.tasks_;
    this->chats_ = other.chats_;
}

User::User(const QJsonObject &object)
{
    this->login_ = object["login"].toString();
    this->password_ = object["password"].toString();
    switch (object["privilage"].toInt()) {
    case 0:
        this->privilage_ = Privilage::Customer;
        break;
    case 1:
        this->privilage_ = Privilage::TechnicalSupport;
        break;
    case 2:
        this->privilage_ = Privilage::Administrator;
        break;
    }
}


void User::addChat(const QString &reciver, const Chat &chat)
{
    chats_.insert(reciver, chat);
}


void User::addChats(const QJsonArray &chats)
{
    for (const auto &chat : chats) {
        QJsonObject chat_object = chat.toObject();
        QString reciver = (chat_object["user1"].toString() == login_)
                              ? chat_object["user2"].toString() : chat_object["user1"].toString();
        chats_.insert(reciver, Chat(chat.toObject()));
    }
}


void User::addRoute(const QJsonArray &json_paths)
{
    for (const auto &path : json_paths) {
        QJsonObject object_path = path.toObject();

        QString tittle = object_path["tittle"].toString();
        QString map_path = object_path["map"].toString();
        Status status = Status(object_path["status"].toInt());

        QVector<QPair<int, int> > coordinates;
        for (const auto &coordinate : object_path["coordinates"].toArray()) {
            QJsonObject object_coordinate = coordinate.toObject();

            coordinates.push_back(std::make_pair(object_coordinate["x"].toInt(), object_coordinate["y"].toInt()));
        }

        this->routes_.push_back(Route(tittle, map_path, status, coordinates));
    }
}


void User::addRoute(const Route &path)
{
    this->routes_.push_back(path);
}


void User::removRoute(const QString &tittle)
{
    for (const Route &path : this->routes_) {
        if (path.tittle() != tittle) continue;

        this->routes_.removeOne(path);
        break;
    }
}


void User::editRoute(const QString &tittle, const Route &path)
{
    for (int i = 0; i < this->routes_.size(); i++) {
        if (this->routes_.at(i).tittle() != tittle) continue;

        this->routes_.replace(i, path);
        break;
    }
}


void User::addTask(const Task &task)
{
    this->tasks_.push_back(task);
}


void User::completeTask(const Task &other_task)
{
    for (const Task &task : this->tasks_) {
        if (task == other_task) {
            this->tasks_.removeOne(task);
            break;
        }
    }
}


void User::addTasks(const QJsonArray &tasks)
{
    for (const auto &task : tasks) {
        QJsonObject object_task = task.toObject();

        QString login = object_task["login"].toString();
        QString route_tittle = object_task["routeTitle"].toString();
        QString map_path = object_task["map"].toString();

        QVector<QPair<int, int> > coordinates;
        for (const auto &coordinate : object_task["coordinates"].toArray()) {
            QJsonObject object_coordinate = coordinate.toObject();

            coordinates.push_back(std::make_pair(object_coordinate["x"].toInt(), object_coordinate["y"].toInt()));
        }

        this->tasks_.push_back(Task(login, map_path, route_tittle, coordinates));
    }
}


bool User::isPassword(const QString &password) const
{
    return this->password_ == password;
}


bool User::routeExists(const QString &tittle) const
{
    for (const auto & path : routes_) {
        if (path.tittle() == tittle) return true;
    }

    return false;
}


QString User::login() const
{
    return this->login_;
}


Privilage User::privilage() const
{
    return this->privilage_;
}


QMap<QString, Chat> User::chats() const
{
    return this->chats_;
}


QVector<Route> User::routes() const
{
    return this->routes_;
}


QVector<Task> User::tasks() const
{
    return this->tasks_;
}


Chat User::chat(const QString &reciver) const
{
    return this->chats_.value(reciver);
}


Route User::route(const QString &tittle) const
{
    for (const auto &path : routes_) {
        if (path.tittle() != tittle) continue;

        return path;
    }

    return Route("", "", Status::Denied);
}


Task User::task(const QString &login, const QString &route_tittle) const
{
    for (const Task &task : this->tasks_) {
        if (task.login() == login && task.routeTittle() == route_tittle) return task;
    }

    return Task("", "", "", {});
}


void User::sendMessage(const QString &reciver, const Message &message)
{
    Chat chat = this->chats_.value(reciver);
    chat.sendMessage(message);
    this->chats_.insert(reciver, chat);
}


void User::operator=(const User &other)
{
    this->login_ = other.login_;
    this->password_ = other.password_;
    this->privilage_ = other.privilage_;
    this->routes_ = other.routes_;
    this->tasks_ = other.tasks_;
    this->chats_ = other.chats_;
}


Route::Route(const Task &task)
{
    this->tittle_ = task.routeTittle();
    this->map_path_ = task.mapPath();
    this->coordinates_ = task.coordinates();
    this->status_ = Status::Waiting;
}


Route::Route(const QJsonObject &route_object)
{
    this->tittle_ = route_object["tittle"].toString();
    this->status_ = Status(route_object["status"].toInt());
    this->map_path_ = route_object["map"].toString();
    for (const auto &coordinate : route_object["coordinates"].toArray()) {
        QJsonObject coordinate_object = coordinate.toObject();
        this->coordinates_.push_back(std::make_pair(coordinate_object["x"].toInt(), coordinate_object["y"].toInt()));
    }
}


void Route::set_status(Status status)
{
    this->status_ = status;
}


void Route::add_coordinate(const QPair<int, int> &coordinate)
{
    this->coordinates_.push_back(coordinate);
}


void Route::set_coordinates(const QVector<QPair<int, int> > &coordinates)
{
    this->coordinates_ = coordinates;
}


void Route::set_mapPath(const QString &map_path)
{
    this->map_path_ = map_path;
}


QJsonObject Route::toJsonObject() const
{
    QJsonObject json_path;

    json_path.insert("tittle", this->tittle_);
    json_path.insert("status", this->status_);
    json_path.insert("map", this->map_path_);

    QJsonArray json_coordinates;
    for (const auto &coordinate : this->coordinates_) {
        QJsonObject json_coordinate;
        json_coordinate.insert("x", coordinate.first);
        json_coordinate.insert("y", coordinate.second);
        json_coordinates.append(json_coordinate);
    }
    json_path.insert("coordinates", json_coordinates);

    return json_path;
}


QString Route::tittle() const
{
    return this->tittle_;
}


QString Route::map() const
{
    return this->map_path_;
}


Status Route::status() const
{
    return this->status_;
}


QVector<QPair<int, int> > Route::coordinates() const
{
    return coordinates_;
}


bool Route::operator==(const Route &other) const
{
    if (this->tittle_ != other.tittle_) return false;
    if (this->map_path_ != other.map_path_) return false;
    if (this->status_ != other.status_) return false;
    if (this->coordinates_ != other.coordinates_) return false;
    return true;
}


void Route::operator=(const Route &other)
{
    this->tittle_ = other.tittle_;
    this->map_path_ = other.map_path_;
    this->status_ = other.status_;
    this->coordinates_ = other.coordinates_;
}

Task::Task(const QString &customer_login, const Route &path)
{
    this->login_ = customer_login;
    this->route_tittle_ = path.tittle();
    this->map_path_ = path.map();
    this->coordinates_ = path.coordinates();
}


QJsonObject Task::toJsonObject() const
{
    QJsonObject json_path;

    json_path.insert("login", this->login_);
    json_path.insert("routeTitle", this->route_tittle_);
    json_path.insert("map", this->map_path_);

    QJsonArray json_coordinates;
    for (const auto &coordinate : this->coordinates_) {
        QJsonObject json_coordinate;
        json_coordinate.insert("x", coordinate.first);
        json_coordinate.insert("y", coordinate.second);
        json_coordinates.append(json_coordinate);
    }
    json_path.insert("coordinates", json_coordinates);

    return json_path;
}


QString Task::login() const
{
    return this->login_;
}


QString Task::mapPath() const
{
    return this->map_path_;
}


QString Task::routeTittle() const
{
    return this->route_tittle_;
}


QVector<QPair<int, int> > Task::coordinates() const
{
    return this->coordinates_;
}


bool Task::completeTask() const
{
    return false;
}


bool Task::operator ==(const Task &other) const
{
    if (this->login_ != other.login_) return false;
    if (this->map_path_ != other.map_path_) return false;
    if (this->route_tittle_ != other.route_tittle_) return false;
    if (this->coordinates_ != other.coordinates_) return false;
    return true;
}


bool Task::operator !=(const Task &other) const
{
    if (this->login_ == other.login_) return false;
    if (this->map_path_ == other.map_path_) return false;
    if (this->route_tittle_ == other.route_tittle_) return false;
    if (this->coordinates_ == other.coordinates_) return false;
    return true;
}


void Task::operator =(const Task &other)
{
    this->login_ = other.login_;
    this->map_path_ = other.map_path_;
    this->route_tittle_ = other.route_tittle_;
    this->coordinates_ = other.coordinates_;
}

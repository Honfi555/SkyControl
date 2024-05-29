#include "database.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>


DataBase::DataBase()
    : authorized_user_(User("", ""))
    , last_error_(DBError::NoErrors)
{
    this->users_path_ = "D:/repos/qt/SkyControl/resource/users.json";
    this->tasks_path_ = "D:/repos/qt/SkyControl/resource/tasks.json";
    this->chats_path_ = "D:/repos/qt/SkyControl/resource/chats.json";

    this->admin_login_ = "admin";
    this->techSup_login_ = "techsup";

    this->load_users();
}


bool DataBase::SignUp(const QString &login, const QString &password)
{
    if (users_.contains(login)) {
        this->last_error_ = DBError::UserAlreadyExist;
        return false;
    }

    QJsonArray json_array = this->read_database(this->users_path_);

    if (json_array.isEmpty()) return false;

    QJsonObject params;
    params.insert("login", login);
    params.insert("password", password);
    params.insert("privilage", Privilage::Customer);
    params.insert("paths", QJsonArray());
    json_array.append(params);

    if (!this->write_database(this->users_path_, json_array)) return false;

    this->authorized_user_ = User(login, password);
    this->load_chats();
    this->users_.insert(login, this->authorized_user_);

    return true;
}


bool DataBase::signIn(const QString &login, const QString &password)
{
    if (!users_.contains(login)) {
        this->last_error_ = DBError::UserNotFound;
        return false;
    }

    User user(this->users_[login]);
    if (!user.isPassword(password)) {
        this->last_error_ = DBError::IncorrectPassword;
        return false;
    }
    else {        
        this->authorized_user_ = user;
        this->load_chats();
        this->last_error_ = DBError::NoErrors;
        return true;
    }
}


bool DataBase::createTask(const QString &route_tittle, const Task &task)
{
    if (!users_.contains(task.login())) {
        this->last_error_ = DBError::UserNotFound;
        return false;
    }

    if (!users_[task.login()].routeExists(route_tittle)) {
        this->last_error_ = DBError::RouteNotFound;
        return false;
    }

    QJsonArray database = this->read_database(this->tasks_path_);

    if (database.isEmpty()) return false;

    database.push_back(task.toJsonObject());

    return this->write_database(this->tasks_path_, database);
}


bool DataBase::completeTask(const Task &task, Status status)
{
    if (!this->removeTask(task.routeTittle(), task))
        return false;

    // set Confirmed status to path
    Route path(task);
    path.set_status(status);
    return this->editRoute(task.login(), task.routeTittle(), path);
}


bool DataBase::removeRoute(const QString &login, const QString &route_tittle)
{
    if (!users_.contains(login)) {
        this->last_error_ = DBError::UserNotFound;
        return false;
    }

    QJsonArray json_array = this->read_database(this->users_path_);

    if (json_array.isEmpty()) return false;

    // remove path for database
    QJsonArray new_jsonArray;
    for (const auto &user : json_array) {
        QJsonObject user_object = user.toObject();
        if (user_object["login"] != login) {
            new_jsonArray.append(user_object);
            continue;
        }

        QJsonArray routes;

        for (const auto &route : user_object["paths"].toArray()) {
            if (route.toObject()["tittle"] == route_tittle) {
                if (route.toObject()["status"] == Status::Waiting) {
                    this->removeTask(route_tittle, Task(login, Route(route.toObject())));
                }
            }

            routes.append(route);
        }
        user_object["paths"] = routes;

        new_jsonArray.append(user_object);
    }

    // remove path from runtime save
    User &user = this->users_[login];
    user.removRoute(route_tittle);

    return this->write_database(this->users_path_, new_jsonArray);
}


bool DataBase::addRoute(const QString &login, const Route &path)
{
    if (!users_.contains(login)) {
        this->last_error_ = DBError::UserNotFound;
        return false;
    }

    if (users_[login].routeExists(path.tittle())) {
        this->last_error_ = DBError::RouteAlreadyExist;
        return false;
    }

    QJsonArray json_array = this->read_database(this->users_path_);

    if (json_array.isEmpty()) return false;

    //add path for database
    QJsonArray new_jsonArray;
    for (const auto &user : json_array) {
        QJsonObject user_object = user.toObject();
        if (user_object["login"] == login) {
            QJsonArray paths = user_object["paths"].toArray();
            paths.append(path.toJsonObject());
            user_object["paths"] = paths;
        }

        new_jsonArray.append(user_object);
    }

    //add path in runtime save
    User &user = this->users_[login];
    user.addRoute(path);

    return this->write_database(this->users_path_, new_jsonArray);
}


bool DataBase::editRoute(const QString &login, const QString &tittle, const Route &route)
{
    if (!users_.contains(login)) {
        this->last_error_ = DBError::UserNotFound;
        return false;
    }

    QJsonArray json_array = this->read_database(this->users_path_);

    // edit path for database
    QJsonArray new_jsonArray;
    for (const auto &user : json_array) {
        QJsonObject user_object = user.toObject();

        if (user_object["login"] != login) {
            new_jsonArray.append(user_object);
            continue;
        }

        QJsonArray paths;
        for (const auto &json_path : user_object["paths"].toArray()) {
            if (json_path.toObject()["tittle"].toString() != tittle) {
                paths.append(json_path);
                continue;
            }

            paths.append(route.toJsonObject());
        }

        user_object["paths"] = paths;

        new_jsonArray.append(user_object);
    }

    //edit path in runtime save
    User user = this->users_[login];

    if (!this->write_database(this->users_path_, new_jsonArray)) {
        return false;
    }

    if (!this->removeTask(tittle, Task(login, route))) return false;

    return this->createTask(tittle, Task(login, route));
}


bool DataBase::createChat(const QString &sender, const QString &reciver)
{
    QJsonArray database = this->read_database(this->chats_path_);

    for (const auto &chat : database) {
        QJsonObject chat_object = chat.toObject();
        if ((chat_object["user1"] == sender && chat_object["user2"] == reciver)
            || (chat_object["user2"] == sender && chat_object["user1"] == reciver)) {
            return true; //chat already exist
        }
    }

    database.push_back(Chat(sender, reciver).toJsonObject());

    return this->write_database(this->chats_path_, database);
}


bool DataBase::sendMessage(const QString &reciver, const Message &message)
{
    QJsonArray database = this->read_database(this->chats_path_);

    QJsonArray new_database;
    for (const auto &chat_value_ref : database) {
        QJsonObject chat_object = chat_value_ref.toObject();
        if ((chat_object["user1"] == message.sender() && chat_object["user2"] == reciver)
            || (chat_object["user2"] == message.sender() && chat_object["user1"] == reciver)) {
            Chat chat(chat_object);
            chat.sendMessage(message);
            chat_object = chat.toJsonObject();
        }

        new_database.push_back(chat_object);
    }

    return this->write_database(this->chats_path_, new_database);
}


QString DataBase::adminLogin() const
{
    return this->admin_login_;
}


QString DataBase::techSupLogin() const
{
    return this->techSup_login_;
}


DBError DataBase::lastError() const
{
    return this->last_error_;
}


User DataBase::authorizedUser() const
{
    return this->authorized_user_;
}


void DataBase::load_users()
{
    QJsonArray json_users = this->read_database(this->users_path_);

    this->users_.clear();
    for (const auto &user_json : json_users) {
        User user(user_json.toObject());
        if (user.privilage() == Privilage::Customer) {
            user.addRoute(user_json.toObject()["paths"].toArray());
        }
        if (user.privilage() == Privilage::Administrator) {
            user.addTasks(this->read_database(this->tasks_path_));
        }

        this->users_.insert(user.login(), user);
    }
}


void DataBase::load_chats()
{
    QJsonArray database = this->read_database(this->chats_path_);

    for (const auto &chat_value_ref : database) {
        QJsonObject chat_object = chat_value_ref.toObject();
        if (chat_object["user1"] == authorized_user_.login()
            || chat_object["user2"] == authorized_user_.login()) {
            Chat chat(chat_object);
            authorized_user_.addChat(
                (chat.users().first == authorized_user_.login()) ? chat.users().second : chat.users().first
                , chat);
        }
    }
}


QJsonArray DataBase::read_database(const QString &path)
{
    QJsonArray json_array;
    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) {
        json_array = QJsonDocument::fromJson(file.readAll()).array();

        file.close();
        this->last_error_ = DBError::NoErrors;
    }
    else {
        this->last_error_ = DBError::DBNotOpened;
    }

    return json_array;
}


bool DataBase::write_database(const QString &path, const QJsonArray &database)
{
    QFile file(path);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(database).toJson());

        this->last_error_ = DBError::NoErrors;
        return true;
    }

    this->last_error_ = DBError::DBNotOpened;
    return false;
}


bool DataBase::removeTask(const QString &tittle, const Task &task)
{
    if (!users_.contains(task.login())) {
        this->last_error_ = DBError::UserNotFound;
        return false;
    }

    if (!users_[task.login()].routeExists(tittle)) {
        this->last_error_ = DBError::RouteNotFound;
        return false;
    }

    QJsonArray database = this->read_database(this->tasks_path_);

    if (database.isEmpty()) return false;

    QJsonArray new_database;
    for (const auto &json_task : database) {
        if (tittle == json_task.toObject()["routeTitle"].toString() && task.login() == json_task.toObject()["login"].toString()) continue;

        new_database.push_back(json_task);
    }

    if (!this->write_database(this->tasks_path_, new_database)) {
        this->last_error_ = DBError::UserNotFound;
        return false;
    }

    return true;
}


DBError::DBError(const DBError &other)
{
    this->error_ = other.error_;
}


DBError::Error DBError::error() const
{
    return this->error_;
}


QString DBError::text() const
{
    switch (this->error_) {
    case Error::NoErrors:
        return "No errors.";
        break;
    case Error::IncorrectPassword:
        return "Entered password is incorrent.";
        break;
    case Error::UserNotFound:
        return "User with this login doesn't exist.";
        break;
    case Error::UserAlreadyExist:
        return "User with this login already exist.";
        break;
    case Error::DBNotOpened:
        return "File with database not opened.";
        break;
    case Error::RouteAlreadyExist:
        return "Path already exist.";
        break;
    case Error::ChatAlredyExist:
        return "Chat alreade exist.";
        break;
    case Error::RouteNotFound:
        return "Route not found.";
        break;
    default:
        return "Error undefind";
    }
}


void DBError::operator=(const Error &error)
{
    this->error_ = error;
}


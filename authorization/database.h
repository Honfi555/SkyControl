#ifndef DATABASE_H
#define DATABASE_H

#include <QString>
#include <QMap>

#include "user.h"


class DBError
{
public:
    enum Error {
        NoErrors,
        UserNotFound,
        UserAlreadyExist,
        IncorrectPassword,
        DBNotOpened,
        RouteNotFound,
        RouteAlreadyExist,
        ChatAlredyExist
    };

    DBError(Error error) : error_(error) {}
    DBError(const DBError& other);

    Error error() const;

    QString text() const;

    void operator=(const Error &error);

private:
    Error error_;
};


class DataBase
{
public:
    DataBase();

    bool SignUp(const QString &login, const QString &password);
    bool signIn(const QString &login, const QString &password);

    bool createTask(const QString &route_tittle, const Task &task);
    bool completeTask(const Task &task, Status status);

    bool removeRoute(const QString &login, const QString &path_tittle);
    bool addRoute(const QString &login, const Route &route);
    bool editRoute(const QString &login, const QString &tittle, const Route &route);

    bool createChat(const QString &sender, const QString &reciver);
    bool sendMessage(const QString &reciver, const Message &message);

    QString adminLogin() const;
    QString techSupLogin() const;

    [[nodiscard]] DBError lastError() const;

    [[nodiscard]] User authorizedUser() const;

private:
    void load_users();
    void load_chats();

    QJsonArray read_database(const QString &path);
    bool write_database(const QString &path, const QJsonArray &database);

    bool removeTask(const QString &tittle, const Task &task);

    QMap<QString, User> users_;
    User authorized_user_;

    DBError last_error_;

    QString users_path_;
    QString tasks_path_;
    QString chats_path_;

    QString admin_login_;
    QString techSup_login_;
};

#endif // DATABASE_H

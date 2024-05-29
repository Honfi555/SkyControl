#ifndef USER_H
#define USER_H

#include "chat.h"

#include <QString>
#include <QJsonObject>


enum Privilage{
    Customer,
    TechnicalSupport,
    Administrator
};


enum Status {
    Confirmed,
    Waiting,
    Denied
};

struct User;
struct Route;
struct Task;


struct User
{
    User(const User &other);
    explicit User(const QJsonObject &object);
    explicit User(const QString &login="", const QString &password="", Privilage privilage=Privilage::Customer)
        : login_(login), password_(password), privilage_(privilage), chats_({}), routes_({}), tasks_({}) {}

    void addChat(const QString &reciver, const Chat &chat);
    void addChats(const QJsonArray & chats);

    void addRoute(const Route &route);
    void addRoute(const QJsonArray &routes);
    void removRoute(const QString &tittle);
    void editRoute(const QString &tittle, const Route &route);

    void addTask(const Task &task);
    void completeTask(const Task &task);
    void addTasks(const QJsonArray &tasks);

    bool isPassword(const QString &password) const;
    bool routeExists(const QString &tittle) const;

    QString login() const;
    Privilage privilage() const;

    QMap<QString, Chat> chats() const;
    QVector<Route> routes() const;
    QVector<Task> tasks() const;

    Chat chat(const QString &reciver) const;
    Route route(const QString &tittle) const;
    Task task(const QString &login, const QString &route_tittle) const;

    void sendMessage(const QString &reciver, const Message &message);

    void operator=(const User &other);

private:
    QString login_;
    QString password_;
    Privilage privilage_;

    QMap<QString, Chat> chats_;
    QVector<Route> routes_;
    QVector<Task> tasks_;
};


struct Route
{
    Route(const Task &task);
    Route(const QJsonObject &route_object);
    Route(const Route &) = default;
    Route(const QString &tittle
         , const QString &map_path
         , Status status
         , const QVector<QPair<int, int>> &coordinates={})
        : tittle_(tittle)
        , status_(status)
        , coordinates_(coordinates)
        , map_path_(map_path) {}

    void set_status(Status status);
    void set_mapPath(const QString &map_path);
    void set_coordinates(const QVector<QPair<int, int>> &coordinates);

    void add_coordinate(const QPair<int, int> &coordinate);

    QJsonObject toJsonObject() const;

    QString tittle() const;
    Status status() const;
    QString map() const;
    QVector<QPair<int, int>> coordinates() const;

    bool operator==(const Route &other) const;
    void operator=(const Route &other);

private:
    QString tittle_;
    Status status_;
    QVector<QPair<int, int>> coordinates_;
    QString map_path_;
};


struct Task
{
    Task(const QString &customer_login, const Route &route);
    Task(const Task &) = default;
    Task(const QString &login
         , const QString &map_path
         , const QString &route_tittle
         , const QVector<QPair<int, int> > &coordinates)
        : login_(login)
        , map_path_(map_path)
        , route_tittle_(route_tittle)
        , coordinates_(coordinates) {}

    QJsonObject toJsonObject() const;

    QString login() const;
    QString mapPath() const;
    QString routeTittle() const;
    QVector<QPair<int, int> > coordinates() const;

    bool completeTask() const;

    bool operator ==(const Task &other) const;
    bool operator !=(const Task &other) const;
    void operator =(const Task &other);
private:
    QString login_;
    QString map_path_;
    QString route_tittle_;
    QVector<QPair<int, int> > coordinates_;
};

#endif // USER_H

#ifndef CHAT_H
#define CHAT_H

#include <QString>
#include <QVector>
#include <QJsonObject>


struct Message;


class Chat
{
public:
    Chat(const Chat &) = default;
    Chat(const QJsonObject &chat);
    Chat(const QString &user1="", const QString &user2="", const QVector<Message> &messages={})
        :user1_(user1), user2_(user2), messages_(messages) {}

    [[nodiscard]] QJsonObject toJsonObject() const;

    [[nodiscard]] QPair<QString, QString> users() const;
    [[nodiscard]] QVector<Message> messages() const;

    void sendMessage(const Message &message);

    void operator=(const Chat &chat);

private:
    QString user1_;
    QString user2_;

    QVector<Message> messages_;
};


struct Message
{
    Message(const QJsonObject &message);
    Message(const Message &messa) = default;
    Message(const QString &sender="", const QString &text="")
        : sender_(sender), text_(text) {}

    QJsonObject toJsonObject() const;

    [[nodiscard]] QString sender() const;
    [[nodiscard]] QString text() const;

private:
    QString sender_;
    QString text_;
};

#endif // CHAT_H

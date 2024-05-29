#include "chat.h"

#include <utility>
#include <QJsonArray>


Chat::Chat(const QJsonObject &chat)
{
    user1_ = chat["user1"].toString();
    user2_ = chat["user2"].toString();

    for (const auto &message : chat["chat"].toArray()) {
        messages_.push_back(Message(message.toObject()));
    }
}


QJsonObject Chat::toJsonObject() const
{
    QJsonObject chat_object;
    chat_object.insert("user1", this->user1_);
    chat_object.insert("user2", this->user2_);

    QJsonArray messages;
    for (const auto &message : this->messages_) {
        messages.push_back(message.toJsonObject());
    }
    chat_object.insert("chat", messages);
    return chat_object;
}


QPair<QString, QString> Chat::users() const
{
    return std::make_pair(user1_, user2_);
}


QVector<Message> Chat::messages() const
{
    return messages_;
}


void Chat::sendMessage(const Message &message)
{
    messages_.push_back(message);
}


void Chat::operator=(const Chat &other)
{
    this->user1_ = other.user1_;
    this->user2_ = other.user2_;
    this->messages_ = other.messages_;
}


Message::Message(const QJsonObject &message)
{
    sender_ = message["sender"].toString();
    text_ = message["message"].toString();
}


QJsonObject Message::toJsonObject() const
{
    QJsonObject message_object;
    message_object.insert("sender", this->sender_);
    message_object.insert("message", this->text_);
    return message_object;
}


QString Message::sender() const
{
    return sender_;
}


QString Message::text() const
{
    return text_;
}


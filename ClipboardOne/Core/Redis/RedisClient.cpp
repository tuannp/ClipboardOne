#include "stdafx.h"
#include "RedisClient.h"

RedisClient::RedisClient(const char * ip, int port) :
    context_(nullptr)
{
    context_.reset(redisConnectWithTimeout(ip, port, REDIS_CONNECTION_TIMEOUT));

    if(!connected())
        qDebug() << "failed to connect to Redis";
}

bool RedisClient::connected() const
{
    return context_ && !context_->err;
}

bool RedisClient::sendCommand(const QString & command)
{
    auto reply = static_cast<redisReply *>(redisCommand(context_.get(), command.toLocal8Bit().constData()));
    return reply && !strcmp(reply->str, REDIS_REPLY_OK);
}

bool RedisClient::set(const QString & key, const QByteArray & value)
{
    auto reply = static_cast<redisReply *>(redisCommand(context_.get(), REDIS_SET_BINARY_COMMAND, 
                                                        key.toLocal8Bit().constData(),
                                                        value.constData(), 
                                                        (size_t)value.size()));
    return reply && !strcmp(reply->str, REDIS_REPLY_OK);
}

QByteArray RedisClient::get(const QString & key)
{
    auto reply = static_cast<redisReply *>(redisCommand(context_.get(), REDIS_GET_COMMAND, 
                                                        key.toLocal8Bit().constData()));
    return reply ? QByteArray(reply->str, reply->len) : QByteArray();
}
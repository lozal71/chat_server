#include "session.h"
#include "protocol_out.h"
#include "protocol_in.h"

session::session(QTcpSocket *socket)
{
    socketSession = socket;
    connect (socketSession, &QTcpSocket::readyRead,
             this,&session::commandHandler);
    connect(socketSession, &QTcpSocket::disconnected,
            this, &session::connectClosed);
}

void session::commandHandler()
{
    QByteArray replayToClient;
    setCodeCommand();
    switch (codeCommand) {
            case 0:
                replayToClient = "{ \"1\": 0, \"2\": \"your have send message-error\"}";
            break;
            default:
                 replayToClient = readFromDB();
            break;
        }
   // qDebug() << "replayToClient " << replayToClient;
    protocolOut OutputMessage(replayToClient);
    socketSession->write(OutputMessage.getMessageToClient());
}

void session::setCodeCommand()
{
    protocolIn InputMessage(socketSession);
    codeCommand = InputMessage.getCode();
    dataCommand = InputMessage.getData();
    //qDebug() << "codeCommand " << codeCommand;
    //qDebug() << "dataCommand " << dataCommand;
}

QByteArray session::readFromDB()
{
    QByteArray replay;
    QSqlQuery query;
    QJsonObject replayDB;
    switch (codeCommand) {
        case 1:
            QString queryString = "select id,name from users where users.login=\"";
            queryString.append(dataCommand);
            queryString.append("\"");
            //qDebug() << "queryString" << queryString;
            if (!query.exec(queryString)){
                    qDebug() << "query error" << query.lastError();
            }
            else {
                //qDebug() << "query success" ;
                while (query.next()){
                    client.id = query.value(0).toInt();
                    client.name = query.value(1).toString();
                }
                replayDB.insert("1",client.id);
                replayDB.insert("2",client.name);
                QJsonDocument doc(replayDB);
                replay = doc.toJson(QJsonDocument::Compact);
            }
        break;
    }
    return replay;
}

ClientAuth session::getClient()
{
    return client;
}


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
    login = InputMessage.getLoginClient();
    pass = InputMessage.getPassClient();
    qDebug() << "codeCommand " << codeCommand;
    qDebug() << "login " << login;
    qDebug() << "pass " << pass;
}

QByteArray session::readFromDB()
{
    QByteArray replayBA;
    QSqlQuery query;
    QJsonObject replayObj;
    QJsonObject replayDB;
    switch (codeCommand) {
        case 1:
            QString queryString = "select id,name from users where users.login=\"";
            queryString.append(login);
            queryString.append("\" and users.password=\"");
            queryString.append(pass);
            queryString.append("\"");
            qDebug() << "queryString" << queryString;
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
                qDebug() << "replayDB" << replayDB;
                replayObj.insert("1",1);
                replayObj.insert("2",replayDB);
              qDebug() << "replayObj" << replayObj;
                QJsonDocument docServer(replayObj);
    //            qDebug() << "docServer" << docServer;
                replayBA = docServer.toJson(QJsonDocument::Compact);
      //          qDebug() << "replayBA" << replayBA;
        //        QString strJson(docServer.toJson(QJsonDocument::Compact));
          //      qDebug() << "strJson" << strJson;
            }
        break;
    }
    return replayBA;
}

ClientAuth session::getClient()
{
    return client;
}


#include "session.h"

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
    protocolIn InputMessage(socketSession);
    codeCommand = InputMessage.getCode();
    QByteArray baRreplayToClient;
    QJsonObject joTemp;
    switch (codeCommand) {
            case setCodeCommand::ErrorMessage:
            {
                joTemp.insert("codeCommand", setCodeCommand::ErrorMessage);
                joTemp.insert("joDataInput", "your have send message-error");
            }
            break;
            case setCodeCommand::Auth:
            {
                joTemp.insert("codeCommand", setCodeCommand::Auth);
                login =InputMessage.getLoginClient();
                pass = InputMessage.getPassClient();
                joTemp.insert("joDataInput", readFromDB());
            }
            break;
            case setCodeCommand::SessionClosed:
            {}
            break;
            case setCodeCommand::NoConnect:
            {}
            break;
        }
    qDebug() << "joTemp " << joTemp;
    QJsonDocument doc(joTemp);
    baRreplayToClient = doc.toJson(QJsonDocument::Compact);
    protocolOut OutputMessage(baRreplayToClient);
    socketSession->write(OutputMessage.getMessageToClient());
}

QJsonObject session::readFromDB()
{
    QSqlQuery query;
    QJsonObject joReplayDB;
    switch (codeCommand)
    {
        case setCodeCommand::ErrorMessage: {} break;
        case setCodeCommand::Auth:
        {
            QString queryString = "select id,name from users where users.login=\"";
            queryString.append(login);
            queryString.append("\" and users.password=\"");
            queryString.append(pass);
            queryString.append("\"");
            //qDebug() << "queryString" << queryString;
            if (!query.exec(queryString)){
                    qDebug() << "query error" << query.lastError();
            }
            else {
                //qDebug() << "query success" ;
                while (query.next()){
                    joReplayDB.insert("id",query.value(0).toInt());
                     joReplayDB.insert("name",query.value(1).toString());
                }
            }
        }
         break;
        case setCodeCommand::NoConnect:
            {} break;
        case setCodeCommand::SessionClosed:
            {} break;
    }
    qDebug() << "joReplayDB " << joReplayDB;
    return joReplayDB;
}


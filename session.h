#ifndef SESSION_H
#define SESSION_H
#include <QDebug>
#include <QTcpSocket>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlDatabase>

struct ClientAuth{
    int id;
    QString name;
};

class session: public QObject
{
    Q_OBJECT
public:
    session(QTcpSocket *socket);
    ClientAuth client;
private:
    QTcpSocket *socketSession;
    int codeCommand;
    QString login;
    QString pass;
//    QByteArray replayToClient;
//    QSqlDatabase dbChat;
    void commandHandler();
    void setCodeCommand();
    QByteArray readFromDB();
//    void connectChatToDB();
    ClientAuth getClient();

signals:
    void connectClosed();
//    void signalConnectDB();
};

#endif // SESSION_H

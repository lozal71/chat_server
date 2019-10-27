#ifndef PROTOCOLIN_H
#define POROTOCOLIN_H
#include <QDataStream>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariantMap>
#include <QDebug>

class protocolIn: public QObject
{
public:
    protocolIn();
    protocolIn(QTcpSocket *socket);
    int getCode();
    QString getLoginClient();
    QString getPassClient();
private:
    int codeCommand;
    QString loginClient;
    QString passClient;
    QJsonObject jsonDataClient;
    QByteArray getMessage(QTcpSocket *socket);
};

#endif // POROTOCOLIN_H

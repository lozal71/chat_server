#include "protocol_in.h"

protocolIn::protocolIn(){
}

protocolIn::protocolIn(QTcpSocket *socket)
{
    if (socket->bytesAvailable()< 4){
        codeCommand = 0;
    }
    else{
        QByteArray mess = getMessage(socket);
        qDebug() << "mess" << mess;
        //QDataStream stream(mess);
        QString jsonStr(mess);
        //stream >> jsonStr;
        qDebug() << "jsonStr" << jsonStr;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonStr.toUtf8());
        QJsonObject jsonObj = jsonDoc.object();
//        for (const QString& eachKey : jsonObj.keys())
//        {
//            qDebug() << eachKey << "=" << jsonObj.value(eachKey).toString();
//        }
        QVariantMap Map = jsonObj.toVariantMap();
        //qWarning() << "Test: " << Map["ID"].toString();
        codeCommand = Map["1"].toInt();
        dataCommand = Map["2"].toString();
    }
}

int protocolIn::getCode()
{
    return codeCommand;
}

QString protocolIn::getData()
{
    return dataCommand;
}

QByteArray protocolIn::getMessage(QTcpSocket *socket)
{
    quint32 packageSize; // размер приходящего сообщения
    quint32 buffer_length = 0;
    qint64 bytes_in_socket = 0;
    QByteArray buffer;
    buffer = socket->read(4); // считываем 4 байта
    QDataStream stream(buffer);
    stream >> packageSize; // получаем размер приходящего сообщения
    qDebug() << "packageSize " << packageSize;
    packageSize+=4;
    buffer.clear();
    buffer_length = quint32(buffer.length());
    bytes_in_socket = socket->bytesAvailable();
    // пока не получены все байты сообщения
    while (true) {
        // если число доступных байт в сокете меньше или равно, чем ожидаемый остаток
        if (bytes_in_socket <= packageSize - buffer_length){
            buffer.append(socket->readAll()); // читаем из буфера все, что есть
            //qDebug() << "buffer в цикле  " << buffer;
        }
        else  {
                // читаем только остаток
                buffer.append(socket->read(packageSize - buffer_length));
            }
        buffer_length += quint32(buffer.length());
        bytes_in_socket = socket->bytesAvailable();
        if (buffer_length == packageSize) break;
    }
    buffer.remove(0, 4);
    //qDebug() << "buffer в конце " << buffer;

    return buffer;
}

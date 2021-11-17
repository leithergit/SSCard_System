#ifndef QPINKEYBROAD_H
#define QPINKEYBROAD_H

#include <QObject>
#include <memory>
using namespace std;

class QPinKeybroad:public QObject
{
    Q_OBJECT
public:
    QPinKeybroad(QString strDevPort,ushort nBaudrate = 9600,QObject *parent = nullptr);
    int     OpenDevice(QString &strError);
    int     CloseDevice(QString &strError);
    int     ReadPin(uchar *pKeyValue);
    bool    m_bDevOpened = false;
    QString  m_strDevPort;
    ushort  m_nBaudreate = 9600;
signals:
    void InputPin(uchar ch);
};

using QPinKeybroadPtr = shared_ptr<QPinKeybroad>;
#endif // QPINKEYBROAD_H

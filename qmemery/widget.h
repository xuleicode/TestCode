#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtGui>
#include<QLineEdit>
#include <QPushButton>
#include <QLabel>


struct frame {
	UINT64 time;
	UINT16 width;
	UINT16 height;
	UINT8 channel;
	UINT8 datas[1];
};
class WorkThread;
class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();
private:

    QLineEdit *editKey;
    QLineEdit *datasizeh;
    QLineEdit *datasizeW;
    QLineEdit *Channel;

    QPushButton *btncreate;
    QPushButton *btnwrite;
    QPushButton *btnsend;
    QPushButton *btnread;

    QLabel *label;
    QLabel *label2;
    WorkThread *pthread;


    void initUI();
public:
    frame *m_sendData;
    frame *m_recvData;

    QMutex m_mutex;
    QWaitCondition m_wait;
    QSharedMemory *sharememory;
    QBuffer m_buffer;

    QSharedMemory* initMemory(const QString &key, unsigned int size = 1024*1024, bool create = true, QString *errorstr = nullptr);
    bool attachMemory(QSharedMemory *ptrShareMemory);
private slots:
    void loadFromFile();
    void loadFromMemory();
    void sendData();
    void slot_time();
    void creatMem();
};

#endif // WIDGET_H

#include "widget.h"
#include <QSharedMemory>
#include <QHBoxLayout>
#include <QThread>
#include <QDebug>
#include <QFileDialog>

Widget *pThis = NULL;

class WorkThread:public QThread{
public:
    void stop(){
        bstop = true;
    }
    WorkThread(QObject*parent):QThread(parent),bstop(false){}
private:
    bool bstop;
protected:
	virtual void run(){
        static int c = 0;
        while(!bstop) {
            qDebug()<<"thread write==begin="<<++c;
            QMutexLocker locker(&pThis->m_mutex);
            pThis->m_wait.wait(&pThis->m_mutex);

            qDebug()<<"thread write==mid="<<c;
            pThis->sharememory->lock(); //锁定共享内存
            char *to = (char*)pThis->sharememory->data();
            if(to == NULL ) {
                qDebug()<<"error sharememory";
                sleep(1000);
            } else {
                const char *from = (const char *)pThis->m_sendData; //获取有效数据在缓冲区中的地址
                memcpy(to, from, qMin(pThis->sharememory->size(), pThis->m_sendData->height*pThis->m_sendData->width)); //将缓冲区中的数据复制到共享内存
            }
            pThis->sharememory->unlock(); //释放共享内存
            qDebug()<<"thread write==over="<<c;
        }
        qDebug()<<"thread stop==over=";
    }
};

Widget::Widget(QWidget *parent)
    : QWidget(parent),pthread(NULL),sharememory(NULL)
{
    pThis = this;
    initUI();
}
Widget::~Widget()
{
    if(sharememory)
        sharememory->detach();
    if(pthread)
        pthread->stop();
}

void Widget::initUI() {

    editKey   = new QLineEdit("rough",this);
    datasizeh = new QLineEdit("1080",this);
    datasizeW = new QLineEdit("1920",this);
    Channel   = new QLineEdit("3",this);

    datasizeh ->setValidator(new QIntValidator(0,3840));
    datasizeW ->setValidator(new QIntValidator(0,3840));
    Channel   ->setValidator(new QIntValidator(0,3840));
    editKey   ->setPlaceholderText("key(rough | precision)");
    datasizeW ->setPlaceholderText("width(1920)");
    datasizeh ->setPlaceholderText("height(1080)");
    Channel   ->setPlaceholderText("channel(0|1)");


    btncreate = new QPushButton("init", this);
    btnwrite = new QPushButton("write",this);
    btnsend = new QPushButton("send",this);
    btnread = new QPushButton("recv",this);

    label = new QLabel(this);
    QLabel *plabels = new QLabel("send:",this);
    QLabel *plabelr = new QLabel("send:",this);
    label2 = new  QLabel(this);

    QVBoxLayout *play = new QVBoxLayout(this);
    QHBoxLayout *plh = new QHBoxLayout();
    plh->addWidget(editKey);
    plh->addWidget(datasizeW);
    plh->addWidget(datasizeh);
    plh->addWidget(Channel);
    QHBoxLayout *pbtnl = new QHBoxLayout();
    pbtnl->addWidget(btncreate);
    pbtnl->addWidget(btnwrite);
    pbtnl->addWidget(btnsend);
    pbtnl->addWidget(btnread);
    play->addLayout(plh);
    play->addLayout(pbtnl);
    play->addWidget(plabels);
    play->addWidget(label);
    play->addWidget(plabelr);
    play->addWidget(label2);


    QObject::connect(btncreate,SIGNAL(clicked()),this,SLOT(creatMem()));
    QObject::connect(btnwrite,SIGNAL(clicked()),this,SLOT(loadFromFile()));
    QObject::connect(btnsend,SIGNAL(clicked()),this,SLOT(sendData()));
    QObject::connect(btnread,SIGNAL(clicked()),this,SLOT(loadFromMemory()));
}

void Widget::creatMem()
{
    QString strKey = editKey->text();
    int nw = datasizeW->text().toInt();
    int nh = datasizeh->text().toInt();
    int nchannel = Channel->text().toInt();
    int size = nh * nw * nchannel;
    if(strKey.isEmpty() || size < 1) {
        return;
    }else {
        editKey->setEnabled(false);
        datasizeh->setEnabled(false);
        datasizeW->setEnabled(false);
        btncreate->setEnabled(false);
        Channel->setEnabled(false);
    }
    m_sendData = (frame*) malloc(sizeof(frame) + size);
    m_recvData = (frame*) malloc(sizeof(frame) + size);
    m_sendData->width = nw;
    m_sendData->height = nh;
    m_sendData->channel = nchannel;
    QString strErro;
    sharememory = initMemory(strKey,size,1,&strErro);
    qDebug()<<strErro;
    pthread = new WorkThread(this);
    pthread->start();
    m_buffer.open(QBuffer::ReadWrite); //构建并打开数据缓冲区，访问方式为读写

}
void Widget::sendData()
{
    if(!m_mutex.tryLock()) {
        static int b = 0;
        qDebug()<<"trylock return==="<<++b;
        return;
    }
    m_sendData->time = QDateTime::currentDateTime().toTime_t();
    m_wait.wakeAll();
    m_mutex.unlock();
    static int a = 0;
    qDebug()<<"write unlock=="<<++a;
}
void Widget::slot_time()
{
    static int index = 0;

    if(!m_mutex.tryLock()) {
        static int b = 0;
        qDebug()<<"trylock return==="<<++b;
        return;
    }
    m_sendData->time = QDateTime::currentDateTime().toTime_t();
    memset(m_sendData->datas, index, m_sendData->width*m_sendData->height*m_sendData->channel);
    m_wait.wakeAll();
    m_mutex.unlock();
    static int a = 0;
    qDebug()<<"write unlock=="<<++a;

    ++index;
}


QSharedMemory *Widget::initMemory(const QString &key, unsigned int size, bool create, QString *errorstr) {
    QSharedMemory *ptrShareMemory = new QSharedMemory(key,this);
    if(nullptr == ptrShareMemory) {
        return nullptr;
    }
    if(create) {
        if (!ptrShareMemory->create(size)) {
            if(ptrShareMemory->error() != QSharedMemory::AlreadyExists) {
                if(errorstr) *errorstr = ptrShareMemory->errorString();
                return nullptr;
            }
        }
    }
    if(!attachMemory(ptrShareMemory)){
        if(errorstr) *errorstr = ptrShareMemory->errorString();
    }
    return ptrShareMemory;
}
bool Widget::attachMemory(QSharedMemory *ptrShareMemory) {
    if(ptrShareMemory == NULL) {
        return false;
    }
    if(!ptrShareMemory->isAttached()) {
        if(!ptrShareMemory->attach()) {
            return false;
        }
    }
    return true;
}

//载入图片按钮响应函数
void Widget::loadFromFile()
{
    if(sharememory==NULL) {
        label->setText(QString::fromLocal8Bit("请先初始化共享内存"));
        return;
    }
    label->setText(QString::fromLocal8Bit("请选择一张图片"));
    QString filename = QFileDialog::getOpenFileName(this,"打开",QString(),tr("Image (*.png *.xpm *.jpg)"));
   // qDebug()<<"新建缓冲区的大小为："<<m_buffer.size(); //测试缓冲区大小(一般为0)
    QImage image;
    if(!image.load(filename)) {
        label->setText(QString::fromLocal8Bit("您选择的不是图片文件，请重新选择"));
        return;
    }
    static int i = 0;
    QMutexLocker locker(&m_mutex);
    label->setPixmap(QPixmap::fromImage(image));
    m_buffer.buffer().clear();
    QDataStream in(&m_buffer); //建立数据流对象，并和缓冲区相关联
    qDebug()<<m_buffer.size();
    in << ++i;
    in << "++i";
    in << image; //向缓冲区写入数据
    qDebug()<<m_buffer.size();
    memcpy(m_sendData->datas,m_buffer.data().data(), qMin((qint64)m_sendData->width*m_sendData->height, m_buffer.size()));
}
//显示图片按钮响应函数
void Widget::loadFromMemory()
{
    //对共享内存进行读写操作
    sharememory->lock(); //锁定共享内存
    //初始化缓冲区中的数据，setData函数用来初始化缓冲区。
    //该函数如果在open()函数之后被调用，则不起任何作用。
    //buffer.open(QBuffer::ReadOnly); //解除注释，则setData函数不起作用，无法加载内存中数据
    memcpy(m_recvData,(void*)sharememory->constData(), qMin((qint64)m_recvData->width*m_recvData->height,m_buffer.size()));
    qDebug()<< sharememory->size();
    sharememory->unlock(); //释放共享内存
    qDebug()<<m_recvData->time<<m_recvData->width;
}

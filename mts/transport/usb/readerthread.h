#ifndef READERTHREAD_H
#define READERTHREAD_H

#include "ptp.h"
#include <QThread>

enum mtpfs_status {
    MTPFS_STATUS_OK,
    MTPFS_STATUS_BUSY,
    MTPFS_STATUS_TXCANCEL
};

class QMutex;

class ControlReaderThread : public QThread {
    Q_OBJECT
public:
    ControlReaderThread(int fd, QObject *parent);
    ~ControlReaderThread();
    void run();

    pthread_t m_handle;
public slots:
    void setStatus(enum mtpfs_status status);

private:
    void handleEvent(struct usb_functionfs_event *event);
    void setupRequest(void *data);
    void sendStatus(enum mtpfs_status status);

    int m_fd;
    int m_state;
    enum mtpfs_status m_status;
signals:
    void startIO();
    void stopIO();
    void clearHalt();
    void deviceReset();
    void transferCancel();
};

class OutReaderThread : public QThread {
    Q_OBJECT
public:
    OutReaderThread(QMutex *mutex, QObject *parent);
    ~OutReaderThread();

    void setFd(int fd);
    void run();

    pthread_t m_handle;
signals:
    void dataRead(char *buffer, int size);

private:
    QMutex *m_lock;
    int m_fd;
};

class InWriterThread : public QThread {
    Q_OBJECT
public:
    InWriterThread(QObject *parent);

    void setData(int fd, const quint8 *buffer, quint32 dataLen, bool isLastPacket, QMutex *sendLock);
    void run();
    bool getResult();

    pthread_t m_handle;
private:
    const quint8 *m_buffer;
    quint32 m_dataLen;
    QMutex *m_lock;
    int m_fd;
    bool m_isLastPacket;
    bool m_result;
};

#endif
#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <QMutex>
#include <QThread>
#include <QQueue>
#include <QStringList>

class Dispatcher : public QThread
{
  Q_OBJECT

public:
  Dispatcher(int twait=1000, QObject *parent = 0);
  void run();                   //!< Run the message dispather thread
  void addMessage(const QString &, const QString &); //!< Add a message to the dispatcher queue
  int numPending();             //!< Return the number of message waiting in the dispatcher queue
  QString current();            //!< Return the most recently dispatched message
  void setCadence(const int &); //!< Set the dispatch cadence in milliseconds
  int cadence();                //!< Return the dispatch cadence in milliseconds
  QStringList queueList();      //!< Return the contents of the dispatcher queue as a QStringList

public slots:
  void abort();                 //!< Abort and clear the dispatcher queue

signals:
  void dispatch(const QString &, const QString &);  //!< Message ready to dispatch
  void error(const QString &);         //!< Error message to emit
  void queueChanged();                 //!< The queue has changed (new entries, or queue advanced)

private:
  QMutex mutex;              //!< Mutex for the dispatcher thread
  QQueue<QString> msgQueue;  //!< Message queue
  QString currentMsg;        //!< The current message
  bool abortQueue;           //!< Dispatcher abort flag (false=keep going, true=abort and clear)
  int  dispatchCadence;      //!< Message dispatch cadence (time to wait between dipatches) in msec
};

#endif

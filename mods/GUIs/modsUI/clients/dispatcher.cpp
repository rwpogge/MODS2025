#include "dispatcher.h"

// The thread constructor

Dispatcher::Dispatcher(int twait, QObject *parent) 
  : QThread(parent), abortQueue(false), dispatchCadence(twait)
{
}

// Add a message to the dispatch queue.  For simplicity, we store it
// as "host msg ..." and then split it apart later as needed

void Dispatcher::addMessage(const QString &host, const QString &msg)
{
  QMutexLocker locker(&mutex);
  if (abortQueue) abortQueue = false;
  msgQueue.enqueue(QString("%1 %2").arg(host,msg));
  if (!isRunning()) start();   // restart the queue thread if inactive

  emit queueChanged();
}

// The thread run() function

void Dispatcher::run()
{
  QString msgStr;

  forever {
    mutex.lock();
    if (msgQueue.isEmpty()) {
      mutex.unlock();
      break;
    }
    if (abortQueue) {
      msgQueue.clear();  // clear the queue
      mutex.unlock();
      emit queueChanged();
      emit error("Message Queue cleared and aborted");
      break;
    }

    // Get the next message in the queue

    msgStr = msgQueue.dequeue();  
    currentMsg = msgStr;
    emit queueChanged();
    mutex.unlock();

    // Send the message then pause for the requested time

    if (!msgStr.isEmpty()) {
      QString hostAddr = msgStr.section(" ",0,0);
      QString msgBody = msgStr.section(" ",1,-1);
      emit dispatch(hostAddr,msgBody);
      if (dispatchCadence > 0) 
	msleep((unsigned long)(dispatchCadence));
    }
  }

}

// Return the number of pending message in the dispatch queue

int Dispatcher::numPending()
{
  QMutexLocker locker(&mutex);
  return msgQueue.size();
}

// Return the current message (last message dispatched)

QString Dispatcher::current()
{
  QMutexLocker locker(&mutex);
  return currentMsg;
}

// Abort the message queue.  Implemented as a public slot so we can
// bind this slot to signals from, for example, an queue abort button
// in the parent application.

void Dispatcher::abort()
{
  QMutexLocker locker(&mutex);
  abortQueue = true;
}

// Set the dispatch cadence (time between queued message dispatches)
// in milliseconds

void Dispatcher::setCadence(const int &msec)
{
  QMutexLocker locker(&mutex);
  if (msec > 0) dispatchCadence = msec;
}

// Return the dispatch candence in milliseconds

int Dispatcher::cadence()
{
  QMutexLocker locker(&mutex);
  return dispatchCadence;
}

// Return the current contents of the dispatch queue

QStringList Dispatcher::queueList()
{
  QMutexLocker locker(&mutex);
  QStringList queue;
  queue.clear();
  if (msgQueue.isEmpty()) return queue;
  for (int i=0;i<msgQueue.size();i++) 
    queue.insert(i,msgQueue.at(i));
  return queue;
}


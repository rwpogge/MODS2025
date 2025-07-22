#ifndef ISISCLIENT_H
#define ISISCLIENT_H

#include <QWidget>
#include <QUdpSocket>

class QString;
class QUdpSocket;
class QMessageBox;

class ISISClient : public QWidget
{
  Q_OBJECT

public:
  ISISClient(const QString &serverID, 
	     const QString &, const int &, 
	     const QString &, const int &,
	     QWidget *parent = 0);

  bool init();           //!< Create the client socket and bind it to its port
  bool isReady();        //!< Is the socket initialized and ready to use?
  bool send(const QString &, const QString &);   //!< Send a datagram to a host
  bool sendAndWait(const QString &, const QString &, const int &); //!< Send a datagram and wait for a reply
  void setClient(const QString &, const int &);  //!< Set the ISIS client ID and port
  QString id();          //!< Return the client ID
  int port();            //!< Return the client port
  void setServer(const QString &, const QString &, const int &);  //!< Set the ISIS server ID, host, and port number
  QString isisID();      //!< Return the ISIS server ID
  QString isisHost();    //!< Return the ISIS server host
  int isisPort();        //!< Return the ISIS server port
  QString text();        //!< Return full text (routing+type+body) of the most recent datagram 
  bool parse(const QString &);  //<! Validate and parse an IMPv2 message string into components 
  QString sender();      //!< Return the sender ID of the most recent datagram
  QString recipient();   //!< Return the recipient ID of the most recent datagram
  QString body();        //!< Return the message body of the most recent datagram
  bool isValid();        //!< Was the most recent datagram a valid IMPv2 message?
  enum msgType {
    UNKNOWN, //!< Unknown message type
    REQ,     //!< Command request
    EXEC,    //!< Executive override command request
    DONE,    //!< Command Completion acknowledgment
    STATUS,  //!< Status message
    WARNING, //!< Warning message
    ERROR,   //!< Error message
    FATAL    //!< Fatal Error message
  };
  int type();        //!< Return the message type code of the most recent datagram (see msgType)
  QString errorString(); //!< Return the most recent error string
  int getKeys(const QString &, QHash<QString,QString> *);  //!< Extract all key=val pairs from a message string and load into a hash table

signals:
  void dataReady();     //!< Signal that data is ready to process
  void logMessage(const QString &);  //!< Signal that we have sent/received a command string

private slots:
  void readDatagram();  //!< Read a pending datagram on our ISIS client socket

private:
  QUdpSocket *udpSocket;   //!< ISIS client UDP socket
  QString clientID;        //!< ISIS client ID
  int clientPort;          //!< ISIS client Port
  bool isConnected;        //!< Is the client socket connected (bound) to its port?
  
  QString isisServerID;         //!< ISIS server ID
  QString isisServerHost;       //!< ISIS server hostname
  QHostAddress *isisServerAddr; //!< ISIS server host address
  int isisServerPort;           //!< ISIS server port number

  QString datagramStr;     //!< Most recent datagram
  QString senderID;        //!< Sender of the most recent datagram
  QString recipientID;     //!< Intended recipient of the most recent datagram
  QString messageBody;     //!< Body of the most recent datagram
  int messageType;         //!< Message type code (see msgType enum)
  bool isIMPv2;            //!< Message has proper IMPv2 format
  QHostAddress senderHost; //!< Host address of the most recent datagram sender
  quint16 senderPort;      //!< Port number of the most recent datagram sender

  QString errorStr;        //!< Most recent error message

};

#endif

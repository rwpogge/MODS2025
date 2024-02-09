/*!
  \brief ISIS UDP socket client

  This class defines an instance of ISIS UDP socket client.  It
  provides data and methods for sending and receiving IMPv2 messages
  to and from an ISIS server and other ISIS nodes.  It also provides a
  method for parsing message body contents into a hash table by
  keyword=value pairs, but otherwise it does no interpretation of the
  incoming message beyond the most basic task of identifying the
  message type and teasing out its component parts.

*/

#include <QtWidgets>
#include <QtNetwork>

#include "isisclient.h"

ISISClient::ISISClient(const QString &serverID,
		       const QString &serverHost, 
		       const int &serverPort, 
		       const QString &myID, 
		       const int &myPort,
		       QWidget *parent)
  : QWidget(parent)
{

  // ISIS server host address, port, and ID
	   
  isisServerHost = serverHost;
  if (serverHost.compare("localhost",Qt::CaseInsensitive)==0)
    isisServerAddr = new QHostAddress(QHostAddress::LocalHost);
  else
    isisServerAddr = new QHostAddress(serverHost);

  isisServerPort = serverPort;
  isisServerID = serverID;

  // This client's ID and socket port (localhost is implicit)

  clientID = myID;
  clientPort = myPort;

  // Basic initializations

  datagramStr.clear();
  errorStr.clear();
  senderID.clear();
  isConnected = false;  // socket not yet bound to the client port
  udpSocket = 0;

}

//
// init() - create the socket and bind it to the client port
//
// This is our success/fail point in establishing the client socket.
// Also connects the readyRead() signal to the readDatagram() slot.
//

bool ISISClient::init()
{
  // Create the client socket

  if (!udpSocket)
    udpSocket = new QUdpSocket(this);

  // Bind this client socket to clientPort, where clientPort=0 is
  // interpreted as "any free port" on localhost.  

  if (!udpSocket->bind(clientPort)) {
    errorStr = "Could not bind client socket on port " +
      QString::number(clientPort)+" - "+udpSocket->errorString();
    isConnected = false;
    return false;
  }
  isConnected = true;

  // Connect the readyRead() signal to the getDatagram() slot

  connect(udpSocket,SIGNAL(readyRead()),this,SLOT(readDatagram()));

  return true;
}

//
// isReady() - is the socket initialized and ready to use?
//

bool ISISClient::isReady()
{
  return isConnected;
}

//
// send() - Send a datagram message string in IMPv2 protocol format
// 

bool ISISClient::send(const QString &toAddr, const QString &msgStr)
{
  // Do not send empty (null) message strings

  if (msgStr.isEmpty()) {
    errorStr = "Datagram message is empty, nothing to send";
    return false;
  }
  if (toAddr.isEmpty()) {
    errorStr = "Destination address is empty, nobody to send a message to";
    return false;
  }

  // We can't send messages unless we've initialized the client UDP socket

  if (!isConnected) {
    errorStr = "No Client socket initialized";
    return false;
  }

  // We have something to send and an initialized socket, go for it

  QByteArray datagram = "";

  // Build the IMPv2 datagram.  We do this quite explicitly here...

  datagram.append(clientID.toUtf8());
  datagram.append(">");
  datagram.append(toAddr.toUpper().toUtf8());  // upcase the destination address
  datagram.append(" ");
  datagram.append(msgStr.toUtf8());
  datagram.append("\r");

  // ...and send it

  udpSocket->writeDatagram(datagram.data(),
                           datagram.size(),
                           *isisServerAddr,
                           isisServerPort);

  // log message signal

  emit logMessage(QString(datagram).trimmed());

  // all done!

  return true;

}

//
// sendAndWait() - Send a datagram string to the ISIS server
//                 and wait for the readyRead signal or timeout
// 

bool ISISClient::sendAndWait(const QString &toAddr, const QString &msgStr, 
                             const int &timeout)
{
  // Send the datagram via our send() method...

  if (!send(toAddr,msgStr)) return false;

  // ...and wait up to timeout milliseconds for a ReadyRead signal.

  if (timeout>0) {
    qApp->processEvents();
    if (!udpSocket->waitForReadyRead(timeout)) {
      errorStr = "Timeout on datagram socket reply wait";
      return false;
    }
  }
  errorStr.clear();
  return true;
}

//
// readDatagram() slot - read a pending datagram from the socket
//
// This is invoked by the udpSocket object's readyRead() signal.  It
// reads the pending datagram from the client socket, sets the
// datagramStr datum, and emits the client's dataReady signal.
//
// It also parses the datagram into components and validates it,
// making these data available through the various methods in this
// class provided for that purpose, though a user can elect to
// separately parse the entire message independently.
//

void ISISClient::readDatagram()
{
  datagramStr.clear();

  if (udpSocket->hasPendingDatagrams()) {
    QByteArray datagram;

    datagram.resize(udpSocket->pendingDatagramSize());
    udpSocket->readDatagram(datagram.data(),datagram.size());

    datagramStr.append(datagram);

    // Strip the datagram of extraneous whitespace & terminators

    datagramStr = datagramStr.trimmed();

  }

  parse(datagramStr); // parse it into components and validate it

  emit dataReady();
  emit logMessage(datagramStr);
}

//
// text() - Return the full text of the most recent datagram text
//

QString ISISClient::text()
{
  return datagramStr;
}

//
// parse() - Parse an IMPv2 message into components, with syntax validation
//
// An IMPv2 protocol message has the following structure
//
//    fromID>destID Type: body of the message
//
// where:
//   fromID = ID of the sender
//   destID = ID of the recipient
//   Type = type of message, one of REQ:, EXEC:, DONE:, STATUS:, WARNING:, 
//          ERROR:, or FATAL:
//   followed by the body of the message
//
// Note that if there is no message type it is interpreted as an
// implicit REQ: (command request) message.
//

bool ISISClient::parse(const QString &msgStr)
{
  // Initializations

  messageType = UNKNOWN;
  messageBody.clear();
  senderID.clear();
  recipientID.clear();
  isIMPv2 = false;
  
  // Message addressing (routing) token
  
  QString msgAddr = msgStr.section(" ",0,0);
  
  // Does the address token have a >?  This is our format validator
  
  if (msgAddr.contains(">",Qt::CaseInsensitive)) {
    senderID = msgAddr.section(">",0,0);
    recipientID = msgAddr.section(">",1,1);
    isIMPv2 = true;
  }
  else {
    errorStr = "Message does not have valid IMPv2 format";
    isIMPv2 = false;
    return false;
  }

  // We have a valid message, determine the message type and extract
  // the body of the message

  QString typeStr = msgStr.section(" ",1,1);  // Message type token
  messageBody = msgStr.section(" ",2,-1);     // Body of the message

  if (typeStr.compare("ERROR:",Qt::CaseInsensitive)==0)
    messageType = ERROR;
  else if (typeStr.compare("WARNING:",Qt::CaseInsensitive)==0)
    messageType = WARNING;
  else if (typeStr.compare("FATAL:",Qt::CaseInsensitive)==0)
    messageType = FATAL;
  else if (typeStr.compare("DONE:",Qt::CaseInsensitive)==0)
    messageType = DONE;
  else if (typeStr.compare("STATUS:",Qt::CaseInsensitive)==0)
    messageType = STATUS;
  else if (typeStr.compare("EXEC:",Qt::CaseInsensitive)==0)
    messageType = EXEC;
  else if (typeStr.compare("REQ:",Qt::CaseInsensitive)==0)
    messageType = REQ;
  else { // implicit REQ:
    messageType = REQ;
    messageBody = msgStr.section(" ",1,-1);     // Body of the message
  }  

  return true;

}

//
// from() - Return the sender ID of the most recent datagram
//

QString ISISClient::sender()
{
  return senderID;
}

//
// dest() - Return the destination ID of the most recent datagram
//

QString ISISClient::recipient()
{
  return recipientID;
}

//
// type() - Return the message type code of the most recent datagra
//

int ISISClient::type()
{
  return messageType;
}

//
// body() - Return the message body of the most recent datagra
//

QString ISISClient::body()
{
  return messageBody;
}

//
// isValid() - Was the most recent datagram a valid IMPv2 message
//

bool ISISClient::isValid()
{
  return isIMPv2;
}

//
// setClient() - Set the client ID (IMPv2 node name) and port
//

void ISISClient::setClient(const QString &idStr, const int &portNum)
{
  clientID = idStr;
  clientPort = portNum;
}

// id() - Return the client ID (IMPv2 node name)

QString ISISClient::id()
{
  return clientID;
}

// port() - Return the client port number

int ISISClient::port()
{
  return clientPort;
}

//
// setServer() - Set the ISIS server host and port settings
//

void ISISClient::setServer(const QString &idStr, const QString &hostStr, 
                           const int &portNum)
{
  isisServerID = idStr;
  isisServerHost = hostStr;
  if (isisServerHost.compare("localhost",Qt::CaseInsensitive)==0)
    isisServerAddr = new QHostAddress(QHostAddress::LocalHost);
  else
    isisServerAddr = new QHostAddress(isisServerHost);
  isisServerPort = portNum;
}

//
// isisID() - Return the ISIS server ID (IMPv2 node name)
//

QString ISISClient::isisID()
{
  return isisServerID;
}

//
// isisHost() - Return the ISIS server hostname
//

QString ISISClient::isisHost()
{
  return isisServerHost;
}

//
// isisPort() - Return the ISIS server port number
//

int ISISClient::isisPort()
{
  return isisServerPort;
}

//
// errorString() - Return the most recent error string
//

QString ISISClient::errorString()
{
  return errorStr;
}

//
// getKeys - Extract key=value pairs from an IMPv2 message string and 
//           load them into keyword dictionary hash table.
//
// IMPv2 format strings can pass keyword=value and +/- boolean flags
// as follows:
//
//    integer:  keyword=123
//    double:   keyword=1.234
//              keyword=1.234E+05
//    string:   keyword=abcdefg
//              keyword='a string of words separated by spaces'
//              keyword=(a string of words separated by spaces) <-- old-style
//    boolean:  keyword=T
//              +keyword  (same as keyword=T)  <-- old-style
//              -keyword  (same as keyword=F)  <-- old-style
//    list:     keyword=(1,2,3,4)
//
// Returns the number of keys found in the msgStr.  Note that it does
// not clear the hash table it is given as the container for the
// keyword dictionary it extracts, so that this hash can used to
// accumulate a keyword dictionary for an entire session if needed.
// How to handle the hash table is left to the calling program - we
// just fill the container they provide a pointer to.
//

int ISISClient::getKeys(const QString &msgStr, QHash<QString,QString> *hash)
{
  // We work with a copy of the string, aggressivly stripping it of
  // extranous white space, tabs, terminators, etc. to make processing
  // into keywords simpler.

  QString testStr = msgStr;
  testStr = testStr.simplified(); // TODO: Evaluate this line. Why is it here? 

  QStringList argList = testStr.split(" ");
  if (argList.size() == 0) return 0;

  // We have at least 1 word to check...

  bool quoted = false;
  QString closeQuote;
  int numKeys = 0;
  int i = 0;

  while (i<argList.size()) {

    // Is this argument a keyword=value pair?

    if (argList.at(i).contains("=",Qt::CaseInsensitive)) {
      quoted = false;
      QString newKey = argList.at(i).section("=",0,0);
      QString newVal = argList.at(i).section("=",1,-1);

      // Look for a single quote or ( in the first character of newVal
      // to see if this is a multi-word string value

      if (newVal.startsWith("'")) {  // single-quote delimited string value
	if (newVal.endsWith("'")) 
	  quoted = false;  // single word inside single quotes
	else {
	  quoted = true;
	  closeQuote = "'";
	}
	newVal.remove("'");
      }
      else if (newVal.startsWith("(")) { // () delimited string value
	if (newVal.endsWith(")")) {
	  quoted = false;  // single word inside ()s
	  newVal.remove(")");
	}
	else {
	  quoted = true;
	  closeQuote = ")";
	}
	newVal.remove("(");
      }
      else {  // single-word value
	quoted = false;
      }

      // If a quoted multi-word string value, build it up until we
      // find the closing quote character

      while (quoted) {
	i++;
	if (i < argList.size()) {
	  QString nextStr = argList.at(i);
	  if (nextStr.endsWith(closeQuote)) {  // closing quote found
	    nextStr.remove(closeQuote);
	    quoted = false;
	  }
	  newVal.append(" ");
	  newVal.append(nextStr);
	}
	else
	  quoted = false;   // premature end of value
      }

      // Add the keyword and its value to the hash

      hash->insert(newKey,newVal);
      numKeys++;
    }

    // Is this argument a +key boolean?

    else if (argList.at(i).size()>1 && argList.at(i).startsWith("+") ){
      QString newKey = argList.at(i);
      newKey.remove("+");
      hash->insert(newKey,"T");
      numKeys++;
    }

    // Is this argument a -key boolean?

    else if (argList.at(i).size()>1 && argList.at(i).startsWith("-")) {
      QString newKey = argList.at(i);
      newKey.remove("-");
      hash->insert(newKey,"F");
      numKeys++;
    }

    i++;  // increment the argument counter

  }

  // All done, return the number of keys we found

  return numKeys;

}

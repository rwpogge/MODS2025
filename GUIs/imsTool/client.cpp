// Client for microLynx Controller (MLC)
// Qt tools
//
// Raymond Gonzalez, OSU Dept. of Astronomy
// rgonzale@astronomy.ohio-state.edu
// 2011 July 28
// 2012 April 30 - Made clearNVM saver by adding a warning [rdg]

#ifdef MECHERRORS_H
 #include <mechErrors.h>
#endif

#include <client.h>
#include <unistd.h>

Client::Client( const QString &host, const QString &port )
{

  maxCmds = 200;

  connection = new QLabel();

  textEdit = new QTextEdit( this );
  textEdit->setMinimumSize(400,400);
  setCentralWidget(textEdit);
  
  Client::setStyleSheet("QPushButton { min-width: 1px; min-height: 10px; }");

  // Quit this GUI

  quit = new QPushButton( tr("Exit") );
  connect( quit, SIGNAL(clicked()), this, SLOT(close()) );
  
  findtext = new QLineEdit();
  connect( findtext, SIGNAL(returnPressed()), this,SLOT(findText()));

  // Upload microcode

  upload = new QAction( tr("upLoad"), this );
  connect( upload, SIGNAL(triggered()), this, SLOT(upLoad()) );

  // Download microcode

  download = new QAction( tr("downLoad"), this );
  connect( download, SIGNAL(triggered()), this, SLOT(downLoad()) );
  
  // Get IP address from User

  ipLabel = new QLabel( tr(" IP: ") );
  inputHost = new QLineEdit();
  connect( inputHost, SIGNAL(returnPressed()),SLOT(openConnection()));
  
  //  Get Socket from User

  socketLabel = new QLabel(tr(" Socket: ") );
  inputPort = new QLineEdit();
  connect( inputPort, SIGNAL(returnPressed()),SLOT(openConnection()));
  
  openIPSocket = new QAction( tr("Open Socket"),this );
  openIPSocket->setEnabled(true);
  connect( openIPSocket, SIGNAL(triggered()), SLOT(openConnection()) );

  if ( host != "" && port != "" ) {
    inputHost->setText( host );
    inputPort->setText( port );

    hostEdit = inputHost;
    socketEdit = socketEdit;

    openIPSocket->setEnabled(false);
    connection->setText("Not Connected");

  } else {
    openIPSocket->setEnabled(true);
    connection->setText("Not Connected");

  }

  socket = new QTcpSocket();
  connect( socket, SIGNAL(connected()), SLOT(socketConnected()) );
  connect( socket, SIGNAL(readyRead()), SLOT(socketReadyRead()) );
  connect( socket, SIGNAL(error(QAbstractSocket::SocketError)),
	   this, SLOT(socketErr(QAbstractSocket::SocketError)));

  // Connect to microLynx controller

  socket->connectToHost( inputHost->text(), inputPort->text().toInt());
  commandEntry = new QStringList;
  commandLine = new QLineEdit;
  inputCommand = new QLineEdit();
  inputCommand->setMinimumSize(365,20);
  connect( inputCommand, SIGNAL(returnPressed()), this, SLOT(sendToServer()));
  createActions();
  createMenus();
  createToolBars();
  createStatusBar();
  createDockWindows();
  readSettings();

  connect(textEdit->document(), SIGNAL(contentsChanged()),
	  this, SLOT(documentWasModified()));

  setCurrentFile("test.plc");
  setWindowTitle( "imsTool [*]" );
}

void Client::NVMWarning()
 {

   QMessageBox::StandardButton reply;
   reply = QMessageBox::critical(this, tr("QMessageBox::critical()"),
				 "<span style='color: red'><big>Are you sure?</big></span> This will Clear Non-Volatile Memory, erasing all microlynx code that had previously been loaded.", QMessageBox::Cancel | QMessageBox::Default, QMessageBox::Ok);

   if (reply == QMessageBox::Cancel) {

     infoText->append(  tr("NVM NOT Cleared!"));

     statusBar()->showMessage( tr("NVM '''NOT''' Cleared!"));

   } else if (reply == QMessageBox::Ok) {

     infoText->append(  tr("Clearing NVM"));
     statusBar()->showMessage(tr("User requested NVM Cleared!"));
     clearNVM();

   }

 }

void Client::createEdit()
{
  textEdit->clear();
}

 void Client::createActions()
 {
   
   //   grid->setSizeConstraint(QLayout::SetMinimumSize);

   newAct = new QAction(QIcon(":images/new.png"), tr("&New"), this);
   newAct->setShortcut(tr("Ctrl+N"));
   newAct->setStatusTip(tr("Create a new file"));
   connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

   openAct = new QAction(QIcon(":images/open.png"), tr("&Open..."), this);
   openAct->setShortcut(tr("Ctrl+O"));
   openAct->setStatusTip(tr("Open an existing file"));
   connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

   saveAct = new QAction(QIcon(":images/save.png"), tr("&Save"), this);
   saveAct->setShortcuts(QKeySequence::Save);
   saveAct->setStatusTip(tr("Save the document to disk"));
   connect(saveAct, SIGNAL(triggered()), this, SLOT(saveAs()));

   saveAsAct = new QAction(tr("Save &As..."), this);
   saveAsAct->setStatusTip(tr("Save the document under a new name"));
   connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));
   
   exitAct = new QAction(tr("E&xit"), this);
   exitAct->setShortcut(tr("Ctrl+Q"));
   exitAct->setStatusTip(tr("Exit the application"));
   connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
   
   cutAct = new QAction(QIcon(":images/cut.png"), tr("Cu&t"), this);
   cutAct->setShortcut(tr("Ctrl+X"));
   cutAct->setStatusTip(tr("Cut the current selection's contents to the "
			   "clipboard"));
   connect(cutAct, SIGNAL(triggered()), textEdit, SLOT(cut()));
   
   undoAct = new QAction(QIcon(":images/undo.png"), tr("&Undo"), this);
   undoAct->setShortcuts(QKeySequence::Undo);
   undoAct->setStatusTip(tr("Undo the last editing action"));
   connect(undoAct, SIGNAL(triggered()), this, SLOT(undo()));
   
   copyAct = new QAction(QIcon(":images/copy.png"), tr("&Copy"), this);
   copyAct->setShortcut(tr("Ctrl+C"));
   copyAct->setStatusTip(tr("Copy the current selection's contents to the "
			    "clipboard"));
   connect(copyAct, SIGNAL(triggered()), textEdit, SLOT(copy()));
   
   pasteAct = new QAction(QIcon(":images/paste.png"), tr("&Paste"), this);
   pasteAct->setShortcut(tr("Ctrl+V"));
   pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current " "selection"));
   connect(pasteAct, SIGNAL(triggered()), textEdit, SLOT(paste()));
   
   spacerWidget = new QWidget(this);
   spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

   clrNVM = new QAction( tr("clearNVM"),this );
   connect( clrNVM, SIGNAL(triggered()), this, SLOT(NVMWarning()) );
   
   closeIPSocket = new QAction( tr("Close Socket"),this );
   connect( closeIPSocket, SIGNAL(triggered()), SLOT(closeConnection()) );
   
   aboutAct = new QAction(tr("&About"), this);
   aboutAct->setStatusTip(tr("Show the application's About box"));
   connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
   
   aboutQtAct = new QAction(tr("About &Qt"), this);
   aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
   connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
   
   cutAct->setEnabled(false);
   copyAct->setEnabled(false);
   connect(textEdit, SIGNAL(copyAvailable(bool)), cutAct, SLOT(setEnabled(bool)));
   connect(textEdit, SIGNAL(copyAvailable(bool)), copyAct, SLOT(setEnabled(bool)));
   
 }

void Client::createMenus()
 {
   //   grid->setSizeConstraint(QLayout::SetMinimumSize);

  fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(newAct);
  fileMenu->addAction(openAct);
  fileMenu->addAction(saveAct);
  fileMenu->addAction(saveAsAct);
  fileMenu->addSeparator();
  fileMenu->addAction(exitAct);

  editMenu = menuBar()->addMenu(tr("&Edit"));
  editMenu->addAction(undoAct);
  editMenu->addAction(cutAct);
  editMenu->addAction(copyAct);
  editMenu->addAction(pasteAct);
  
  viewMenu = menuBar()->addMenu(tr("&View"));
  viewMenu->addAction( tr("New Edit Window") );
  
  imsTerm = new QPushButton( tr("imsTool Terminal") );
  QWidgetAction *imsTerminal = new QWidgetAction(this);
  imsTerminal->setDefaultWidget(imsTerm);
  viewMenu->addAction(imsTerminal);
  connect(imsTerm, SIGNAL(clicked()), this, SLOT(imsTerminal()));

  transferMenu = menuBar()->addMenu(tr("&Transfer"));
  transferMenu->addAction(upload);
  //  transferMenu->addAction(download);

  
  menuBar()->addSeparator();

  helpMenu = menuBar()->addMenu(tr("&Help"));
  helpMenu->addAction(aboutAct);
  helpMenu->addAction(aboutQtAct);

 }

 void Client::createToolBars()
 {
   //   grid->setSizeConstraint(QLayout::SetMinimumSize);

  fileToolBar = addToolBar( tr("File") );
  fileToolBar->addAction(newAct);
  fileToolBar->addAction(openAct);
  fileToolBar->addAction(saveAct);

  editToolBar = addToolBar( tr("Edit") );
  editToolBar->addAction(cutAct);
  editToolBar->addAction(copyAct);
  editToolBar->addAction(pasteAct);

  QLabel *findLabel  = new QLabel( tr("Find") );
  editToolBar->addWidget(findLabel);

  findAct  = new QWidgetAction(this);
  findAct->setDefaultWidget( findtext );

  editToolBar->addAction(findAct);
  
  editToolBar->addWidget(spacerWidget);
  editToolBar->addSeparator();

  hostAct  = new QWidgetAction(this);
  hostAct->setDefaultWidget( inputHost );
  hostEdit = new QLineEdit(inputHost->text());

  socketAct = new QWidgetAction( this );
  socketAct->setDefaultWidget( inputPort );
  socketEdit = new QLineEdit(inputPort->text());

  QLabel *ipLabel  = new QLabel( tr("IP:") );
  editToolBar->addWidget(ipLabel);
  editToolBar->addAction(hostAct);

  QLabel *socketLabel  = new QLabel( tr("Socket:") );
  editToolBar->addWidget(socketLabel);
  editToolBar->addAction(socketAct);

  editToolBar->addAction(clrNVM);
  editToolBar->addAction(openIPSocket);
  editToolBar->addAction(closeIPSocket);

 }

 void Client::createStatusBar()
 {
   statusBar()->showMessage(tr("Ready"));
 }

 void Client::newFile()
 {
     if (maybeSave()) {
         textEdit->clear();
         setCurrentFile("");
     }
 }

void Client::open()
{
     if (maybeSave()) {
         newFile();
         fileName = QFileDialog::getOpenFileName(this);
         if (!fileName.isEmpty())
             loadFile(fileName);
     }
 }

 void Client::save()
 {
     QString fileName = QFileDialog::getSaveFileName(this,
                         tr("Choose a file name"), ".",
                         tr("PLC (*.plc *.txt)"));
     if (fileName.isEmpty())
         return;
     QFile file(fileName);
     if (!file.open(QFile::WriteOnly | QFile::Text)) {
         QMessageBox::warning(this, tr("Dock Widgets"),
                              tr("Cannot write file %1:\n%2.")
                              .arg(fileName)
                              .arg(file.errorString()));
         return;
     }

     QTextStream out(&file);
     QApplication::setOverrideCursor(Qt::WaitCursor);
     out << textEdit->toHtml();
     QApplication::restoreOverrideCursor();

     statusBar()->showMessage(tr("Saved '%1'").arg(fileName));
 }

 bool Client::saveit()
 {
     if (curFile.isEmpty()) {
         return saveAs();
     } else {
         return saveFile(curFile);
     }
 }

 bool Client::saveAs()
 {
     fileName = QFileDialog::getSaveFileName(this);
     if (fileName.isEmpty())
         return false;

     return saveFile(fileName);
 }

 void Client::undo()
 {
     QTextDocument *document = textEdit->document();
     document->undo();
 }

 void Client::about()
 {
    QMessageBox::about(this, tr("About imsTool"),
             tr("<b>imsTool</b> is used to communicate with a MicroLYNX"
		" motor controller, and to modify, upload, and download"
                "\nMicroLYNX program files."));
 }

 void Client::documentWasModified()
 {
   setWindowModified(textEdit->document()->isModified());
 }


// Quit event from the terminal [x] will use the same
// Close method to close all ims GUI related widgets.
//

 void Client::closeEvent(QCloseEvent *event)
 {
     if (maybeSave()) {
       writeSettings();
       event->accept();
     } else {
       event->ignore();
     }
 }

 void Client::readSettings()
 {
     QSettings settings("imsTool", "Read settings");
     QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
     QSize size = settings.value("size", QSize(400, 400)).toSize();
     resize(size);
     move(pos);
 }

 void Client::writeSettings()
 {
     QSettings settings("imsTool", "Write setttings");
     settings.setValue("pos", pos());
     settings.setValue("size", size());
 }

 bool Client::maybeSave()
 {
     if (textEdit->document()->isModified()) {
         QMessageBox::StandardButton ret;
         ret = QMessageBox::warning(this, tr("imsTool"),
                      tr("The document has been modified.\n"
                         "Do you want to save your changes?"),
                      QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
         if (ret == QMessageBox::Save)
	   return saveit();
         else if (ret == QMessageBox::Cancel)
	   return false;
     }
     return true;
 }

 void Client::loadFile(const QString &fileName)
 {
     QFile file(fileName);
     if (!file.open(QFile::ReadOnly | QFile::Text)) {
         QMessageBox::warning(this, tr("imsTool"),
                              tr("Cannot read file %1:\n%2.")
                              .arg(fileName)
                              .arg(file.errorString()));
         return;
     }

     QTextStream in(&file);
     QApplication::setOverrideCursor(Qt::WaitCursor);
     textEdit->setPlainText(in.readAll());
     QApplication::restoreOverrideCursor();

     setCurrentFile(fileName);
     statusBar()->showMessage(tr("File loaded"));
 }

void Client::imsTerminal()
{
  QTextStream os(socket);

  QString host;
  QString port;

  Client *mw = new Client( host, port );
  mw->setWindowTitle( "MicroLynx IMS Controller [*]" );
  //mw->infoText = textEdit;
  mw->show();
}

 bool Client::saveFile(const QString &fileName)
 {
     QFile file(fileName);
     if (!file.open(QFile::WriteOnly | QFile::Text)) {
         QMessageBox::warning(this, tr("imsTool"),
                              tr("Cannot write file %1:\n%2.")
                              .arg(fileName)
                              .arg(file.errorString()));
         return false;
     }

     QTextStream out(&file);
     QApplication::setOverrideCursor(Qt::WaitCursor);
     out << textEdit->toPlainText();
     QApplication::restoreOverrideCursor();

     setCurrentFile(fileName);
     statusBar()->showMessage(tr("File saved"));
     return true;
 }

 void Client::setCurrentFile(const QString &fileName)
 {
     curFile = fileName;
     textEdit->document()->setModified(false);
     setWindowModified(false);

     QString shownName;
     if (curFile.isEmpty())
         shownName = "untitled.txt";
     else
         shownName = strippedName(curFile);

     setWindowTitle(tr("%1[*] - %2").arg(shownName).arg(tr("imsTool")));
 }

 QString Client::strippedName(const QString &fullFileName)
 {
     return QFileInfo(fullFileName).fileName();
 }

void Client::closeConnection()
{
  socket->close();
  
  if ( socket->state() ) { // We have a delayed close
    connect( socket, SIGNAL(delayedCloseFinished()),
	     SLOT(socketClosed()) );
    openIPSocket->setEnabled(true);
    
  } else { // The socket is closed
    socketClosed();
    openIPSocket->setEnabled(true);
    
  }
}

//
//
void Client::openConnection()
{
  socket = new QTcpSocket();
  
  connect( socket, SIGNAL(connected()), SLOT(socketConnected()) );
  connect( socket, SIGNAL(readyRead()), SLOT(socketReadyRead()) );
  connect( socket, SIGNAL(error(QAbstractSocket::SocketError)),
	   this, SLOT(socketErr(QAbstractSocket::SocketError)));

  socket->connectToHost( inputHost->text(), inputPort->text().toInt());
  openIPSocket->setEnabled(false);

  inputCommand->clear();
  inputCommand->setFocus();

}

//
//
void Client::sendToServer()
{
  QTextStream os(socket);

  inputCommand->text().replace(">","");

  fileLoader = 0;
  e = infoText;

  QTextEdit::ExtraSelection highlight;
  highlight.cursor = e->textCursor();
  highlight.format.setProperty(QTextFormat::FullWidthSelection, true);
  highlight.format.setBackground( QColor(220,220,220));
  
  QList<QTextEdit::ExtraSelection> extras;
  extras << highlight;
  e->setExtraSelections( extras );

  cmdString.append(inputCommand->text());

  cmdString = cmdString.trimmed();  // Clean up whitespace & terminators
  if ( maxCommands < maxCmds ) maxCommands++;
  else maxCommands = 0;

  commandEntry->append(cmdString);
  commandLine->setText(cmdString);

  if ( commandEntry->size() == maxCmds )
    commandEntry->removeFirst();
  
  commandNum = commandEntry->size(); // current history index

  cmdString.replace(">","");

  if ( cmdString.isEmpty() ) { os << "\r"; } 
  else { os << cmdString.append("\r"); }

  cmdString.clear();
  inputCommand->clear();
  inputCommand->setFocus();
  statusBar()->showMessage(tr("Ready"));

}

void Client::socketReadyRead()
{
 #include <microLynxErrors.h>

  quint64 numRead      = 0;
  quint64 numReadTotal = 0;
  int j                = -1;
  int err              = -1;
  QString errStr;
  QString errStr2;

  // QLineEdit *displyData = new QLineEdit();   //TODO: What was this used for?

  QTextStream os(socket);

  while ( (numRead = socket->canReadLine()) ) {
    inputCommand->clear();
    numReadTotal += numRead;
    dat += socket->readLine();
    if ( (dat.isEmpty()) && (!socket->canReadLine()) ) { 
      break;
    }

    // Select an error message, if an error(?) was detected.
    // microLynx controllers return a '?' when an error is
    // detected, imsError() prints the error#

    if ( dat.contains("?print error") ) {
      dat = socket->readLine();
      errStr = dat.section("",1,4); // get error # from the message
      dat.clear();

      // Search for the error message in the microLynxErrors.h file.

      for ( j = 0; j < 146; j++ ) {
	errStr2 = microLynxErrors[j].section("",1,4);
	if ( errStr.contains(errStr2) ) break;

      }

      // Color the error error number with the message 

      QString redError("<font color=red>ERROR #" + microLynxErrors[j] + "</font>");
      dat.append(redError);

    }
    dat.remove("\r\n");
    if ( !dat.isEmpty() ) {
      e->append(dat);
      dat.clear();
    }

  }

  // Check the inputCommand input window for prompt ">".
  
  if ( numReadTotal != 0 ) {

  // Check for Errors '?'

    dat = socket->readLine();
    dat.remove("\r\n");
    err = dat.indexOf("?",0); 
    if ( err == 0 ) {	
      imsError();

    }

    // The inputCommand works with the imsTool window, it will give the
    // prompt ">" when finished everything else will be displayed so the
    // user will know that it is still working on the command or and upload.

    inputCommand->setText(dat.trimmed());

  }

}

void Client::socketConnected()
{
  QTextStream os(socket);
  QString version;

  fileLoader = 0;  

  infoText->append( tr("imsTool IP:%1, Port:%2 Connected").arg(inputHost->text()).arg(inputPort->text()));

  connection->setText("Connected");

  // Version and Serial Number;
  infoText->append( tr(" Program Copyright 1996-2002 by:\n Intelligent Motion System, Inc.\n Marlborough, CT 06447"));

  os << "print ver,ser\r";

}

void Client::socketClosed()
{
  infoText->append( tr("imsTool IP:%1, Port:%2, Closed").arg(inputHost->text()).arg(inputPort->text()));
  connection->setText("Not Connected");

  openIPSocket->setEnabled(false);

}

void Client::socketErr(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
     case QAbstractSocket::RemoteHostClosedError:
       break;

     case QAbstractSocket::SocketAccessError:
       infoText->append( tr("ERROR: Address IP:%1, Port:%2 already in use and set to be exclusive.").arg(inputHost->text()).arg(inputPort->text()));
       statusBar()->showMessage(tr("%1").arg(socket->errorString()));       
       break;

     case QAbstractSocket::SocketTimeoutError:
       infoText->append( tr("ERROR: Socket Timeout on IP:%1, Socket:%2").arg(inputHost->text()).arg(inputPort->text()));
       statusBar()->showMessage(tr("%1").arg(socket->errorString()));       
       break;

     case QAbstractSocket::HostNotFoundError:
       infoText->append( tr("ERROR: Connection was refused. IP %1 with Port %2 was not found.").arg(inputHost->text()).arg(inputPort->text()));
       statusBar()->showMessage(tr("%1").arg(socket->errorString()));
       break;

    case QAbstractSocket::ConnectionRefusedError:
      infoText->append( tr("ERROR: Connection was refused. Make sure the IE is 'ON' and check your IP %1 and Port %2 ").arg(inputHost->text()).arg(inputPort->text()));
      statusBar()->showMessage(tr("%1").arg(socket->errorString()));
      break;

     default:
       infoText->append( tr("ERROR: Communication problems, %1  check IP %2 and/or Port %3").arg(socket->errorString()).arg(inputHost->text()).arg(inputPort->text()));
       
       statusBar()->showMessage(tr("Host and port %1").arg(socket->errorString()));
    }

}

// microLynx microcode loader
//
void Client::upLoad()
{
  e = infoText;
  // const int Timeout = 1 * 1000;  //TODO: What was this used for?
  int i = 0;
  int j = 0;
  int k = 0;

  // Get PLC microcode file

  QFile file( fileName );

  if( !file.open( QIODevice::ReadOnly | QIODevice::Text))
      return;

  QTextStream stream( &file );

  QTextStream os( socket );

  QString line;

  // Do not include header when sending the PLC microcode to the
  // microLynx controller

  // Also check if this is a microLynx microcode file if must have the
  // .plc extension and a "END HEADER" in the PLC file.

  if ( (fileName.contains(".lxt") == 1) ) {
      os << "'You are loading a .lxt extension file\r";

    ; // This file is from an engineer please just continue

  } else {
    if( (fileName.contains(".plc") == 1) ) {
      while( !stream.atEnd() ) {
	line = stream.readLine();
	if( line.contains("END HEADER") == 1 ) { fileLoader = 1; break; }
	fileLoader = 0;
      }

    } else {

      // Don't load the microcode if not .plc extension found

      os << "'Stop!!!!!!!!!!!\nOnly .plc and .lxt extensions files are allowed!\r";
      return;
    }

    if ( fileLoader == 0 ) {

      // Don't load the microcode if no 'END HEADER' was found

      os << "'Stop!!!!!!!!!\nYou MUST have an 'END HEADER' for .plc extension files before the first microLynx PLC microcode starts loading\nAn 'END HEADER' is not required for .lxt extension files. Change the extension to .lxt if you don't have a header\r";
      fileLoader = 0;
      return; 
    }
  }

  sleep(1); // Rest for a second

  // Start the sending the code to the microLynx controller

  while( !stream.atEnd() ) {

    // Wait for 100ms, so the microLynx controller can return a response
    // before sending another line.

    while ( !socket->canReadLine() ) {
      for ( i = 0; i < 100; i++ ) { os << '\0'; }
      break;
      
    }

    // Clear line and read another string

    line.clear();
    line = stream.readLine();

    k = line.length();      // string length
    j = line.indexOf("'");  // any comments (')

    // Don't send the line if:
    // 1.) first character is a comment(')
    // 2.) line has a null
    // 3.) line is empty
    // 3.) line has a (*)

    if ( (j == 0) || (line.isNull()) || (line.isEmpty()) || (line.indexOf("*",0) == 0) ) {
      continue;

    } else {

      // Not comment was found send the line as is

      if ( j == -1 ) {
      	line = line.simplified();

      	line.append("\r");
      	os << line;
	
      } else {

	// Comment found remove it and send the line

      	line.remove(j, k-j);
	line = line.simplified();

	if ( line.isEmpty() ) {
	  continue;
	} else {
	  line.append("\r");
	  os << line;

	}
      }
    }
  }

  // Give time to settle

  for ( i = 0; i < 100; i++ ) os << '\0';

  // Save the PLC microcode

  os << "save\r";

  // Give it time to return, and send a Sucess message

  for ( i = 0; i < 1000; i++ ) os << '\0';
  os << "'Sucessfully loaded\r";

  statusBar()->showMessage(tr("%1").arg(socket->errorString()));

  infoText->append( tr("upLoad for %1 Finished").arg(fileName) );

  file.close();

}

// Clear microLynx Non-Volatile Memory (NVM)

void Client::clearNVM()
{
  int i;
  // const int Timeout = 1 * 1000;  //TODO: What was this used for?

  e = infoText;
  fileLoader = 0;  

  inputCommand->clear();

  QTextStream os( socket );

  QString ipCmd = "ip all 'Initialize parameters\r";
  QString dvfCmd = "dvf 0,0,0 'Delete User Defined Variables and Flags\r";
  QString cpCmd = "cp 1,1 'Clear User Program from NVM\r";
  QString saveCmd = "save 'Save PLC Instructions\r";

  os << ipCmd;
  inputCommand->clear();
  while ( !socket->canReadLine() ) {
    for ( i = 0; i < 1000; i++ ) { os << '\0'; }
    break;
    
  }

  os << dvfCmd;
  inputCommand->clear();
  while ( !socket->canReadLine() ) {
    for ( i = 0; i < 1000; i++ ) { os << '\0'; }
    break;
    
  }

  os << cpCmd;
  inputCommand->clear();
  while ( !socket->canReadLine() ) {
    for ( i = 0; i < 2000; i++ ) { os << '\0'; }
    break;
    
  }

  os << saveCmd;
  inputCommand->clear();
  while ( !socket->canReadLine() ) {
    for ( i = 0; i < 2000; i++ ) { os << '\0'; }
    break;
    
  }

  os << "'NVM Sucessfully Cleared!\r";
  inputCommand->clear();
  while ( !socket->canReadLine() ) {
    for ( i = 0; i < 1000; i++ ) { os << '\0'; }
    break;
    
  }
}

// downLoad microcode
//
void Client::downLoad()
{
  QTextStream os( socket );
  QString line;

  os << "list\r";
  e = textEdit;
  e->setText( tr("END HEADER") );

}

void Client::createDockWindows()
 {
   buildTerminal();
   dockTerminal = new QDockWidget( tr("imsTool Terminal"), this);
   dockTerminal->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
   addDockWidget(Qt::RightDockWidgetArea, dockTerminal);
   dockTerminal->setWidget(terminalFrame);
   viewMenu->addAction(dockTerminal->toggleViewAction());
   inputCommand->setFocus();

 }

//

void Client::buildTerminal()
{
  terminalFrame = new QFrame();
  
  infoText = new QTextEdit();
  e = new QTextEdit();
  e = infoText;

  terminalFrame->setMinimumSize(400, 400);

  //infoText->setStyleSheet( QString( "border: 0;"));
  //inputCommand->setStyleSheet( QString( "border: 0;"));

  layout = new QGridLayout();
  layout->addWidget(infoText, 0, 0, 12, 12);
  layout->addWidget(inputCommand, 12, 0, 1, 1);
  layout->addWidget(connection, 14, 0, 1, 1);


  terminalFrame->setLayout(layout);
  terminalFrame->show();
  inputCommand->setFocus();

}

//

void Client::terminalWindowEnterPressed()
{
  infoText->append(inputCommand->text());

}

// Command list

void Client::addCommand()
{
  if (commandEntry->size() == 20)
    commandEntry->removeFirst();

  commandEntry->append(inputCommand->text());
  commandNum = commandEntry->size(); // current history index

}

// Key-press events

//void Client::keyPressEvent(QKeyEvent *event) 
void Client::keyPressEvent(QKeyEvent *event) 
{
  QTextStream os(socket);

  
  switch (event->key()) {
  case Qt::Key_Up:
    if ( commandEntry->size() == 0 || maxCommands == 0 ) {
      return; 
    }
    if ( commandNum == 0 ) {
      return;
    }
    commandNum--;
    inputCommand->setText(commandEntry->at(commandNum));
    break;

  case Qt::Key_Down:
    if ( commandEntry->size() == 0 || maxCommands == 0 ) return;

    if ( commandNum == commandEntry->size()-1 ) {
      inputCommand->clear();
      inputCommand->setText(tr(">"));
      //      commandEntry->clear(); // bottom of the history stack

    } else {
      commandNum++;
      if ( commandNum >= commandEntry->size() ) commandNum = commandEntry->size()-1;
      inputCommand->setText(commandEntry->at(commandNum));
    }
    break;

  case Qt::Key_Escape:

    // send an escape to the MicroLynx

    os << '\e';
    infoText->append( tr("An ESC key was sent") );
    break;

  default:
    QWidget::keyPressEvent(event);  // handle everything else as usual

  }

}

// slot for lineEdit textChanged

void Client::inputChanged(const QString &text )
{	
  inputCommand->setText(text);
}

void Client::imsError()
{
  QString errorString;
  QTextStream os(socket);

  QString error = "print error\r";
  os << error;

}

void Client::findAndCutText()
{
  searchString = findtext->text();
  QTextDocument *document = textEdit->document();
  
  bool found = false;
  
  if (isFirstTime == false)
    document->undo();
  
  if (searchString.isEmpty()) {
    QMessageBox::information(this, tr("Empty Search Field"),
			     "The search field is empty. Please enter a word and click Find.");
  } else {
    
    QTextCursor highlightCursor(document);
    QTextCursor cursor(document);
    
    cursor.beginEditBlock();
    
    QTextCharFormat plainFormat(highlightCursor.charFormat());
    QTextCharFormat colorFormat = plainFormat;
    colorFormat.setForeground(Qt::red);
    
    while (!highlightCursor.isNull() && !highlightCursor.atEnd()) {
      highlightCursor = document->find(searchString, highlightCursor, QTextDocument::FindWholeWords);
      
      if (!highlightCursor.isNull()) {
	found = true;
	highlightCursor.movePosition(QTextCursor::WordRight,
				     QTextCursor::KeepAnchor);
	highlightCursor.mergeCharFormat(colorFormat);
	QString str = highlightCursor.selectedText();
        highlightCursor.removeSelectedText();
	qDebug() << str;
	
      }
    }
    
    cursor.endEditBlock();
    isFirstTime = false;
    
    if (found == false) {
      QMessageBox::information(this, tr("Word Not Found"),
			       "Sorry, the word cannot be found.");
    }
  }
}

void Client::findText()
{
  searchString = findtext->text();
  QTextDocument *document = textEdit->document();
  
  bool found = false;
  
  if (isFirstTime == false)
    document->undo();
  
  if (searchString.isEmpty()) {
    QMessageBox::information(this, tr("Empty Search Field"),
			     "The search field is empty. Please enter a word and click Find.");
  } else {
    
    QTextCursor highlightCursor(document);
    QTextCursor cursor(document);
    
    cursor.beginEditBlock();
    
    QTextCharFormat plainFormat(highlightCursor.charFormat());
    QTextCharFormat colorFormat = plainFormat;
    colorFormat.setForeground(Qt::red);
    
    while (!highlightCursor.isNull() && !highlightCursor.atEnd()) {
      highlightCursor = document->find(searchString, highlightCursor, QTextDocument::FindWholeWords);
      
      if (!highlightCursor.isNull()) {
	found = true;
	highlightCursor.movePosition(QTextCursor::WordRight,
				     QTextCursor::KeepAnchor);
	highlightCursor.mergeCharFormat(colorFormat);
      }
    }
    
    cursor.endEditBlock();
    isFirstTime = false;
    
    if (found == false) {
      QMessageBox::information(this, tr("Word Not Found"),
			       "Sorry, the word cannot be found.");
    }
  }
}

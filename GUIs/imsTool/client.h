// Client for microLynx Controller (MLC)
// Qt tools
//
// Raymond Gonzalez, OSU Dept. of Astronomy
// rgonzale@astronomy.ohio-state.edu
// 2011 July 28
//
#ifndef CLIENT_H
#define CLIENT_H
#include <Qt>
#include <QtWidgets>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QLineEdit>
#include <QMenuBar>
#include <QMainWindow>
#include <iostream>
#include <client.h>
#include <timeMethods.h>
#include <QtCore/QObject>
#include <QtNetwork/QTcpSocket>

class QAction;
class QMenu;
class QMenuBar;
class QTextEdit;
class QLineEdit;
class QTcpSocket;
class QAbstractSocket;
class QNetworkSession;
class QPlainTextEdit;
class QMainWindow;
class QStringList;
class QFont;

#ifdef MICROLYNXERRORS_H
#include <microLynxErrors.h>
#endif
//#ifdef MECHERRORS_H
//#include <mechErrors.h>
//#endif

class Client : public QMainWindow
{
  Q_OBJECT

public:
  Client( const QString &host, const QString &port );
  Client(QWidget *parent = 0 );
  QTextEdit *infoText;
  QTextEdit *e;
  QMainWindow *mainWindow;

  QTextEdit *textEdit;
  QTcpSocket *socket;
  QGridLayout *grid;

  QFrame *terminalFrame;
  QFrame *editorFrame;
  QTextEdit *terminalWindow;
  QLineEdit *enterArea;
  QLabel *enterLabel;
  QGridLayout *layout;
  static void process_line(const QByteArray &)
    {
    }
  static void process_line(const QString &)
    {
    }
  int commandNum;
  int maxCommands;
  int maxEntry;
  int maxCmds;
  int socketState;
  int fileLoader;
  QString cmdString;
  QString dat;

public slots:
  void buildTerminal();
  void terminalWindowEnterPressed();
  void imsError();

 protected:
  void closeEvent(QCloseEvent *event);
  void keyPressEvent(QKeyEvent *event);

private slots:

  void inputChanged(const QString &text );
  void addCommand();
  void createDockWindows();
  void closeConnection();
  void openConnection();
  void sendToServer();
  void socketReadyRead();
  void socketConnected();
  void socketClosed();
  void socketErr(QAbstractSocket::SocketError socketError);
  void clearNVM();
  void upLoad();
  void downLoad();
  void newFile();
  void open();
  void save();
  bool saveit();
  bool saveAs();
  void findText();
  void findAndCutText();
  void about();
  void undo();
  void documentWasModified();
  void imsTerminal();
  void NVMWarning();


 private:
   QWidget *spacerWidget;
   QNetworkSession *networkSession;

   void createEdit();
   void createActions();
   void createMenus();
   void createToolBars();
   void createStatusBar();
   void createWindows();
   void readSettings();
   void writeSettings();
   bool maybeSave();
   void loadFile(const QString &fileName);
   bool saveFile(const QString &fileName);
   void setCurrentFile(const QString &fileName);
   QString strippedName(const QString &fullFileName);

   QString curFile;
   QString searchString;
   QLabel *connection;
   QMenu *fileMenu;
   QMenu *viewMenu;
   QMenu *transferMenu;
   QMenu *ipMenu;
   QMenu *editMenu;
   QMenu *helpMenu;
   QMenu *viewDockMenu;
   QToolBar *editorToolBar;
   QToolBar *fileToolBar;
   QToolBar *editToolBar;
   QToolBar *cmdToolBar;
   QAction *newAct;
   QAction *openAct;
   QAction *saveAct;
   QAction *saveAsAct;
   QAction *exitAct;
   QAction *undoAct;
   QAction *cutAct;
   QAction *copyAct;
   QAction *pasteAct;
   QAction *aboutAct;
   QAction *aboutQtAct;
   QAction *openIPSocket;
   QAction *closeIPSocket;
   QAction *clrNVM;
   QAction *upload;
   QAction *download;
   QLineEdit *hostEdit;
   QLineEdit *socketEdit;
   QWidgetAction *hostAct;
   QWidgetAction *socketAct;
   QDockWidget *dockEditor;
   QDockWidget *dockTerminal;
   QWidgetAction *findAct;
   QLineEdit *findtext;
   bool isFirstTime;

  QString textString;
  QString fileName;
  QLabel *commandWindowLabel;
  QLineEdit *inputCommand;
  QStringList *commandEntry;
  QLineEdit *commandLine;
  QLineEdit *inputMLC;
  QLabel *ipLabel;
  QLabel *socketLabel;
  QLineEdit *inputHost;
  QLineEdit *inputPort;
  QLabel *editorLabel;
  QPushButton *quit;
  QPushButton *imsTerm;

};

#endif // CLIENT_H

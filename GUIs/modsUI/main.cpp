//
// Main program - prototype MODS Graphical User Interface
//
// R. Pogge, OSU Astronomy Dept.
// pogge@astronomy.ohio-state.edu
// Last Modification: 2009 Feb 1
//
//---------------------------------------------------------------------------

#include <QApplication>
#include <QStyleFactory>
#include <QtDebug>
#include <QtGui/QIcon>

#include "mods.h"
#include "mainwindow.h"

// Global Host Table

QString modsIEHost[2];
QString modsBCHost[2];
QString modsBDHost[2];
QString modsRCHost[2];
QString modsRDHost[2];
QString modsTCHost[2];
QString modsIMHost[2];

// Instrument instance mode global

QString instMode[2];

// LBT Side Global

QString modsSide[2];

// Miscellaneous but useful Globals

int appFontSize;
bool useExpBell;

// Main Program

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  QString cfgFile = QString("%1/Config/%2").arg(MODS_BASE_PATH).arg(MODS_INI_FILE);
  appFontSize = MODS_BASE_FONTSIZE;
  useExpBell = false;

  // Any command-line arguments?  So far we only accept one, the name
  // including full path, of a runtime config file to use instead of
  // the default.

  if (argc==2) {
    cfgFile = argv[1];
    appFontSize = MODS_BASE_FONTSIZE;
  }
  else if (argc==3) {
    cfgFile = argv[1];
    appFontSize = atoi(argv[2]);
  }
  
  qApp->setStyle(QStyleFactory::create("Fusion"));
  qApp->setFont(QFont(MODS_BASE_FONT,appFontSize,QFont::Normal));
  app.setWindowIcon(QIcon(":images/MODS.png"));
  MainWindow console(cfgFile);
  console.show();
  return app.exec();
}

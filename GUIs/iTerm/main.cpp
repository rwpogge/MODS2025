//
// ITerm - Interactive IMPv2 Command Terminal
//
// R. Pogge, OSU Astronomy Dept.
// pogge@astronomy.ohio-state.edu
// Last Modification: 2009 March 23
//
// Based on the ITerm control panel in the MODS user interface
//
//---------------------------------------------------------------------------

#include <QApplication>
#include <QtDebug>
#include <QStyleFactory>
#include <QtGui/QIcon>

#include "iterm.h"
#include "console.h"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  
  // Any command-line arguments are processed here

  QString cfgFile;
  int fontSize;

  if (argc==2) {
    cfgFile = ITERM_CONFIG_FILE;
    fontSize = atoi(argv[1]);
  }	
  else if (argc==3) {
    cfgFile = argv[1];
    fontSize = atoi(argv[2]);
  }	
  else {
    fontSize = ITERM_BASE_FONTSIZE;
    cfgFile = ITERM_CONFIG_FILE;
  }

  qApp->setStyle(QStyleFactory::create("Fusion"));
  qApp->setFont(QFont(ITERM_BASE_FONT,fontSize,QFont::Normal));
  app.setWindowIcon(QIcon(":images/prospero.xbm"));
  
  ITermConsole iterm(cfgFile);

  iterm.show();
  return app.exec();
}

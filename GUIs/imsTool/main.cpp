#include <QApplication>

 #include "client.h"

 int main(int argc, char *argv[])
 {
   //   Q_INIT_RESOURCE(application);

     QApplication app(argc, argv);
     //     app.setOrganizationName("Ohio State University");
     //     app.setApplicationName("IMS Editor");
     //     Editor editor;
     Client client( argc<2 ? "" : argv[1], argv[2]);
     client.show();
     //     editor.show();

     return app.exec();
 }

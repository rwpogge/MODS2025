#ifndef CATVIEWER_H
#define CATVIEWER_H

#include <QDialog>
#include <QTableWidget>

class QString;
class QLabel;
class QFileDialog;
class ActionButton;
class TextEntry;

class CatalogViewer : public QDialog
{
  Q_OBJECT

public:
  CatalogViewer(const QString &myTitle, QWidget *parent=0);

protected:
  void hideEvent(QHideEvent *event);
  void showEvent(QShowEvent *event);

private slots:
  void clearCatalog();    //!< Clear the catalog viewer
  void dismissCatalog();  //!< Dismiss the catalog viewer
  void loadCatalog();     //!< Load a catalog file into the viewer
  void commit();          //!< Commit a catalog selection
  void select(QTableWidgetItem *); // handle catalog selection single-click events
  void selection();       //!< a whole selection?
  void findTarget(const QString &); //!< find a target name in the catalog

signals:
  void newTarget(const QString &);  //!< Send the selected catalog entry string

private:
  QTableWidget *catalogTable;     //!< Table showing the catalog
  ActionButton *dismissButton;    //!< dismiss catalog button
  ActionButton *commitButton;     //!< commit choice button
  ActionButton *fileButton;       //!< select catalog file button
  ActionButton *clearButton;      //!< clear catalog button
  QHash<QString,QString> catalog; //!< Catalog hash table
  // QStringList *catalog;         //!< Catalog entries
  QString catFileName;            //!< Catalog file name
  bool haveCatalog;               //!< do we have a catalog loaded
  QString catEntry;               //!< selected catalog entry
  QRect geom;                     //!< widget geometry
  bool firstShow;                 //!< first display of the widget
  QLabel *catalogLabel;           //!< Title of the catalog
  TextEntry *findEntry;           //!< Search the catalog for a target (case sensitive)
  QLabel *findStatus;             //!< Target search status
};

#endif

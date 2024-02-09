#include <QtWidgets>

#include "mods.h"
#include "catviewer.h"
#include "actionbutton.h"
#include "textentry.h"

//
// A modeless dialog to load and browse target and guide star catalogs
// Uses a QTableWidget convenience class, and for now we adopt the Lucifer
// catalog format (entries separated by |)
//

CatalogViewer::CatalogViewer(const QString &myTitle, QWidget *parent) 
  : QDialog(parent)
{
  
  // File button, Catalog Name label, and clear button across the top

  fileButton = new ActionButton(tr("File..."),this);
  connect(fileButton, SIGNAL(clicked()), this, SLOT(loadCatalog()));

  catalogLabel = new QLabel("");
  catalogLabel->setFont(QFont("Helvetica",appFontSize+2,QFont::Normal));

  clearButton = new ActionButton(tr("Clear"),this);
  connect(clearButton, SIGNAL(clicked()), this, SLOT(clearCatalog()));

  int tbw = qMax((int)(fileButton->minimumSizeHint().width()),
		 (int)(clearButton->minimumSizeHint().width()));
  fileButton->setMinimumWidth(tbw);
  clearButton->setMinimumWidth(tbw);

  QHBoxLayout *topLayout = new QHBoxLayout;
  topLayout->addWidget(fileButton);
  topLayout->addStretch();
  topLayout->addWidget(catalogLabel);
  topLayout->addStretch();
  topLayout->addWidget(clearButton);

  // Create a blank QTableWidget for the catalog viewer, load the
  // header, and connect its signals for item selection and click

  catalogTable = new QTableWidget(this);
  catalogTable->clear();
  catalogTable->setColumnCount(MODS_CATALOG_NCOLS);
  catalogTable->setSortingEnabled(false);
  QStringList catHeader;
  catHeader << "Target" << "RA" << "Dec" << "pmRA" << "pmDec" << "Equinox" << "Epoch" << "PA" << "Guide" << "RA" << "Dec" << "pmRA" << "pmDec" << "Equinox" << "Epoch" << "Rmag" << "B-R";
  catalogTable->setHorizontalHeaderLabels(catHeader); 
  catalogTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  catalogTable->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
  catalogTable->resizeColumnsToContents();
  connect(catalogTable,SIGNAL(itemSelectionChanged()),this,SLOT(selection()));
  connect(catalogTable,SIGNAL(itemClicked(QTableWidgetItem *)),
	  this,SLOT(select(QTableWidgetItem *)));

  // Search tool: TextEntry to search for a target by name

  findEntry = new TextEntry(tr("Find:"),"",12,this);
  connect(findEntry,SIGNAL(dataReady(const QString &)),
	  this,SLOT(findTarget(const QString &)));

  findStatus = new QLabel("");
  findStatus->clear();

  // Commit and Dismiss buttons.  Commit does not become active until
  // a selection has been made.

  commitButton = new ActionButton(tr("Commit"),this);
  connect(commitButton, SIGNAL(clicked()), this, SLOT(commit()));
  commitButton->disable();

  dismissButton = new ActionButton(tr("Dismiss"),this);
  connect(dismissButton, SIGNAL(clicked()), this, SLOT(dismissCatalog()));

  commitButton->setMinimumWidth((int)(dismissButton->minimumSizeHint().width()));

  QHBoxLayout *buttonLayout = new QHBoxLayout;
  buttonLayout->addWidget(findEntry);
  buttonLayout->addWidget(findStatus);
  buttonLayout->addStretch();
  buttonLayout->addWidget(commitButton);
  buttonLayout->addWidget(dismissButton);

  // Vertical layout of the dialog

  QVBoxLayout *viewerLayout = new QVBoxLayout;
  viewerLayout->addLayout(topLayout);
  viewerLayout->addWidget(catalogTable);
  viewerLayout->addLayout(buttonLayout);
  setLayout(viewerLayout);  

  // Put up the decor (size it), and let it go

  setWindowTitle(myTitle);
  setMinimumSize(720,320);
  // setMinimumSize(width,height);

  // Allow the user to resize the window 

  setSizeGripEnabled(true);
  firstShow = true;
  haveCatalog = false;
  catalog.clear();
}

// hideEvent for this dialog, so we can save the catalog geometry when hidden

void CatalogViewer::hideEvent(QHideEvent *)
{
  geom = geometry();
}

// showEvent for this dialog, so we can restore the catalog's geometry
// when it is shown again

void CatalogViewer::showEvent(QShowEvent *)
{
  if (!firstShow)
    setGeometry(geom);
  firstShow = false;
}

// Clear the catalog viewer

void CatalogViewer::clearCatalog()
{
  catFileName.clear(); // forget which catalog we are using
  commitButton->disable(); // disable the commit button
  catalogLabel->clear();
  catalogTable->setSortingEnabled(false);
  for (int i=0;i<catalogTable->rowCount();i++)
    catalogTable->removeRow(i);
  haveCatalog = false;
  catalogTable->clearContents(); // clear everything but the header
  catalogTable->setRowCount(0);
  findEntry->clear();
  findEntry->setNormal();
  findStatus->clear();
}

// Dismiss the catalog viewer

void CatalogViewer::dismissCatalog()
{
  hide();
}

// Load a catalog into the viewer

void CatalogViewer::loadCatalog()
{
  QString fileName;
  QString catLine;

  fileName = QFileDialog::getOpenFileName(this, 
					  tr("Open a Target Catalog"),
					  QString("%1/Catalogs").arg(MODS_BASE_PATH), 
					  tr("Catalog Files (*.cat);;All Files (*.*)"));
  if (!fileName.isEmpty()) {
    QFile file(fileName);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
      QTextStream stream(&file);
      catalogTable->setSortingEnabled(false);
      QString line;
      if (catalogTable->rowCount() > 0) {
	for (int i=0;i<catalogTable->rowCount();i++)
	  catalogTable->removeRow(i);
	clearCatalog();  // clear the current catalog
      }
      catalog.clear();
      catalogTable->setRowCount(0);
      int row = 0;
      do {
	line = stream.readLine();
	catLine = line.simplified().replace(":"," ");  // simplify and remove any :'s
	if (!catLine.isEmpty() && 
	    !catLine.startsWith("#",Qt::CaseInsensitive)) {
	  QStringList bits = catLine.split("|");
	  QString catStr;
	  catStr.clear();
	  if (bits.size() == MODS_CATALOG_NCOLS) { // right number of bits
	    catalogTable->insertRow(row);
	    for (int col=0;col<MODS_CATALOG_NCOLS;col++) {
	      QString itemStr = bits.at(col).simplified();
	      if (col == 1 || col == 2 || col == 4 || col == 5)
		itemStr.replace(" ",":");
	      QTableWidgetItem *item = new QTableWidgetItem(itemStr);
	      item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
	      catalogTable->setItem(row,col,item);
	      if (col > 0)
		catStr.append(QString("|%1").arg(itemStr));
	      else
		catStr = itemStr;
	    }

	    // Create the hash table entry. The hash key is the
	    // lowercase target name to permit case-insensitive
	    // searching, and the hash table value is the laundered
	    // catalog entry string

	    catalog.insert(bits.at(0).simplified().toLower(),catStr);

	  }
	  row++;
	}
      } while (!line.isNull());

      // Done reading, clean up

      QFileInfo fileInfo(file);
      catFileName = fileInfo.fileName();
      catalogLabel->setText(catFileName);

      // Resize the table and sort by target RA

      //      catalogTable->horizontalHeader()->resizeSections(QHeaderView::Stretch);
      catalogTable->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
      catalogTable->setSortingEnabled(true);
      catalogTable->sortByColumn(1,Qt::AscendingOrder);
      catalogTable->resizeColumnsToContents();
      haveCatalog = true;

      file.close();
    }
  }
}

// Get a catalog selection (response to single-click) but do not commit.
// Instead, enable the commit button

void CatalogViewer::select(QTableWidgetItem *item) 
{
  if (!haveCatalog) return;  // can't select if no catalog...
  int row = item->row();
  if (row < 0 || row > catalog.count()) return;
  QString targKey = catalogTable->item(row,0)->text().toLower();
  catEntry = catalog.value(targKey);
  catalogTable->setCurrentCell(row,0,
			       QItemSelectionModel::Rows | 
			       QItemSelectionModel::ClearAndSelect);
  commitButton->enable();
  commitButton->setChange();
  findEntry->clear();
  findEntry->setNormal();
  findStatus->clear();
}

// Commit a catalog selection

void CatalogViewer::commit()
{
  if (!haveCatalog) return; // can't commit if we have no catalog
  if (catEntry.isEmpty()) return; // don't send blank entries (sanity)
  emit newTarget(catEntry);
  commitButton->setNormal();
}

// Select a catalog entry

void CatalogViewer::selection()
{
  int count = catalogTable->selectedRanges().count();
  if (count == 0) return;
  int rowcount = catalogTable->selectedRanges().at(0).rowCount();
  if (rowcount == 1) {
    int row = catalogTable->selectedRanges().at(0).topRow();
    if (row < 0 || row > catalog.count()) return;
    QString targKey = catalogTable->item(row,0)->text().toLower();
    catEntry = catalog.value(targKey);
    catalogTable->setCurrentCell(row,0,
				 QItemSelectionModel::Rows | 
				 QItemSelectionModel::ClearAndSelect);
    commitButton->enable();
    commitButton->setChange();
    findEntry->clear();
    findEntry->setNormal();
    findStatus->clear();
  }
  else {
    catEntry.clear();
    commitButton->disable();
  }
}

// Search the catalog for the given target name (case insensitive) and
// select it

void CatalogViewer::findTarget(const QString &findStr)
{
  if (!haveCatalog) return;
  QString targName = findStr.simplified().toLower();
  findEntry->setNormal();
  findStatus->clear();
  if (targName.isEmpty()) return;
  if (catalog.count() == 0) return;

  if (catalog.contains(targName)) {
    catEntry = catalog.value(targName);
    for (int i=0;i<catalog.size();i++) {
      QString targStr = catalogTable->item(i,0)->text();
      if (targStr.compare(targName,Qt::CaseInsensitive)==0) {
	catalogTable->setCurrentCell(i,0,
				     QItemSelectionModel::Rows | 
				     QItemSelectionModel::ClearAndSelect);
	catalogTable->scrollToItem(catalogTable->item(i,0),
				   QAbstractItemView::PositionAtCenter);
	commitButton->enable();
	commitButton->setChange();
	return;
      }
    }
  }
  findEntry->setEntryColor(QColor(255,0,0,127));
  commitButton->disable();
  findStatus->setText(tr("Target not found"));
}

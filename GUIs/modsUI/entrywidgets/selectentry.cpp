//---------------------------------------------------------------------------
//
// SelectEntry - a labeled combo-box widget for selecting from a list of
//               items
//
// Combines QLabel and QComboBox to produce a configurable pull-down
// menu of parameters
//
// R. Pogge, OSU Astronomy Dept.
// pogge@astronomy.ohio-state.edu
// 2008 May 26 (huge overhaul)
//
// Modification History
//   2009 Feb 16 - changed color scheme - note different from other
//                 select-entry implementations [rwp/osu]
//---------------------------------------------------------------------------

#include <QtWidgets>

#include "selectentry.h"

SelectEntry::SelectEntry(const QString &labelStr, const QString &unitsStr, 
			 QWidget *parent)
  : QWidget(parent)
{
  entry = new QComboBox(this);

  label = new QLabel(labelStr);
  labelText = labelStr;

  units = new QLabel(unitsStr);
  unitsText = unitsStr;

  // color palettes for the widget components

  entryPalette = new QPalette;
  *entryPalette = entry->palette();

  labelPalette = new QPalette;
  *labelPalette = label->palette();

  unitsPalette = new QPalette;
  *unitsPalette = units->palette();

  // Connect QComboBox's activated(int) signal to our getData() slot

  connect(entry,SIGNAL(activated(int)),this, SLOT(getData()));

  // This height looks good

  int hMax = (int)(0.8*entry->minimumSizeHint().height());
  entry->setMaximumHeight(hMax);

  // Make the combo box adjust to the contents

  // entry->setSizeAdjustPolicy(QComboBox::AdjustToContents);

  // Layout of the elements of this widget: Label Selector Units

  QHBoxLayout *layout = new QHBoxLayout;
  if (!labelText.isEmpty())
    layout->addWidget(label);
  layout->addWidget(entry);
  if (!unitsText.isEmpty())
    layout->addWidget(units);
  layout->addStretch();                 // left justify contents
  layout->setContentsMargins(0,0,0,0);  // removes extra margins
  setLayout(layout);

  // Set the focus policy to click only (conservative, avoids accidental
  // tabbing or giving keyboard focus when not awares).

  entry->setFocusPolicy(Qt::ClickFocus);

  // Clear the combo box

  entry->clear();

  // Assume we start out in the normal, enabled state

  enable();

}

// Set the label text

void SelectEntry::setLabelText(const QString &txt)
{
  label->setText(txt);
  labelText = txt;
}

// Set the units text

void SelectEntry::setUnitsText(const QString &txt)
{
  units->setText(txt);
  unitsText = txt;
}

// Change font of all elements

void SelectEntry::setFont(const QFont &font)
{
  entry->setFont(font);
  label->setFont(font);
  units->setFont(font);
}

// getData() slot - emits the dataReady() and indexReady() signals

void SelectEntry::getData()
{
  iSelected = entry->currentIndex();
  emit dataReady(entry->itemData(iSelected).toString());
  emit indexReady(iSelected);
}

// Return the index of the currently selected item

int SelectEntry::currentIndex() const
{
  return entry->currentIndex();
}

// Set the current index to be index

void SelectEntry::setCurrentIndex(const int &index)
{
  entry->setCurrentIndex(index);
}

// Add an item to the combo box (at end)

void SelectEntry::addItem(const QString &text, const QString &data) 
{
  entry->addItem(text,data);
}

// Insert an item to the combo box at index

void SelectEntry::insertItem(const int &index, 
			     const QString &text, const QString &data)
{
  entry->insertItem(index,text,data);
}

// Insert a separator

void SelectEntry::insertSeparator(const int &index)
{
  entry->insertSeparator(index);
}

// Remove the item at index

void SelectEntry::removeItem(const int &index)
{
  entry->removeItem(index);
}

// Change item text at index

void SelectEntry::setItemText(const int &index, const QString &text)
{
  entry->setItemText(index,text);
}

// Return the item data at the given index

QString SelectEntry::itemData(const int &index)
{
  return entry->itemData(index).toString();
}

// Clear the combo box

void SelectEntry::clear()
{
  entry->clear();
}

// Add all items in a QStringList to the combo box.  We clear it first

void SelectEntry::addItems(const QStringList &itemList)
{
  entry->clear();
  entry->addItems(itemList);
}

// Return the item text at the given index

QString SelectEntry::itemText(const int &index)
{
  return entry->itemText(index);
}

// Return the index of the given data entry.  This uses the default
// matching (Qt::MatchExactly|Qt::MatchCaseSensitive)

int SelectEntry::findData(const QString &data) const
{
  return entry->findData(data);
}

// Return the index of the given text entry.  We allow a more generous
// Qt::MatchFixedString matching, which allows string comparisons
// which are case insensitive.

int SelectEntry::findText(const QString &text) const
{
  return entry->findText(text,Qt::MatchFixedString);
}

// Set the color of the select widget entry value

void SelectEntry::setValueColor(const QColor &color)
{  
  entryPalette->setColor(QPalette::Text,color);
  entry->setPalette(*entryPalette);
}

// Change the color of the label

void SelectEntry::setLabelColor(const QColor &color)
{
  labelPalette->setColor(QPalette::WindowText,color);
  label->setPalette(*labelPalette);
}

// Change the color of the units

void SelectEntry::setUnitsColor(const QColor &color)
{
  unitsPalette->setColor(QPalette::WindowText,color);
  units->setPalette(*unitsPalette);
}

// Change the entry color for the "normal" condition

void SelectEntry::setNormal()
{
  if (!entry->isEnabled()) return;
  entryPalette->setColor(QPalette::Text,Qt::black);
  entryPalette->setColor(QPalette::Button,palette().color(QPalette::Window));
  entry->setPalette(*entryPalette);
}

// Change the entry color for the "changing" condition.  
// We use Chroma Amber (#FFBF00, RGB=(255,191,0))

void SelectEntry::setChange()
{
  if (!entry->isEnabled()) return;
  entryPalette->setColor(QPalette::Text,QColor(255,191,0,255));
  entryPalette->setColor(QPalette::Button,Qt::darkYellow);
  entry->setPalette(*entryPalette);
}

// Change the entry color for the "fault" condition

void SelectEntry::setFault()
{
  if (!entry->isEnabled()) return;
  entryPalette->setColor(QPalette::Text,Qt::red);
  entryPalette->setColor(QPalette::Button,palette().color(QPalette::Window));
  entry->setPalette(*entryPalette);
}

// Enable the widget - also sets the colors to "Normal" state

void SelectEntry::enable()
{
  entry->setEnabled(true);
  setNormal();
}

// Disable the widget

void SelectEntry::disable()
{
  entry->setEnabled(false);
  entryPalette->setColor(QPalette::Text,Qt::darkGray);
  entry->setPalette(*entryPalette);
}

// Set the minimum contents length

void SelectEntry::setMinimumContentsLength(const int &mcl)
{
  entry->setMinimumContentsLength(mcl);
}

// Return the number of items in the QComboBox entry menu

int SelectEntry::count()
{
  return entry->count();
}

// Set the maximum width of the ComboBox widget

void SelectEntry::setMaxSelectorWidth(const int &maxw)
{
  entry->setMaximumWidth((int)(0.5*maxw*entry->minimumSizeHint().width()));
}

// Scale the height of the widget

void SelectEntry::scaleHeight(const double &scale)
{
  int hMax = (int)(scale*entry->minimumSizeHint().height());
  entry->setMaximumHeight(hMax);
}

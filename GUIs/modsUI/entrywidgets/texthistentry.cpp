//---------------------------------------------------------------------------
//
// TextHistEntry - a labeled text-entry widget with entry history
//
// Combines QLabel and QLineEdit to produce a text-entry widget
// that keeps a history of previous entries.
//
// R. Pogge, OSU Astronomy Dept.
// pogge@astronomy.ohio-state.edu
// 2008 Feb 24
//
// here is a case where inheritance from base class TextEntry will
// be the next step in development after we decide if this is what
// we want to do...
//
//---------------------------------------------------------------------------

#include <QtGui>

#include "texthistentry.h"

TextHistEntry::TextHistEntry(const QString &labelStr, 
			     const QString &unitsStr, 
			     int maxw, int maxHist, 
			     QWidget *parent)
  : QWidget(parent)
{
  // Widgets used: 2 QLabels (label and units) and a QLineEdit

  entry = new QLineEdit;

  label = new QLabel(labelStr);
  labelText = labelStr;

  units = new QLabel(unitsStr);
  unitsText = unitsStr;

  // Color palettes for the widget components

  entryPalette = new QPalette;
  *entryPalette = entry->palette();

  labelPalette = new QPalette;
  *labelPalette = label->palette();

  unitsPalette = new QPalette;
  *unitsPalette = units->palette();

  // Connect the QLineEdit's returnPressed() signal to this widget's
  // private getData() slot

  connect(entry,SIGNAL(returnPressed()),this,SLOT(getData()));

  // If a maximum entry length is set, set an appropriate width for
  // the QLineEdit widget, otherwise let it autosize

  if (maxw > 0) {
    entry->setMaxLength(maxw);
    entry->setMaximumWidth(maxw*entry->minimumSizeHint().width());
  }

  // This height looks good for most applications

  int hMax = (int)(0.8*entry->minimumSizeHint().height());
  entry->setMaximumHeight(hMax);

  // If maxHist=0, don't enable the history mechanism, otherwise
  // fire it up

  entryHistory = new QStringList;
  if (maxHist>0) 
    maxHistory = maxHist;
  else
    maxHistory = 0; // no history...

  entryHistory->clear();
  numHistory = 0;

  // Layout of the elements of this widget: Label Editor Units

  QHBoxLayout *entryLayout = new QHBoxLayout;
  if (!labelText.isEmpty())
    entryLayout->addWidget(label);
  entryLayout->addWidget(entry);
  if (!unitsText.isEmpty())
    entryLayout->addWidget(units);
  if (maxw>0)
    entryLayout->addStretch();  // left justify contents
  entryLayout->setContentsMargins(0,0,0,0);  // removes extra margins
  setLayout(entryLayout);

  // Set the focus policy to click only (conservative, avoids accidental
  // tabbing or giving keyboard focus when not awares).

  entry->setFocusPolicy(Qt::ClickFocus);


  // Default colors for the widget states

  setNormalColors(Qt::white,Qt::black);
  setChangeColors(Qt::yellow,Qt::black);
  setFaultColors(Qt::red,Qt::black);

  // Assume we start out in the normal, enabled state

  enable();

}

// Set the text in the QLineEdit widget

void TextHistEntry::setText(const QString &txt)
{
  entry->setText(txt);
}

// Set the label text

void TextHistEntry::setLabelText(const QString &txt)
{
  label->setText(txt);
  labelText = txt;
}

// Set the units text

void TextHistEntry::setUnitsText(const QString &txt)
{
  units->setText(txt);
  unitsText = txt;
}

// Change font of all elements

void TextHistEntry::setFont(const QFont &font)
{
  entry->setFont(font);
  label->setFont(font);
  units->setFont(font);
}

// getData() slot - emits the dataReady() signal

void TextHistEntry::getData()
{
  entryText = entry->text();
  if (entryText.isEmpty()) return;
  if (maxHistory>0) addHistory();
  emit dataReady(entryText);
}

// Return the contents of the entry text

QString TextHistEntry::value() const
{
  return entryText;
}

// Clear the entry box

void TextHistEntry::clear()
{
  entry->clear();
}

// Set the color of the entry text 

void TextHistEntry::setTextColor(const QColor &color)
{
  entryPalette->setColor(QPalette::Text,color);
  entry->setPalette(*entryPalette);
}

// Set the color of the text entry background

void TextHistEntry::setEntryColor(const QColor &color)
{  
  entryPalette->setColor(QPalette::Base,color);
  entry->setPalette(*entryPalette);
}

// Set the color of the widget label

void TextHistEntry::setLabelColor(const QColor &color)
{  
  labelPalette->setColor(QPalette::WindowText,color);
  label->setPalette(*labelPalette);
}

// Set the color of the widget units

void TextHistEntry::setUnitsColor(const QColor &color)
{  
  unitsPalette->setColor(QPalette::WindowText,color);
  units->setPalette(*unitsPalette);
}

//---------------------------------------------------------------------------
//
// Command History
//

// Clear the history list

void TextHistEntry::clearHistory()
{
  entryHistory->clear();
}

// Add the latest entry to the command history list.  If the list is
// full, pop the oldest entry off the top of the list.

void TextHistEntry::addHistory()
{
  if (entryHistory->size() == maxHistory)
    entryHistory->removeFirst();  // pop oldest history entry off top

  entryHistory->append(entryText);
  numHistory = entryHistory->size(); // current history index
}

// Handler for key-press events (overload keyPressEvent).  This is how
// we trap the up and down arrow keys for the command history
// mechanism

void TextHistEntry::keyPressEvent(QKeyEvent *event) 
{
  switch (event->key()) {
  case Qt::Key_Up:
    if (entryHistory->size() == 0 || maxHistory==0) return;
    if (numHistory == 0) return;  // top of the history stack
    numHistory--;
    entry->setText(entryHistory->at(numHistory));
    break;
  case Qt::Key_Down:
    if (entryHistory->size() == 0 || maxHistory==0) return;
    if (numHistory == entryHistory->size()-1)
      entry->clear(); // bottom of the history stack
    else {
      numHistory++;
      if (numHistory >= entryHistory->size()) numHistory = entryHistory->size()-1;
      entry->setText(entryHistory->at(numHistory));
    }
    break;
  case Qt::Key_Escape:
    // swallow ESC - does bad things
    break;
  default:
    QWidget::keyPressEvent(event);  // handle everything else as usual
  }
}

//----------------------------------------------------------------
//
// Widget state changes
//

// Change the entry color for the "normal" condition

void TextHistEntry::setNormal()
{
  if (!entry->isEnabled()) return;
  entryPalette->setColor(QPalette::Base,bgNormal);
  entryPalette->setColor(QPalette::Text,fgNormal);
  entry->setPalette(*entryPalette);
}

// Change the entry color for the "changing" condition

void TextHistEntry::setChange()
{
  if (!entry->isEnabled()) return;
  entryPalette->setColor(QPalette::Base,bgChange);
  entryPalette->setColor(QPalette::Text,fgChange);
  entry->setPalette(*entryPalette);
}

// Change the entry color for the "fault" condition

void TextHistEntry::setFault()
{
  if (!entry->isEnabled()) return;
  entryPalette->setColor(QPalette::Base,bgFault);
  entryPalette->setColor(QPalette::Text,fgFault);
  entry->setPalette(*entryPalette);
}

// Set the colors for the widget states
//   Does not actually change any colors until one of setNormal() etc. are
//   invoked.  Also does not (cannot) validate colors

void TextHistEntry::setNormalColors(const QColor &bg, const QColor &fg)
{
  bgNormal = bg;
  fgNormal = fg;
}

void TextHistEntry::setChangeColors(const QColor &bg, const QColor &fg)
{
  bgChange = bg;
  fgChange = fg;
}

void TextHistEntry::setFaultColors(const QColor &bg, const QColor &fg)
{
  bgFault = bg;
  fgFault = fg;
}

// Enable the widget - also sets the colors to "Normal" state

void TextHistEntry::enable()
{
  entry->setEnabled(true);
  setNormal();
}

// Disable the widget

void TextHistEntry::disable()
{
  entry->setEnabled(false);
  entryPalette->setColor(QPalette::Base,entryPalette->color(QPalette::Window));
  entryPalette->setColor(QPalette::Text,Qt::darkGray);
  entry->setPalette(*entryPalette);
}

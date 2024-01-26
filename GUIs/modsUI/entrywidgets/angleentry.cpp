//---------------------------------------------------------------------------
//
// AngleEntry - a labeled sexagesimal entry widget
//
// Combines QLabel and QLineEdit to produce an entry widget that
// for sexagesimal numbers (e.g., 24:23:14.56 or -22:13:14.123)
//
// R. Pogge, OSU Astronomy Dept.
// pogge@astronomy.ohio-state.edu
// 2010 Feb 7
//
//---------------------------------------------------------------------------

#include <QtGui>

#include "angleentry.h"

AngleEntry::AngleEntry(const QString &labelStr, const QString &unitsStr, 
		     int maxw, QWidget *parent)
  : QWidget(parent)
{

  // This widget accepts keyboard events, so set FocusPolicy() accordingly

  setFocusPolicy(Qt::StrongFocus);

  // Widgets we use: 2 QLabels (label and units), 1 QLineEdit

  label = new QLabel(labelStr);
  labelText = labelStr;

  entry = new QLineEdit(this);

  units = new QLabel(unitsStr);
  unitsText = unitsStr;

  // Color palettes for the widget components

  entryPalette = new QPalette;
  *entryPalette = entry->palette();

  labelPalette = new QPalette;
  *labelPalette = label->palette();

  unitsPalette = new QPalette;
  *unitsPalette = units->palette();

  connect(entry,SIGNAL(textEdited(const QString &)),this,SLOT(entryChanged()));
  connect(entry,SIGNAL(editingFinished()),this,SLOT(getData()));
  connect(entry,SIGNAL(returnPressed()),this,SLOT(commitData()));

  commit = true;

  // If a maximum entry length is set, set an appropriate width for
  // the QLineEdit widget, otherwise let it autosize

  if (maxw > 0) {
    entry->setMaxLength(maxw);
    int mWid = (int)(0.5*maxw*entry->minimumSizeHint().width());
    entry->setMinimumWidth(mWid);
  }

  // This height looks good

  int hMax = (int)(0.8*entry->minimumSizeHint().height());
  entry->setMaximumHeight(hMax);

  // Layout of the elements of this widget: Label EntryWidget Units

  QHBoxLayout *entryLayout = new QHBoxLayout;
  if (!labelText.isEmpty())
    entryLayout->addWidget(label);
  entryLayout->addWidget(entry);
  if (!unitsText.isEmpty())
    entryLayout->addWidget(units);
  if (maxw > 0)
    entryLayout->addStretch();  // left justify contents if not autosized
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

void AngleEntry::setText(const QString &txt)
{
  entry->setText(txt);
}

// Set the label text

void AngleEntry::setLabelText(const QString &txt)
{
  label->setText(txt);
  labelText = txt;
}

// Set the units text

void AngleEntry::setUnitsText(const QString &txt)
{
  units->setText(txt);
  unitsText = txt;
}

// Change font of all elements

void AngleEntry::setFont(const QFont &font)
{
  entry->setFont(font);
  label->setFont(font);
  units->setFont(font);
}

// commitData() slot - emits the dataReady() signal.  This is invoked
// when the Return key is hit in the entry to signal that the user has
// "committed" the entry.  See getData()

void AngleEntry::commitData()
{
  entryValue = entry->text();
  setNormal(); 
  emit dataReady(entryValue);
}

// getData() slot - emits the textReady() signal when the entry widget
// either looses focus or the user hits Return.  This allows a more
// familiar "tab to commit" feel to the widget when used in
// non-time-critical applications data-entry forms.

void AngleEntry::getData()
{
  entryValue = entry->text();
  if (!commit) setNormal(); 
  emit textReady(entryValue);
}

// Return the contents of the entry

QString AngleEntry::value() const
{
  return entryValue;
}

// Clear the entry box

void AngleEntry::clear()
{
  entry->clear();
}

// Set the color of the text entry background

void AngleEntry::setEntryColor(const QColor &color)
{  
  entryPalette->setColor(QPalette::Base,color);
  entry->setPalette(*entryPalette);
}

// Set the color of the entry text 

void AngleEntry::setTextColor(const QColor &color)
{
  entryPalette->setColor(QPalette::Text,color);
  entry->setPalette(*entryPalette);
}

// Set the color of the widget label

void AngleEntry::setLabelColor(const QColor &color)
{  
  labelPalette->setColor(QPalette::WindowText,color);
  label->setPalette(*labelPalette);
}

// Set the color of the widget units

void AngleEntry::setUnitsColor(const QColor &color)
{  
  unitsPalette->setColor(QPalette::WindowText,color);
  units->setPalette(*unitsPalette);
}

// Change the entry color for the "normal" condition

void AngleEntry::setNormal()
{
  if (!entry->isEnabled()) return;
  entryPalette->setColor(QPalette::Base,bgNormal);
  entryPalette->setColor(QPalette::Text,fgNormal);
  entry->setPalette(*entryPalette);
}

// Change the entry color for the "changing" condition

void AngleEntry::setChange()
{
  if (!entry->isEnabled()) return;
  entryPalette->setColor(QPalette::Base,bgChange);
  entryPalette->setColor(QPalette::Text,fgChange);
  entry->setPalette(*entryPalette);
}

// Change the entry color for the "fault" condition

void AngleEntry::setFault()
{
  if (!entry->isEnabled()) return;
  entryPalette->setColor(QPalette::Base,bgFault);
  entryPalette->setColor(QPalette::Text,fgFault);
  entry->setPalette(*entryPalette);
}

// Set the colors for the widget states
//   Does not actually change any colors until one of setNormal() etc. are
//   invoked.  Also does not (cannot) validate colors

void AngleEntry::setNormalColors(const QColor &bg, const QColor &fg)
{
  bgNormal = bg;
  fgNormal = fg;
}

void AngleEntry::setChangeColors(const QColor &bg, const QColor &fg)
{
  bgChange = bg;
  fgChange = fg;
}

void AngleEntry::setFaultColors(const QColor &bg, const QColor &fg)
{
  bgFault = bg;
  fgFault = fg;
}

// Enable the widget - also sets the colors to "Normal" state

void AngleEntry::enable()
{
  entry->setEnabled(true);
  setNormal();
}

// Disable the widget

void AngleEntry::disable()
{
  entry->setEnabled(false);
  entryPalette->setColor(QPalette::Base,entryPalette->color(QPalette::Window));
  entryPalette->setColor(QPalette::Text,Qt::darkGray);
  entry->setPalette(*entryPalette);
}
 
// Set/Unset the text entry widget input mask

void AngleEntry::setInputMask(const QString &mask)
{
  entry->setInputMask(mask);
}

// If the contents of the entry appear to have been changed, but not
// committed by hitting Return, turn the background a light yellow

void AngleEntry::entryChanged() 
{
  entryPalette->setColor(QPalette::Base,QColor(255,255,204,255));
  entry->setPalette(*entryPalette);
}

// Change the entry widget focus policy.  The default is click focus only
// but sometimes you want "strong focus", which includes tabbing into the
// widget

void AngleEntry::setEntryFocusPolicy(Qt::FocusPolicy policy)
{
  entry->setFocusPolicy(policy);
}

void AngleEntry::setCommit(const bool &commitFlag)
{
  commit = commitFlag;
}

//---------------------------------------------------------------------------
//
// TextEntry - a labeled text-entry widget
//
// Combines QLabel and QLineEdit to produce a text-entry widget
//
// R. Pogge, OSU Astronomy Dept.
// pogge@astronomy.ohio-state.edu
// 2008 Feb 24
//
//---------------------------------------------------------------------------

#include <QtWidgets>

#include "textentry.h"

TextEntry::TextEntry(const QString &labelStr, const QString &unitsStr, 
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
    int mWid = (int)(0.75*maxw*entry->minimumSizeHint().width());
    entry->setMaximumWidth(mWid);
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

void TextEntry::setText(const QString &txt)
{
  entry->setText(txt);
}

// Set the label text

void TextEntry::setLabelText(const QString &txt)
{
  label->setText(txt);
  labelText = txt;
}

// Set the units text

void TextEntry::setUnitsText(const QString &txt)
{
  units->setText(txt);
  unitsText = txt;
}

// Change font of all elements

void TextEntry::setFont(const QFont &font)
{
  entry->setFont(font);
  label->setFont(font);
  units->setFont(font);
}

// commitData() slot - emits the dataReady() signal.  This is invoked
// when the Return key is hit in the entry to signal that the user has
// "committed" the entry.  See getData()

void TextEntry::commitData()
{
  entryValue = entry->text();
  setNormal(); 
  emit dataReady(entryValue);
}

// getData() slot - emits the textReady() signal when the entry widget
// either looses focus or the user hits Return.  This allows a more
// familiar "tab to commit" feel to the widget when used in
// non-time-critical applications data-entry forms.

void TextEntry::getData()
{
  entryValue = entry->text();
  if (!commit) setNormal(); 
  emit textReady(entryValue);
}

// Return the contents of the entry

QString TextEntry::value() const
{
  return entryValue;
}

// Clear the entry box

void TextEntry::clear()
{
  entry->clear();
}

// Set the color of the text entry background

void TextEntry::setEntryColor(const QColor &color)
{  
  entryPalette->setColor(QPalette::Base,color);
  entry->setPalette(*entryPalette);
}

// Set the color of the entry text 

void TextEntry::setTextColor(const QColor &color)
{
  entryPalette->setColor(QPalette::Text,color);
  entry->setPalette(*entryPalette);
}

// Set the color of the widget label

void TextEntry::setLabelColor(const QColor &color)
{  
  labelPalette->setColor(QPalette::WindowText,color);
  label->setPalette(*labelPalette);
}

// Set the color of the widget units

void TextEntry::setUnitsColor(const QColor &color)
{  
  unitsPalette->setColor(QPalette::WindowText,color);
  units->setPalette(*unitsPalette);
}

// Change the entry color for the "normal" condition

void TextEntry::setNormal()
{
  if (!entry->isEnabled()) return;
  entryPalette->setColor(QPalette::Base,bgNormal);
  entryPalette->setColor(QPalette::Text,fgNormal);
  entry->setPalette(*entryPalette);
}

// Change the entry color for the "changing" condition

void TextEntry::setChange()
{
  if (!entry->isEnabled()) return;
  entryPalette->setColor(QPalette::Base,bgChange);
  entryPalette->setColor(QPalette::Text,fgChange);
  entry->setPalette(*entryPalette);
}

// Change the entry color for the "fault" condition

void TextEntry::setFault()
{
  if (!entry->isEnabled()) return;
  entryPalette->setColor(QPalette::Base,bgFault);
  entryPalette->setColor(QPalette::Text,fgFault);
  entry->setPalette(*entryPalette);
}

// Set the colors for the widget states
//   Does not actually change any colors until one of setNormal() etc. are
//   invoked.  Also does not (cannot) validate colors

void TextEntry::setNormalColors(const QColor &bg, const QColor &fg)
{
  bgNormal = bg;
  fgNormal = fg;
}

void TextEntry::setChangeColors(const QColor &bg, const QColor &fg)
{
  bgChange = bg;
  fgChange = fg;
}

void TextEntry::setFaultColors(const QColor &bg, const QColor &fg)
{
  bgFault = bg;
  fgFault = fg;
}

// Enable the widget - also sets the colors to "Normal" state

void TextEntry::enable()
{
  entry->setEnabled(true);
  setNormal();
}

// Disable the widget

void TextEntry::disable()
{
  entry->setEnabled(false);
  entryPalette->setColor(QPalette::Base,entryPalette->color(QPalette::Window));
  entryPalette->setColor(QPalette::Text,Qt::darkGray);
  entry->setPalette(*entryPalette);
}
 
// Set/Unset the text entry widget input mask

void TextEntry::setInputMask(const QString &mask)
{
  entry->setInputMask(mask);
}

// If the contents of the entry appear to have been changed, but not
// committed by hitting Return, turn the background a light yellow

void TextEntry::entryChanged() 
{
  entryPalette->setColor(QPalette::Base,QColor(255,255,204,255));
  entry->setPalette(*entryPalette);
}

// Change the entry widget focus policy.  The default is click focus only
// but sometimes you want "strong focus", which includes tabbing into the
// widget

void TextEntry::setEntryFocusPolicy(Qt::FocusPolicy policy)
{
  entry->setFocusPolicy(policy);
}

void TextEntry::setCommit(const bool &commitFlag)
{
  commit = commitFlag;
}

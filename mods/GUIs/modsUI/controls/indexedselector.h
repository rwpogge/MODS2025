#ifndef INDEXEDSELECTOR_H
#define INDEXEDSELECTOR_H

// Qt Widgets

#include <QWidget>

class QString;
class QComboBox;
class QLabel;

// Custom Widgets

class IndexedMechanism;
class ISISClient;

// IndexedSelector control Class definition

class IndexedSelector : public QWidget
{
  Q_OBJECT

public:
  IndexedSelector(IndexedMechanism *iMech, const QString &labelStr, 
		  QWidget *parent=0);

  // Basic Methods

  QString command() const;     //!< Command to execute
  void setState(const int &);  //!< Set the selector state (see states enum)
  void setFault();             //!< Set the selector state to Fault
  void setNormal();            //!< Set the selector state to Normal
  void setChange();            //!< Set the selector state to Changing
  void setUnknown();           //!< Set the selector state to Unknown
  void setISISState(const int &);  //!< Set the state based on the ISIS message type

  // State flags

  enum states {
    Normal,   //!< IndexedSelector is Normal (indicates selection)
    Changing, //!< IndexedSelector is changing state (yellow)
    Fault,    //!< A fault occurred while changing state (red)
    Unknown   //!< We don't know the selection state (gray)
  };

  // Enable/disable the indexed selector widget

  bool isEnabled() const; //!< Is the indexed selector widget enabled?
  void enable();          //!< Enable the indexed selector widget
  void disable();         //!< Disable the indexed selector widget

signals:
  void cmdReady(const QString &);  //!< Command is ready for execution

public slots:
  void update();      //!< Update the selector
  void updateIDs();   //!< Update the ID table

private slots:
  void setCommand();  //!< Indexed Selector has been invoked
  void setNormalColor();  //!< set the "normal" color (black text)
  void setChangeColor();  //!< set the "change" color (orange/yellow text)
  void setFaultColor();   //!< set the "fault" color (red text)
  void setUnknownColor(); //!< set the "unknown" color (gray text)

private:
  IndexedMechanism *mech;    //!< IndexedMechanism object bound to this selector
  QComboBox *selector;       //!< QComboBox widget for the selector
  QString cmdStr;            //!< Command string to be executed
  QPalette *selectorPalette; //!< Color pallet of the QComboBox selector
  int state;                 //!< Status of the indexed selector (see states enum)
  int nullPos;               //!< Special "null" position at the end of the selector
  int curPos;                //!< Current selector index position 
  bool enabled;              //!< True if the indexed selector is enabled, false if disabled
};

#endif

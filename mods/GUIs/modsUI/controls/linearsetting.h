#ifndef LINEARSETTING_H
#define LINEARSETTING_H

// Qt Widgets

#include <QWidget>

class QString;

// Custom Widgets

class NumberEntry;
class LinearMechanism;

// LinearSetting Class definition

class LinearSetting : public QWidget
{
  Q_OBJECT

public:
  LinearSetting(LinearMechanism *lMech, const QString &labelStr, 
		const QString &unitsStr, int lMaxW=0, QWidget *parent=0);

  // Basic Methods

  QString command() const;     //!< Command to execute
  double value() const;        //!< Value in the entry widget (independent of command context)
  int intValue() const;        //!< Integer representation of the entry widget value
  void setState(const int &);  //!< Set the widget state
  void setNormal();            //!< Set the widget state to Normal
  void setFault();             //!< Set the widget state to Fault
  void setChange();            //!< Set the widget state to Changing
  void setUnknown();           //!< Set the widget state to Unknown
  void setISISState(const int &);  //!< Set the state based on the ISIS message type

  // State flags

  enum states {
    Normal,   //!< LinearSetting is Normal (indicates selection)
    Changing, //!< LinearSetting is changing state (yellow)
    Fault,    //!< A fault occurred while changing state (red)
    Unknown   //!< We don't know the selection state (gray)
  };

  // Enable/disable the widget

  bool isEnabled() const; //!< Is the widget enabled?
  void enable();          //!< Enable the widget
  void disable();         //!< Disable the widget

signals:
  void cmdReady(const QString &); //!< Command is ready for execution
  void dataReady(const double &); //!< Entry data is ready

public slots:
  void update();   //!< Update the widget appearance

private slots:
  void setEntry(); //!< Process NumberEntry widget events

private:
  LinearMechanism *mech;  //!< LinearMechanism object bound to this widget
  NumberEntry *entry;     //!< Mechanism position entry/display widget
  QString cmdStr;         //!< Command string to be executed
  int state;              //!< State of the widget (see states enum)
  bool enabled;  //!< True if the widget is enabled, false if disabled

};

#endif

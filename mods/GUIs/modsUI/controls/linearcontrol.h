#ifndef LINEARCONTROL_H
#define LINEARCONTROL_H

// Qt Widgets

#include <QWidget>

class QString;

// Custom Widgets

class NumberEntry;
class LinearMechanism;

// LinearControl control Class definition

class LinearControl : public QWidget
{
  Q_OBJECT

public:
  LinearControl(LinearMechanism *lMech, const QString &labelStr, 
		const QString &unitsStr, int lMaxW=0, QWidget *parent=0);

  // Basic Methods

  QString command() const;     //!< Command to execute
  double value() const;        //!< Value in the entry widget (independent of command context)
  int intValue() const;        //!< Integer representation of the entry widget value
  void setState(const int &);  //!< Set the control widget state
  void setNormal();            //!< Set the control widget state to Normal
  void setFault();             //!< Set the control widget state to Fault
  void setChange();            //!< Set the control widget state to Changing
  void setUnknown();           //!< Set the control widget state to Unknown
  void setISISState(const int &);  //!< Set the state based on the ISIS message type

  // State flags

  enum states {
    Normal,   //!< LinearControl is Normal (indicates selection)
    Changing, //!< LinearControl is changing state (yellow)
    Fault,    //!< A fault occurred while changing state (red)
    Unknown   //!< We don't know the selection state (gray)
  };

  // Enable/disable the linear control widget

  bool isEnabled() const; //!< Is the linear control widget enabled?
  void enable();          //!< Enable the linear control widget
  void disable();         //!< Disable the linear control widget

signals:
  void cmdReady(const QString &);  //!< Command is ready for execution
  void dataReady(const double &);  //!< Entry data ready
  void statusMsg(const QString &); //!< A status message is ready to be passed

public slots:
  void update();      //!< Update the widget appearance

private slots:
  void setCommand();  //!< Process NumberEntry widget events

private:
  LinearMechanism *mech; //!< LinearMechanism object bound to this control widget
  NumberEntry *entry;    //!< Mechanism position entry/display widget
  QString cmdStr;        //!< Command string to be executed
  int state;             //!< State of the linear control (see states enum)
  bool enabled;          //!< True if the widget is enabled, false if disabled

};

#endif

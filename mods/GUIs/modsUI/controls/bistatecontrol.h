#ifndef BISTATECONTROL_H
#define BISTATECONTROL_H

// Qt Widgets

#include <QWidget>

class QLabel;
class QString;

// Custom Widgets

class ActionButton;
class BiStateMechanism;
class ISISClient;

// BiStateControl control Class definition

class BiStateControl : public QWidget
{
  Q_OBJECT

public:
  BiStateControl(BiStateMechanism *bMech, const QString &labelStr, 
		 const QString &lab0, const QString &lab1, 
		 QWidget *parent=0);

  // Basic Methods

  QString command() const;  //!< command to execute
  void setState(const int &, const int &);   //!< Set the control state
  void setFault();   //!< Set the fault condition
  void setUnknown(); //!< Set the unknown state
  void setNormal();  //!< Set the normal state
  void setChange();  //!< Set the changing state
  void setISISState(const int &);  //!< Set the state based on the ISIS message type

  // State flags

  enum states {
    Deselected, //!< Button State is De-Selected and Normal
    Selected,   //!< Button State is Selected and Normal
    Changing,   //!< Button State is Selected and Changing
    Fault,      //!< A fault occurred while changing the button state
    Unknown     //!< We don't know the button state (e.g., on startup)
  };

  // Enable/disable the bistate control widget

  bool isEnabled() const; //!< Is the bistate control widget enabled?
  void enable();          //!< Enable the bistate control widget
  void disable();         //!< Disable the bistate control widget

signals:
  void cmdReady(const QString &); //!< Emit this signal when a command is ready for execution

public slots:
  void update();      //!< Update the bistate control widget

private slots:
  void setState0();    //!< Button for State 0 has been pressed
  void setState1();    //!< Button for State 1 has been pressed

private:
  BiStateMechanism *mech; //!< BiState Mechanism bound to this control widget
  QLabel *label;          //!< Control label widget
  QString cmdStr;         //!< Command string to execute

  ActionButton *button0; //!< State 0 action button
  int status0;           //!< Status of State 0 (see states enum)

  ActionButton *button1; //!< State 1 action button
  int status1;           //!< Status of State 1 (see states enum)

  bool enabled;  //!< True if the control is enabled, false if disabled
};

#endif

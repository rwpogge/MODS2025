/*! header file for service_ports.
 * service_ports is used to hold port information for the modsServers task.
 * modsServers task is responsible for loading the PORTS
 * it has choosen into shared memory. Clients written for modsServers
 * will get the port it needs here. 
 */

/*
 * All ports will have an initialization to -1 
 */
struct service_ports {    // mechanism server port data structure
  int darkslidePort = -1;
  int calibrationPort = -1;
  int maskInsertPort = -1;
  int maskSelectPort = -1;
  int agwYPort = -1;
  int agwXPort = -1;
  int agwPrimeFocusPort = -1;
  int agwFilterWheelPort = -1;
  int dichroicPort = -1;
  int blueTipPort = -1;
  int blueTiltPort = -1;
  int blueFocusPort = -1;
  int blueFilterWheelPort = -1;
  int blueGT1Port = -1;
  int blueGT2Port = -1;
  int blueGTSPort = -1;
  int blueShutterPort = -1;
  int blueCameraFocusPort = -1;
  int blueCameraFilterWheelPort = -1;
  int redTipPort = -1;
  int redTiltPort = -1;
  int redFocusPort = -1;
  int redFilterWheelPort = -1;
  int redGT1Port = -1;
  int redGT2Port = -1;
  int redGTSPort = -1;
  int redShutterPort = -1;
  int redCameraFocusPort = -1;
  int redCameraFilterWheelPort = -1;
};

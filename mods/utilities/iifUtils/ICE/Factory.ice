#pragma once

#include "IIFServer.ice"

module lbto
{

interface Factory
{
  IIFServer * create(string proxyName, string focalStation, string instrumentID);
  void destroy(IIFServer * proxy);
  void destroyProxy(string proxyName);
  string getProxy(string proxyName);
  string getProxyInstrument(string proxyName);
  string getProxyFocalStation(string proxyName);
  string listConnections();
  void debugMode(bool b);
};

};

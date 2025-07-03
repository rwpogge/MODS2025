#ifndef FACTORY_ICE
#define FACTORY_ICE

#include "IIFServer.ice"

module lbto
{

interface Factory
{
        IIFServer* create(string proxyName, string focalStation, string instrumentID);
        void destroy(IIFServer* proxy);
	string listConnections();
	void debugMode(bool b);
};

};
#endif

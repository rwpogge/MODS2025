#include <iif/ice/FactoryI.hpp>
#include <iif/ice/IIFServerI.hpp>
#include <iostream>

using namespace std;
using namespace lbto;

Proxy::Proxy(string _focalStation, string _instrument, shared_ptr<IIFServerPrx> _server) :
	focalStation(_focalStation), instrument(_instrument), server(_server)
{
}

std::string
Proxy::str()
{
	return instrument + " at " + focalStation;
}

//! class constructor
FactoryI::FactoryI()
{
	m_debug = true; //by default enable the debug mode
}

//!class destructor
FactoryI::~FactoryI()
{
}

//! Creates or links an ICE proxy with the client connection.
shared_ptr<IIFServerPrx>
FactoryI::create(string _proxyName, string _focalStation, string _instrumentID, const Ice::Current& _c)
{
	// Get the Ice logger.
	Ice::Print info(_c.adapter->getCommunicator()->getLogger());
	/* convert to lower case */
	string focalStation = _focalStation;
	transform(focalStation.begin(),focalStation.end(),focalStation.begin(),(int(*)(int))tolower);
	/* convert to upper case */
	string instrument = _instrumentID;
	transform(instrument.begin(),instrument.end(),instrument.begin(),(int(*)(int))toupper);

	IceUtil::LockT < IceUtil::RecMutex > guard (__connMutex);

	try
	{
		// if proxy does not exists, exception will be thrown
		Proxy& fi = m_proxies.at(_proxyName);

		// if it's signature isn't the same, remove it and create new proxy (when catching the exception..)
		if (fi.focalStation != focalStation || fi.instrument != instrument)
		{
			destroyProxy(_proxyName, _c);
			throw out_of_range("proxy destroyed");
		}
	}
	catch (out_of_range)
	{
		// create a new IIF Proxy instance
		IIF *anIIF = new IIF(focalStation.c_str(), instrument.c_str(), _proxyName.c_str());
		IIFServerPtr iif = make_shared<IIFServerI>(_c.adapter->getCommunicator(), anIIF);
		shared_ptr<IIFServerPrx> sharedIIF = Ice::uncheckedCast<IIFServerPrx>(_c.adapter->add(iif,Ice::stringToIdentity(_proxyName)));

		m_proxies.insert(pair<string, Proxy>(_proxyName, Proxy(focalStation, instrument, sharedIIF)));
		return sharedIIF;
	}

	return m_proxies.at(_proxyName).server;
}

//! Return the proxy definition.
string
FactoryI::getProxy(string _proxyName, const Ice::Current& _c __attribute__((unused)))
{
	try
	{
		IceUtil::LockT < IceUtil::RecMutex > guard (__connMutex);
		return m_proxies.at(_proxyName).str();
	}
	catch (out_of_range)
	{
		return "";
	}
}

//! Return the proxy instrument.
string
FactoryI::getProxyInstrument(string _proxyName, const Ice::Current& _c __attribute__((unused)))
{
	try
	{
		IceUtil::LockT < IceUtil::RecMutex > guard (__connMutex);
		return m_proxies.at(_proxyName).instrument;
	}
	catch (out_of_range)
	{
		return "";
	}
}

//! Return the proxy focal station.
string
FactoryI::getProxyFocalStation(string _proxyName, const Ice::Current& _c __attribute__((unused)))
{
	try
	{
		IceUtil::LockT < IceUtil::RecMutex > guard (__connMutex);
		return m_proxies.at(_proxyName).focalStation;
	}
	catch (out_of_range)
	{
		return "";
	}
}

//! Creates a list with all the current connections.
string
FactoryI::listConnections(const Ice::Current& _c __attribute__((unused)))
{
	ostringstream tmp;
	IceUtil::LockT < IceUtil::RecMutex > guard (__connMutex);
	for(auto& p : m_proxies) {
		tmp << p.first << " : " << p.second.str() << endl;
	}
	return tmp.str();
}

//! Remove the proxy.
void
FactoryI::destroyProxy(string _proxyName, const Ice::Current& _c)
{
	Ice::Print info(_c.adapter->getCommunicator()->getLogger());

	IceUtil::LockT < IceUtil::RecMutex > guard (__connMutex);
	auto iter = m_proxies.find(_proxyName);
	if (iter != m_proxies.end())
	{
		_c.adapter->remove(iter->second.server->ice_getIdentity());
		m_proxies.erase(iter);

		info << _proxyName << ":Erased proxy";

	}
	else
	{
		info << "Attempt to erase non-existent proxy: " << _proxyName;
	}
	info.flush();
}

//! Remove the proxy.
void
FactoryI::destroy(shared_ptr<IIFServerPrx> proxy, const Ice::Current& _c)
{
	try
	{
		destroyProxy(proxy->ice_getIdentity().name, _c);
	}
	catch(const Ice::ObjectAdapterDeactivatedException&)
	{
		// This method is called on shutdown of the server, in which
		// case this exception is expected.
	}
}

//! enable/disable debug mode.
void 
FactoryI::debugMode(bool b, const Ice::Current& _c __attribute__((unused)))
{
	m_debug = b;
}

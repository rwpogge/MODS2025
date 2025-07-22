#pragma once

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <iif/ice/IIFServer.slice.hpp>
#include <iif/ice/Factory.slice.hpp>
#include <map>
#include <memory>

namespace lbto {

/**
 * @class Proxy
 * Holds IIFServerPrx proxies. Implements contract between client and server object.
 */
class Proxy
{
public:
	/**
	 * Construct the class.
	 *
	 * @param _focalStation name of the focal station. Contains selected focus and side.
	 * @param _instrument instrument name (LUCIFER, LBC,..)
	 * @param _server server associated with the proxy
	 */
	Proxy(std::string _focalStation, std::string _instrument, std::shared_ptr<IIFServerPrx> _server);

	/**
	 * Converts proxy parameters to string describing the proxy.
	 *
	 * @return "<instrument> at <focus> <side>"
	 */
	std::string str();

	std::string focalStation;
	std::string instrument;

	std::shared_ptr<IIFServerPrx> server;
};

/**
* @class FactoryI
* @author Jose Borelli <borelli@mpia.de>
* @brief creates and links an ICE proxy object with a client connection. 
*/
class FactoryI : public Factory
{
	//! List of per-session allocated DirtI objects.
	std::map<std::string, Proxy > m_proxies;

	//!debug mode flag
	bool m_debug;

public:
	/** @brief class constructor */
	FactoryI();

	/** @brief class destructor */
	~FactoryI();

	/** @brief Creates or links an ICE proxy with the client connection. 
	@param  _proxyName Proxy Identification. If two or more clients use the same name, 
		both are linked to the same IIFServer object, sharing guide stars, target, etc.
	@param  _focalStation Instrument focal station
	@param _instrumentID Instrument ID
	@return The proxy pointer associated with this client. */
	std::shared_ptr<IIFServerPrx> create(std::string _proxyName, std::string _focalStation, std::string _instrumentID, const Ice::Current&) override;

	/** @brief Releases the proxy object and removes the entry from the DirtContainer
	@param _proxyName the client proxy name */
	void destroyProxy(std::string _proxyName, const Ice::Current&) override;

	/** @brief Releases the proxy object and removes the entry from the DirtContainer
	@param _prx the client proxy pointer */
	void destroy(std::shared_ptr<IIFServerPrx> _prx, const Ice::Current&) override;

	/** @brief Get the proxy definition.
	@return the proxy definition */
	std::string getProxy(std::string _proxyName, const Ice::Current&) override;

	/** @brief Get the proxy instrument.
	@return the proxy instrument */
	std::string getProxyInstrument(std::string _proxyName, const Ice::Current&) override;

	/** @brief Get the proxy focal station.
	@return the proxy focal station */
	std::string getProxyFocalStation(std::string _proxyName, const Ice::Current&) override;

	/** @brief Creates a list with all the current connections.
	@return the list of established connections */
	std::string listConnections(const Ice::Current&) override;

	/** @brief enable/disable debug mode.
	@param  b setting b to true, it will print out debug information in the syslog.*/
	void debugMode(bool b, const Ice::Current&) override;
private:
	//! Recursive Ice Mutex protection.
	IceUtil::RecMutex __connMutex;
};

} /* namespace lbto */

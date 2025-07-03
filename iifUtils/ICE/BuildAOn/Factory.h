// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

// Ice version 3.3.1
// Generated from file `Factory.ice'

#ifndef __Factory_h__
#define __Factory_h__

#include <Ice/LocalObjectF.h>
#include <Ice/ProxyF.h>
#include <Ice/ObjectF.h>
#include <Ice/Exception.h>
#include <Ice/LocalObject.h>
#include <Ice/Proxy.h>
#include <Ice/Object.h>
#include <Ice/Outgoing.h>
#include <Ice/OutgoingAsync.h>
#include <Ice/Incoming.h>
#include <Ice/Direct.h>
#include <Ice/StreamF.h>
#include <Ice/UndefSysMacros.h>

#include "IIFServer.h"

#ifndef ICE_IGNORE_VERSION
#   if ICE_INT_VERSION / 100 != 303
#       error Ice version mismatch!
#   endif
#   if ICE_INT_VERSION % 100 > 50
#       error Beta header file detected
#   endif
#   if ICE_INT_VERSION % 100 < 1
#       error Ice patch level mismatch!
#   endif
#endif

namespace IceProxy
{

namespace lbto
{

class Factory;

}

}

namespace lbto
{

class Factory;
bool operator==(const Factory&, const Factory&);
bool operator<(const Factory&, const Factory&);

}

namespace IceInternal
{

::Ice::Object* upCast(::lbto::Factory*);
::IceProxy::Ice::Object* upCast(::IceProxy::lbto::Factory*);

}

namespace lbto
{

typedef ::IceInternal::Handle< ::lbto::Factory> FactoryPtr;
typedef ::IceInternal::ProxyHandle< ::IceProxy::lbto::Factory> FactoryPrx;

void __read(::IceInternal::BasicStream*, FactoryPrx&);
void __patch__FactoryPtr(void*, ::Ice::ObjectPtr&);

}

namespace IceProxy
{

namespace lbto
{

class Factory : virtual public ::IceProxy::Ice::Object
{
public:

    ::lbto::IIFServerPrx create(const ::std::string& proxyName, const ::std::string& focalStation, const ::std::string& instrumentID)
    {
        return create(proxyName, focalStation, instrumentID, 0);
    }
    ::lbto::IIFServerPrx create(const ::std::string& proxyName, const ::std::string& focalStation, const ::std::string& instrumentID, const ::Ice::Context& __ctx)
    {
        return create(proxyName, focalStation, instrumentID, &__ctx);
    }
    
private:

    ::lbto::IIFServerPrx create(const ::std::string&, const ::std::string&, const ::std::string&, const ::Ice::Context*);
    
public:

    void destroy(const ::lbto::IIFServerPrx& proxy)
    {
        destroy(proxy, 0);
    }
    void destroy(const ::lbto::IIFServerPrx& proxy, const ::Ice::Context& __ctx)
    {
        destroy(proxy, &__ctx);
    }
    
private:

    void destroy(const ::lbto::IIFServerPrx&, const ::Ice::Context*);
    
public:

    ::std::string listConnections()
    {
        return listConnections(0);
    }
    ::std::string listConnections(const ::Ice::Context& __ctx)
    {
        return listConnections(&__ctx);
    }
    
private:

    ::std::string listConnections(const ::Ice::Context*);
    
public:

    void debugMode(bool b)
    {
        debugMode(b, 0);
    }
    void debugMode(bool b, const ::Ice::Context& __ctx)
    {
        debugMode(b, &__ctx);
    }
    
private:

    void debugMode(bool, const ::Ice::Context*);
    
public:
    
    ::IceInternal::ProxyHandle<Factory> ice_context(const ::Ice::Context& __context) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Factory*>(_Base::ice_context(__context).get());
    #else
        return dynamic_cast<Factory*>(::IceProxy::Ice::Object::ice_context(__context).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Factory> ice_adapterId(const std::string& __id) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Factory*>(_Base::ice_adapterId(__id).get());
    #else
        return dynamic_cast<Factory*>(::IceProxy::Ice::Object::ice_adapterId(__id).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Factory> ice_endpoints(const ::Ice::EndpointSeq& __endpoints) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Factory*>(_Base::ice_endpoints(__endpoints).get());
    #else
        return dynamic_cast<Factory*>(::IceProxy::Ice::Object::ice_endpoints(__endpoints).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Factory> ice_locatorCacheTimeout(int __timeout) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Factory*>(_Base::ice_locatorCacheTimeout(__timeout).get());
    #else
        return dynamic_cast<Factory*>(::IceProxy::Ice::Object::ice_locatorCacheTimeout(__timeout).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Factory> ice_connectionCached(bool __cached) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Factory*>(_Base::ice_connectionCached(__cached).get());
    #else
        return dynamic_cast<Factory*>(::IceProxy::Ice::Object::ice_connectionCached(__cached).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Factory> ice_endpointSelection(::Ice::EndpointSelectionType __est) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Factory*>(_Base::ice_endpointSelection(__est).get());
    #else
        return dynamic_cast<Factory*>(::IceProxy::Ice::Object::ice_endpointSelection(__est).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Factory> ice_secure(bool __secure) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Factory*>(_Base::ice_secure(__secure).get());
    #else
        return dynamic_cast<Factory*>(::IceProxy::Ice::Object::ice_secure(__secure).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Factory> ice_preferSecure(bool __preferSecure) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Factory*>(_Base::ice_preferSecure(__preferSecure).get());
    #else
        return dynamic_cast<Factory*>(::IceProxy::Ice::Object::ice_preferSecure(__preferSecure).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Factory> ice_router(const ::Ice::RouterPrx& __router) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Factory*>(_Base::ice_router(__router).get());
    #else
        return dynamic_cast<Factory*>(::IceProxy::Ice::Object::ice_router(__router).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Factory> ice_locator(const ::Ice::LocatorPrx& __locator) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Factory*>(_Base::ice_locator(__locator).get());
    #else
        return dynamic_cast<Factory*>(::IceProxy::Ice::Object::ice_locator(__locator).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Factory> ice_collocationOptimized(bool __co) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Factory*>(_Base::ice_collocationOptimized(__co).get());
    #else
        return dynamic_cast<Factory*>(::IceProxy::Ice::Object::ice_collocationOptimized(__co).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Factory> ice_twoway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Factory*>(_Base::ice_twoway().get());
    #else
        return dynamic_cast<Factory*>(::IceProxy::Ice::Object::ice_twoway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Factory> ice_oneway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Factory*>(_Base::ice_oneway().get());
    #else
        return dynamic_cast<Factory*>(::IceProxy::Ice::Object::ice_oneway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Factory> ice_batchOneway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Factory*>(_Base::ice_batchOneway().get());
    #else
        return dynamic_cast<Factory*>(::IceProxy::Ice::Object::ice_batchOneway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Factory> ice_datagram() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Factory*>(_Base::ice_datagram().get());
    #else
        return dynamic_cast<Factory*>(::IceProxy::Ice::Object::ice_datagram().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Factory> ice_batchDatagram() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Factory*>(_Base::ice_batchDatagram().get());
    #else
        return dynamic_cast<Factory*>(::IceProxy::Ice::Object::ice_batchDatagram().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Factory> ice_compress(bool __compress) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Factory*>(_Base::ice_compress(__compress).get());
    #else
        return dynamic_cast<Factory*>(::IceProxy::Ice::Object::ice_compress(__compress).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Factory> ice_timeout(int __timeout) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Factory*>(_Base::ice_timeout(__timeout).get());
    #else
        return dynamic_cast<Factory*>(::IceProxy::Ice::Object::ice_timeout(__timeout).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<Factory> ice_connectionId(const std::string& __id) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<Factory*>(_Base::ice_connectionId(__id).get());
    #else
        return dynamic_cast<Factory*>(::IceProxy::Ice::Object::ice_connectionId(__id).get());
    #endif
    }
    
    static const ::std::string& ice_staticId();

private: 

    virtual ::IceInternal::Handle< ::IceDelegateM::Ice::Object> __createDelegateM();
    virtual ::IceInternal::Handle< ::IceDelegateD::Ice::Object> __createDelegateD();
    virtual ::IceProxy::Ice::Object* __newInstance() const;
};

}

}

namespace IceDelegate
{

namespace lbto
{

class Factory : virtual public ::IceDelegate::Ice::Object
{
public:

    virtual ::lbto::IIFServerPrx create(const ::std::string&, const ::std::string&, const ::std::string&, const ::Ice::Context*) = 0;

    virtual void destroy(const ::lbto::IIFServerPrx&, const ::Ice::Context*) = 0;

    virtual ::std::string listConnections(const ::Ice::Context*) = 0;

    virtual void debugMode(bool, const ::Ice::Context*) = 0;
};

}

}

namespace IceDelegateM
{

namespace lbto
{

class Factory : virtual public ::IceDelegate::lbto::Factory,
                virtual public ::IceDelegateM::Ice::Object
{
public:

    virtual ::lbto::IIFServerPrx create(const ::std::string&, const ::std::string&, const ::std::string&, const ::Ice::Context*);

    virtual void destroy(const ::lbto::IIFServerPrx&, const ::Ice::Context*);

    virtual ::std::string listConnections(const ::Ice::Context*);

    virtual void debugMode(bool, const ::Ice::Context*);
};

}

}

namespace IceDelegateD
{

namespace lbto
{

class Factory : virtual public ::IceDelegate::lbto::Factory,
                virtual public ::IceDelegateD::Ice::Object
{
public:

    virtual ::lbto::IIFServerPrx create(const ::std::string&, const ::std::string&, const ::std::string&, const ::Ice::Context*);

    virtual void destroy(const ::lbto::IIFServerPrx&, const ::Ice::Context*);

    virtual ::std::string listConnections(const ::Ice::Context*);

    virtual void debugMode(bool, const ::Ice::Context*);
};

}

}

namespace lbto
{

class Factory : virtual public ::Ice::Object
{
public:

    typedef FactoryPrx ProxyType;
    typedef FactoryPtr PointerType;
    
    virtual ::Ice::ObjectPtr ice_clone() const;

    virtual bool ice_isA(const ::std::string&, const ::Ice::Current& = ::Ice::Current()) const;
    virtual ::std::vector< ::std::string> ice_ids(const ::Ice::Current& = ::Ice::Current()) const;
    virtual const ::std::string& ice_id(const ::Ice::Current& = ::Ice::Current()) const;
    static const ::std::string& ice_staticId();

    virtual ::lbto::IIFServerPrx create(const ::std::string&, const ::std::string&, const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___create(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual void destroy(const ::lbto::IIFServerPrx&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___destroy(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::std::string listConnections(const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___listConnections(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual void debugMode(bool, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___debugMode(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::Ice::DispatchStatus __dispatch(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual void __write(::IceInternal::BasicStream*) const;
    virtual void __read(::IceInternal::BasicStream*, bool);
    virtual void __write(const ::Ice::OutputStreamPtr&) const;
    virtual void __read(const ::Ice::InputStreamPtr&, bool);
};

}

#endif

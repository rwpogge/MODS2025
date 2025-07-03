// **********************************************************************
//
// Copyright (c) 2003-2009 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

// Ice version 3.3.1
// Generated from file `IIFServer.ice'

#ifndef __IIFServer_h__
#define __IIFServer_h__

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

class IIFServer;

}

}

namespace lbto
{

class IIFServer;
bool operator==(const IIFServer&, const IIFServer&);
bool operator<(const IIFServer&, const IIFServer&);

}

namespace IceInternal
{

::Ice::Object* upCast(::lbto::IIFServer*);
::IceProxy::Ice::Object* upCast(::IceProxy::lbto::IIFServer*);

}

namespace lbto
{

typedef ::IceInternal::Handle< ::lbto::IIFServer> IIFServerPtr;
typedef ::IceInternal::ProxyHandle< ::IceProxy::lbto::IIFServer> IIFServerPrx;

void __read(::IceInternal::BasicStream*, IIFServerPrx&);
void __patch__IIFServerPtr(void*, ::Ice::ObjectPtr&);

}

namespace lbto
{

struct DDstruct
{
    ::std::string DDname;
    ::std::string DDkey;
    ::std::string dataType;
    ::std::string comment;

    bool operator==(const DDstruct&) const;
    bool operator<(const DDstruct&) const;
    bool operator!=(const DDstruct& __rhs) const
    {
        return !operator==(__rhs);
    }
    bool operator<=(const DDstruct& __rhs) const
    {
        return operator<(__rhs) || operator==(__rhs);
    }
    bool operator>(const DDstruct& __rhs) const
    {
        return !operator<(__rhs) && !operator==(__rhs);
    }
    bool operator>=(const DDstruct& __rhs) const
    {
        return !operator<(__rhs);
    }

    void __write(::IceInternal::BasicStream*) const;
    void __read(::IceInternal::BasicStream*);
};

typedef ::std::vector< ::lbto::DDstruct> SeqDD;
void __writeSeqDD(::IceInternal::BasicStream*, const ::lbto::DDstruct*, const ::lbto::DDstruct*);
void __readSeqDD(::IceInternal::BasicStream*, SeqDD&);

struct position
{
    ::Ice::Double coord1;
    ::Ice::Double coord2;
    ::std::string system;
    ::Ice::Double epoch;
    ::std::string equinox;
    ::Ice::Double pmcoord1;
    ::Ice::Double pmcoord2;
    ::Ice::Double apparentMagnitude;
    ::std::string filter;
    ::Ice::Double color;
    ::std::string colorType;
    ::Ice::Float wavelength;
    ::std::string targname;

    bool operator==(const position&) const;
    bool operator<(const position&) const;
    bool operator!=(const position& __rhs) const
    {
        return !operator==(__rhs);
    }
    bool operator<=(const position& __rhs) const
    {
        return operator<(__rhs) || operator==(__rhs);
    }
    bool operator>(const position& __rhs) const
    {
        return !operator<(__rhs) && !operator==(__rhs);
    }
    bool operator>=(const position& __rhs) const
    {
        return !operator<(__rhs);
    }

    void __write(::IceInternal::BasicStream*) const;
    void __read(::IceInternal::BasicStream*);
};

typedef ::std::vector< ::lbto::position> SeqPos;
void __writeSeqPos(::IceInternal::BasicStream*, const ::lbto::position*, const ::lbto::position*);
void __readSeqPos(::IceInternal::BasicStream*, SeqPos&);

struct newposition
{
    ::std::string type;
    ::Ice::Double coord1;
    ::Ice::Double coord2;
    ::std::string system;
    ::std::string equinox;
    ::Ice::Double epoch;
    ::Ice::Double pmcoord1;
    ::Ice::Double pmcoord2;
    ::Ice::Double apparentMagnitude;
    ::std::string filter;
    ::Ice::Double color;
    ::std::string colorType;
    ::Ice::Float wavelength;
    ::std::string file;
    ::Ice::Double time;
    ::Ice::Double RARate;
    ::Ice::Double DECRate;

    bool operator==(const newposition&) const;
    bool operator<(const newposition&) const;
    bool operator!=(const newposition& __rhs) const
    {
        return !operator==(__rhs);
    }
    bool operator<=(const newposition& __rhs) const
    {
        return operator<(__rhs) || operator==(__rhs);
    }
    bool operator>(const newposition& __rhs) const
    {
        return !operator<(__rhs) && !operator==(__rhs);
    }
    bool operator>=(const newposition& __rhs) const
    {
        return !operator<(__rhs);
    }

    void __write(::IceInternal::BasicStream*) const;
    void __read(::IceInternal::BasicStream*);
};

typedef ::std::vector< ::lbto::newposition> SeqNewPos;
void __writeSeqNewPos(::IceInternal::BasicStream*, const ::lbto::newposition*, const ::lbto::newposition*);
void __readSeqNewPos(::IceInternal::BasicStream*, SeqNewPos&);

struct nonsidereal
{
    ::std::string type;
    ::Ice::Float wavelength;
    ::std::string file;
    ::Ice::Double value0;
    ::Ice::Double value1;
    ::Ice::Double value2;
    ::Ice::Double value3;
    ::Ice::Double value4;

    bool operator==(const nonsidereal&) const;
    bool operator<(const nonsidereal&) const;
    bool operator!=(const nonsidereal& __rhs) const
    {
        return !operator==(__rhs);
    }
    bool operator<=(const nonsidereal& __rhs) const
    {
        return operator<(__rhs) || operator==(__rhs);
    }
    bool operator>(const nonsidereal& __rhs) const
    {
        return !operator<(__rhs) && !operator==(__rhs);
    }
    bool operator>=(const nonsidereal& __rhs) const
    {
        return !operator<(__rhs);
    }

    void __write(::IceInternal::BasicStream*) const;
    void __read(::IceInternal::BasicStream*);
};

typedef ::std::vector< ::Ice::Double> SeqWF;

typedef ::std::vector< ::Ice::Double> SeqModes;

typedef ::std::vector< ::std::string> SeqRes;

struct result
{
    ::Ice::Int rescode;
    ::lbto::SeqRes resmsg;

    bool operator==(const result&) const;
    bool operator<(const result&) const;
    bool operator!=(const result& __rhs) const
    {
        return !operator==(__rhs);
    }
    bool operator<=(const result& __rhs) const
    {
        return operator<(__rhs) || operator==(__rhs);
    }
    bool operator>(const result& __rhs) const
    {
        return !operator<(__rhs) && !operator==(__rhs);
    }
    bool operator>=(const result& __rhs) const
    {
        return !operator<(__rhs);
    }

    void __write(::IceInternal::BasicStream*) const;
    void __read(::IceInternal::BasicStream*);
};

}

namespace lbto
{

class AMI_IIFServer_Authorize : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_Authorize> AMI_IIFServer_AuthorizePtr;

class AMI_IIFServer_StepFocus : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, ::Ice::Double, const ::std::string&, const ::std::string&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_StepFocus> AMI_IIFServer_StepFocusPtr;

class AMI_IIFServer_MoveFocus : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, ::Ice::Double, const ::std::string&, const ::std::string&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_MoveFocus> AMI_IIFServer_MoveFocusPtr;

class AMI_IIFServer_OffsetPointing : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, ::Ice::Double, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::std::string&, const ::std::string&, const ::std::string&, const ::std::string&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_OffsetPointing> AMI_IIFServer_OffsetPointingPtr;

class AMI_IIFServer_OffsetGuiding : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, ::Ice::Double, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::std::string&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_OffsetGuiding> AMI_IIFServer_OffsetGuidingPtr;

class AMI_IIFServer_UpdatePointingReference : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, const ::std::string&, const ::std::string&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_UpdatePointingReference> AMI_IIFServer_UpdatePointingReferencePtr;

class AMI_IIFServer_TipTilt : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::std::string&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_TipTilt> AMI_IIFServer_TipTiltPtr;

class AMI_IIFServer_SetPMTerm : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, const ::std::string&, ::Ice::Double, const ::std::string&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_SetPMTerm> AMI_IIFServer_SetPMTermPtr;

class AMI_IIFServer_MoveXYZ : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, ::Ice::Double, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_MoveXYZ> AMI_IIFServer_MoveXYZPtr;

class AMI_IIFServer_MoveXY : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::std::string&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_MoveXY> AMI_IIFServer_MoveXYPtr;

class AMI_IIFServer_Move : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, ::Ice::Double, ::Ice::Double, ::Ice::Double, ::Ice::Double, ::Ice::Double, ::Ice::Double, ::Ice::Int, const ::std::string&, const ::std::string&, ::Ice::Int, const ::std::string&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_Move> AMI_IIFServer_MovePtr;

class AMI_IIFServer_SetTarget : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, ::Ice::Double, ::Ice::Double, const ::std::string&, ::Ice::Double, ::Ice::Float, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_SetTarget> AMI_IIFServer_SetTargetPtr;

class AMI_IIFServer_SetStars : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, const ::lbto::SeqPos&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_SetStars> AMI_IIFServer_SetStarsPtr;

class AMI_IIFServer_SetStarsNew : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, const ::lbto::SeqNewPos&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_SetStarsNew> AMI_IIFServer_SetStarsNewPtr;

class AMI_IIFServer_ClearStars : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_ClearStars> AMI_IIFServer_ClearStarsPtr;

class AMI_IIFServer_SetHotspot : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, ::Ice::Double, ::Ice::Double, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_SetHotspot> AMI_IIFServer_SetHotspotPtr;

class AMI_IIFServer_ClearHotspot : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_ClearHotspot> AMI_IIFServer_ClearHotspotPtr;

class AMI_IIFServer_SetOffset : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_SetOffset> AMI_IIFServer_SetOffsetPtr;

class AMI_IIFServer_ClearOffset : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_ClearOffset> AMI_IIFServer_ClearOffsetPtr;

class AMI_IIFServer_SetNonSidereal : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, const ::lbto::nonsidereal&, bool, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_SetNonSidereal> AMI_IIFServer_SetNonSiderealPtr;

class AMI_IIFServer_ClearNonSidereal : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, bool, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_ClearNonSidereal> AMI_IIFServer_ClearNonSiderealPtr;

class AMI_IIFServer_UpdateNonSiderealTarget : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, ::Ice::Double, ::Ice::Double, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_UpdateNonSiderealTarget> AMI_IIFServer_UpdateNonSiderealTargetPtr;

class AMI_IIFServer_PresetTelescope : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, ::Ice::Double, const ::std::string&, const ::std::string&, const ::std::string&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_PresetTelescope> AMI_IIFServer_PresetTelescopePtr;

class AMI_IIFServer_BinocularControl : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, const ::std::string&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_BinocularControl> AMI_IIFServer_BinocularControlPtr;

class AMI_IIFServer_GetParameter : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, const ::lbto::SeqDD&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_GetParameter> AMI_IIFServer_GetParameterPtr;

class AMI_IIFServer_SetParameter : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, const ::lbto::SeqDD&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_SetParameter> AMI_IIFServer_SetParameterPtr;

class AMI_IIFServer_RotateCommon : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, ::Ice::Double, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_RotateCommon> AMI_IIFServer_RotateCommonPtr;

class AMI_IIFServer_RotateZ : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, ::Ice::Double, const ::std::string&, const ::std::string&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_RotateZ> AMI_IIFServer_RotateZPtr;

class AMI_IIFServer_RotatePrimary : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, ::Ice::Double, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_RotatePrimary> AMI_IIFServer_RotatePrimaryPtr;

class AMI_IIFServer_SendWavefront : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, const ::lbto::SeqWF&, const ::std::string&, const ::std::string&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_SendWavefront> AMI_IIFServer_SendWavefrontPtr;

class AMI_IIFServer_UpdateTargetWavelength : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, ::Ice::Float, const ::std::string&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_UpdateTargetWavelength> AMI_IIFServer_UpdateTargetWavelengthPtr;

class AMI_IIFServer_LogEvent : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, const ::std::string&, const ::std::string&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_LogEvent> AMI_IIFServer_LogEventPtr;

class AMI_IIFServer_GetRotatorTrajectory : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, ::Ice::Double, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_GetRotatorTrajectory> AMI_IIFServer_GetRotatorTrajectoryPtr;

class AMI_IIFServer_GetRotatorPolynomials : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, ::Ice::Double, ::Ice::Int, ::Ice::Double, ::Ice::Int, const ::std::string&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_GetRotatorPolynomials> AMI_IIFServer_GetRotatorPolynomialsPtr;

class AMI_IIFServer_Standby : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, ::Ice::Int, const ::std::string&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_Standby> AMI_IIFServer_StandbyPtr;

class AMI_IIFServer_MaximizeWrapTime : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, bool, bool, const ::std::string&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_MaximizeWrapTime> AMI_IIFServer_MaximizeWrapTimePtr;

class AMI_IIFServer_PauseGuiding : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, const ::std::string&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_PauseGuiding> AMI_IIFServer_PauseGuidingPtr;

class AMI_IIFServer_ResumeGuiding : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, const ::std::string&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_ResumeGuiding> AMI_IIFServer_ResumeGuidingPtr;

class AMI_IIFServer_SetGuidingHotspot : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::std::string&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_SetGuidingHotspot> AMI_IIFServer_SetGuidingHotspotPtr;

class AMI_IIFServer_RotReady : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, bool, const ::std::string&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_RotReady> AMI_IIFServer_RotReadyPtr;

class AMI_IIFServer_RotHold : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, const ::std::string&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_RotHold> AMI_IIFServer_RotHoldPtr;

class AMI_IIFServer_RotTrack : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, const ::std::string&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_RotTrack> AMI_IIFServer_RotTrackPtr;

class AMI_IIFServer_RotServicePosition : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, ::Ice::Double, const ::std::string&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_RotServicePosition> AMI_IIFServer_RotServicePositionPtr;

class AMI_IIFServer_PauseAO : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, const ::std::string&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_PauseAO> AMI_IIFServer_PauseAOPtr;

class AMI_IIFServer_ResumeAO : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, const ::std::string&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_ResumeAO> AMI_IIFServer_ResumeAOPtr;

class AMI_IIFServer_StartAO : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, const ::std::string&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_StartAO> AMI_IIFServer_StartAOPtr;

class AMI_IIFServer_StopAO : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, const ::std::string&, const ::std::string&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_StopAO> AMI_IIFServer_StopAOPtr;

class AMI_IIFServer_AcquireRefAO : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, bool, const ::std::string&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_AcquireRefAO> AMI_IIFServer_AcquireRefAOPtr;

class AMI_IIFServer_CheckRefAO : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, const ::std::string&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_CheckRefAO> AMI_IIFServer_CheckRefAOPtr;

class AMI_IIFServer_RefineAO : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, const ::std::string&, const ::std::string&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_RefineAO> AMI_IIFServer_RefineAOPtr;

class AMI_IIFServer_RunAO : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, const ::std::string&, const ::std::string&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_RunAO> AMI_IIFServer_RunAOPtr;

class AMI_IIFServer_PresetFlatAO : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, const ::std::string&, const ::std::string&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_PresetFlatAO> AMI_IIFServer_PresetFlatAOPtr;

class AMI_IIFServer_CorrectModesAO : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, const ::lbto::SeqModes&, const ::std::string&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_CorrectModesAO> AMI_IIFServer_CorrectModesAOPtr;

class AMI_IIFServer_ModifyAO : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, ::Ice::Int, ::Ice::Double, ::Ice::Int, ::Ice::Double, const ::std::string&, const ::std::string&, const ::std::string&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_ModifyAO> AMI_IIFServer_ModifyAOPtr;

class AMI_IIFServer_PresetAO : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, const ::std::string&, const ::std::string&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_PresetAO> AMI_IIFServer_PresetAOPtr;

class AMI_IIFServer_OffsetXYAO : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_OffsetXYAO> AMI_IIFServer_OffsetXYAOPtr;

class AMI_IIFServer_OffsetZAO : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, ::Ice::Double, const ::std::string&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_OffsetZAO> AMI_IIFServer_OffsetZAOPtr;

class AMI_IIFServer_SetReference : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, const ::lbto::SeqPos&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_SetReference> AMI_IIFServer_SetReferencePtr;

class AMI_IIFServer_SetReferenceNew : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, const ::lbto::SeqNewPos&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_SetReferenceNew> AMI_IIFServer_SetReferenceNewPtr;

class AMI_IIFServer_ClearReference : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_ClearReference> AMI_IIFServer_ClearReferencePtr;

class AMI_IIFServer_GetKFPCoordinates : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, const ::lbto::SeqPos&, const ::std::string&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_GetKFPCoordinates> AMI_IIFServer_GetKFPCoordinatesPtr;

class AMI_IIFServer_GetKFPCoordinatesNew : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, const ::lbto::SeqNewPos&, const ::std::string&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_GetKFPCoordinatesNew> AMI_IIFServer_GetKFPCoordinatesNewPtr;

class AMI_IIFServer_SetAGWFilter : public ::IceInternal::OutgoingAsync
{
public:

    virtual void ice_response(const ::lbto::result&) = 0;
    virtual void ice_exception(const ::Ice::Exception&) = 0;

    bool __invoke(const ::lbto::IIFServerPrx&, ::Ice::Int, const ::std::string&, const ::Ice::Context*);

protected:

    virtual void __response(bool);
};

typedef ::IceUtil::Handle< ::lbto::AMI_IIFServer_SetAGWFilter> AMI_IIFServer_SetAGWFilterPtr;

}

namespace IceProxy
{

namespace lbto
{

class IIFServer : virtual public ::IceProxy::Ice::Object
{
public:

    ::lbto::result Authorize()
    {
        return Authorize(0);
    }
    ::lbto::result Authorize(const ::Ice::Context& __ctx)
    {
        return Authorize(&__ctx);
    }
    
private:

    ::lbto::result Authorize(const ::Ice::Context*);
    
public:
    bool Authorize_async(const ::lbto::AMI_IIFServer_AuthorizePtr&);
    bool Authorize_async(const ::lbto::AMI_IIFServer_AuthorizePtr&, const ::Ice::Context&);

    ::lbto::result StepFocus(::Ice::Double relPos, const ::std::string& ope, const ::std::string& side)
    {
        return StepFocus(relPos, ope, side, 0);
    }
    ::lbto::result StepFocus(::Ice::Double relPos, const ::std::string& ope, const ::std::string& side, const ::Ice::Context& __ctx)
    {
        return StepFocus(relPos, ope, side, &__ctx);
    }
    
private:

    ::lbto::result StepFocus(::Ice::Double, const ::std::string&, const ::std::string&, const ::Ice::Context*);
    
public:
    bool StepFocus_async(const ::lbto::AMI_IIFServer_StepFocusPtr&, ::Ice::Double, const ::std::string&, const ::std::string&);
    bool StepFocus_async(const ::lbto::AMI_IIFServer_StepFocusPtr&, ::Ice::Double, const ::std::string&, const ::std::string&, const ::Ice::Context&);

    ::lbto::result MoveFocus(::Ice::Double absPos, const ::std::string& ope, const ::std::string& side)
    {
        return MoveFocus(absPos, ope, side, 0);
    }
    ::lbto::result MoveFocus(::Ice::Double absPos, const ::std::string& ope, const ::std::string& side, const ::Ice::Context& __ctx)
    {
        return MoveFocus(absPos, ope, side, &__ctx);
    }
    
private:

    ::lbto::result MoveFocus(::Ice::Double, const ::std::string&, const ::std::string&, const ::Ice::Context*);
    
public:
    bool MoveFocus_async(const ::lbto::AMI_IIFServer_MoveFocusPtr&, ::Ice::Double, const ::std::string&, const ::std::string&);
    bool MoveFocus_async(const ::lbto::AMI_IIFServer_MoveFocusPtr&, ::Ice::Double, const ::std::string&, const ::std::string&, const ::Ice::Context&);

    ::lbto::result OffsetPointing(::Ice::Double rotangle, ::Ice::Double offx, ::Ice::Double offy, const ::std::string& coordsys, const ::std::string& ope, const ::std::string& np, const ::std::string& movetype, const ::std::string& side)
    {
        return OffsetPointing(rotangle, offx, offy, coordsys, ope, np, movetype, side, 0);
    }
    ::lbto::result OffsetPointing(::Ice::Double rotangle, ::Ice::Double offx, ::Ice::Double offy, const ::std::string& coordsys, const ::std::string& ope, const ::std::string& np, const ::std::string& movetype, const ::std::string& side, const ::Ice::Context& __ctx)
    {
        return OffsetPointing(rotangle, offx, offy, coordsys, ope, np, movetype, side, &__ctx);
    }
    
private:

    ::lbto::result OffsetPointing(::Ice::Double, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::std::string&, const ::std::string&, const ::std::string&, const ::std::string&, const ::Ice::Context*);
    
public:
    bool OffsetPointing_async(const ::lbto::AMI_IIFServer_OffsetPointingPtr&, ::Ice::Double, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::std::string&, const ::std::string&, const ::std::string&, const ::std::string&);
    bool OffsetPointing_async(const ::lbto::AMI_IIFServer_OffsetPointingPtr&, ::Ice::Double, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::std::string&, const ::std::string&, const ::std::string&, const ::std::string&, const ::Ice::Context&);

    ::lbto::result OffsetGuiding(::Ice::Double rotangle, ::Ice::Double offx, ::Ice::Double offy, const ::std::string& coordsys, const ::std::string& side)
    {
        return OffsetGuiding(rotangle, offx, offy, coordsys, side, 0);
    }
    ::lbto::result OffsetGuiding(::Ice::Double rotangle, ::Ice::Double offx, ::Ice::Double offy, const ::std::string& coordsys, const ::std::string& side, const ::Ice::Context& __ctx)
    {
        return OffsetGuiding(rotangle, offx, offy, coordsys, side, &__ctx);
    }
    
private:

    ::lbto::result OffsetGuiding(::Ice::Double, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::std::string&, const ::Ice::Context*);
    
public:
    bool OffsetGuiding_async(const ::lbto::AMI_IIFServer_OffsetGuidingPtr&, ::Ice::Double, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::std::string&);
    bool OffsetGuiding_async(const ::lbto::AMI_IIFServer_OffsetGuidingPtr&, ::Ice::Double, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::std::string&, const ::Ice::Context&);

    ::lbto::result UpdatePointingReference(const ::std::string& offsetcoord, const ::std::string& side)
    {
        return UpdatePointingReference(offsetcoord, side, 0);
    }
    ::lbto::result UpdatePointingReference(const ::std::string& offsetcoord, const ::std::string& side, const ::Ice::Context& __ctx)
    {
        return UpdatePointingReference(offsetcoord, side, &__ctx);
    }
    
private:

    ::lbto::result UpdatePointingReference(const ::std::string&, const ::std::string&, const ::Ice::Context*);
    
public:
    bool UpdatePointingReference_async(const ::lbto::AMI_IIFServer_UpdatePointingReferencePtr&, const ::std::string&, const ::std::string&);
    bool UpdatePointingReference_async(const ::lbto::AMI_IIFServer_UpdatePointingReferencePtr&, const ::std::string&, const ::std::string&, const ::Ice::Context&);

    ::lbto::result TipTilt(::Ice::Double xrot, ::Ice::Double yrot, const ::std::string& ope, const ::std::string& side)
    {
        return TipTilt(xrot, yrot, ope, side, 0);
    }
    ::lbto::result TipTilt(::Ice::Double xrot, ::Ice::Double yrot, const ::std::string& ope, const ::std::string& side, const ::Ice::Context& __ctx)
    {
        return TipTilt(xrot, yrot, ope, side, &__ctx);
    }
    
private:

    ::lbto::result TipTilt(::Ice::Double, ::Ice::Double, const ::std::string&, const ::std::string&, const ::Ice::Context*);
    
public:
    bool TipTilt_async(const ::lbto::AMI_IIFServer_TipTiltPtr&, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::std::string&);
    bool TipTilt_async(const ::lbto::AMI_IIFServer_TipTiltPtr&, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::std::string&, const ::Ice::Context&);

    ::lbto::result SetPMTerm(const ::std::string& name, ::Ice::Double value, const ::std::string& side)
    {
        return SetPMTerm(name, value, side, 0);
    }
    ::lbto::result SetPMTerm(const ::std::string& name, ::Ice::Double value, const ::std::string& side, const ::Ice::Context& __ctx)
    {
        return SetPMTerm(name, value, side, &__ctx);
    }
    
private:

    ::lbto::result SetPMTerm(const ::std::string&, ::Ice::Double, const ::std::string&, const ::Ice::Context*);
    
public:
    bool SetPMTerm_async(const ::lbto::AMI_IIFServer_SetPMTermPtr&, const ::std::string&, ::Ice::Double, const ::std::string&);
    bool SetPMTerm_async(const ::lbto::AMI_IIFServer_SetPMTermPtr&, const ::std::string&, ::Ice::Double, const ::std::string&, const ::Ice::Context&);

    ::lbto::result MoveXYZ(::Ice::Double xmov, ::Ice::Double ymov, ::Ice::Double zmov, const ::std::string& side)
    {
        return MoveXYZ(xmov, ymov, zmov, side, 0);
    }
    ::lbto::result MoveXYZ(::Ice::Double xmov, ::Ice::Double ymov, ::Ice::Double zmov, const ::std::string& side, const ::Ice::Context& __ctx)
    {
        return MoveXYZ(xmov, ymov, zmov, side, &__ctx);
    }
    
private:

    ::lbto::result MoveXYZ(::Ice::Double, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::Ice::Context*);
    
public:
    bool MoveXYZ_async(const ::lbto::AMI_IIFServer_MoveXYZPtr&, ::Ice::Double, ::Ice::Double, ::Ice::Double, const ::std::string&);
    bool MoveXYZ_async(const ::lbto::AMI_IIFServer_MoveXYZPtr&, ::Ice::Double, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::Ice::Context&);

    ::lbto::result MoveXY(::Ice::Double xmov, ::Ice::Double ymov, const ::std::string& ope, const ::std::string& side)
    {
        return MoveXY(xmov, ymov, ope, side, 0);
    }
    ::lbto::result MoveXY(::Ice::Double xmov, ::Ice::Double ymov, const ::std::string& ope, const ::std::string& side, const ::Ice::Context& __ctx)
    {
        return MoveXY(xmov, ymov, ope, side, &__ctx);
    }
    
private:

    ::lbto::result MoveXY(::Ice::Double, ::Ice::Double, const ::std::string&, const ::std::string&, const ::Ice::Context*);
    
public:
    bool MoveXY_async(const ::lbto::AMI_IIFServer_MoveXYPtr&, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::std::string&);
    bool MoveXY_async(const ::lbto::AMI_IIFServer_MoveXYPtr&, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::std::string&, const ::Ice::Context&);

    ::lbto::result Move(::Ice::Double x, ::Ice::Double y, ::Ice::Double z, ::Ice::Double rx, ::Ice::Double ry, ::Ice::Double rz, ::Ice::Int dflag, const ::std::string& movetype, const ::std::string& OPE, ::Ice::Int time, const ::std::string& side)
    {
        return Move(x, y, z, rx, ry, rz, dflag, movetype, OPE, time, side, 0);
    }
    ::lbto::result Move(::Ice::Double x, ::Ice::Double y, ::Ice::Double z, ::Ice::Double rx, ::Ice::Double ry, ::Ice::Double rz, ::Ice::Int dflag, const ::std::string& movetype, const ::std::string& OPE, ::Ice::Int time, const ::std::string& side, const ::Ice::Context& __ctx)
    {
        return Move(x, y, z, rx, ry, rz, dflag, movetype, OPE, time, side, &__ctx);
    }
    
private:

    ::lbto::result Move(::Ice::Double, ::Ice::Double, ::Ice::Double, ::Ice::Double, ::Ice::Double, ::Ice::Double, ::Ice::Int, const ::std::string&, const ::std::string&, ::Ice::Int, const ::std::string&, const ::Ice::Context*);
    
public:
    bool Move_async(const ::lbto::AMI_IIFServer_MovePtr&, ::Ice::Double, ::Ice::Double, ::Ice::Double, ::Ice::Double, ::Ice::Double, ::Ice::Double, ::Ice::Int, const ::std::string&, const ::std::string&, ::Ice::Int, const ::std::string&);
    bool Move_async(const ::lbto::AMI_IIFServer_MovePtr&, ::Ice::Double, ::Ice::Double, ::Ice::Double, ::Ice::Double, ::Ice::Double, ::Ice::Double, ::Ice::Int, const ::std::string&, const ::std::string&, ::Ice::Int, const ::std::string&, const ::Ice::Context&);

    ::lbto::result SetTarget(::Ice::Double coord1, ::Ice::Double coord2, const ::std::string& system, ::Ice::Double epoch, ::Ice::Float wavelength)
    {
        return SetTarget(coord1, coord2, system, epoch, wavelength, 0);
    }
    ::lbto::result SetTarget(::Ice::Double coord1, ::Ice::Double coord2, const ::std::string& system, ::Ice::Double epoch, ::Ice::Float wavelength, const ::Ice::Context& __ctx)
    {
        return SetTarget(coord1, coord2, system, epoch, wavelength, &__ctx);
    }
    
private:

    ::lbto::result SetTarget(::Ice::Double, ::Ice::Double, const ::std::string&, ::Ice::Double, ::Ice::Float, const ::Ice::Context*);
    
public:
    bool SetTarget_async(const ::lbto::AMI_IIFServer_SetTargetPtr&, ::Ice::Double, ::Ice::Double, const ::std::string&, ::Ice::Double, ::Ice::Float);
    bool SetTarget_async(const ::lbto::AMI_IIFServer_SetTargetPtr&, ::Ice::Double, ::Ice::Double, const ::std::string&, ::Ice::Double, ::Ice::Float, const ::Ice::Context&);

    ::lbto::result SetStars(const ::lbto::SeqPos& stars)
    {
        return SetStars(stars, 0);
    }
    ::lbto::result SetStars(const ::lbto::SeqPos& stars, const ::Ice::Context& __ctx)
    {
        return SetStars(stars, &__ctx);
    }
    
private:

    ::lbto::result SetStars(const ::lbto::SeqPos&, const ::Ice::Context*);
    
public:
    bool SetStars_async(const ::lbto::AMI_IIFServer_SetStarsPtr&, const ::lbto::SeqPos&);
    bool SetStars_async(const ::lbto::AMI_IIFServer_SetStarsPtr&, const ::lbto::SeqPos&, const ::Ice::Context&);

    ::lbto::result SetStarsNew(const ::lbto::SeqNewPos& stars)
    {
        return SetStarsNew(stars, 0);
    }
    ::lbto::result SetStarsNew(const ::lbto::SeqNewPos& stars, const ::Ice::Context& __ctx)
    {
        return SetStarsNew(stars, &__ctx);
    }
    
private:

    ::lbto::result SetStarsNew(const ::lbto::SeqNewPos&, const ::Ice::Context*);
    
public:
    bool SetStarsNew_async(const ::lbto::AMI_IIFServer_SetStarsNewPtr&, const ::lbto::SeqNewPos&);
    bool SetStarsNew_async(const ::lbto::AMI_IIFServer_SetStarsNewPtr&, const ::lbto::SeqNewPos&, const ::Ice::Context&);

    ::lbto::result ClearStars()
    {
        return ClearStars(0);
    }
    ::lbto::result ClearStars(const ::Ice::Context& __ctx)
    {
        return ClearStars(&__ctx);
    }
    
private:

    ::lbto::result ClearStars(const ::Ice::Context*);
    
public:
    bool ClearStars_async(const ::lbto::AMI_IIFServer_ClearStarsPtr&);
    bool ClearStars_async(const ::lbto::AMI_IIFServer_ClearStarsPtr&, const ::Ice::Context&);

    ::lbto::result SetHotspot(::Ice::Double coord1, ::Ice::Double coord2)
    {
        return SetHotspot(coord1, coord2, 0);
    }
    ::lbto::result SetHotspot(::Ice::Double coord1, ::Ice::Double coord2, const ::Ice::Context& __ctx)
    {
        return SetHotspot(coord1, coord2, &__ctx);
    }
    
private:

    ::lbto::result SetHotspot(::Ice::Double, ::Ice::Double, const ::Ice::Context*);
    
public:
    bool SetHotspot_async(const ::lbto::AMI_IIFServer_SetHotspotPtr&, ::Ice::Double, ::Ice::Double);
    bool SetHotspot_async(const ::lbto::AMI_IIFServer_SetHotspotPtr&, ::Ice::Double, ::Ice::Double, const ::Ice::Context&);

    ::lbto::result ClearHotspot()
    {
        return ClearHotspot(0);
    }
    ::lbto::result ClearHotspot(const ::Ice::Context& __ctx)
    {
        return ClearHotspot(&__ctx);
    }
    
private:

    ::lbto::result ClearHotspot(const ::Ice::Context*);
    
public:
    bool ClearHotspot_async(const ::lbto::AMI_IIFServer_ClearHotspotPtr&);
    bool ClearHotspot_async(const ::lbto::AMI_IIFServer_ClearHotspotPtr&, const ::Ice::Context&);

    ::lbto::result SetOffset(::Ice::Double coord1, ::Ice::Double coord2, const ::std::string& system)
    {
        return SetOffset(coord1, coord2, system, 0);
    }
    ::lbto::result SetOffset(::Ice::Double coord1, ::Ice::Double coord2, const ::std::string& system, const ::Ice::Context& __ctx)
    {
        return SetOffset(coord1, coord2, system, &__ctx);
    }
    
private:

    ::lbto::result SetOffset(::Ice::Double, ::Ice::Double, const ::std::string&, const ::Ice::Context*);
    
public:
    bool SetOffset_async(const ::lbto::AMI_IIFServer_SetOffsetPtr&, ::Ice::Double, ::Ice::Double, const ::std::string&);
    bool SetOffset_async(const ::lbto::AMI_IIFServer_SetOffsetPtr&, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::Ice::Context&);

    ::lbto::result ClearOffset()
    {
        return ClearOffset(0);
    }
    ::lbto::result ClearOffset(const ::Ice::Context& __ctx)
    {
        return ClearOffset(&__ctx);
    }
    
private:

    ::lbto::result ClearOffset(const ::Ice::Context*);
    
public:
    bool ClearOffset_async(const ::lbto::AMI_IIFServer_ClearOffsetPtr&);
    bool ClearOffset_async(const ::lbto::AMI_IIFServer_ClearOffsetPtr&, const ::Ice::Context&);

    ::lbto::result SetNonSidereal(const ::lbto::nonsidereal& target, bool override)
    {
        return SetNonSidereal(target, override, 0);
    }
    ::lbto::result SetNonSidereal(const ::lbto::nonsidereal& target, bool override, const ::Ice::Context& __ctx)
    {
        return SetNonSidereal(target, override, &__ctx);
    }
    
private:

    ::lbto::result SetNonSidereal(const ::lbto::nonsidereal&, bool, const ::Ice::Context*);
    
public:
    bool SetNonSidereal_async(const ::lbto::AMI_IIFServer_SetNonSiderealPtr&, const ::lbto::nonsidereal&, bool);
    bool SetNonSidereal_async(const ::lbto::AMI_IIFServer_SetNonSiderealPtr&, const ::lbto::nonsidereal&, bool, const ::Ice::Context&);

    ::lbto::result ClearNonSidereal(bool override)
    {
        return ClearNonSidereal(override, 0);
    }
    ::lbto::result ClearNonSidereal(bool override, const ::Ice::Context& __ctx)
    {
        return ClearNonSidereal(override, &__ctx);
    }
    
private:

    ::lbto::result ClearNonSidereal(bool, const ::Ice::Context*);
    
public:
    bool ClearNonSidereal_async(const ::lbto::AMI_IIFServer_ClearNonSiderealPtr&, bool);
    bool ClearNonSidereal_async(const ::lbto::AMI_IIFServer_ClearNonSiderealPtr&, bool, const ::Ice::Context&);

    ::lbto::result UpdateNonSiderealTarget(::Ice::Double RArate, ::Ice::Double DECrate)
    {
        return UpdateNonSiderealTarget(RArate, DECrate, 0);
    }
    ::lbto::result UpdateNonSiderealTarget(::Ice::Double RArate, ::Ice::Double DECrate, const ::Ice::Context& __ctx)
    {
        return UpdateNonSiderealTarget(RArate, DECrate, &__ctx);
    }
    
private:

    ::lbto::result UpdateNonSiderealTarget(::Ice::Double, ::Ice::Double, const ::Ice::Context*);
    
public:
    bool UpdateNonSiderealTarget_async(const ::lbto::AMI_IIFServer_UpdateNonSiderealTargetPtr&, ::Ice::Double, ::Ice::Double);
    bool UpdateNonSiderealTarget_async(const ::lbto::AMI_IIFServer_UpdateNonSiderealTargetPtr&, ::Ice::Double, ::Ice::Double, const ::Ice::Context&);

    ::lbto::result PresetTelescope(::Ice::Double rotangle, const ::std::string& rotmode, const ::std::string& mode, const ::std::string& side)
    {
        return PresetTelescope(rotangle, rotmode, mode, side, 0);
    }
    ::lbto::result PresetTelescope(::Ice::Double rotangle, const ::std::string& rotmode, const ::std::string& mode, const ::std::string& side, const ::Ice::Context& __ctx)
    {
        return PresetTelescope(rotangle, rotmode, mode, side, &__ctx);
    }
    
private:

    ::lbto::result PresetTelescope(::Ice::Double, const ::std::string&, const ::std::string&, const ::std::string&, const ::Ice::Context*);
    
public:
    bool PresetTelescope_async(const ::lbto::AMI_IIFServer_PresetTelescopePtr&, ::Ice::Double, const ::std::string&, const ::std::string&, const ::std::string&);
    bool PresetTelescope_async(const ::lbto::AMI_IIFServer_PresetTelescopePtr&, ::Ice::Double, const ::std::string&, const ::std::string&, const ::std::string&, const ::Ice::Context&);

    ::lbto::result BinocularControl(const ::std::string& flag)
    {
        return BinocularControl(flag, 0);
    }
    ::lbto::result BinocularControl(const ::std::string& flag, const ::Ice::Context& __ctx)
    {
        return BinocularControl(flag, &__ctx);
    }
    
private:

    ::lbto::result BinocularControl(const ::std::string&, const ::Ice::Context*);
    
public:
    bool BinocularControl_async(const ::lbto::AMI_IIFServer_BinocularControlPtr&, const ::std::string&);
    bool BinocularControl_async(const ::lbto::AMI_IIFServer_BinocularControlPtr&, const ::std::string&, const ::Ice::Context&);

    ::lbto::result GetParameter(const ::lbto::SeqDD& list)
    {
        return GetParameter(list, 0);
    }
    ::lbto::result GetParameter(const ::lbto::SeqDD& list, const ::Ice::Context& __ctx)
    {
        return GetParameter(list, &__ctx);
    }
    
private:

    ::lbto::result GetParameter(const ::lbto::SeqDD&, const ::Ice::Context*);
    
public:
    bool GetParameter_async(const ::lbto::AMI_IIFServer_GetParameterPtr&, const ::lbto::SeqDD&);
    bool GetParameter_async(const ::lbto::AMI_IIFServer_GetParameterPtr&, const ::lbto::SeqDD&, const ::Ice::Context&);

    ::lbto::result SetParameter(const ::lbto::SeqDD& list)
    {
        return SetParameter(list, 0);
    }
    ::lbto::result SetParameter(const ::lbto::SeqDD& list, const ::Ice::Context& __ctx)
    {
        return SetParameter(list, &__ctx);
    }
    
private:

    ::lbto::result SetParameter(const ::lbto::SeqDD&, const ::Ice::Context*);
    
public:
    bool SetParameter_async(const ::lbto::AMI_IIFServer_SetParameterPtr&, const ::lbto::SeqDD&);
    bool SetParameter_async(const ::lbto::AMI_IIFServer_SetParameterPtr&, const ::lbto::SeqDD&, const ::Ice::Context&);

    ::lbto::result RotateCommon(::Ice::Double z, ::Ice::Double angle, ::Ice::Double direction, const ::std::string& side)
    {
        return RotateCommon(z, angle, direction, side, 0);
    }
    ::lbto::result RotateCommon(::Ice::Double z, ::Ice::Double angle, ::Ice::Double direction, const ::std::string& side, const ::Ice::Context& __ctx)
    {
        return RotateCommon(z, angle, direction, side, &__ctx);
    }
    
private:

    ::lbto::result RotateCommon(::Ice::Double, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::Ice::Context*);
    
public:
    bool RotateCommon_async(const ::lbto::AMI_IIFServer_RotateCommonPtr&, ::Ice::Double, ::Ice::Double, ::Ice::Double, const ::std::string&);
    bool RotateCommon_async(const ::lbto::AMI_IIFServer_RotateCommonPtr&, ::Ice::Double, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::Ice::Context&);

    ::lbto::result RotateZ(::Ice::Double angle, const ::std::string& movetype, const ::std::string& side)
    {
        return RotateZ(angle, movetype, side, 0);
    }
    ::lbto::result RotateZ(::Ice::Double angle, const ::std::string& movetype, const ::std::string& side, const ::Ice::Context& __ctx)
    {
        return RotateZ(angle, movetype, side, &__ctx);
    }
    
private:

    ::lbto::result RotateZ(::Ice::Double, const ::std::string&, const ::std::string&, const ::Ice::Context*);
    
public:
    bool RotateZ_async(const ::lbto::AMI_IIFServer_RotateZPtr&, ::Ice::Double, const ::std::string&, const ::std::string&);
    bool RotateZ_async(const ::lbto::AMI_IIFServer_RotateZPtr&, ::Ice::Double, const ::std::string&, const ::std::string&, const ::Ice::Context&);

    ::lbto::result RotatePrimary(::Ice::Double distance, ::Ice::Double angle, ::Ice::Double direction, const ::std::string& side)
    {
        return RotatePrimary(distance, angle, direction, side, 0);
    }
    ::lbto::result RotatePrimary(::Ice::Double distance, ::Ice::Double angle, ::Ice::Double direction, const ::std::string& side, const ::Ice::Context& __ctx)
    {
        return RotatePrimary(distance, angle, direction, side, &__ctx);
    }
    
private:

    ::lbto::result RotatePrimary(::Ice::Double, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::Ice::Context*);
    
public:
    bool RotatePrimary_async(const ::lbto::AMI_IIFServer_RotatePrimaryPtr&, ::Ice::Double, ::Ice::Double, ::Ice::Double, const ::std::string&);
    bool RotatePrimary_async(const ::lbto::AMI_IIFServer_RotatePrimaryPtr&, ::Ice::Double, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::Ice::Context&);

    ::lbto::result SendWavefront(const ::lbto::SeqWF& wfs, const ::std::string& OPE, const ::std::string& side)
    {
        return SendWavefront(wfs, OPE, side, 0);
    }
    ::lbto::result SendWavefront(const ::lbto::SeqWF& wfs, const ::std::string& OPE, const ::std::string& side, const ::Ice::Context& __ctx)
    {
        return SendWavefront(wfs, OPE, side, &__ctx);
    }
    
private:

    ::lbto::result SendWavefront(const ::lbto::SeqWF&, const ::std::string&, const ::std::string&, const ::Ice::Context*);
    
public:
    bool SendWavefront_async(const ::lbto::AMI_IIFServer_SendWavefrontPtr&, const ::lbto::SeqWF&, const ::std::string&, const ::std::string&);
    bool SendWavefront_async(const ::lbto::AMI_IIFServer_SendWavefrontPtr&, const ::lbto::SeqWF&, const ::std::string&, const ::std::string&, const ::Ice::Context&);

    ::lbto::result UpdateTargetWavelength(::Ice::Float wavelength, const ::std::string& side)
    {
        return UpdateTargetWavelength(wavelength, side, 0);
    }
    ::lbto::result UpdateTargetWavelength(::Ice::Float wavelength, const ::std::string& side, const ::Ice::Context& __ctx)
    {
        return UpdateTargetWavelength(wavelength, side, &__ctx);
    }
    
private:

    ::lbto::result UpdateTargetWavelength(::Ice::Float, const ::std::string&, const ::Ice::Context*);
    
public:
    bool UpdateTargetWavelength_async(const ::lbto::AMI_IIFServer_UpdateTargetWavelengthPtr&, ::Ice::Float, const ::std::string&);
    bool UpdateTargetWavelength_async(const ::lbto::AMI_IIFServer_UpdateTargetWavelengthPtr&, ::Ice::Float, const ::std::string&, const ::Ice::Context&);

    ::lbto::result LogEvent(const ::std::string& event, const ::std::string& description)
    {
        return LogEvent(event, description, 0);
    }
    ::lbto::result LogEvent(const ::std::string& event, const ::std::string& description, const ::Ice::Context& __ctx)
    {
        return LogEvent(event, description, &__ctx);
    }
    
private:

    ::lbto::result LogEvent(const ::std::string&, const ::std::string&, const ::Ice::Context*);
    
public:
    bool LogEvent_async(const ::lbto::AMI_IIFServer_LogEventPtr&, const ::std::string&, const ::std::string&);
    bool LogEvent_async(const ::lbto::AMI_IIFServer_LogEventPtr&, const ::std::string&, const ::std::string&, const ::Ice::Context&);

    ::lbto::result GetRotatorTrajectory(::Ice::Double seconds, ::Ice::Double interval, ::Ice::Double start, const ::std::string& side)
    {
        return GetRotatorTrajectory(seconds, interval, start, side, 0);
    }
    ::lbto::result GetRotatorTrajectory(::Ice::Double seconds, ::Ice::Double interval, ::Ice::Double start, const ::std::string& side, const ::Ice::Context& __ctx)
    {
        return GetRotatorTrajectory(seconds, interval, start, side, &__ctx);
    }
    
private:

    ::lbto::result GetRotatorTrajectory(::Ice::Double, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::Ice::Context*);
    
public:
    bool GetRotatorTrajectory_async(const ::lbto::AMI_IIFServer_GetRotatorTrajectoryPtr&, ::Ice::Double, ::Ice::Double, ::Ice::Double, const ::std::string&);
    bool GetRotatorTrajectory_async(const ::lbto::AMI_IIFServer_GetRotatorTrajectoryPtr&, ::Ice::Double, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::Ice::Context&);

    ::lbto::result GetRotatorPolynomials(::Ice::Double start, ::Ice::Int count, ::Ice::Double interval, ::Ice::Int order, const ::std::string& side)
    {
        return GetRotatorPolynomials(start, count, interval, order, side, 0);
    }
    ::lbto::result GetRotatorPolynomials(::Ice::Double start, ::Ice::Int count, ::Ice::Double interval, ::Ice::Int order, const ::std::string& side, const ::Ice::Context& __ctx)
    {
        return GetRotatorPolynomials(start, count, interval, order, side, &__ctx);
    }
    
private:

    ::lbto::result GetRotatorPolynomials(::Ice::Double, ::Ice::Int, ::Ice::Double, ::Ice::Int, const ::std::string&, const ::Ice::Context*);
    
public:
    bool GetRotatorPolynomials_async(const ::lbto::AMI_IIFServer_GetRotatorPolynomialsPtr&, ::Ice::Double, ::Ice::Int, ::Ice::Double, ::Ice::Int, const ::std::string&);
    bool GetRotatorPolynomials_async(const ::lbto::AMI_IIFServer_GetRotatorPolynomialsPtr&, ::Ice::Double, ::Ice::Int, ::Ice::Double, ::Ice::Int, const ::std::string&, const ::Ice::Context&);

    ::lbto::result Standby(::Ice::Int level, const ::std::string& side)
    {
        return Standby(level, side, 0);
    }
    ::lbto::result Standby(::Ice::Int level, const ::std::string& side, const ::Ice::Context& __ctx)
    {
        return Standby(level, side, &__ctx);
    }
    
private:

    ::lbto::result Standby(::Ice::Int, const ::std::string&, const ::Ice::Context*);
    
public:
    bool Standby_async(const ::lbto::AMI_IIFServer_StandbyPtr&, ::Ice::Int, const ::std::string&);
    bool Standby_async(const ::lbto::AMI_IIFServer_StandbyPtr&, ::Ice::Int, const ::std::string&, const ::Ice::Context&);

    ::lbto::result MaximizeWrapTime(bool azflag, bool rotflag, const ::std::string& side)
    {
        return MaximizeWrapTime(azflag, rotflag, side, 0);
    }
    ::lbto::result MaximizeWrapTime(bool azflag, bool rotflag, const ::std::string& side, const ::Ice::Context& __ctx)
    {
        return MaximizeWrapTime(azflag, rotflag, side, &__ctx);
    }
    
private:

    ::lbto::result MaximizeWrapTime(bool, bool, const ::std::string&, const ::Ice::Context*);
    
public:
    bool MaximizeWrapTime_async(const ::lbto::AMI_IIFServer_MaximizeWrapTimePtr&, bool, bool, const ::std::string&);
    bool MaximizeWrapTime_async(const ::lbto::AMI_IIFServer_MaximizeWrapTimePtr&, bool, bool, const ::std::string&, const ::Ice::Context&);

    ::lbto::result PauseGuiding(const ::std::string& side)
    {
        return PauseGuiding(side, 0);
    }
    ::lbto::result PauseGuiding(const ::std::string& side, const ::Ice::Context& __ctx)
    {
        return PauseGuiding(side, &__ctx);
    }
    
private:

    ::lbto::result PauseGuiding(const ::std::string&, const ::Ice::Context*);
    
public:
    bool PauseGuiding_async(const ::lbto::AMI_IIFServer_PauseGuidingPtr&, const ::std::string&);
    bool PauseGuiding_async(const ::lbto::AMI_IIFServer_PauseGuidingPtr&, const ::std::string&, const ::Ice::Context&);

    ::lbto::result ResumeGuiding(const ::std::string& side)
    {
        return ResumeGuiding(side, 0);
    }
    ::lbto::result ResumeGuiding(const ::std::string& side, const ::Ice::Context& __ctx)
    {
        return ResumeGuiding(side, &__ctx);
    }
    
private:

    ::lbto::result ResumeGuiding(const ::std::string&, const ::Ice::Context*);
    
public:
    bool ResumeGuiding_async(const ::lbto::AMI_IIFServer_ResumeGuidingPtr&, const ::std::string&);
    bool ResumeGuiding_async(const ::lbto::AMI_IIFServer_ResumeGuidingPtr&, const ::std::string&, const ::Ice::Context&);

    ::lbto::result SetGuidingHotspot(::Ice::Double coord1, ::Ice::Double coord2, const ::std::string& movetype, const ::std::string& side)
    {
        return SetGuidingHotspot(coord1, coord2, movetype, side, 0);
    }
    ::lbto::result SetGuidingHotspot(::Ice::Double coord1, ::Ice::Double coord2, const ::std::string& movetype, const ::std::string& side, const ::Ice::Context& __ctx)
    {
        return SetGuidingHotspot(coord1, coord2, movetype, side, &__ctx);
    }
    
private:

    ::lbto::result SetGuidingHotspot(::Ice::Double, ::Ice::Double, const ::std::string&, const ::std::string&, const ::Ice::Context*);
    
public:
    bool SetGuidingHotspot_async(const ::lbto::AMI_IIFServer_SetGuidingHotspotPtr&, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::std::string&);
    bool SetGuidingHotspot_async(const ::lbto::AMI_IIFServer_SetGuidingHotspotPtr&, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::std::string&, const ::Ice::Context&);

    ::lbto::result RotReady(bool enable, const ::std::string& side)
    {
        return RotReady(enable, side, 0);
    }
    ::lbto::result RotReady(bool enable, const ::std::string& side, const ::Ice::Context& __ctx)
    {
        return RotReady(enable, side, &__ctx);
    }
    
private:

    ::lbto::result RotReady(bool, const ::std::string&, const ::Ice::Context*);
    
public:
    bool RotReady_async(const ::lbto::AMI_IIFServer_RotReadyPtr&, bool, const ::std::string&);
    bool RotReady_async(const ::lbto::AMI_IIFServer_RotReadyPtr&, bool, const ::std::string&, const ::Ice::Context&);

    ::lbto::result RotHold(const ::std::string& side)
    {
        return RotHold(side, 0);
    }
    ::lbto::result RotHold(const ::std::string& side, const ::Ice::Context& __ctx)
    {
        return RotHold(side, &__ctx);
    }
    
private:

    ::lbto::result RotHold(const ::std::string&, const ::Ice::Context*);
    
public:
    bool RotHold_async(const ::lbto::AMI_IIFServer_RotHoldPtr&, const ::std::string&);
    bool RotHold_async(const ::lbto::AMI_IIFServer_RotHoldPtr&, const ::std::string&, const ::Ice::Context&);

    ::lbto::result RotTrack(const ::std::string& side)
    {
        return RotTrack(side, 0);
    }
    ::lbto::result RotTrack(const ::std::string& side, const ::Ice::Context& __ctx)
    {
        return RotTrack(side, &__ctx);
    }
    
private:

    ::lbto::result RotTrack(const ::std::string&, const ::Ice::Context*);
    
public:
    bool RotTrack_async(const ::lbto::AMI_IIFServer_RotTrackPtr&, const ::std::string&);
    bool RotTrack_async(const ::lbto::AMI_IIFServer_RotTrackPtr&, const ::std::string&, const ::Ice::Context&);

    ::lbto::result RotServicePosition(::Ice::Double angle, const ::std::string& side)
    {
        return RotServicePosition(angle, side, 0);
    }
    ::lbto::result RotServicePosition(::Ice::Double angle, const ::std::string& side, const ::Ice::Context& __ctx)
    {
        return RotServicePosition(angle, side, &__ctx);
    }
    
private:

    ::lbto::result RotServicePosition(::Ice::Double, const ::std::string&, const ::Ice::Context*);
    
public:
    bool RotServicePosition_async(const ::lbto::AMI_IIFServer_RotServicePositionPtr&, ::Ice::Double, const ::std::string&);
    bool RotServicePosition_async(const ::lbto::AMI_IIFServer_RotServicePositionPtr&, ::Ice::Double, const ::std::string&, const ::Ice::Context&);

    ::lbto::result PauseAO(const ::std::string& side)
    {
        return PauseAO(side, 0);
    }
    ::lbto::result PauseAO(const ::std::string& side, const ::Ice::Context& __ctx)
    {
        return PauseAO(side, &__ctx);
    }
    
private:

    ::lbto::result PauseAO(const ::std::string&, const ::Ice::Context*);
    
public:
    bool PauseAO_async(const ::lbto::AMI_IIFServer_PauseAOPtr&, const ::std::string&);
    bool PauseAO_async(const ::lbto::AMI_IIFServer_PauseAOPtr&, const ::std::string&, const ::Ice::Context&);

    ::lbto::result ResumeAO(const ::std::string& side)
    {
        return ResumeAO(side, 0);
    }
    ::lbto::result ResumeAO(const ::std::string& side, const ::Ice::Context& __ctx)
    {
        return ResumeAO(side, &__ctx);
    }
    
private:

    ::lbto::result ResumeAO(const ::std::string&, const ::Ice::Context*);
    
public:
    bool ResumeAO_async(const ::lbto::AMI_IIFServer_ResumeAOPtr&, const ::std::string&);
    bool ResumeAO_async(const ::lbto::AMI_IIFServer_ResumeAOPtr&, const ::std::string&, const ::Ice::Context&);

    ::lbto::result StartAO(const ::std::string& side)
    {
        return StartAO(side, 0);
    }
    ::lbto::result StartAO(const ::std::string& side, const ::Ice::Context& __ctx)
    {
        return StartAO(side, &__ctx);
    }
    
private:

    ::lbto::result StartAO(const ::std::string&, const ::Ice::Context*);
    
public:
    bool StartAO_async(const ::lbto::AMI_IIFServer_StartAOPtr&, const ::std::string&);
    bool StartAO_async(const ::lbto::AMI_IIFServer_StartAOPtr&, const ::std::string&, const ::Ice::Context&);

    ::lbto::result StopAO(const ::std::string& reason, const ::std::string& side)
    {
        return StopAO(reason, side, 0);
    }
    ::lbto::result StopAO(const ::std::string& reason, const ::std::string& side, const ::Ice::Context& __ctx)
    {
        return StopAO(reason, side, &__ctx);
    }
    
private:

    ::lbto::result StopAO(const ::std::string&, const ::std::string&, const ::Ice::Context*);
    
public:
    bool StopAO_async(const ::lbto::AMI_IIFServer_StopAOPtr&, const ::std::string&, const ::std::string&);
    bool StopAO_async(const ::lbto::AMI_IIFServer_StopAOPtr&, const ::std::string&, const ::std::string&, const ::Ice::Context&);

    ::lbto::result AcquireRefAO(bool repointFlag, const ::std::string& side)
    {
        return AcquireRefAO(repointFlag, side, 0);
    }
    ::lbto::result AcquireRefAO(bool repointFlag, const ::std::string& side, const ::Ice::Context& __ctx)
    {
        return AcquireRefAO(repointFlag, side, &__ctx);
    }
    
private:

    ::lbto::result AcquireRefAO(bool, const ::std::string&, const ::Ice::Context*);
    
public:
    bool AcquireRefAO_async(const ::lbto::AMI_IIFServer_AcquireRefAOPtr&, bool, const ::std::string&);
    bool AcquireRefAO_async(const ::lbto::AMI_IIFServer_AcquireRefAOPtr&, bool, const ::std::string&, const ::Ice::Context&);

    ::lbto::result CheckRefAO(const ::std::string& side)
    {
        return CheckRefAO(side, 0);
    }
    ::lbto::result CheckRefAO(const ::std::string& side, const ::Ice::Context& __ctx)
    {
        return CheckRefAO(side, &__ctx);
    }
    
private:

    ::lbto::result CheckRefAO(const ::std::string&, const ::Ice::Context*);
    
public:
    bool CheckRefAO_async(const ::lbto::AMI_IIFServer_CheckRefAOPtr&, const ::std::string&);
    bool CheckRefAO_async(const ::lbto::AMI_IIFServer_CheckRefAOPtr&, const ::std::string&, const ::Ice::Context&);

    ::lbto::result RefineAO(const ::std::string& method, const ::std::string& side)
    {
        return RefineAO(method, side, 0);
    }
    ::lbto::result RefineAO(const ::std::string& method, const ::std::string& side, const ::Ice::Context& __ctx)
    {
        return RefineAO(method, side, &__ctx);
    }
    
private:

    ::lbto::result RefineAO(const ::std::string&, const ::std::string&, const ::Ice::Context*);
    
public:
    bool RefineAO_async(const ::lbto::AMI_IIFServer_RefineAOPtr&, const ::std::string&, const ::std::string&);
    bool RefineAO_async(const ::lbto::AMI_IIFServer_RefineAOPtr&, const ::std::string&, const ::std::string&, const ::Ice::Context&);

    ::lbto::result RunAO(const ::std::string& type, const ::std::string& side)
    {
        return RunAO(type, side, 0);
    }
    ::lbto::result RunAO(const ::std::string& type, const ::std::string& side, const ::Ice::Context& __ctx)
    {
        return RunAO(type, side, &__ctx);
    }
    
private:

    ::lbto::result RunAO(const ::std::string&, const ::std::string&, const ::Ice::Context*);
    
public:
    bool RunAO_async(const ::lbto::AMI_IIFServer_RunAOPtr&, const ::std::string&, const ::std::string&);
    bool RunAO_async(const ::lbto::AMI_IIFServer_RunAOPtr&, const ::std::string&, const ::std::string&, const ::Ice::Context&);

    ::lbto::result PresetFlatAO(const ::std::string& flat, const ::std::string& side)
    {
        return PresetFlatAO(flat, side, 0);
    }
    ::lbto::result PresetFlatAO(const ::std::string& flat, const ::std::string& side, const ::Ice::Context& __ctx)
    {
        return PresetFlatAO(flat, side, &__ctx);
    }
    
private:

    ::lbto::result PresetFlatAO(const ::std::string&, const ::std::string&, const ::Ice::Context*);
    
public:
    bool PresetFlatAO_async(const ::lbto::AMI_IIFServer_PresetFlatAOPtr&, const ::std::string&, const ::std::string&);
    bool PresetFlatAO_async(const ::lbto::AMI_IIFServer_PresetFlatAOPtr&, const ::std::string&, const ::std::string&, const ::Ice::Context&);

    ::lbto::result CorrectModesAO(const ::lbto::SeqModes& modes, const ::std::string& side)
    {
        return CorrectModesAO(modes, side, 0);
    }
    ::lbto::result CorrectModesAO(const ::lbto::SeqModes& modes, const ::std::string& side, const ::Ice::Context& __ctx)
    {
        return CorrectModesAO(modes, side, &__ctx);
    }
    
private:

    ::lbto::result CorrectModesAO(const ::lbto::SeqModes&, const ::std::string&, const ::Ice::Context*);
    
public:
    bool CorrectModesAO_async(const ::lbto::AMI_IIFServer_CorrectModesAOPtr&, const ::lbto::SeqModes&, const ::std::string&);
    bool CorrectModesAO_async(const ::lbto::AMI_IIFServer_CorrectModesAOPtr&, const ::lbto::SeqModes&, const ::std::string&, const ::Ice::Context&);

    ::lbto::result ModifyAO(::Ice::Int NModes, ::Ice::Double Freq, ::Ice::Int Nbins, ::Ice::Double TTMod, const ::std::string& F1spec, const ::std::string& F2spec, const ::std::string& side)
    {
        return ModifyAO(NModes, Freq, Nbins, TTMod, F1spec, F2spec, side, 0);
    }
    ::lbto::result ModifyAO(::Ice::Int NModes, ::Ice::Double Freq, ::Ice::Int Nbins, ::Ice::Double TTMod, const ::std::string& F1spec, const ::std::string& F2spec, const ::std::string& side, const ::Ice::Context& __ctx)
    {
        return ModifyAO(NModes, Freq, Nbins, TTMod, F1spec, F2spec, side, &__ctx);
    }
    
private:

    ::lbto::result ModifyAO(::Ice::Int, ::Ice::Double, ::Ice::Int, ::Ice::Double, const ::std::string&, const ::std::string&, const ::std::string&, const ::Ice::Context*);
    
public:
    bool ModifyAO_async(const ::lbto::AMI_IIFServer_ModifyAOPtr&, ::Ice::Int, ::Ice::Double, ::Ice::Int, ::Ice::Double, const ::std::string&, const ::std::string&, const ::std::string&);
    bool ModifyAO_async(const ::lbto::AMI_IIFServer_ModifyAOPtr&, ::Ice::Int, ::Ice::Double, ::Ice::Int, ::Ice::Double, const ::std::string&, const ::std::string&, const ::std::string&, const ::Ice::Context&);

    ::lbto::result PresetAO(const ::std::string& AOMode, const ::std::string& side)
    {
        return PresetAO(AOMode, side, 0);
    }
    ::lbto::result PresetAO(const ::std::string& AOMode, const ::std::string& side, const ::Ice::Context& __ctx)
    {
        return PresetAO(AOMode, side, &__ctx);
    }
    
private:

    ::lbto::result PresetAO(const ::std::string&, const ::std::string&, const ::Ice::Context*);
    
public:
    bool PresetAO_async(const ::lbto::AMI_IIFServer_PresetAOPtr&, const ::std::string&, const ::std::string&);
    bool PresetAO_async(const ::lbto::AMI_IIFServer_PresetAOPtr&, const ::std::string&, const ::std::string&, const ::Ice::Context&);

    ::lbto::result OffsetXYAO(::Ice::Double DeltaX, ::Ice::Double DeltaY, const ::std::string& side)
    {
        return OffsetXYAO(DeltaX, DeltaY, side, 0);
    }
    ::lbto::result OffsetXYAO(::Ice::Double DeltaX, ::Ice::Double DeltaY, const ::std::string& side, const ::Ice::Context& __ctx)
    {
        return OffsetXYAO(DeltaX, DeltaY, side, &__ctx);
    }
    
private:

    ::lbto::result OffsetXYAO(::Ice::Double, ::Ice::Double, const ::std::string&, const ::Ice::Context*);
    
public:
    bool OffsetXYAO_async(const ::lbto::AMI_IIFServer_OffsetXYAOPtr&, ::Ice::Double, ::Ice::Double, const ::std::string&);
    bool OffsetXYAO_async(const ::lbto::AMI_IIFServer_OffsetXYAOPtr&, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::Ice::Context&);

    ::lbto::result OffsetZAO(::Ice::Double DeltaZ, const ::std::string& side)
    {
        return OffsetZAO(DeltaZ, side, 0);
    }
    ::lbto::result OffsetZAO(::Ice::Double DeltaZ, const ::std::string& side, const ::Ice::Context& __ctx)
    {
        return OffsetZAO(DeltaZ, side, &__ctx);
    }
    
private:

    ::lbto::result OffsetZAO(::Ice::Double, const ::std::string&, const ::Ice::Context*);
    
public:
    bool OffsetZAO_async(const ::lbto::AMI_IIFServer_OffsetZAOPtr&, ::Ice::Double, const ::std::string&);
    bool OffsetZAO_async(const ::lbto::AMI_IIFServer_OffsetZAOPtr&, ::Ice::Double, const ::std::string&, const ::Ice::Context&);

    ::lbto::result SetReference(const ::lbto::SeqPos& stars)
    {
        return SetReference(stars, 0);
    }
    ::lbto::result SetReference(const ::lbto::SeqPos& stars, const ::Ice::Context& __ctx)
    {
        return SetReference(stars, &__ctx);
    }
    
private:

    ::lbto::result SetReference(const ::lbto::SeqPos&, const ::Ice::Context*);
    
public:
    bool SetReference_async(const ::lbto::AMI_IIFServer_SetReferencePtr&, const ::lbto::SeqPos&);
    bool SetReference_async(const ::lbto::AMI_IIFServer_SetReferencePtr&, const ::lbto::SeqPos&, const ::Ice::Context&);

    ::lbto::result SetReferenceNew(const ::lbto::SeqNewPos& stars)
    {
        return SetReferenceNew(stars, 0);
    }
    ::lbto::result SetReferenceNew(const ::lbto::SeqNewPos& stars, const ::Ice::Context& __ctx)
    {
        return SetReferenceNew(stars, &__ctx);
    }
    
private:

    ::lbto::result SetReferenceNew(const ::lbto::SeqNewPos&, const ::Ice::Context*);
    
public:
    bool SetReferenceNew_async(const ::lbto::AMI_IIFServer_SetReferenceNewPtr&, const ::lbto::SeqNewPos&);
    bool SetReferenceNew_async(const ::lbto::AMI_IIFServer_SetReferenceNewPtr&, const ::lbto::SeqNewPos&, const ::Ice::Context&);

    ::lbto::result ClearReference()
    {
        return ClearReference(0);
    }
    ::lbto::result ClearReference(const ::Ice::Context& __ctx)
    {
        return ClearReference(&__ctx);
    }
    
private:

    ::lbto::result ClearReference(const ::Ice::Context*);
    
public:
    bool ClearReference_async(const ::lbto::AMI_IIFServer_ClearReferencePtr&);
    bool ClearReference_async(const ::lbto::AMI_IIFServer_ClearReferencePtr&, const ::Ice::Context&);

    ::lbto::result GetKFPCoordinates(const ::lbto::SeqPos& stars, const ::std::string& side)
    {
        return GetKFPCoordinates(stars, side, 0);
    }
    ::lbto::result GetKFPCoordinates(const ::lbto::SeqPos& stars, const ::std::string& side, const ::Ice::Context& __ctx)
    {
        return GetKFPCoordinates(stars, side, &__ctx);
    }
    
private:

    ::lbto::result GetKFPCoordinates(const ::lbto::SeqPos&, const ::std::string&, const ::Ice::Context*);
    
public:
    bool GetKFPCoordinates_async(const ::lbto::AMI_IIFServer_GetKFPCoordinatesPtr&, const ::lbto::SeqPos&, const ::std::string&);
    bool GetKFPCoordinates_async(const ::lbto::AMI_IIFServer_GetKFPCoordinatesPtr&, const ::lbto::SeqPos&, const ::std::string&, const ::Ice::Context&);

    ::lbto::result GetKFPCoordinatesNew(const ::lbto::SeqNewPos& stars, const ::std::string& side)
    {
        return GetKFPCoordinatesNew(stars, side, 0);
    }
    ::lbto::result GetKFPCoordinatesNew(const ::lbto::SeqNewPos& stars, const ::std::string& side, const ::Ice::Context& __ctx)
    {
        return GetKFPCoordinatesNew(stars, side, &__ctx);
    }
    
private:

    ::lbto::result GetKFPCoordinatesNew(const ::lbto::SeqNewPos&, const ::std::string&, const ::Ice::Context*);
    
public:
    bool GetKFPCoordinatesNew_async(const ::lbto::AMI_IIFServer_GetKFPCoordinatesNewPtr&, const ::lbto::SeqNewPos&, const ::std::string&);
    bool GetKFPCoordinatesNew_async(const ::lbto::AMI_IIFServer_GetKFPCoordinatesNewPtr&, const ::lbto::SeqNewPos&, const ::std::string&, const ::Ice::Context&);

    ::lbto::result SetAGWFilter(::Ice::Int filterNumber, const ::std::string& side)
    {
        return SetAGWFilter(filterNumber, side, 0);
    }
    ::lbto::result SetAGWFilter(::Ice::Int filterNumber, const ::std::string& side, const ::Ice::Context& __ctx)
    {
        return SetAGWFilter(filterNumber, side, &__ctx);
    }
    
private:

    ::lbto::result SetAGWFilter(::Ice::Int, const ::std::string&, const ::Ice::Context*);
    
public:
    bool SetAGWFilter_async(const ::lbto::AMI_IIFServer_SetAGWFilterPtr&, ::Ice::Int, const ::std::string&);
    bool SetAGWFilter_async(const ::lbto::AMI_IIFServer_SetAGWFilterPtr&, ::Ice::Int, const ::std::string&, const ::Ice::Context&);

    void remove()
    {
        remove(0);
    }
    void remove(const ::Ice::Context& __ctx)
    {
        remove(&__ctx);
    }
    
private:

    void remove(const ::Ice::Context*);
    
public:
    
    ::IceInternal::ProxyHandle<IIFServer> ice_context(const ::Ice::Context& __context) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<IIFServer*>(_Base::ice_context(__context).get());
    #else
        return dynamic_cast<IIFServer*>(::IceProxy::Ice::Object::ice_context(__context).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<IIFServer> ice_adapterId(const std::string& __id) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<IIFServer*>(_Base::ice_adapterId(__id).get());
    #else
        return dynamic_cast<IIFServer*>(::IceProxy::Ice::Object::ice_adapterId(__id).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<IIFServer> ice_endpoints(const ::Ice::EndpointSeq& __endpoints) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<IIFServer*>(_Base::ice_endpoints(__endpoints).get());
    #else
        return dynamic_cast<IIFServer*>(::IceProxy::Ice::Object::ice_endpoints(__endpoints).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<IIFServer> ice_locatorCacheTimeout(int __timeout) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<IIFServer*>(_Base::ice_locatorCacheTimeout(__timeout).get());
    #else
        return dynamic_cast<IIFServer*>(::IceProxy::Ice::Object::ice_locatorCacheTimeout(__timeout).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<IIFServer> ice_connectionCached(bool __cached) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<IIFServer*>(_Base::ice_connectionCached(__cached).get());
    #else
        return dynamic_cast<IIFServer*>(::IceProxy::Ice::Object::ice_connectionCached(__cached).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<IIFServer> ice_endpointSelection(::Ice::EndpointSelectionType __est) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<IIFServer*>(_Base::ice_endpointSelection(__est).get());
    #else
        return dynamic_cast<IIFServer*>(::IceProxy::Ice::Object::ice_endpointSelection(__est).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<IIFServer> ice_secure(bool __secure) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<IIFServer*>(_Base::ice_secure(__secure).get());
    #else
        return dynamic_cast<IIFServer*>(::IceProxy::Ice::Object::ice_secure(__secure).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<IIFServer> ice_preferSecure(bool __preferSecure) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<IIFServer*>(_Base::ice_preferSecure(__preferSecure).get());
    #else
        return dynamic_cast<IIFServer*>(::IceProxy::Ice::Object::ice_preferSecure(__preferSecure).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<IIFServer> ice_router(const ::Ice::RouterPrx& __router) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<IIFServer*>(_Base::ice_router(__router).get());
    #else
        return dynamic_cast<IIFServer*>(::IceProxy::Ice::Object::ice_router(__router).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<IIFServer> ice_locator(const ::Ice::LocatorPrx& __locator) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<IIFServer*>(_Base::ice_locator(__locator).get());
    #else
        return dynamic_cast<IIFServer*>(::IceProxy::Ice::Object::ice_locator(__locator).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<IIFServer> ice_collocationOptimized(bool __co) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<IIFServer*>(_Base::ice_collocationOptimized(__co).get());
    #else
        return dynamic_cast<IIFServer*>(::IceProxy::Ice::Object::ice_collocationOptimized(__co).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<IIFServer> ice_twoway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<IIFServer*>(_Base::ice_twoway().get());
    #else
        return dynamic_cast<IIFServer*>(::IceProxy::Ice::Object::ice_twoway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<IIFServer> ice_oneway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<IIFServer*>(_Base::ice_oneway().get());
    #else
        return dynamic_cast<IIFServer*>(::IceProxy::Ice::Object::ice_oneway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<IIFServer> ice_batchOneway() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<IIFServer*>(_Base::ice_batchOneway().get());
    #else
        return dynamic_cast<IIFServer*>(::IceProxy::Ice::Object::ice_batchOneway().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<IIFServer> ice_datagram() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<IIFServer*>(_Base::ice_datagram().get());
    #else
        return dynamic_cast<IIFServer*>(::IceProxy::Ice::Object::ice_datagram().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<IIFServer> ice_batchDatagram() const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<IIFServer*>(_Base::ice_batchDatagram().get());
    #else
        return dynamic_cast<IIFServer*>(::IceProxy::Ice::Object::ice_batchDatagram().get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<IIFServer> ice_compress(bool __compress) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<IIFServer*>(_Base::ice_compress(__compress).get());
    #else
        return dynamic_cast<IIFServer*>(::IceProxy::Ice::Object::ice_compress(__compress).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<IIFServer> ice_timeout(int __timeout) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<IIFServer*>(_Base::ice_timeout(__timeout).get());
    #else
        return dynamic_cast<IIFServer*>(::IceProxy::Ice::Object::ice_timeout(__timeout).get());
    #endif
    }
    
    ::IceInternal::ProxyHandle<IIFServer> ice_connectionId(const std::string& __id) const
    {
    #if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
        typedef ::IceProxy::Ice::Object _Base;
        return dynamic_cast<IIFServer*>(_Base::ice_connectionId(__id).get());
    #else
        return dynamic_cast<IIFServer*>(::IceProxy::Ice::Object::ice_connectionId(__id).get());
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

class IIFServer : virtual public ::IceDelegate::Ice::Object
{
public:

    virtual ::lbto::result Authorize(const ::Ice::Context*) = 0;

    virtual ::lbto::result StepFocus(::Ice::Double, const ::std::string&, const ::std::string&, const ::Ice::Context*) = 0;

    virtual ::lbto::result MoveFocus(::Ice::Double, const ::std::string&, const ::std::string&, const ::Ice::Context*) = 0;

    virtual ::lbto::result OffsetPointing(::Ice::Double, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::std::string&, const ::std::string&, const ::std::string&, const ::std::string&, const ::Ice::Context*) = 0;

    virtual ::lbto::result OffsetGuiding(::Ice::Double, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::std::string&, const ::Ice::Context*) = 0;

    virtual ::lbto::result UpdatePointingReference(const ::std::string&, const ::std::string&, const ::Ice::Context*) = 0;

    virtual ::lbto::result TipTilt(::Ice::Double, ::Ice::Double, const ::std::string&, const ::std::string&, const ::Ice::Context*) = 0;

    virtual ::lbto::result SetPMTerm(const ::std::string&, ::Ice::Double, const ::std::string&, const ::Ice::Context*) = 0;

    virtual ::lbto::result MoveXYZ(::Ice::Double, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::Ice::Context*) = 0;

    virtual ::lbto::result MoveXY(::Ice::Double, ::Ice::Double, const ::std::string&, const ::std::string&, const ::Ice::Context*) = 0;

    virtual ::lbto::result Move(::Ice::Double, ::Ice::Double, ::Ice::Double, ::Ice::Double, ::Ice::Double, ::Ice::Double, ::Ice::Int, const ::std::string&, const ::std::string&, ::Ice::Int, const ::std::string&, const ::Ice::Context*) = 0;

    virtual ::lbto::result SetTarget(::Ice::Double, ::Ice::Double, const ::std::string&, ::Ice::Double, ::Ice::Float, const ::Ice::Context*) = 0;

    virtual ::lbto::result SetStars(const ::lbto::SeqPos&, const ::Ice::Context*) = 0;

    virtual ::lbto::result SetStarsNew(const ::lbto::SeqNewPos&, const ::Ice::Context*) = 0;

    virtual ::lbto::result ClearStars(const ::Ice::Context*) = 0;

    virtual ::lbto::result SetHotspot(::Ice::Double, ::Ice::Double, const ::Ice::Context*) = 0;

    virtual ::lbto::result ClearHotspot(const ::Ice::Context*) = 0;

    virtual ::lbto::result SetOffset(::Ice::Double, ::Ice::Double, const ::std::string&, const ::Ice::Context*) = 0;

    virtual ::lbto::result ClearOffset(const ::Ice::Context*) = 0;

    virtual ::lbto::result SetNonSidereal(const ::lbto::nonsidereal&, bool, const ::Ice::Context*) = 0;

    virtual ::lbto::result ClearNonSidereal(bool, const ::Ice::Context*) = 0;

    virtual ::lbto::result UpdateNonSiderealTarget(::Ice::Double, ::Ice::Double, const ::Ice::Context*) = 0;

    virtual ::lbto::result PresetTelescope(::Ice::Double, const ::std::string&, const ::std::string&, const ::std::string&, const ::Ice::Context*) = 0;

    virtual ::lbto::result BinocularControl(const ::std::string&, const ::Ice::Context*) = 0;

    virtual ::lbto::result GetParameter(const ::lbto::SeqDD&, const ::Ice::Context*) = 0;

    virtual ::lbto::result SetParameter(const ::lbto::SeqDD&, const ::Ice::Context*) = 0;

    virtual ::lbto::result RotateCommon(::Ice::Double, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::Ice::Context*) = 0;

    virtual ::lbto::result RotateZ(::Ice::Double, const ::std::string&, const ::std::string&, const ::Ice::Context*) = 0;

    virtual ::lbto::result RotatePrimary(::Ice::Double, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::Ice::Context*) = 0;

    virtual ::lbto::result SendWavefront(const ::lbto::SeqWF&, const ::std::string&, const ::std::string&, const ::Ice::Context*) = 0;

    virtual ::lbto::result UpdateTargetWavelength(::Ice::Float, const ::std::string&, const ::Ice::Context*) = 0;

    virtual ::lbto::result LogEvent(const ::std::string&, const ::std::string&, const ::Ice::Context*) = 0;

    virtual ::lbto::result GetRotatorTrajectory(::Ice::Double, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::Ice::Context*) = 0;

    virtual ::lbto::result GetRotatorPolynomials(::Ice::Double, ::Ice::Int, ::Ice::Double, ::Ice::Int, const ::std::string&, const ::Ice::Context*) = 0;

    virtual ::lbto::result Standby(::Ice::Int, const ::std::string&, const ::Ice::Context*) = 0;

    virtual ::lbto::result MaximizeWrapTime(bool, bool, const ::std::string&, const ::Ice::Context*) = 0;

    virtual ::lbto::result PauseGuiding(const ::std::string&, const ::Ice::Context*) = 0;

    virtual ::lbto::result ResumeGuiding(const ::std::string&, const ::Ice::Context*) = 0;

    virtual ::lbto::result SetGuidingHotspot(::Ice::Double, ::Ice::Double, const ::std::string&, const ::std::string&, const ::Ice::Context*) = 0;

    virtual ::lbto::result RotReady(bool, const ::std::string&, const ::Ice::Context*) = 0;

    virtual ::lbto::result RotHold(const ::std::string&, const ::Ice::Context*) = 0;

    virtual ::lbto::result RotTrack(const ::std::string&, const ::Ice::Context*) = 0;

    virtual ::lbto::result RotServicePosition(::Ice::Double, const ::std::string&, const ::Ice::Context*) = 0;

    virtual ::lbto::result PauseAO(const ::std::string&, const ::Ice::Context*) = 0;

    virtual ::lbto::result ResumeAO(const ::std::string&, const ::Ice::Context*) = 0;

    virtual ::lbto::result StartAO(const ::std::string&, const ::Ice::Context*) = 0;

    virtual ::lbto::result StopAO(const ::std::string&, const ::std::string&, const ::Ice::Context*) = 0;

    virtual ::lbto::result AcquireRefAO(bool, const ::std::string&, const ::Ice::Context*) = 0;

    virtual ::lbto::result CheckRefAO(const ::std::string&, const ::Ice::Context*) = 0;

    virtual ::lbto::result RefineAO(const ::std::string&, const ::std::string&, const ::Ice::Context*) = 0;

    virtual ::lbto::result RunAO(const ::std::string&, const ::std::string&, const ::Ice::Context*) = 0;

    virtual ::lbto::result PresetFlatAO(const ::std::string&, const ::std::string&, const ::Ice::Context*) = 0;

    virtual ::lbto::result CorrectModesAO(const ::lbto::SeqModes&, const ::std::string&, const ::Ice::Context*) = 0;

    virtual ::lbto::result ModifyAO(::Ice::Int, ::Ice::Double, ::Ice::Int, ::Ice::Double, const ::std::string&, const ::std::string&, const ::std::string&, const ::Ice::Context*) = 0;

    virtual ::lbto::result PresetAO(const ::std::string&, const ::std::string&, const ::Ice::Context*) = 0;

    virtual ::lbto::result OffsetXYAO(::Ice::Double, ::Ice::Double, const ::std::string&, const ::Ice::Context*) = 0;

    virtual ::lbto::result OffsetZAO(::Ice::Double, const ::std::string&, const ::Ice::Context*) = 0;

    virtual ::lbto::result SetReference(const ::lbto::SeqPos&, const ::Ice::Context*) = 0;

    virtual ::lbto::result SetReferenceNew(const ::lbto::SeqNewPos&, const ::Ice::Context*) = 0;

    virtual ::lbto::result ClearReference(const ::Ice::Context*) = 0;

    virtual ::lbto::result GetKFPCoordinates(const ::lbto::SeqPos&, const ::std::string&, const ::Ice::Context*) = 0;

    virtual ::lbto::result GetKFPCoordinatesNew(const ::lbto::SeqNewPos&, const ::std::string&, const ::Ice::Context*) = 0;

    virtual ::lbto::result SetAGWFilter(::Ice::Int, const ::std::string&, const ::Ice::Context*) = 0;

    virtual void remove(const ::Ice::Context*) = 0;
};

}

}

namespace IceDelegateM
{

namespace lbto
{

class IIFServer : virtual public ::IceDelegate::lbto::IIFServer,
                  virtual public ::IceDelegateM::Ice::Object
{
public:

    virtual ::lbto::result Authorize(const ::Ice::Context*);

    virtual ::lbto::result StepFocus(::Ice::Double, const ::std::string&, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result MoveFocus(::Ice::Double, const ::std::string&, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result OffsetPointing(::Ice::Double, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::std::string&, const ::std::string&, const ::std::string&, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result OffsetGuiding(::Ice::Double, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result UpdatePointingReference(const ::std::string&, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result TipTilt(::Ice::Double, ::Ice::Double, const ::std::string&, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result SetPMTerm(const ::std::string&, ::Ice::Double, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result MoveXYZ(::Ice::Double, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result MoveXY(::Ice::Double, ::Ice::Double, const ::std::string&, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result Move(::Ice::Double, ::Ice::Double, ::Ice::Double, ::Ice::Double, ::Ice::Double, ::Ice::Double, ::Ice::Int, const ::std::string&, const ::std::string&, ::Ice::Int, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result SetTarget(::Ice::Double, ::Ice::Double, const ::std::string&, ::Ice::Double, ::Ice::Float, const ::Ice::Context*);

    virtual ::lbto::result SetStars(const ::lbto::SeqPos&, const ::Ice::Context*);

    virtual ::lbto::result SetStarsNew(const ::lbto::SeqNewPos&, const ::Ice::Context*);

    virtual ::lbto::result ClearStars(const ::Ice::Context*);

    virtual ::lbto::result SetHotspot(::Ice::Double, ::Ice::Double, const ::Ice::Context*);

    virtual ::lbto::result ClearHotspot(const ::Ice::Context*);

    virtual ::lbto::result SetOffset(::Ice::Double, ::Ice::Double, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result ClearOffset(const ::Ice::Context*);

    virtual ::lbto::result SetNonSidereal(const ::lbto::nonsidereal&, bool, const ::Ice::Context*);

    virtual ::lbto::result ClearNonSidereal(bool, const ::Ice::Context*);

    virtual ::lbto::result UpdateNonSiderealTarget(::Ice::Double, ::Ice::Double, const ::Ice::Context*);

    virtual ::lbto::result PresetTelescope(::Ice::Double, const ::std::string&, const ::std::string&, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result BinocularControl(const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result GetParameter(const ::lbto::SeqDD&, const ::Ice::Context*);

    virtual ::lbto::result SetParameter(const ::lbto::SeqDD&, const ::Ice::Context*);

    virtual ::lbto::result RotateCommon(::Ice::Double, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result RotateZ(::Ice::Double, const ::std::string&, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result RotatePrimary(::Ice::Double, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result SendWavefront(const ::lbto::SeqWF&, const ::std::string&, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result UpdateTargetWavelength(::Ice::Float, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result LogEvent(const ::std::string&, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result GetRotatorTrajectory(::Ice::Double, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result GetRotatorPolynomials(::Ice::Double, ::Ice::Int, ::Ice::Double, ::Ice::Int, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result Standby(::Ice::Int, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result MaximizeWrapTime(bool, bool, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result PauseGuiding(const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result ResumeGuiding(const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result SetGuidingHotspot(::Ice::Double, ::Ice::Double, const ::std::string&, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result RotReady(bool, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result RotHold(const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result RotTrack(const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result RotServicePosition(::Ice::Double, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result PauseAO(const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result ResumeAO(const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result StartAO(const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result StopAO(const ::std::string&, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result AcquireRefAO(bool, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result CheckRefAO(const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result RefineAO(const ::std::string&, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result RunAO(const ::std::string&, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result PresetFlatAO(const ::std::string&, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result CorrectModesAO(const ::lbto::SeqModes&, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result ModifyAO(::Ice::Int, ::Ice::Double, ::Ice::Int, ::Ice::Double, const ::std::string&, const ::std::string&, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result PresetAO(const ::std::string&, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result OffsetXYAO(::Ice::Double, ::Ice::Double, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result OffsetZAO(::Ice::Double, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result SetReference(const ::lbto::SeqPos&, const ::Ice::Context*);

    virtual ::lbto::result SetReferenceNew(const ::lbto::SeqNewPos&, const ::Ice::Context*);

    virtual ::lbto::result ClearReference(const ::Ice::Context*);

    virtual ::lbto::result GetKFPCoordinates(const ::lbto::SeqPos&, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result GetKFPCoordinatesNew(const ::lbto::SeqNewPos&, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result SetAGWFilter(::Ice::Int, const ::std::string&, const ::Ice::Context*);

    virtual void remove(const ::Ice::Context*);
};

}

}

namespace IceDelegateD
{

namespace lbto
{

class IIFServer : virtual public ::IceDelegate::lbto::IIFServer,
                  virtual public ::IceDelegateD::Ice::Object
{
public:

    virtual ::lbto::result Authorize(const ::Ice::Context*);

    virtual ::lbto::result StepFocus(::Ice::Double, const ::std::string&, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result MoveFocus(::Ice::Double, const ::std::string&, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result OffsetPointing(::Ice::Double, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::std::string&, const ::std::string&, const ::std::string&, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result OffsetGuiding(::Ice::Double, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result UpdatePointingReference(const ::std::string&, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result TipTilt(::Ice::Double, ::Ice::Double, const ::std::string&, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result SetPMTerm(const ::std::string&, ::Ice::Double, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result MoveXYZ(::Ice::Double, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result MoveXY(::Ice::Double, ::Ice::Double, const ::std::string&, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result Move(::Ice::Double, ::Ice::Double, ::Ice::Double, ::Ice::Double, ::Ice::Double, ::Ice::Double, ::Ice::Int, const ::std::string&, const ::std::string&, ::Ice::Int, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result SetTarget(::Ice::Double, ::Ice::Double, const ::std::string&, ::Ice::Double, ::Ice::Float, const ::Ice::Context*);

    virtual ::lbto::result SetStars(const ::lbto::SeqPos&, const ::Ice::Context*);

    virtual ::lbto::result SetStarsNew(const ::lbto::SeqNewPos&, const ::Ice::Context*);

    virtual ::lbto::result ClearStars(const ::Ice::Context*);

    virtual ::lbto::result SetHotspot(::Ice::Double, ::Ice::Double, const ::Ice::Context*);

    virtual ::lbto::result ClearHotspot(const ::Ice::Context*);

    virtual ::lbto::result SetOffset(::Ice::Double, ::Ice::Double, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result ClearOffset(const ::Ice::Context*);

    virtual ::lbto::result SetNonSidereal(const ::lbto::nonsidereal&, bool, const ::Ice::Context*);

    virtual ::lbto::result ClearNonSidereal(bool, const ::Ice::Context*);

    virtual ::lbto::result UpdateNonSiderealTarget(::Ice::Double, ::Ice::Double, const ::Ice::Context*);

    virtual ::lbto::result PresetTelescope(::Ice::Double, const ::std::string&, const ::std::string&, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result BinocularControl(const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result GetParameter(const ::lbto::SeqDD&, const ::Ice::Context*);

    virtual ::lbto::result SetParameter(const ::lbto::SeqDD&, const ::Ice::Context*);

    virtual ::lbto::result RotateCommon(::Ice::Double, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result RotateZ(::Ice::Double, const ::std::string&, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result RotatePrimary(::Ice::Double, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result SendWavefront(const ::lbto::SeqWF&, const ::std::string&, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result UpdateTargetWavelength(::Ice::Float, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result LogEvent(const ::std::string&, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result GetRotatorTrajectory(::Ice::Double, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result GetRotatorPolynomials(::Ice::Double, ::Ice::Int, ::Ice::Double, ::Ice::Int, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result Standby(::Ice::Int, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result MaximizeWrapTime(bool, bool, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result PauseGuiding(const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result ResumeGuiding(const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result SetGuidingHotspot(::Ice::Double, ::Ice::Double, const ::std::string&, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result RotReady(bool, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result RotHold(const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result RotTrack(const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result RotServicePosition(::Ice::Double, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result PauseAO(const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result ResumeAO(const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result StartAO(const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result StopAO(const ::std::string&, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result AcquireRefAO(bool, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result CheckRefAO(const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result RefineAO(const ::std::string&, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result RunAO(const ::std::string&, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result PresetFlatAO(const ::std::string&, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result CorrectModesAO(const ::lbto::SeqModes&, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result ModifyAO(::Ice::Int, ::Ice::Double, ::Ice::Int, ::Ice::Double, const ::std::string&, const ::std::string&, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result PresetAO(const ::std::string&, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result OffsetXYAO(::Ice::Double, ::Ice::Double, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result OffsetZAO(::Ice::Double, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result SetReference(const ::lbto::SeqPos&, const ::Ice::Context*);

    virtual ::lbto::result SetReferenceNew(const ::lbto::SeqNewPos&, const ::Ice::Context*);

    virtual ::lbto::result ClearReference(const ::Ice::Context*);

    virtual ::lbto::result GetKFPCoordinates(const ::lbto::SeqPos&, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result GetKFPCoordinatesNew(const ::lbto::SeqNewPos&, const ::std::string&, const ::Ice::Context*);

    virtual ::lbto::result SetAGWFilter(::Ice::Int, const ::std::string&, const ::Ice::Context*);

    virtual void remove(const ::Ice::Context*);
};

}

}

namespace lbto
{

class IIFServer : virtual public ::Ice::Object
{
public:

    typedef IIFServerPrx ProxyType;
    typedef IIFServerPtr PointerType;
    
    virtual ::Ice::ObjectPtr ice_clone() const;

    virtual bool ice_isA(const ::std::string&, const ::Ice::Current& = ::Ice::Current()) const;
    virtual ::std::vector< ::std::string> ice_ids(const ::Ice::Current& = ::Ice::Current()) const;
    virtual const ::std::string& ice_id(const ::Ice::Current& = ::Ice::Current()) const;
    static const ::std::string& ice_staticId();

    virtual ::lbto::result Authorize(const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___Authorize(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result StepFocus(::Ice::Double, const ::std::string&, const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___StepFocus(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result MoveFocus(::Ice::Double, const ::std::string&, const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___MoveFocus(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result OffsetPointing(::Ice::Double, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::std::string&, const ::std::string&, const ::std::string&, const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___OffsetPointing(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result OffsetGuiding(::Ice::Double, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___OffsetGuiding(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result UpdatePointingReference(const ::std::string&, const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___UpdatePointingReference(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result TipTilt(::Ice::Double, ::Ice::Double, const ::std::string&, const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___TipTilt(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result SetPMTerm(const ::std::string&, ::Ice::Double, const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___SetPMTerm(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result MoveXYZ(::Ice::Double, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___MoveXYZ(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result MoveXY(::Ice::Double, ::Ice::Double, const ::std::string&, const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___MoveXY(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result Move(::Ice::Double, ::Ice::Double, ::Ice::Double, ::Ice::Double, ::Ice::Double, ::Ice::Double, ::Ice::Int, const ::std::string&, const ::std::string&, ::Ice::Int, const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___Move(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result SetTarget(::Ice::Double, ::Ice::Double, const ::std::string&, ::Ice::Double, ::Ice::Float, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___SetTarget(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result SetStars(const ::lbto::SeqPos&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___SetStars(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result SetStarsNew(const ::lbto::SeqNewPos&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___SetStarsNew(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result ClearStars(const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___ClearStars(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result SetHotspot(::Ice::Double, ::Ice::Double, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___SetHotspot(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result ClearHotspot(const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___ClearHotspot(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result SetOffset(::Ice::Double, ::Ice::Double, const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___SetOffset(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result ClearOffset(const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___ClearOffset(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result SetNonSidereal(const ::lbto::nonsidereal&, bool, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___SetNonSidereal(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result ClearNonSidereal(bool, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___ClearNonSidereal(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result UpdateNonSiderealTarget(::Ice::Double, ::Ice::Double, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___UpdateNonSiderealTarget(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result PresetTelescope(::Ice::Double, const ::std::string&, const ::std::string&, const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___PresetTelescope(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result BinocularControl(const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___BinocularControl(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result GetParameter(const ::lbto::SeqDD&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___GetParameter(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result SetParameter(const ::lbto::SeqDD&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___SetParameter(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result RotateCommon(::Ice::Double, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___RotateCommon(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result RotateZ(::Ice::Double, const ::std::string&, const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___RotateZ(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result RotatePrimary(::Ice::Double, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___RotatePrimary(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result SendWavefront(const ::lbto::SeqWF&, const ::std::string&, const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___SendWavefront(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result UpdateTargetWavelength(::Ice::Float, const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___UpdateTargetWavelength(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result LogEvent(const ::std::string&, const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___LogEvent(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result GetRotatorTrajectory(::Ice::Double, ::Ice::Double, ::Ice::Double, const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___GetRotatorTrajectory(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result GetRotatorPolynomials(::Ice::Double, ::Ice::Int, ::Ice::Double, ::Ice::Int, const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___GetRotatorPolynomials(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result Standby(::Ice::Int, const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___Standby(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result MaximizeWrapTime(bool, bool, const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___MaximizeWrapTime(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result PauseGuiding(const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___PauseGuiding(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result ResumeGuiding(const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___ResumeGuiding(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result SetGuidingHotspot(::Ice::Double, ::Ice::Double, const ::std::string&, const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___SetGuidingHotspot(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result RotReady(bool, const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___RotReady(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result RotHold(const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___RotHold(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result RotTrack(const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___RotTrack(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result RotServicePosition(::Ice::Double, const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___RotServicePosition(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result PauseAO(const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___PauseAO(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result ResumeAO(const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___ResumeAO(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result StartAO(const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___StartAO(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result StopAO(const ::std::string&, const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___StopAO(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result AcquireRefAO(bool, const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___AcquireRefAO(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result CheckRefAO(const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___CheckRefAO(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result RefineAO(const ::std::string&, const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___RefineAO(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result RunAO(const ::std::string&, const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___RunAO(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result PresetFlatAO(const ::std::string&, const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___PresetFlatAO(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result CorrectModesAO(const ::lbto::SeqModes&, const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___CorrectModesAO(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result ModifyAO(::Ice::Int, ::Ice::Double, ::Ice::Int, ::Ice::Double, const ::std::string&, const ::std::string&, const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___ModifyAO(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result PresetAO(const ::std::string&, const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___PresetAO(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result OffsetXYAO(::Ice::Double, ::Ice::Double, const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___OffsetXYAO(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result OffsetZAO(::Ice::Double, const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___OffsetZAO(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result SetReference(const ::lbto::SeqPos&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___SetReference(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result SetReferenceNew(const ::lbto::SeqNewPos&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___SetReferenceNew(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result ClearReference(const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___ClearReference(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result GetKFPCoordinates(const ::lbto::SeqPos&, const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___GetKFPCoordinates(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result GetKFPCoordinatesNew(const ::lbto::SeqNewPos&, const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___GetKFPCoordinatesNew(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::lbto::result SetAGWFilter(::Ice::Int, const ::std::string&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___SetAGWFilter(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual void remove(const ::Ice::Current& = ::Ice::Current()) = 0;
    ::Ice::DispatchStatus ___remove(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::Ice::DispatchStatus __dispatch(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual void __write(::IceInternal::BasicStream*) const;
    virtual void __read(::IceInternal::BasicStream*, bool);
    virtual void __write(const ::Ice::OutputStreamPtr&) const;
    virtual void __read(const ::Ice::InputStreamPtr&, bool);
};

}

#endif

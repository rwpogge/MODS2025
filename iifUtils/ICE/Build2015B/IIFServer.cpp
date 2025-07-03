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

#include <IIFServer.h>
#include <Ice/LocalException.h>
#include <Ice/ObjectFactory.h>
#include <Ice/BasicStream.h>
#include <IceUtil/Iterator.h>
#include <IceUtil/ScopedArray.h>

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

static const ::std::string __lbto__IIFServer__Authorize_name = "Authorize";

static const ::std::string __lbto__IIFServer__StepFocus_name = "StepFocus";

static const ::std::string __lbto__IIFServer__MoveFocus_name = "MoveFocus";

static const ::std::string __lbto__IIFServer__OffsetPointing_name = "OffsetPointing";

static const ::std::string __lbto__IIFServer__OffsetGuiding_name = "OffsetGuiding";

static const ::std::string __lbto__IIFServer__UpdatePointingReference_name = "UpdatePointingReference";

static const ::std::string __lbto__IIFServer__TipTilt_name = "TipTilt";

static const ::std::string __lbto__IIFServer__SetPMTerm_name = "SetPMTerm";

static const ::std::string __lbto__IIFServer__MoveXYZ_name = "MoveXYZ";

static const ::std::string __lbto__IIFServer__MoveXY_name = "MoveXY";

static const ::std::string __lbto__IIFServer__Move_name = "Move";

static const ::std::string __lbto__IIFServer__SetTarget_name = "SetTarget";

static const ::std::string __lbto__IIFServer__SetStars_name = "SetStars";

static const ::std::string __lbto__IIFServer__SetStarsNew_name = "SetStarsNew";

static const ::std::string __lbto__IIFServer__ClearStars_name = "ClearStars";

static const ::std::string __lbto__IIFServer__SetHotspot_name = "SetHotspot";

static const ::std::string __lbto__IIFServer__ClearHotspot_name = "ClearHotspot";

static const ::std::string __lbto__IIFServer__SetOffset_name = "SetOffset";

static const ::std::string __lbto__IIFServer__ClearOffset_name = "ClearOffset";

static const ::std::string __lbto__IIFServer__SetNonSidereal_name = "SetNonSidereal";

static const ::std::string __lbto__IIFServer__ClearNonSidereal_name = "ClearNonSidereal";

static const ::std::string __lbto__IIFServer__UpdateNonSiderealTarget_name = "UpdateNonSiderealTarget";

static const ::std::string __lbto__IIFServer__PresetTelescope_name = "PresetTelescope";

static const ::std::string __lbto__IIFServer__BinocularControl_name = "BinocularControl";

static const ::std::string __lbto__IIFServer__GetParameter_name = "GetParameter";

static const ::std::string __lbto__IIFServer__SetParameter_name = "SetParameter";

static const ::std::string __lbto__IIFServer__RotateCommon_name = "RotateCommon";

static const ::std::string __lbto__IIFServer__RotateZ_name = "RotateZ";

static const ::std::string __lbto__IIFServer__RotatePrimary_name = "RotatePrimary";

static const ::std::string __lbto__IIFServer__SendWavefront_name = "SendWavefront";

static const ::std::string __lbto__IIFServer__UpdateTargetWavelength_name = "UpdateTargetWavelength";

static const ::std::string __lbto__IIFServer__LogEvent_name = "LogEvent";

static const ::std::string __lbto__IIFServer__GetRotatorTrajectory_name = "GetRotatorTrajectory";

static const ::std::string __lbto__IIFServer__GetRotatorPolynomials_name = "GetRotatorPolynomials";

static const ::std::string __lbto__IIFServer__Standby_name = "Standby";

static const ::std::string __lbto__IIFServer__MaximizeWrapTime_name = "MaximizeWrapTime";

static const ::std::string __lbto__IIFServer__PauseGuiding_name = "PauseGuiding";

static const ::std::string __lbto__IIFServer__ResumeGuiding_name = "ResumeGuiding";

static const ::std::string __lbto__IIFServer__SetGuidingHotspot_name = "SetGuidingHotspot";

static const ::std::string __lbto__IIFServer__RotReady_name = "RotReady";

static const ::std::string __lbto__IIFServer__RotHold_name = "RotHold";

static const ::std::string __lbto__IIFServer__RotTrack_name = "RotTrack";

static const ::std::string __lbto__IIFServer__RotServicePosition_name = "RotServicePosition";

static const ::std::string __lbto__IIFServer__PauseAO_name = "PauseAO";

static const ::std::string __lbto__IIFServer__ResumeAO_name = "ResumeAO";

static const ::std::string __lbto__IIFServer__StartAO_name = "StartAO";

static const ::std::string __lbto__IIFServer__StopAO_name = "StopAO";

static const ::std::string __lbto__IIFServer__AcquireRefAO_name = "AcquireRefAO";

static const ::std::string __lbto__IIFServer__CheckRefAO_name = "CheckRefAO";

static const ::std::string __lbto__IIFServer__RefineAO_name = "RefineAO";

static const ::std::string __lbto__IIFServer__RunAO_name = "RunAO";

static const ::std::string __lbto__IIFServer__PresetFlatAO_name = "PresetFlatAO";

static const ::std::string __lbto__IIFServer__CorrectModesAO_name = "CorrectModesAO";

static const ::std::string __lbto__IIFServer__ModifyAO_name = "ModifyAO";

static const ::std::string __lbto__IIFServer__PresetAO_name = "PresetAO";

static const ::std::string __lbto__IIFServer__OffsetXYAO_name = "OffsetXYAO";

static const ::std::string __lbto__IIFServer__OffsetZAO_name = "OffsetZAO";

static const ::std::string __lbto__IIFServer__SetReference_name = "SetReference";

static const ::std::string __lbto__IIFServer__SetReferenceNew_name = "SetReferenceNew";

static const ::std::string __lbto__IIFServer__ClearReference_name = "ClearReference";

static const ::std::string __lbto__IIFServer__GetKFPCoordinates_name = "GetKFPCoordinates";

static const ::std::string __lbto__IIFServer__GetKFPCoordinatesNew_name = "GetKFPCoordinatesNew";

static const ::std::string __lbto__IIFServer__SetAGWFilter_name = "SetAGWFilter";

static const ::std::string __lbto__IIFServer__remove_name = "remove";

::Ice::Object* IceInternal::upCast(::lbto::IIFServer* p) { return p; }
::IceProxy::Ice::Object* IceInternal::upCast(::IceProxy::lbto::IIFServer* p) { return p; }

void
lbto::__read(::IceInternal::BasicStream* __is, ::lbto::IIFServerPrx& v)
{
    ::Ice::ObjectPrx proxy;
    __is->read(proxy);
    if(!proxy)
    {
        v = 0;
    }
    else
    {
        v = new ::IceProxy::lbto::IIFServer;
        v->__copyFrom(proxy);
    }
}

bool
lbto::DDstruct::operator==(const DDstruct& __rhs) const
{
    if(this == &__rhs)
    {
        return true;
    }
    if(DDname != __rhs.DDname)
    {
        return false;
    }
    if(DDkey != __rhs.DDkey)
    {
        return false;
    }
    if(dataType != __rhs.dataType)
    {
        return false;
    }
    if(comment != __rhs.comment)
    {
        return false;
    }
    return true;
}

bool
lbto::DDstruct::operator<(const DDstruct& __rhs) const
{
    if(this == &__rhs)
    {
        return false;
    }
    if(DDname < __rhs.DDname)
    {
        return true;
    }
    else if(__rhs.DDname < DDname)
    {
        return false;
    }
    if(DDkey < __rhs.DDkey)
    {
        return true;
    }
    else if(__rhs.DDkey < DDkey)
    {
        return false;
    }
    if(dataType < __rhs.dataType)
    {
        return true;
    }
    else if(__rhs.dataType < dataType)
    {
        return false;
    }
    if(comment < __rhs.comment)
    {
        return true;
    }
    else if(__rhs.comment < comment)
    {
        return false;
    }
    return false;
}

void
lbto::DDstruct::__write(::IceInternal::BasicStream* __os) const
{
    __os->write(DDname);
    __os->write(DDkey);
    __os->write(dataType);
    __os->write(comment);
}

void
lbto::DDstruct::__read(::IceInternal::BasicStream* __is)
{
    __is->read(DDname);
    __is->read(DDkey);
    __is->read(dataType);
    __is->read(comment);
}

void
lbto::__writeSeqDD(::IceInternal::BasicStream* __os, const ::lbto::DDstruct* begin, const ::lbto::DDstruct* end)
{
    ::Ice::Int size = static_cast< ::Ice::Int>(end - begin);
    __os->writeSize(size);
    for(int i = 0; i < size; ++i)
    {
        begin[i].__write(__os);
    }
}

void
lbto::__readSeqDD(::IceInternal::BasicStream* __is, ::lbto::SeqDD& v)
{
    ::Ice::Int sz;
    __is->readSize(sz);
    __is->startSeq(sz, 4);
    v.resize(sz);
    for(int i = 0; i < sz; ++i)
    {
        v[i].__read(__is);
        __is->checkSeq();
        __is->endElement();
    }
    __is->endSeq(sz);
}

bool
lbto::position::operator==(const position& __rhs) const
{
    if(this == &__rhs)
    {
        return true;
    }
    if(coord1 != __rhs.coord1)
    {
        return false;
    }
    if(coord2 != __rhs.coord2)
    {
        return false;
    }
    if(system != __rhs.system)
    {
        return false;
    }
    if(epoch != __rhs.epoch)
    {
        return false;
    }
    if(equinox != __rhs.equinox)
    {
        return false;
    }
    if(pmcoord1 != __rhs.pmcoord1)
    {
        return false;
    }
    if(pmcoord2 != __rhs.pmcoord2)
    {
        return false;
    }
    if(apparentMagnitude != __rhs.apparentMagnitude)
    {
        return false;
    }
    if(filter != __rhs.filter)
    {
        return false;
    }
    if(color != __rhs.color)
    {
        return false;
    }
    if(colorType != __rhs.colorType)
    {
        return false;
    }
    if(wavelength != __rhs.wavelength)
    {
        return false;
    }
    if(targname != __rhs.targname)
    {
        return false;
    }
    return true;
}

bool
lbto::position::operator<(const position& __rhs) const
{
    if(this == &__rhs)
    {
        return false;
    }
    if(coord1 < __rhs.coord1)
    {
        return true;
    }
    else if(__rhs.coord1 < coord1)
    {
        return false;
    }
    if(coord2 < __rhs.coord2)
    {
        return true;
    }
    else if(__rhs.coord2 < coord2)
    {
        return false;
    }
    if(system < __rhs.system)
    {
        return true;
    }
    else if(__rhs.system < system)
    {
        return false;
    }
    if(epoch < __rhs.epoch)
    {
        return true;
    }
    else if(__rhs.epoch < epoch)
    {
        return false;
    }
    if(equinox < __rhs.equinox)
    {
        return true;
    }
    else if(__rhs.equinox < equinox)
    {
        return false;
    }
    if(pmcoord1 < __rhs.pmcoord1)
    {
        return true;
    }
    else if(__rhs.pmcoord1 < pmcoord1)
    {
        return false;
    }
    if(pmcoord2 < __rhs.pmcoord2)
    {
        return true;
    }
    else if(__rhs.pmcoord2 < pmcoord2)
    {
        return false;
    }
    if(apparentMagnitude < __rhs.apparentMagnitude)
    {
        return true;
    }
    else if(__rhs.apparentMagnitude < apparentMagnitude)
    {
        return false;
    }
    if(filter < __rhs.filter)
    {
        return true;
    }
    else if(__rhs.filter < filter)
    {
        return false;
    }
    if(color < __rhs.color)
    {
        return true;
    }
    else if(__rhs.color < color)
    {
        return false;
    }
    if(colorType < __rhs.colorType)
    {
        return true;
    }
    else if(__rhs.colorType < colorType)
    {
        return false;
    }
    if(wavelength < __rhs.wavelength)
    {
        return true;
    }
    else if(__rhs.wavelength < wavelength)
    {
        return false;
    }
    if(targname < __rhs.targname)
    {
        return true;
    }
    else if(__rhs.targname < targname)
    {
        return false;
    }
    return false;
}

void
lbto::position::__write(::IceInternal::BasicStream* __os) const
{
    __os->write(coord1);
    __os->write(coord2);
    __os->write(system);
    __os->write(epoch);
    __os->write(equinox);
    __os->write(pmcoord1);
    __os->write(pmcoord2);
    __os->write(apparentMagnitude);
    __os->write(filter);
    __os->write(color);
    __os->write(colorType);
    __os->write(wavelength);
    __os->write(targname);
}

void
lbto::position::__read(::IceInternal::BasicStream* __is)
{
    __is->read(coord1);
    __is->read(coord2);
    __is->read(system);
    __is->read(epoch);
    __is->read(equinox);
    __is->read(pmcoord1);
    __is->read(pmcoord2);
    __is->read(apparentMagnitude);
    __is->read(filter);
    __is->read(color);
    __is->read(colorType);
    __is->read(wavelength);
    __is->read(targname);
}

void
lbto::__writeSeqPos(::IceInternal::BasicStream* __os, const ::lbto::position* begin, const ::lbto::position* end)
{
    ::Ice::Int size = static_cast< ::Ice::Int>(end - begin);
    __os->writeSize(size);
    for(int i = 0; i < size; ++i)
    {
        begin[i].__write(__os);
    }
}

void
lbto::__readSeqPos(::IceInternal::BasicStream* __is, ::lbto::SeqPos& v)
{
    ::Ice::Int sz;
    __is->readSize(sz);
    __is->startSeq(sz, 65);
    v.resize(sz);
    for(int i = 0; i < sz; ++i)
    {
        v[i].__read(__is);
        __is->checkSeq();
        __is->endElement();
    }
    __is->endSeq(sz);
}

bool
lbto::newposition::operator==(const newposition& __rhs) const
{
    if(this == &__rhs)
    {
        return true;
    }
    if(type != __rhs.type)
    {
        return false;
    }
    if(coord1 != __rhs.coord1)
    {
        return false;
    }
    if(coord2 != __rhs.coord2)
    {
        return false;
    }
    if(system != __rhs.system)
    {
        return false;
    }
    if(equinox != __rhs.equinox)
    {
        return false;
    }
    if(epoch != __rhs.epoch)
    {
        return false;
    }
    if(pmcoord1 != __rhs.pmcoord1)
    {
        return false;
    }
    if(pmcoord2 != __rhs.pmcoord2)
    {
        return false;
    }
    if(apparentMagnitude != __rhs.apparentMagnitude)
    {
        return false;
    }
    if(filter != __rhs.filter)
    {
        return false;
    }
    if(color != __rhs.color)
    {
        return false;
    }
    if(colorType != __rhs.colorType)
    {
        return false;
    }
    if(wavelength != __rhs.wavelength)
    {
        return false;
    }
    if(file != __rhs.file)
    {
        return false;
    }
    if(time != __rhs.time)
    {
        return false;
    }
    if(RARate != __rhs.RARate)
    {
        return false;
    }
    if(DECRate != __rhs.DECRate)
    {
        return false;
    }
    return true;
}

bool
lbto::newposition::operator<(const newposition& __rhs) const
{
    if(this == &__rhs)
    {
        return false;
    }
    if(type < __rhs.type)
    {
        return true;
    }
    else if(__rhs.type < type)
    {
        return false;
    }
    if(coord1 < __rhs.coord1)
    {
        return true;
    }
    else if(__rhs.coord1 < coord1)
    {
        return false;
    }
    if(coord2 < __rhs.coord2)
    {
        return true;
    }
    else if(__rhs.coord2 < coord2)
    {
        return false;
    }
    if(system < __rhs.system)
    {
        return true;
    }
    else if(__rhs.system < system)
    {
        return false;
    }
    if(equinox < __rhs.equinox)
    {
        return true;
    }
    else if(__rhs.equinox < equinox)
    {
        return false;
    }
    if(epoch < __rhs.epoch)
    {
        return true;
    }
    else if(__rhs.epoch < epoch)
    {
        return false;
    }
    if(pmcoord1 < __rhs.pmcoord1)
    {
        return true;
    }
    else if(__rhs.pmcoord1 < pmcoord1)
    {
        return false;
    }
    if(pmcoord2 < __rhs.pmcoord2)
    {
        return true;
    }
    else if(__rhs.pmcoord2 < pmcoord2)
    {
        return false;
    }
    if(apparentMagnitude < __rhs.apparentMagnitude)
    {
        return true;
    }
    else if(__rhs.apparentMagnitude < apparentMagnitude)
    {
        return false;
    }
    if(filter < __rhs.filter)
    {
        return true;
    }
    else if(__rhs.filter < filter)
    {
        return false;
    }
    if(color < __rhs.color)
    {
        return true;
    }
    else if(__rhs.color < color)
    {
        return false;
    }
    if(colorType < __rhs.colorType)
    {
        return true;
    }
    else if(__rhs.colorType < colorType)
    {
        return false;
    }
    if(wavelength < __rhs.wavelength)
    {
        return true;
    }
    else if(__rhs.wavelength < wavelength)
    {
        return false;
    }
    if(file < __rhs.file)
    {
        return true;
    }
    else if(__rhs.file < file)
    {
        return false;
    }
    if(time < __rhs.time)
    {
        return true;
    }
    else if(__rhs.time < time)
    {
        return false;
    }
    if(RARate < __rhs.RARate)
    {
        return true;
    }
    else if(__rhs.RARate < RARate)
    {
        return false;
    }
    if(DECRate < __rhs.DECRate)
    {
        return true;
    }
    else if(__rhs.DECRate < DECRate)
    {
        return false;
    }
    return false;
}

void
lbto::newposition::__write(::IceInternal::BasicStream* __os) const
{
    __os->write(type);
    __os->write(coord1);
    __os->write(coord2);
    __os->write(system);
    __os->write(equinox);
    __os->write(epoch);
    __os->write(pmcoord1);
    __os->write(pmcoord2);
    __os->write(apparentMagnitude);
    __os->write(filter);
    __os->write(color);
    __os->write(colorType);
    __os->write(wavelength);
    __os->write(file);
    __os->write(time);
    __os->write(RARate);
    __os->write(DECRate);
}

void
lbto::newposition::__read(::IceInternal::BasicStream* __is)
{
    __is->read(type);
    __is->read(coord1);
    __is->read(coord2);
    __is->read(system);
    __is->read(equinox);
    __is->read(epoch);
    __is->read(pmcoord1);
    __is->read(pmcoord2);
    __is->read(apparentMagnitude);
    __is->read(filter);
    __is->read(color);
    __is->read(colorType);
    __is->read(wavelength);
    __is->read(file);
    __is->read(time);
    __is->read(RARate);
    __is->read(DECRate);
}

void
lbto::__writeSeqNewPos(::IceInternal::BasicStream* __os, const ::lbto::newposition* begin, const ::lbto::newposition* end)
{
    ::Ice::Int size = static_cast< ::Ice::Int>(end - begin);
    __os->writeSize(size);
    for(int i = 0; i < size; ++i)
    {
        begin[i].__write(__os);
    }
}

void
lbto::__readSeqNewPos(::IceInternal::BasicStream* __is, ::lbto::SeqNewPos& v)
{
    ::Ice::Int sz;
    __is->readSize(sz);
    __is->startSeq(sz, 90);
    v.resize(sz);
    for(int i = 0; i < sz; ++i)
    {
        v[i].__read(__is);
        __is->checkSeq();
        __is->endElement();
    }
    __is->endSeq(sz);
}

bool
lbto::nonsidereal::operator==(const nonsidereal& __rhs) const
{
    if(this == &__rhs)
    {
        return true;
    }
    if(type != __rhs.type)
    {
        return false;
    }
    if(wavelength != __rhs.wavelength)
    {
        return false;
    }
    if(file != __rhs.file)
    {
        return false;
    }
    if(value0 != __rhs.value0)
    {
        return false;
    }
    if(value1 != __rhs.value1)
    {
        return false;
    }
    if(value2 != __rhs.value2)
    {
        return false;
    }
    if(value3 != __rhs.value3)
    {
        return false;
    }
    if(value4 != __rhs.value4)
    {
        return false;
    }
    return true;
}

bool
lbto::nonsidereal::operator<(const nonsidereal& __rhs) const
{
    if(this == &__rhs)
    {
        return false;
    }
    if(type < __rhs.type)
    {
        return true;
    }
    else if(__rhs.type < type)
    {
        return false;
    }
    if(wavelength < __rhs.wavelength)
    {
        return true;
    }
    else if(__rhs.wavelength < wavelength)
    {
        return false;
    }
    if(file < __rhs.file)
    {
        return true;
    }
    else if(__rhs.file < file)
    {
        return false;
    }
    if(value0 < __rhs.value0)
    {
        return true;
    }
    else if(__rhs.value0 < value0)
    {
        return false;
    }
    if(value1 < __rhs.value1)
    {
        return true;
    }
    else if(__rhs.value1 < value1)
    {
        return false;
    }
    if(value2 < __rhs.value2)
    {
        return true;
    }
    else if(__rhs.value2 < value2)
    {
        return false;
    }
    if(value3 < __rhs.value3)
    {
        return true;
    }
    else if(__rhs.value3 < value3)
    {
        return false;
    }
    if(value4 < __rhs.value4)
    {
        return true;
    }
    else if(__rhs.value4 < value4)
    {
        return false;
    }
    return false;
}

void
lbto::nonsidereal::__write(::IceInternal::BasicStream* __os) const
{
    __os->write(type);
    __os->write(wavelength);
    __os->write(file);
    __os->write(value0);
    __os->write(value1);
    __os->write(value2);
    __os->write(value3);
    __os->write(value4);
}

void
lbto::nonsidereal::__read(::IceInternal::BasicStream* __is)
{
    __is->read(type);
    __is->read(wavelength);
    __is->read(file);
    __is->read(value0);
    __is->read(value1);
    __is->read(value2);
    __is->read(value3);
    __is->read(value4);
}

bool
lbto::result::operator==(const result& __rhs) const
{
    if(this == &__rhs)
    {
        return true;
    }
    if(rescode != __rhs.rescode)
    {
        return false;
    }
    if(resmsg != __rhs.resmsg)
    {
        return false;
    }
    return true;
}

bool
lbto::result::operator<(const result& __rhs) const
{
    if(this == &__rhs)
    {
        return false;
    }
    if(rescode < __rhs.rescode)
    {
        return true;
    }
    else if(__rhs.rescode < rescode)
    {
        return false;
    }
    if(resmsg < __rhs.resmsg)
    {
        return true;
    }
    else if(__rhs.resmsg < resmsg)
    {
        return false;
    }
    return false;
}

void
lbto::result::__write(::IceInternal::BasicStream* __os) const
{
    __os->write(rescode);
    if(resmsg.size() == 0)
    {
        __os->writeSize(0);
    }
    else
    {
        __os->write(&resmsg[0], &resmsg[0] + resmsg.size());
    }
}

void
lbto::result::__read(::IceInternal::BasicStream* __is)
{
    __is->read(rescode);
    __is->read(resmsg);
}

bool
lbto::AMI_IIFServer_Authorize::__invoke(const ::lbto::IIFServerPrx& __prx, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__Authorize_name);
        __prepare(__prx, __lbto__IIFServer__Authorize_name, ::Ice::Normal, __ctx);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_Authorize::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_StepFocus::__invoke(const ::lbto::IIFServerPrx& __prx, ::Ice::Double relPos, const ::std::string& ope, const ::std::string& side, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__StepFocus_name);
        __prepare(__prx, __lbto__IIFServer__StepFocus_name, ::Ice::Normal, __ctx);
        __os->write(relPos);
        __os->write(ope);
        __os->write(side);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_StepFocus::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_MoveFocus::__invoke(const ::lbto::IIFServerPrx& __prx, ::Ice::Double absPos, const ::std::string& ope, const ::std::string& side, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__MoveFocus_name);
        __prepare(__prx, __lbto__IIFServer__MoveFocus_name, ::Ice::Normal, __ctx);
        __os->write(absPos);
        __os->write(ope);
        __os->write(side);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_MoveFocus::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_OffsetPointing::__invoke(const ::lbto::IIFServerPrx& __prx, ::Ice::Double rotangle, ::Ice::Double offx, ::Ice::Double offy, const ::std::string& coordsys, const ::std::string& ope, const ::std::string& np, const ::std::string& movetype, const ::std::string& side, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__OffsetPointing_name);
        __prepare(__prx, __lbto__IIFServer__OffsetPointing_name, ::Ice::Normal, __ctx);
        __os->write(rotangle);
        __os->write(offx);
        __os->write(offy);
        __os->write(coordsys);
        __os->write(ope);
        __os->write(np);
        __os->write(movetype);
        __os->write(side);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_OffsetPointing::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_OffsetGuiding::__invoke(const ::lbto::IIFServerPrx& __prx, ::Ice::Double rotangle, ::Ice::Double offx, ::Ice::Double offy, const ::std::string& coordsys, const ::std::string& side, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__OffsetGuiding_name);
        __prepare(__prx, __lbto__IIFServer__OffsetGuiding_name, ::Ice::Normal, __ctx);
        __os->write(rotangle);
        __os->write(offx);
        __os->write(offy);
        __os->write(coordsys);
        __os->write(side);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_OffsetGuiding::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_UpdatePointingReference::__invoke(const ::lbto::IIFServerPrx& __prx, const ::std::string& offsetcoord, const ::std::string& side, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__UpdatePointingReference_name);
        __prepare(__prx, __lbto__IIFServer__UpdatePointingReference_name, ::Ice::Normal, __ctx);
        __os->write(offsetcoord);
        __os->write(side);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_UpdatePointingReference::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_TipTilt::__invoke(const ::lbto::IIFServerPrx& __prx, ::Ice::Double xrot, ::Ice::Double yrot, const ::std::string& ope, const ::std::string& side, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__TipTilt_name);
        __prepare(__prx, __lbto__IIFServer__TipTilt_name, ::Ice::Normal, __ctx);
        __os->write(xrot);
        __os->write(yrot);
        __os->write(ope);
        __os->write(side);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_TipTilt::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_SetPMTerm::__invoke(const ::lbto::IIFServerPrx& __prx, const ::std::string& name, ::Ice::Double value, const ::std::string& side, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__SetPMTerm_name);
        __prepare(__prx, __lbto__IIFServer__SetPMTerm_name, ::Ice::Normal, __ctx);
        __os->write(name);
        __os->write(value);
        __os->write(side);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_SetPMTerm::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_MoveXYZ::__invoke(const ::lbto::IIFServerPrx& __prx, ::Ice::Double xmov, ::Ice::Double ymov, ::Ice::Double zmov, const ::std::string& side, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__MoveXYZ_name);
        __prepare(__prx, __lbto__IIFServer__MoveXYZ_name, ::Ice::Normal, __ctx);
        __os->write(xmov);
        __os->write(ymov);
        __os->write(zmov);
        __os->write(side);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_MoveXYZ::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_MoveXY::__invoke(const ::lbto::IIFServerPrx& __prx, ::Ice::Double xmov, ::Ice::Double ymov, const ::std::string& ope, const ::std::string& side, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__MoveXY_name);
        __prepare(__prx, __lbto__IIFServer__MoveXY_name, ::Ice::Normal, __ctx);
        __os->write(xmov);
        __os->write(ymov);
        __os->write(ope);
        __os->write(side);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_MoveXY::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_Move::__invoke(const ::lbto::IIFServerPrx& __prx, ::Ice::Double x, ::Ice::Double y, ::Ice::Double z, ::Ice::Double rx, ::Ice::Double ry, ::Ice::Double rz, ::Ice::Int dflag, const ::std::string& movetype, const ::std::string& OPE, ::Ice::Int time, const ::std::string& side, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__Move_name);
        __prepare(__prx, __lbto__IIFServer__Move_name, ::Ice::Normal, __ctx);
        __os->write(x);
        __os->write(y);
        __os->write(z);
        __os->write(rx);
        __os->write(ry);
        __os->write(rz);
        __os->write(dflag);
        __os->write(movetype);
        __os->write(OPE);
        __os->write(time);
        __os->write(side);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_Move::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_SetTarget::__invoke(const ::lbto::IIFServerPrx& __prx, ::Ice::Double coord1, ::Ice::Double coord2, const ::std::string& system, ::Ice::Double epoch, ::Ice::Float wavelength, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__SetTarget_name);
        __prepare(__prx, __lbto__IIFServer__SetTarget_name, ::Ice::Normal, __ctx);
        __os->write(coord1);
        __os->write(coord2);
        __os->write(system);
        __os->write(epoch);
        __os->write(wavelength);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_SetTarget::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_SetStars::__invoke(const ::lbto::IIFServerPrx& __prx, const ::lbto::SeqPos& stars, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__SetStars_name);
        __prepare(__prx, __lbto__IIFServer__SetStars_name, ::Ice::Normal, __ctx);
        if(stars.size() == 0)
        {
            __os->writeSize(0);
        }
        else
        {
            ::lbto::__writeSeqPos(__os, &stars[0], &stars[0] + stars.size());
        }
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_SetStars::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_SetStarsNew::__invoke(const ::lbto::IIFServerPrx& __prx, const ::lbto::SeqNewPos& stars, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__SetStarsNew_name);
        __prepare(__prx, __lbto__IIFServer__SetStarsNew_name, ::Ice::Normal, __ctx);
        if(stars.size() == 0)
        {
            __os->writeSize(0);
        }
        else
        {
            ::lbto::__writeSeqNewPos(__os, &stars[0], &stars[0] + stars.size());
        }
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_SetStarsNew::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_ClearStars::__invoke(const ::lbto::IIFServerPrx& __prx, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__ClearStars_name);
        __prepare(__prx, __lbto__IIFServer__ClearStars_name, ::Ice::Normal, __ctx);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_ClearStars::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_SetHotspot::__invoke(const ::lbto::IIFServerPrx& __prx, ::Ice::Double coord1, ::Ice::Double coord2, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__SetHotspot_name);
        __prepare(__prx, __lbto__IIFServer__SetHotspot_name, ::Ice::Normal, __ctx);
        __os->write(coord1);
        __os->write(coord2);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_SetHotspot::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_ClearHotspot::__invoke(const ::lbto::IIFServerPrx& __prx, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__ClearHotspot_name);
        __prepare(__prx, __lbto__IIFServer__ClearHotspot_name, ::Ice::Normal, __ctx);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_ClearHotspot::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_SetOffset::__invoke(const ::lbto::IIFServerPrx& __prx, ::Ice::Double coord1, ::Ice::Double coord2, const ::std::string& system, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__SetOffset_name);
        __prepare(__prx, __lbto__IIFServer__SetOffset_name, ::Ice::Normal, __ctx);
        __os->write(coord1);
        __os->write(coord2);
        __os->write(system);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_SetOffset::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_ClearOffset::__invoke(const ::lbto::IIFServerPrx& __prx, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__ClearOffset_name);
        __prepare(__prx, __lbto__IIFServer__ClearOffset_name, ::Ice::Normal, __ctx);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_ClearOffset::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_SetNonSidereal::__invoke(const ::lbto::IIFServerPrx& __prx, const ::lbto::nonsidereal& target, bool override, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__SetNonSidereal_name);
        __prepare(__prx, __lbto__IIFServer__SetNonSidereal_name, ::Ice::Normal, __ctx);
        target.__write(__os);
        __os->write(override);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_SetNonSidereal::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_ClearNonSidereal::__invoke(const ::lbto::IIFServerPrx& __prx, bool override, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__ClearNonSidereal_name);
        __prepare(__prx, __lbto__IIFServer__ClearNonSidereal_name, ::Ice::Normal, __ctx);
        __os->write(override);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_ClearNonSidereal::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_UpdateNonSiderealTarget::__invoke(const ::lbto::IIFServerPrx& __prx, ::Ice::Double RArate, ::Ice::Double DECrate, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__UpdateNonSiderealTarget_name);
        __prepare(__prx, __lbto__IIFServer__UpdateNonSiderealTarget_name, ::Ice::Normal, __ctx);
        __os->write(RArate);
        __os->write(DECrate);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_UpdateNonSiderealTarget::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_PresetTelescope::__invoke(const ::lbto::IIFServerPrx& __prx, ::Ice::Double rotangle, const ::std::string& rotmode, const ::std::string& mode, const ::std::string& side, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__PresetTelescope_name);
        __prepare(__prx, __lbto__IIFServer__PresetTelescope_name, ::Ice::Normal, __ctx);
        __os->write(rotangle);
        __os->write(rotmode);
        __os->write(mode);
        __os->write(side);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_PresetTelescope::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_BinocularControl::__invoke(const ::lbto::IIFServerPrx& __prx, const ::std::string& flag, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__BinocularControl_name);
        __prepare(__prx, __lbto__IIFServer__BinocularControl_name, ::Ice::Normal, __ctx);
        __os->write(flag);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_BinocularControl::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_GetParameter::__invoke(const ::lbto::IIFServerPrx& __prx, const ::lbto::SeqDD& list, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__GetParameter_name);
        __prepare(__prx, __lbto__IIFServer__GetParameter_name, ::Ice::Normal, __ctx);
        if(list.size() == 0)
        {
            __os->writeSize(0);
        }
        else
        {
            ::lbto::__writeSeqDD(__os, &list[0], &list[0] + list.size());
        }
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_GetParameter::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_SetParameter::__invoke(const ::lbto::IIFServerPrx& __prx, const ::lbto::SeqDD& list, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__SetParameter_name);
        __prepare(__prx, __lbto__IIFServer__SetParameter_name, ::Ice::Normal, __ctx);
        if(list.size() == 0)
        {
            __os->writeSize(0);
        }
        else
        {
            ::lbto::__writeSeqDD(__os, &list[0], &list[0] + list.size());
        }
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_SetParameter::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_RotateCommon::__invoke(const ::lbto::IIFServerPrx& __prx, ::Ice::Double z, ::Ice::Double angle, ::Ice::Double direction, const ::std::string& side, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__RotateCommon_name);
        __prepare(__prx, __lbto__IIFServer__RotateCommon_name, ::Ice::Normal, __ctx);
        __os->write(z);
        __os->write(angle);
        __os->write(direction);
        __os->write(side);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_RotateCommon::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_RotateZ::__invoke(const ::lbto::IIFServerPrx& __prx, ::Ice::Double angle, const ::std::string& movetype, const ::std::string& side, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__RotateZ_name);
        __prepare(__prx, __lbto__IIFServer__RotateZ_name, ::Ice::Normal, __ctx);
        __os->write(angle);
        __os->write(movetype);
        __os->write(side);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_RotateZ::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_RotatePrimary::__invoke(const ::lbto::IIFServerPrx& __prx, ::Ice::Double distance, ::Ice::Double angle, ::Ice::Double direction, const ::std::string& side, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__RotatePrimary_name);
        __prepare(__prx, __lbto__IIFServer__RotatePrimary_name, ::Ice::Normal, __ctx);
        __os->write(distance);
        __os->write(angle);
        __os->write(direction);
        __os->write(side);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_RotatePrimary::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_SendWavefront::__invoke(const ::lbto::IIFServerPrx& __prx, const ::lbto::SeqWF& wfs, const ::std::string& OPE, const ::std::string& side, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__SendWavefront_name);
        __prepare(__prx, __lbto__IIFServer__SendWavefront_name, ::Ice::Normal, __ctx);
        if(wfs.size() == 0)
        {
            __os->writeSize(0);
        }
        else
        {
            __os->write(&wfs[0], &wfs[0] + wfs.size());
        }
        __os->write(OPE);
        __os->write(side);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_SendWavefront::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_UpdateTargetWavelength::__invoke(const ::lbto::IIFServerPrx& __prx, ::Ice::Float wavelength, const ::std::string& side, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__UpdateTargetWavelength_name);
        __prepare(__prx, __lbto__IIFServer__UpdateTargetWavelength_name, ::Ice::Normal, __ctx);
        __os->write(wavelength);
        __os->write(side);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_UpdateTargetWavelength::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_LogEvent::__invoke(const ::lbto::IIFServerPrx& __prx, const ::std::string& event, const ::std::string& description, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__LogEvent_name);
        __prepare(__prx, __lbto__IIFServer__LogEvent_name, ::Ice::Normal, __ctx);
        __os->write(event);
        __os->write(description);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_LogEvent::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_GetRotatorTrajectory::__invoke(const ::lbto::IIFServerPrx& __prx, ::Ice::Double seconds, ::Ice::Double interval, ::Ice::Double start, const ::std::string& side, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__GetRotatorTrajectory_name);
        __prepare(__prx, __lbto__IIFServer__GetRotatorTrajectory_name, ::Ice::Normal, __ctx);
        __os->write(seconds);
        __os->write(interval);
        __os->write(start);
        __os->write(side);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_GetRotatorTrajectory::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_GetRotatorPolynomials::__invoke(const ::lbto::IIFServerPrx& __prx, ::Ice::Double start, ::Ice::Int count, ::Ice::Double interval, ::Ice::Int order, const ::std::string& side, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__GetRotatorPolynomials_name);
        __prepare(__prx, __lbto__IIFServer__GetRotatorPolynomials_name, ::Ice::Normal, __ctx);
        __os->write(start);
        __os->write(count);
        __os->write(interval);
        __os->write(order);
        __os->write(side);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_GetRotatorPolynomials::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_Standby::__invoke(const ::lbto::IIFServerPrx& __prx, ::Ice::Int level, const ::std::string& side, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__Standby_name);
        __prepare(__prx, __lbto__IIFServer__Standby_name, ::Ice::Normal, __ctx);
        __os->write(level);
        __os->write(side);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_Standby::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_MaximizeWrapTime::__invoke(const ::lbto::IIFServerPrx& __prx, bool azflag, bool rotflag, const ::std::string& side, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__MaximizeWrapTime_name);
        __prepare(__prx, __lbto__IIFServer__MaximizeWrapTime_name, ::Ice::Normal, __ctx);
        __os->write(azflag);
        __os->write(rotflag);
        __os->write(side);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_MaximizeWrapTime::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_PauseGuiding::__invoke(const ::lbto::IIFServerPrx& __prx, const ::std::string& side, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__PauseGuiding_name);
        __prepare(__prx, __lbto__IIFServer__PauseGuiding_name, ::Ice::Normal, __ctx);
        __os->write(side);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_PauseGuiding::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_ResumeGuiding::__invoke(const ::lbto::IIFServerPrx& __prx, const ::std::string& side, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__ResumeGuiding_name);
        __prepare(__prx, __lbto__IIFServer__ResumeGuiding_name, ::Ice::Normal, __ctx);
        __os->write(side);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_ResumeGuiding::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_SetGuidingHotspot::__invoke(const ::lbto::IIFServerPrx& __prx, ::Ice::Double coord1, ::Ice::Double coord2, const ::std::string& movetype, const ::std::string& side, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__SetGuidingHotspot_name);
        __prepare(__prx, __lbto__IIFServer__SetGuidingHotspot_name, ::Ice::Normal, __ctx);
        __os->write(coord1);
        __os->write(coord2);
        __os->write(movetype);
        __os->write(side);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_SetGuidingHotspot::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_RotReady::__invoke(const ::lbto::IIFServerPrx& __prx, bool enable, const ::std::string& side, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__RotReady_name);
        __prepare(__prx, __lbto__IIFServer__RotReady_name, ::Ice::Normal, __ctx);
        __os->write(enable);
        __os->write(side);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_RotReady::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_RotHold::__invoke(const ::lbto::IIFServerPrx& __prx, const ::std::string& side, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__RotHold_name);
        __prepare(__prx, __lbto__IIFServer__RotHold_name, ::Ice::Normal, __ctx);
        __os->write(side);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_RotHold::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_RotTrack::__invoke(const ::lbto::IIFServerPrx& __prx, const ::std::string& side, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__RotTrack_name);
        __prepare(__prx, __lbto__IIFServer__RotTrack_name, ::Ice::Normal, __ctx);
        __os->write(side);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_RotTrack::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_RotServicePosition::__invoke(const ::lbto::IIFServerPrx& __prx, ::Ice::Double angle, const ::std::string& side, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__RotServicePosition_name);
        __prepare(__prx, __lbto__IIFServer__RotServicePosition_name, ::Ice::Normal, __ctx);
        __os->write(angle);
        __os->write(side);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_RotServicePosition::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_PauseAO::__invoke(const ::lbto::IIFServerPrx& __prx, const ::std::string& side, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__PauseAO_name);
        __prepare(__prx, __lbto__IIFServer__PauseAO_name, ::Ice::Normal, __ctx);
        __os->write(side);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_PauseAO::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_ResumeAO::__invoke(const ::lbto::IIFServerPrx& __prx, const ::std::string& side, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__ResumeAO_name);
        __prepare(__prx, __lbto__IIFServer__ResumeAO_name, ::Ice::Normal, __ctx);
        __os->write(side);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_ResumeAO::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_StartAO::__invoke(const ::lbto::IIFServerPrx& __prx, const ::std::string& side, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__StartAO_name);
        __prepare(__prx, __lbto__IIFServer__StartAO_name, ::Ice::Normal, __ctx);
        __os->write(side);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_StartAO::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_StopAO::__invoke(const ::lbto::IIFServerPrx& __prx, const ::std::string& reason, const ::std::string& side, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__StopAO_name);
        __prepare(__prx, __lbto__IIFServer__StopAO_name, ::Ice::Normal, __ctx);
        __os->write(reason);
        __os->write(side);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_StopAO::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_AcquireRefAO::__invoke(const ::lbto::IIFServerPrx& __prx, bool repointFlag, const ::std::string& side, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__AcquireRefAO_name);
        __prepare(__prx, __lbto__IIFServer__AcquireRefAO_name, ::Ice::Normal, __ctx);
        __os->write(repointFlag);
        __os->write(side);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_AcquireRefAO::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_CheckRefAO::__invoke(const ::lbto::IIFServerPrx& __prx, const ::std::string& side, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__CheckRefAO_name);
        __prepare(__prx, __lbto__IIFServer__CheckRefAO_name, ::Ice::Normal, __ctx);
        __os->write(side);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_CheckRefAO::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_RefineAO::__invoke(const ::lbto::IIFServerPrx& __prx, const ::std::string& method, const ::std::string& side, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__RefineAO_name);
        __prepare(__prx, __lbto__IIFServer__RefineAO_name, ::Ice::Normal, __ctx);
        __os->write(method);
        __os->write(side);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_RefineAO::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_RunAO::__invoke(const ::lbto::IIFServerPrx& __prx, const ::std::string& type, const ::std::string& side, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__RunAO_name);
        __prepare(__prx, __lbto__IIFServer__RunAO_name, ::Ice::Normal, __ctx);
        __os->write(type);
        __os->write(side);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_RunAO::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_PresetFlatAO::__invoke(const ::lbto::IIFServerPrx& __prx, const ::std::string& flat, const ::std::string& side, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__PresetFlatAO_name);
        __prepare(__prx, __lbto__IIFServer__PresetFlatAO_name, ::Ice::Normal, __ctx);
        __os->write(flat);
        __os->write(side);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_PresetFlatAO::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_CorrectModesAO::__invoke(const ::lbto::IIFServerPrx& __prx, const ::lbto::SeqModes& modes, const ::std::string& side, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__CorrectModesAO_name);
        __prepare(__prx, __lbto__IIFServer__CorrectModesAO_name, ::Ice::Normal, __ctx);
        if(modes.size() == 0)
        {
            __os->writeSize(0);
        }
        else
        {
            __os->write(&modes[0], &modes[0] + modes.size());
        }
        __os->write(side);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_CorrectModesAO::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_ModifyAO::__invoke(const ::lbto::IIFServerPrx& __prx, ::Ice::Int NModes, ::Ice::Double Freq, ::Ice::Int Nbins, ::Ice::Double TTMod, const ::std::string& F1spec, const ::std::string& F2spec, const ::std::string& side, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__ModifyAO_name);
        __prepare(__prx, __lbto__IIFServer__ModifyAO_name, ::Ice::Normal, __ctx);
        __os->write(NModes);
        __os->write(Freq);
        __os->write(Nbins);
        __os->write(TTMod);
        __os->write(F1spec);
        __os->write(F2spec);
        __os->write(side);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_ModifyAO::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_PresetAO::__invoke(const ::lbto::IIFServerPrx& __prx, const ::std::string& AOMode, const ::std::string& side, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__PresetAO_name);
        __prepare(__prx, __lbto__IIFServer__PresetAO_name, ::Ice::Normal, __ctx);
        __os->write(AOMode);
        __os->write(side);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_PresetAO::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_OffsetXYAO::__invoke(const ::lbto::IIFServerPrx& __prx, ::Ice::Double DeltaX, ::Ice::Double DeltaY, const ::std::string& side, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__OffsetXYAO_name);
        __prepare(__prx, __lbto__IIFServer__OffsetXYAO_name, ::Ice::Normal, __ctx);
        __os->write(DeltaX);
        __os->write(DeltaY);
        __os->write(side);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_OffsetXYAO::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_OffsetZAO::__invoke(const ::lbto::IIFServerPrx& __prx, ::Ice::Double DeltaZ, const ::std::string& side, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__OffsetZAO_name);
        __prepare(__prx, __lbto__IIFServer__OffsetZAO_name, ::Ice::Normal, __ctx);
        __os->write(DeltaZ);
        __os->write(side);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_OffsetZAO::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_SetReference::__invoke(const ::lbto::IIFServerPrx& __prx, const ::lbto::SeqPos& stars, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__SetReference_name);
        __prepare(__prx, __lbto__IIFServer__SetReference_name, ::Ice::Normal, __ctx);
        if(stars.size() == 0)
        {
            __os->writeSize(0);
        }
        else
        {
            ::lbto::__writeSeqPos(__os, &stars[0], &stars[0] + stars.size());
        }
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_SetReference::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_SetReferenceNew::__invoke(const ::lbto::IIFServerPrx& __prx, const ::lbto::SeqNewPos& stars, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__SetReferenceNew_name);
        __prepare(__prx, __lbto__IIFServer__SetReferenceNew_name, ::Ice::Normal, __ctx);
        if(stars.size() == 0)
        {
            __os->writeSize(0);
        }
        else
        {
            ::lbto::__writeSeqNewPos(__os, &stars[0], &stars[0] + stars.size());
        }
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_SetReferenceNew::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_ClearReference::__invoke(const ::lbto::IIFServerPrx& __prx, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__ClearReference_name);
        __prepare(__prx, __lbto__IIFServer__ClearReference_name, ::Ice::Normal, __ctx);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_ClearReference::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_GetKFPCoordinates::__invoke(const ::lbto::IIFServerPrx& __prx, const ::lbto::SeqPos& stars, const ::std::string& side, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__GetKFPCoordinates_name);
        __prepare(__prx, __lbto__IIFServer__GetKFPCoordinates_name, ::Ice::Normal, __ctx);
        if(stars.size() == 0)
        {
            __os->writeSize(0);
        }
        else
        {
            ::lbto::__writeSeqPos(__os, &stars[0], &stars[0] + stars.size());
        }
        __os->write(side);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_GetKFPCoordinates::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_GetKFPCoordinatesNew::__invoke(const ::lbto::IIFServerPrx& __prx, const ::lbto::SeqNewPos& stars, const ::std::string& side, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__GetKFPCoordinatesNew_name);
        __prepare(__prx, __lbto__IIFServer__GetKFPCoordinatesNew_name, ::Ice::Normal, __ctx);
        if(stars.size() == 0)
        {
            __os->writeSize(0);
        }
        else
        {
            ::lbto::__writeSeqNewPos(__os, &stars[0], &stars[0] + stars.size());
        }
        __os->write(side);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_GetKFPCoordinatesNew::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

bool
lbto::AMI_IIFServer_SetAGWFilter::__invoke(const ::lbto::IIFServerPrx& __prx, ::Ice::Int filterNumber, const ::std::string& side, const ::Ice::Context* __ctx)
{
    __acquireCallback(__prx);
    try
    {
        __prx->__checkTwowayOnly(__lbto__IIFServer__SetAGWFilter_name);
        __prepare(__prx, __lbto__IIFServer__SetAGWFilter_name, ::Ice::Normal, __ctx);
        __os->write(filterNumber);
        __os->write(side);
        __os->endWriteEncaps();
        return __send();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __releaseCallback(__ex);
        return false;
    }
}

void
lbto::AMI_IIFServer_SetAGWFilter::__response(bool __ok)
{
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
            return;
        }
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __finished(__ex);
        return;
    }
    ice_response(__ret);
    __releaseCallback();
}

::lbto::result
IceProxy::lbto::IIFServer::Authorize(const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__Authorize_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->Authorize(__ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::Authorize_async(const ::lbto::AMI_IIFServer_AuthorizePtr& __cb)
{
    return __cb->__invoke(this, 0);
}

bool
IceProxy::lbto::IIFServer::Authorize_async(const ::lbto::AMI_IIFServer_AuthorizePtr& __cb, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::StepFocus(::Ice::Double relPos, const ::std::string& ope, const ::std::string& side, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__StepFocus_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->StepFocus(relPos, ope, side, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::StepFocus_async(const ::lbto::AMI_IIFServer_StepFocusPtr& __cb, ::Ice::Double relPos, const ::std::string& ope, const ::std::string& side)
{
    return __cb->__invoke(this, relPos, ope, side, 0);
}

bool
IceProxy::lbto::IIFServer::StepFocus_async(const ::lbto::AMI_IIFServer_StepFocusPtr& __cb, ::Ice::Double relPos, const ::std::string& ope, const ::std::string& side, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, relPos, ope, side, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::MoveFocus(::Ice::Double absPos, const ::std::string& ope, const ::std::string& side, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__MoveFocus_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->MoveFocus(absPos, ope, side, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::MoveFocus_async(const ::lbto::AMI_IIFServer_MoveFocusPtr& __cb, ::Ice::Double absPos, const ::std::string& ope, const ::std::string& side)
{
    return __cb->__invoke(this, absPos, ope, side, 0);
}

bool
IceProxy::lbto::IIFServer::MoveFocus_async(const ::lbto::AMI_IIFServer_MoveFocusPtr& __cb, ::Ice::Double absPos, const ::std::string& ope, const ::std::string& side, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, absPos, ope, side, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::OffsetPointing(::Ice::Double rotangle, ::Ice::Double offx, ::Ice::Double offy, const ::std::string& coordsys, const ::std::string& ope, const ::std::string& np, const ::std::string& movetype, const ::std::string& side, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__OffsetPointing_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->OffsetPointing(rotangle, offx, offy, coordsys, ope, np, movetype, side, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::OffsetPointing_async(const ::lbto::AMI_IIFServer_OffsetPointingPtr& __cb, ::Ice::Double rotangle, ::Ice::Double offx, ::Ice::Double offy, const ::std::string& coordsys, const ::std::string& ope, const ::std::string& np, const ::std::string& movetype, const ::std::string& side)
{
    return __cb->__invoke(this, rotangle, offx, offy, coordsys, ope, np, movetype, side, 0);
}

bool
IceProxy::lbto::IIFServer::OffsetPointing_async(const ::lbto::AMI_IIFServer_OffsetPointingPtr& __cb, ::Ice::Double rotangle, ::Ice::Double offx, ::Ice::Double offy, const ::std::string& coordsys, const ::std::string& ope, const ::std::string& np, const ::std::string& movetype, const ::std::string& side, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, rotangle, offx, offy, coordsys, ope, np, movetype, side, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::OffsetGuiding(::Ice::Double rotangle, ::Ice::Double offx, ::Ice::Double offy, const ::std::string& coordsys, const ::std::string& side, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__OffsetGuiding_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->OffsetGuiding(rotangle, offx, offy, coordsys, side, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::OffsetGuiding_async(const ::lbto::AMI_IIFServer_OffsetGuidingPtr& __cb, ::Ice::Double rotangle, ::Ice::Double offx, ::Ice::Double offy, const ::std::string& coordsys, const ::std::string& side)
{
    return __cb->__invoke(this, rotangle, offx, offy, coordsys, side, 0);
}

bool
IceProxy::lbto::IIFServer::OffsetGuiding_async(const ::lbto::AMI_IIFServer_OffsetGuidingPtr& __cb, ::Ice::Double rotangle, ::Ice::Double offx, ::Ice::Double offy, const ::std::string& coordsys, const ::std::string& side, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, rotangle, offx, offy, coordsys, side, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::UpdatePointingReference(const ::std::string& offsetcoord, const ::std::string& side, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__UpdatePointingReference_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->UpdatePointingReference(offsetcoord, side, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::UpdatePointingReference_async(const ::lbto::AMI_IIFServer_UpdatePointingReferencePtr& __cb, const ::std::string& offsetcoord, const ::std::string& side)
{
    return __cb->__invoke(this, offsetcoord, side, 0);
}

bool
IceProxy::lbto::IIFServer::UpdatePointingReference_async(const ::lbto::AMI_IIFServer_UpdatePointingReferencePtr& __cb, const ::std::string& offsetcoord, const ::std::string& side, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, offsetcoord, side, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::TipTilt(::Ice::Double xrot, ::Ice::Double yrot, const ::std::string& ope, const ::std::string& side, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__TipTilt_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->TipTilt(xrot, yrot, ope, side, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::TipTilt_async(const ::lbto::AMI_IIFServer_TipTiltPtr& __cb, ::Ice::Double xrot, ::Ice::Double yrot, const ::std::string& ope, const ::std::string& side)
{
    return __cb->__invoke(this, xrot, yrot, ope, side, 0);
}

bool
IceProxy::lbto::IIFServer::TipTilt_async(const ::lbto::AMI_IIFServer_TipTiltPtr& __cb, ::Ice::Double xrot, ::Ice::Double yrot, const ::std::string& ope, const ::std::string& side, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, xrot, yrot, ope, side, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::SetPMTerm(const ::std::string& name, ::Ice::Double value, const ::std::string& side, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__SetPMTerm_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->SetPMTerm(name, value, side, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::SetPMTerm_async(const ::lbto::AMI_IIFServer_SetPMTermPtr& __cb, const ::std::string& name, ::Ice::Double value, const ::std::string& side)
{
    return __cb->__invoke(this, name, value, side, 0);
}

bool
IceProxy::lbto::IIFServer::SetPMTerm_async(const ::lbto::AMI_IIFServer_SetPMTermPtr& __cb, const ::std::string& name, ::Ice::Double value, const ::std::string& side, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, name, value, side, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::MoveXYZ(::Ice::Double xmov, ::Ice::Double ymov, ::Ice::Double zmov, const ::std::string& side, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__MoveXYZ_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->MoveXYZ(xmov, ymov, zmov, side, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::MoveXYZ_async(const ::lbto::AMI_IIFServer_MoveXYZPtr& __cb, ::Ice::Double xmov, ::Ice::Double ymov, ::Ice::Double zmov, const ::std::string& side)
{
    return __cb->__invoke(this, xmov, ymov, zmov, side, 0);
}

bool
IceProxy::lbto::IIFServer::MoveXYZ_async(const ::lbto::AMI_IIFServer_MoveXYZPtr& __cb, ::Ice::Double xmov, ::Ice::Double ymov, ::Ice::Double zmov, const ::std::string& side, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, xmov, ymov, zmov, side, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::MoveXY(::Ice::Double xmov, ::Ice::Double ymov, const ::std::string& ope, const ::std::string& side, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__MoveXY_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->MoveXY(xmov, ymov, ope, side, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::MoveXY_async(const ::lbto::AMI_IIFServer_MoveXYPtr& __cb, ::Ice::Double xmov, ::Ice::Double ymov, const ::std::string& ope, const ::std::string& side)
{
    return __cb->__invoke(this, xmov, ymov, ope, side, 0);
}

bool
IceProxy::lbto::IIFServer::MoveXY_async(const ::lbto::AMI_IIFServer_MoveXYPtr& __cb, ::Ice::Double xmov, ::Ice::Double ymov, const ::std::string& ope, const ::std::string& side, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, xmov, ymov, ope, side, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::Move(::Ice::Double x, ::Ice::Double y, ::Ice::Double z, ::Ice::Double rx, ::Ice::Double ry, ::Ice::Double rz, ::Ice::Int dflag, const ::std::string& movetype, const ::std::string& OPE, ::Ice::Int time, const ::std::string& side, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__Move_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->Move(x, y, z, rx, ry, rz, dflag, movetype, OPE, time, side, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::Move_async(const ::lbto::AMI_IIFServer_MovePtr& __cb, ::Ice::Double x, ::Ice::Double y, ::Ice::Double z, ::Ice::Double rx, ::Ice::Double ry, ::Ice::Double rz, ::Ice::Int dflag, const ::std::string& movetype, const ::std::string& OPE, ::Ice::Int time, const ::std::string& side)
{
    return __cb->__invoke(this, x, y, z, rx, ry, rz, dflag, movetype, OPE, time, side, 0);
}

bool
IceProxy::lbto::IIFServer::Move_async(const ::lbto::AMI_IIFServer_MovePtr& __cb, ::Ice::Double x, ::Ice::Double y, ::Ice::Double z, ::Ice::Double rx, ::Ice::Double ry, ::Ice::Double rz, ::Ice::Int dflag, const ::std::string& movetype, const ::std::string& OPE, ::Ice::Int time, const ::std::string& side, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, x, y, z, rx, ry, rz, dflag, movetype, OPE, time, side, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::SetTarget(::Ice::Double coord1, ::Ice::Double coord2, const ::std::string& system, ::Ice::Double epoch, ::Ice::Float wavelength, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__SetTarget_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->SetTarget(coord1, coord2, system, epoch, wavelength, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::SetTarget_async(const ::lbto::AMI_IIFServer_SetTargetPtr& __cb, ::Ice::Double coord1, ::Ice::Double coord2, const ::std::string& system, ::Ice::Double epoch, ::Ice::Float wavelength)
{
    return __cb->__invoke(this, coord1, coord2, system, epoch, wavelength, 0);
}

bool
IceProxy::lbto::IIFServer::SetTarget_async(const ::lbto::AMI_IIFServer_SetTargetPtr& __cb, ::Ice::Double coord1, ::Ice::Double coord2, const ::std::string& system, ::Ice::Double epoch, ::Ice::Float wavelength, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, coord1, coord2, system, epoch, wavelength, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::SetStars(const ::lbto::SeqPos& stars, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__SetStars_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->SetStars(stars, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::SetStars_async(const ::lbto::AMI_IIFServer_SetStarsPtr& __cb, const ::lbto::SeqPos& stars)
{
    return __cb->__invoke(this, stars, 0);
}

bool
IceProxy::lbto::IIFServer::SetStars_async(const ::lbto::AMI_IIFServer_SetStarsPtr& __cb, const ::lbto::SeqPos& stars, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, stars, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::SetStarsNew(const ::lbto::SeqNewPos& stars, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__SetStarsNew_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->SetStarsNew(stars, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::SetStarsNew_async(const ::lbto::AMI_IIFServer_SetStarsNewPtr& __cb, const ::lbto::SeqNewPos& stars)
{
    return __cb->__invoke(this, stars, 0);
}

bool
IceProxy::lbto::IIFServer::SetStarsNew_async(const ::lbto::AMI_IIFServer_SetStarsNewPtr& __cb, const ::lbto::SeqNewPos& stars, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, stars, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::ClearStars(const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__ClearStars_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->ClearStars(__ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::ClearStars_async(const ::lbto::AMI_IIFServer_ClearStarsPtr& __cb)
{
    return __cb->__invoke(this, 0);
}

bool
IceProxy::lbto::IIFServer::ClearStars_async(const ::lbto::AMI_IIFServer_ClearStarsPtr& __cb, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::SetHotspot(::Ice::Double coord1, ::Ice::Double coord2, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__SetHotspot_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->SetHotspot(coord1, coord2, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::SetHotspot_async(const ::lbto::AMI_IIFServer_SetHotspotPtr& __cb, ::Ice::Double coord1, ::Ice::Double coord2)
{
    return __cb->__invoke(this, coord1, coord2, 0);
}

bool
IceProxy::lbto::IIFServer::SetHotspot_async(const ::lbto::AMI_IIFServer_SetHotspotPtr& __cb, ::Ice::Double coord1, ::Ice::Double coord2, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, coord1, coord2, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::ClearHotspot(const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__ClearHotspot_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->ClearHotspot(__ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::ClearHotspot_async(const ::lbto::AMI_IIFServer_ClearHotspotPtr& __cb)
{
    return __cb->__invoke(this, 0);
}

bool
IceProxy::lbto::IIFServer::ClearHotspot_async(const ::lbto::AMI_IIFServer_ClearHotspotPtr& __cb, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::SetOffset(::Ice::Double coord1, ::Ice::Double coord2, const ::std::string& system, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__SetOffset_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->SetOffset(coord1, coord2, system, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::SetOffset_async(const ::lbto::AMI_IIFServer_SetOffsetPtr& __cb, ::Ice::Double coord1, ::Ice::Double coord2, const ::std::string& system)
{
    return __cb->__invoke(this, coord1, coord2, system, 0);
}

bool
IceProxy::lbto::IIFServer::SetOffset_async(const ::lbto::AMI_IIFServer_SetOffsetPtr& __cb, ::Ice::Double coord1, ::Ice::Double coord2, const ::std::string& system, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, coord1, coord2, system, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::ClearOffset(const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__ClearOffset_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->ClearOffset(__ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::ClearOffset_async(const ::lbto::AMI_IIFServer_ClearOffsetPtr& __cb)
{
    return __cb->__invoke(this, 0);
}

bool
IceProxy::lbto::IIFServer::ClearOffset_async(const ::lbto::AMI_IIFServer_ClearOffsetPtr& __cb, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::SetNonSidereal(const ::lbto::nonsidereal& target, bool override, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__SetNonSidereal_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->SetNonSidereal(target, override, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::SetNonSidereal_async(const ::lbto::AMI_IIFServer_SetNonSiderealPtr& __cb, const ::lbto::nonsidereal& target, bool override)
{
    return __cb->__invoke(this, target, override, 0);
}

bool
IceProxy::lbto::IIFServer::SetNonSidereal_async(const ::lbto::AMI_IIFServer_SetNonSiderealPtr& __cb, const ::lbto::nonsidereal& target, bool override, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, target, override, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::ClearNonSidereal(bool override, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__ClearNonSidereal_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->ClearNonSidereal(override, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::ClearNonSidereal_async(const ::lbto::AMI_IIFServer_ClearNonSiderealPtr& __cb, bool override)
{
    return __cb->__invoke(this, override, 0);
}

bool
IceProxy::lbto::IIFServer::ClearNonSidereal_async(const ::lbto::AMI_IIFServer_ClearNonSiderealPtr& __cb, bool override, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, override, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::UpdateNonSiderealTarget(::Ice::Double RArate, ::Ice::Double DECrate, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__UpdateNonSiderealTarget_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->UpdateNonSiderealTarget(RArate, DECrate, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::UpdateNonSiderealTarget_async(const ::lbto::AMI_IIFServer_UpdateNonSiderealTargetPtr& __cb, ::Ice::Double RArate, ::Ice::Double DECrate)
{
    return __cb->__invoke(this, RArate, DECrate, 0);
}

bool
IceProxy::lbto::IIFServer::UpdateNonSiderealTarget_async(const ::lbto::AMI_IIFServer_UpdateNonSiderealTargetPtr& __cb, ::Ice::Double RArate, ::Ice::Double DECrate, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, RArate, DECrate, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::PresetTelescope(::Ice::Double rotangle, const ::std::string& rotmode, const ::std::string& mode, const ::std::string& side, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__PresetTelescope_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->PresetTelescope(rotangle, rotmode, mode, side, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::PresetTelescope_async(const ::lbto::AMI_IIFServer_PresetTelescopePtr& __cb, ::Ice::Double rotangle, const ::std::string& rotmode, const ::std::string& mode, const ::std::string& side)
{
    return __cb->__invoke(this, rotangle, rotmode, mode, side, 0);
}

bool
IceProxy::lbto::IIFServer::PresetTelescope_async(const ::lbto::AMI_IIFServer_PresetTelescopePtr& __cb, ::Ice::Double rotangle, const ::std::string& rotmode, const ::std::string& mode, const ::std::string& side, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, rotangle, rotmode, mode, side, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::BinocularControl(const ::std::string& flag, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__BinocularControl_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->BinocularControl(flag, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::BinocularControl_async(const ::lbto::AMI_IIFServer_BinocularControlPtr& __cb, const ::std::string& flag)
{
    return __cb->__invoke(this, flag, 0);
}

bool
IceProxy::lbto::IIFServer::BinocularControl_async(const ::lbto::AMI_IIFServer_BinocularControlPtr& __cb, const ::std::string& flag, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, flag, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::GetParameter(const ::lbto::SeqDD& list, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__GetParameter_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->GetParameter(list, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::GetParameter_async(const ::lbto::AMI_IIFServer_GetParameterPtr& __cb, const ::lbto::SeqDD& list)
{
    return __cb->__invoke(this, list, 0);
}

bool
IceProxy::lbto::IIFServer::GetParameter_async(const ::lbto::AMI_IIFServer_GetParameterPtr& __cb, const ::lbto::SeqDD& list, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, list, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::SetParameter(const ::lbto::SeqDD& list, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__SetParameter_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->SetParameter(list, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::SetParameter_async(const ::lbto::AMI_IIFServer_SetParameterPtr& __cb, const ::lbto::SeqDD& list)
{
    return __cb->__invoke(this, list, 0);
}

bool
IceProxy::lbto::IIFServer::SetParameter_async(const ::lbto::AMI_IIFServer_SetParameterPtr& __cb, const ::lbto::SeqDD& list, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, list, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::RotateCommon(::Ice::Double z, ::Ice::Double angle, ::Ice::Double direction, const ::std::string& side, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__RotateCommon_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->RotateCommon(z, angle, direction, side, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::RotateCommon_async(const ::lbto::AMI_IIFServer_RotateCommonPtr& __cb, ::Ice::Double z, ::Ice::Double angle, ::Ice::Double direction, const ::std::string& side)
{
    return __cb->__invoke(this, z, angle, direction, side, 0);
}

bool
IceProxy::lbto::IIFServer::RotateCommon_async(const ::lbto::AMI_IIFServer_RotateCommonPtr& __cb, ::Ice::Double z, ::Ice::Double angle, ::Ice::Double direction, const ::std::string& side, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, z, angle, direction, side, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::RotateZ(::Ice::Double angle, const ::std::string& movetype, const ::std::string& side, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__RotateZ_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->RotateZ(angle, movetype, side, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::RotateZ_async(const ::lbto::AMI_IIFServer_RotateZPtr& __cb, ::Ice::Double angle, const ::std::string& movetype, const ::std::string& side)
{
    return __cb->__invoke(this, angle, movetype, side, 0);
}

bool
IceProxy::lbto::IIFServer::RotateZ_async(const ::lbto::AMI_IIFServer_RotateZPtr& __cb, ::Ice::Double angle, const ::std::string& movetype, const ::std::string& side, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, angle, movetype, side, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::RotatePrimary(::Ice::Double distance, ::Ice::Double angle, ::Ice::Double direction, const ::std::string& side, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__RotatePrimary_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->RotatePrimary(distance, angle, direction, side, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::RotatePrimary_async(const ::lbto::AMI_IIFServer_RotatePrimaryPtr& __cb, ::Ice::Double distance, ::Ice::Double angle, ::Ice::Double direction, const ::std::string& side)
{
    return __cb->__invoke(this, distance, angle, direction, side, 0);
}

bool
IceProxy::lbto::IIFServer::RotatePrimary_async(const ::lbto::AMI_IIFServer_RotatePrimaryPtr& __cb, ::Ice::Double distance, ::Ice::Double angle, ::Ice::Double direction, const ::std::string& side, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, distance, angle, direction, side, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::SendWavefront(const ::lbto::SeqWF& wfs, const ::std::string& OPE, const ::std::string& side, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__SendWavefront_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->SendWavefront(wfs, OPE, side, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::SendWavefront_async(const ::lbto::AMI_IIFServer_SendWavefrontPtr& __cb, const ::lbto::SeqWF& wfs, const ::std::string& OPE, const ::std::string& side)
{
    return __cb->__invoke(this, wfs, OPE, side, 0);
}

bool
IceProxy::lbto::IIFServer::SendWavefront_async(const ::lbto::AMI_IIFServer_SendWavefrontPtr& __cb, const ::lbto::SeqWF& wfs, const ::std::string& OPE, const ::std::string& side, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, wfs, OPE, side, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::UpdateTargetWavelength(::Ice::Float wavelength, const ::std::string& side, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__UpdateTargetWavelength_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->UpdateTargetWavelength(wavelength, side, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::UpdateTargetWavelength_async(const ::lbto::AMI_IIFServer_UpdateTargetWavelengthPtr& __cb, ::Ice::Float wavelength, const ::std::string& side)
{
    return __cb->__invoke(this, wavelength, side, 0);
}

bool
IceProxy::lbto::IIFServer::UpdateTargetWavelength_async(const ::lbto::AMI_IIFServer_UpdateTargetWavelengthPtr& __cb, ::Ice::Float wavelength, const ::std::string& side, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, wavelength, side, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::LogEvent(const ::std::string& event, const ::std::string& description, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__LogEvent_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->LogEvent(event, description, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::LogEvent_async(const ::lbto::AMI_IIFServer_LogEventPtr& __cb, const ::std::string& event, const ::std::string& description)
{
    return __cb->__invoke(this, event, description, 0);
}

bool
IceProxy::lbto::IIFServer::LogEvent_async(const ::lbto::AMI_IIFServer_LogEventPtr& __cb, const ::std::string& event, const ::std::string& description, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, event, description, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::GetRotatorTrajectory(::Ice::Double seconds, ::Ice::Double interval, ::Ice::Double start, const ::std::string& side, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__GetRotatorTrajectory_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->GetRotatorTrajectory(seconds, interval, start, side, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::GetRotatorTrajectory_async(const ::lbto::AMI_IIFServer_GetRotatorTrajectoryPtr& __cb, ::Ice::Double seconds, ::Ice::Double interval, ::Ice::Double start, const ::std::string& side)
{
    return __cb->__invoke(this, seconds, interval, start, side, 0);
}

bool
IceProxy::lbto::IIFServer::GetRotatorTrajectory_async(const ::lbto::AMI_IIFServer_GetRotatorTrajectoryPtr& __cb, ::Ice::Double seconds, ::Ice::Double interval, ::Ice::Double start, const ::std::string& side, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, seconds, interval, start, side, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::GetRotatorPolynomials(::Ice::Double start, ::Ice::Int count, ::Ice::Double interval, ::Ice::Int order, const ::std::string& side, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__GetRotatorPolynomials_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->GetRotatorPolynomials(start, count, interval, order, side, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::GetRotatorPolynomials_async(const ::lbto::AMI_IIFServer_GetRotatorPolynomialsPtr& __cb, ::Ice::Double start, ::Ice::Int count, ::Ice::Double interval, ::Ice::Int order, const ::std::string& side)
{
    return __cb->__invoke(this, start, count, interval, order, side, 0);
}

bool
IceProxy::lbto::IIFServer::GetRotatorPolynomials_async(const ::lbto::AMI_IIFServer_GetRotatorPolynomialsPtr& __cb, ::Ice::Double start, ::Ice::Int count, ::Ice::Double interval, ::Ice::Int order, const ::std::string& side, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, start, count, interval, order, side, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::Standby(::Ice::Int level, const ::std::string& side, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__Standby_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->Standby(level, side, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::Standby_async(const ::lbto::AMI_IIFServer_StandbyPtr& __cb, ::Ice::Int level, const ::std::string& side)
{
    return __cb->__invoke(this, level, side, 0);
}

bool
IceProxy::lbto::IIFServer::Standby_async(const ::lbto::AMI_IIFServer_StandbyPtr& __cb, ::Ice::Int level, const ::std::string& side, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, level, side, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::MaximizeWrapTime(bool azflag, bool rotflag, const ::std::string& side, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__MaximizeWrapTime_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->MaximizeWrapTime(azflag, rotflag, side, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::MaximizeWrapTime_async(const ::lbto::AMI_IIFServer_MaximizeWrapTimePtr& __cb, bool azflag, bool rotflag, const ::std::string& side)
{
    return __cb->__invoke(this, azflag, rotflag, side, 0);
}

bool
IceProxy::lbto::IIFServer::MaximizeWrapTime_async(const ::lbto::AMI_IIFServer_MaximizeWrapTimePtr& __cb, bool azflag, bool rotflag, const ::std::string& side, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, azflag, rotflag, side, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::PauseGuiding(const ::std::string& side, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__PauseGuiding_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->PauseGuiding(side, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::PauseGuiding_async(const ::lbto::AMI_IIFServer_PauseGuidingPtr& __cb, const ::std::string& side)
{
    return __cb->__invoke(this, side, 0);
}

bool
IceProxy::lbto::IIFServer::PauseGuiding_async(const ::lbto::AMI_IIFServer_PauseGuidingPtr& __cb, const ::std::string& side, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, side, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::ResumeGuiding(const ::std::string& side, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__ResumeGuiding_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->ResumeGuiding(side, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::ResumeGuiding_async(const ::lbto::AMI_IIFServer_ResumeGuidingPtr& __cb, const ::std::string& side)
{
    return __cb->__invoke(this, side, 0);
}

bool
IceProxy::lbto::IIFServer::ResumeGuiding_async(const ::lbto::AMI_IIFServer_ResumeGuidingPtr& __cb, const ::std::string& side, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, side, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::SetGuidingHotspot(::Ice::Double coord1, ::Ice::Double coord2, const ::std::string& movetype, const ::std::string& side, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__SetGuidingHotspot_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->SetGuidingHotspot(coord1, coord2, movetype, side, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::SetGuidingHotspot_async(const ::lbto::AMI_IIFServer_SetGuidingHotspotPtr& __cb, ::Ice::Double coord1, ::Ice::Double coord2, const ::std::string& movetype, const ::std::string& side)
{
    return __cb->__invoke(this, coord1, coord2, movetype, side, 0);
}

bool
IceProxy::lbto::IIFServer::SetGuidingHotspot_async(const ::lbto::AMI_IIFServer_SetGuidingHotspotPtr& __cb, ::Ice::Double coord1, ::Ice::Double coord2, const ::std::string& movetype, const ::std::string& side, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, coord1, coord2, movetype, side, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::RotReady(bool enable, const ::std::string& side, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__RotReady_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->RotReady(enable, side, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::RotReady_async(const ::lbto::AMI_IIFServer_RotReadyPtr& __cb, bool enable, const ::std::string& side)
{
    return __cb->__invoke(this, enable, side, 0);
}

bool
IceProxy::lbto::IIFServer::RotReady_async(const ::lbto::AMI_IIFServer_RotReadyPtr& __cb, bool enable, const ::std::string& side, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, enable, side, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::RotHold(const ::std::string& side, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__RotHold_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->RotHold(side, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::RotHold_async(const ::lbto::AMI_IIFServer_RotHoldPtr& __cb, const ::std::string& side)
{
    return __cb->__invoke(this, side, 0);
}

bool
IceProxy::lbto::IIFServer::RotHold_async(const ::lbto::AMI_IIFServer_RotHoldPtr& __cb, const ::std::string& side, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, side, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::RotTrack(const ::std::string& side, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__RotTrack_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->RotTrack(side, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::RotTrack_async(const ::lbto::AMI_IIFServer_RotTrackPtr& __cb, const ::std::string& side)
{
    return __cb->__invoke(this, side, 0);
}

bool
IceProxy::lbto::IIFServer::RotTrack_async(const ::lbto::AMI_IIFServer_RotTrackPtr& __cb, const ::std::string& side, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, side, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::RotServicePosition(::Ice::Double angle, const ::std::string& side, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__RotServicePosition_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->RotServicePosition(angle, side, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::RotServicePosition_async(const ::lbto::AMI_IIFServer_RotServicePositionPtr& __cb, ::Ice::Double angle, const ::std::string& side)
{
    return __cb->__invoke(this, angle, side, 0);
}

bool
IceProxy::lbto::IIFServer::RotServicePosition_async(const ::lbto::AMI_IIFServer_RotServicePositionPtr& __cb, ::Ice::Double angle, const ::std::string& side, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, angle, side, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::PauseAO(const ::std::string& side, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__PauseAO_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->PauseAO(side, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::PauseAO_async(const ::lbto::AMI_IIFServer_PauseAOPtr& __cb, const ::std::string& side)
{
    return __cb->__invoke(this, side, 0);
}

bool
IceProxy::lbto::IIFServer::PauseAO_async(const ::lbto::AMI_IIFServer_PauseAOPtr& __cb, const ::std::string& side, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, side, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::ResumeAO(const ::std::string& side, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__ResumeAO_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->ResumeAO(side, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::ResumeAO_async(const ::lbto::AMI_IIFServer_ResumeAOPtr& __cb, const ::std::string& side)
{
    return __cb->__invoke(this, side, 0);
}

bool
IceProxy::lbto::IIFServer::ResumeAO_async(const ::lbto::AMI_IIFServer_ResumeAOPtr& __cb, const ::std::string& side, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, side, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::StartAO(const ::std::string& side, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__StartAO_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->StartAO(side, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::StartAO_async(const ::lbto::AMI_IIFServer_StartAOPtr& __cb, const ::std::string& side)
{
    return __cb->__invoke(this, side, 0);
}

bool
IceProxy::lbto::IIFServer::StartAO_async(const ::lbto::AMI_IIFServer_StartAOPtr& __cb, const ::std::string& side, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, side, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::StopAO(const ::std::string& reason, const ::std::string& side, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__StopAO_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->StopAO(reason, side, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::StopAO_async(const ::lbto::AMI_IIFServer_StopAOPtr& __cb, const ::std::string& reason, const ::std::string& side)
{
    return __cb->__invoke(this, reason, side, 0);
}

bool
IceProxy::lbto::IIFServer::StopAO_async(const ::lbto::AMI_IIFServer_StopAOPtr& __cb, const ::std::string& reason, const ::std::string& side, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, reason, side, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::AcquireRefAO(bool repointFlag, const ::std::string& side, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__AcquireRefAO_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->AcquireRefAO(repointFlag, side, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::AcquireRefAO_async(const ::lbto::AMI_IIFServer_AcquireRefAOPtr& __cb, bool repointFlag, const ::std::string& side)
{
    return __cb->__invoke(this, repointFlag, side, 0);
}

bool
IceProxy::lbto::IIFServer::AcquireRefAO_async(const ::lbto::AMI_IIFServer_AcquireRefAOPtr& __cb, bool repointFlag, const ::std::string& side, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, repointFlag, side, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::CheckRefAO(const ::std::string& side, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__CheckRefAO_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->CheckRefAO(side, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::CheckRefAO_async(const ::lbto::AMI_IIFServer_CheckRefAOPtr& __cb, const ::std::string& side)
{
    return __cb->__invoke(this, side, 0);
}

bool
IceProxy::lbto::IIFServer::CheckRefAO_async(const ::lbto::AMI_IIFServer_CheckRefAOPtr& __cb, const ::std::string& side, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, side, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::RefineAO(const ::std::string& method, const ::std::string& side, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__RefineAO_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->RefineAO(method, side, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::RefineAO_async(const ::lbto::AMI_IIFServer_RefineAOPtr& __cb, const ::std::string& method, const ::std::string& side)
{
    return __cb->__invoke(this, method, side, 0);
}

bool
IceProxy::lbto::IIFServer::RefineAO_async(const ::lbto::AMI_IIFServer_RefineAOPtr& __cb, const ::std::string& method, const ::std::string& side, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, method, side, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::RunAO(const ::std::string& type, const ::std::string& side, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__RunAO_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->RunAO(type, side, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::RunAO_async(const ::lbto::AMI_IIFServer_RunAOPtr& __cb, const ::std::string& type, const ::std::string& side)
{
    return __cb->__invoke(this, type, side, 0);
}

bool
IceProxy::lbto::IIFServer::RunAO_async(const ::lbto::AMI_IIFServer_RunAOPtr& __cb, const ::std::string& type, const ::std::string& side, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, type, side, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::PresetFlatAO(const ::std::string& flat, const ::std::string& side, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__PresetFlatAO_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->PresetFlatAO(flat, side, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::PresetFlatAO_async(const ::lbto::AMI_IIFServer_PresetFlatAOPtr& __cb, const ::std::string& flat, const ::std::string& side)
{
    return __cb->__invoke(this, flat, side, 0);
}

bool
IceProxy::lbto::IIFServer::PresetFlatAO_async(const ::lbto::AMI_IIFServer_PresetFlatAOPtr& __cb, const ::std::string& flat, const ::std::string& side, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, flat, side, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::CorrectModesAO(const ::lbto::SeqModes& modes, const ::std::string& side, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__CorrectModesAO_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->CorrectModesAO(modes, side, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::CorrectModesAO_async(const ::lbto::AMI_IIFServer_CorrectModesAOPtr& __cb, const ::lbto::SeqModes& modes, const ::std::string& side)
{
    return __cb->__invoke(this, modes, side, 0);
}

bool
IceProxy::lbto::IIFServer::CorrectModesAO_async(const ::lbto::AMI_IIFServer_CorrectModesAOPtr& __cb, const ::lbto::SeqModes& modes, const ::std::string& side, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, modes, side, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::ModifyAO(::Ice::Int NModes, ::Ice::Double Freq, ::Ice::Int Nbins, ::Ice::Double TTMod, const ::std::string& F1spec, const ::std::string& F2spec, const ::std::string& side, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__ModifyAO_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->ModifyAO(NModes, Freq, Nbins, TTMod, F1spec, F2spec, side, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::ModifyAO_async(const ::lbto::AMI_IIFServer_ModifyAOPtr& __cb, ::Ice::Int NModes, ::Ice::Double Freq, ::Ice::Int Nbins, ::Ice::Double TTMod, const ::std::string& F1spec, const ::std::string& F2spec, const ::std::string& side)
{
    return __cb->__invoke(this, NModes, Freq, Nbins, TTMod, F1spec, F2spec, side, 0);
}

bool
IceProxy::lbto::IIFServer::ModifyAO_async(const ::lbto::AMI_IIFServer_ModifyAOPtr& __cb, ::Ice::Int NModes, ::Ice::Double Freq, ::Ice::Int Nbins, ::Ice::Double TTMod, const ::std::string& F1spec, const ::std::string& F2spec, const ::std::string& side, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, NModes, Freq, Nbins, TTMod, F1spec, F2spec, side, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::PresetAO(const ::std::string& AOMode, const ::std::string& side, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__PresetAO_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->PresetAO(AOMode, side, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::PresetAO_async(const ::lbto::AMI_IIFServer_PresetAOPtr& __cb, const ::std::string& AOMode, const ::std::string& side)
{
    return __cb->__invoke(this, AOMode, side, 0);
}

bool
IceProxy::lbto::IIFServer::PresetAO_async(const ::lbto::AMI_IIFServer_PresetAOPtr& __cb, const ::std::string& AOMode, const ::std::string& side, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, AOMode, side, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::OffsetXYAO(::Ice::Double DeltaX, ::Ice::Double DeltaY, const ::std::string& side, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__OffsetXYAO_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->OffsetXYAO(DeltaX, DeltaY, side, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::OffsetXYAO_async(const ::lbto::AMI_IIFServer_OffsetXYAOPtr& __cb, ::Ice::Double DeltaX, ::Ice::Double DeltaY, const ::std::string& side)
{
    return __cb->__invoke(this, DeltaX, DeltaY, side, 0);
}

bool
IceProxy::lbto::IIFServer::OffsetXYAO_async(const ::lbto::AMI_IIFServer_OffsetXYAOPtr& __cb, ::Ice::Double DeltaX, ::Ice::Double DeltaY, const ::std::string& side, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, DeltaX, DeltaY, side, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::OffsetZAO(::Ice::Double DeltaZ, const ::std::string& side, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__OffsetZAO_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->OffsetZAO(DeltaZ, side, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::OffsetZAO_async(const ::lbto::AMI_IIFServer_OffsetZAOPtr& __cb, ::Ice::Double DeltaZ, const ::std::string& side)
{
    return __cb->__invoke(this, DeltaZ, side, 0);
}

bool
IceProxy::lbto::IIFServer::OffsetZAO_async(const ::lbto::AMI_IIFServer_OffsetZAOPtr& __cb, ::Ice::Double DeltaZ, const ::std::string& side, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, DeltaZ, side, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::SetReference(const ::lbto::SeqPos& stars, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__SetReference_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->SetReference(stars, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::SetReference_async(const ::lbto::AMI_IIFServer_SetReferencePtr& __cb, const ::lbto::SeqPos& stars)
{
    return __cb->__invoke(this, stars, 0);
}

bool
IceProxy::lbto::IIFServer::SetReference_async(const ::lbto::AMI_IIFServer_SetReferencePtr& __cb, const ::lbto::SeqPos& stars, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, stars, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::SetReferenceNew(const ::lbto::SeqNewPos& stars, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__SetReferenceNew_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->SetReferenceNew(stars, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::SetReferenceNew_async(const ::lbto::AMI_IIFServer_SetReferenceNewPtr& __cb, const ::lbto::SeqNewPos& stars)
{
    return __cb->__invoke(this, stars, 0);
}

bool
IceProxy::lbto::IIFServer::SetReferenceNew_async(const ::lbto::AMI_IIFServer_SetReferenceNewPtr& __cb, const ::lbto::SeqNewPos& stars, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, stars, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::ClearReference(const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__ClearReference_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->ClearReference(__ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::ClearReference_async(const ::lbto::AMI_IIFServer_ClearReferencePtr& __cb)
{
    return __cb->__invoke(this, 0);
}

bool
IceProxy::lbto::IIFServer::ClearReference_async(const ::lbto::AMI_IIFServer_ClearReferencePtr& __cb, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::GetKFPCoordinates(const ::lbto::SeqPos& stars, const ::std::string& side, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__GetKFPCoordinates_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->GetKFPCoordinates(stars, side, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::GetKFPCoordinates_async(const ::lbto::AMI_IIFServer_GetKFPCoordinatesPtr& __cb, const ::lbto::SeqPos& stars, const ::std::string& side)
{
    return __cb->__invoke(this, stars, side, 0);
}

bool
IceProxy::lbto::IIFServer::GetKFPCoordinates_async(const ::lbto::AMI_IIFServer_GetKFPCoordinatesPtr& __cb, const ::lbto::SeqPos& stars, const ::std::string& side, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, stars, side, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::GetKFPCoordinatesNew(const ::lbto::SeqNewPos& stars, const ::std::string& side, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__GetKFPCoordinatesNew_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->GetKFPCoordinatesNew(stars, side, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::GetKFPCoordinatesNew_async(const ::lbto::AMI_IIFServer_GetKFPCoordinatesNewPtr& __cb, const ::lbto::SeqNewPos& stars, const ::std::string& side)
{
    return __cb->__invoke(this, stars, side, 0);
}

bool
IceProxy::lbto::IIFServer::GetKFPCoordinatesNew_async(const ::lbto::AMI_IIFServer_GetKFPCoordinatesNewPtr& __cb, const ::lbto::SeqNewPos& stars, const ::std::string& side, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, stars, side, &__ctx);
}

::lbto::result
IceProxy::lbto::IIFServer::SetAGWFilter(::Ice::Int filterNumber, const ::std::string& side, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __checkTwowayOnly(__lbto__IIFServer__SetAGWFilter_name);
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            return __del->SetAGWFilter(filterNumber, side, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

bool
IceProxy::lbto::IIFServer::SetAGWFilter_async(const ::lbto::AMI_IIFServer_SetAGWFilterPtr& __cb, ::Ice::Int filterNumber, const ::std::string& side)
{
    return __cb->__invoke(this, filterNumber, side, 0);
}

bool
IceProxy::lbto::IIFServer::SetAGWFilter_async(const ::lbto::AMI_IIFServer_SetAGWFilterPtr& __cb, ::Ice::Int filterNumber, const ::std::string& side, const ::Ice::Context& __ctx)
{
    return __cb->__invoke(this, filterNumber, side, &__ctx);
}

void
IceProxy::lbto::IIFServer::remove(const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
#if defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0600) // C++Builder 2009 compiler bug
            IceUtil::DummyBCC dummy;
#endif
            __delBase = __getDelegate(false);
            ::IceDelegate::lbto::IIFServer* __del = dynamic_cast< ::IceDelegate::lbto::IIFServer*>(__delBase.get());
            __del->remove(__ctx);
            return;
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex, 0);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, 0, __cnt);
        }
    }
}

const ::std::string&
IceProxy::lbto::IIFServer::ice_staticId()
{
    return ::lbto::IIFServer::ice_staticId();
}

::IceInternal::Handle< ::IceDelegateM::Ice::Object>
IceProxy::lbto::IIFServer::__createDelegateM()
{
    return ::IceInternal::Handle< ::IceDelegateM::Ice::Object>(new ::IceDelegateM::lbto::IIFServer);
}

::IceInternal::Handle< ::IceDelegateD::Ice::Object>
IceProxy::lbto::IIFServer::__createDelegateD()
{
    return ::IceInternal::Handle< ::IceDelegateD::Ice::Object>(new ::IceDelegateD::lbto::IIFServer);
}

::IceProxy::Ice::Object*
IceProxy::lbto::IIFServer::__newInstance() const
{
    return new IIFServer;
}

::lbto::result
IceDelegateM::lbto::IIFServer::Authorize(const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__Authorize_name, ::Ice::Normal, __context);
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::StepFocus(::Ice::Double relPos, const ::std::string& ope, const ::std::string& side, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__StepFocus_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(relPos);
        __os->write(ope);
        __os->write(side);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::MoveFocus(::Ice::Double absPos, const ::std::string& ope, const ::std::string& side, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__MoveFocus_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(absPos);
        __os->write(ope);
        __os->write(side);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::OffsetPointing(::Ice::Double rotangle, ::Ice::Double offx, ::Ice::Double offy, const ::std::string& coordsys, const ::std::string& ope, const ::std::string& np, const ::std::string& movetype, const ::std::string& side, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__OffsetPointing_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(rotangle);
        __os->write(offx);
        __os->write(offy);
        __os->write(coordsys);
        __os->write(ope);
        __os->write(np);
        __os->write(movetype);
        __os->write(side);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::OffsetGuiding(::Ice::Double rotangle, ::Ice::Double offx, ::Ice::Double offy, const ::std::string& coordsys, const ::std::string& side, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__OffsetGuiding_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(rotangle);
        __os->write(offx);
        __os->write(offy);
        __os->write(coordsys);
        __os->write(side);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::UpdatePointingReference(const ::std::string& offsetcoord, const ::std::string& side, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__UpdatePointingReference_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(offsetcoord);
        __os->write(side);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::TipTilt(::Ice::Double xrot, ::Ice::Double yrot, const ::std::string& ope, const ::std::string& side, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__TipTilt_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(xrot);
        __os->write(yrot);
        __os->write(ope);
        __os->write(side);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::SetPMTerm(const ::std::string& name, ::Ice::Double value, const ::std::string& side, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__SetPMTerm_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(name);
        __os->write(value);
        __os->write(side);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::MoveXYZ(::Ice::Double xmov, ::Ice::Double ymov, ::Ice::Double zmov, const ::std::string& side, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__MoveXYZ_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(xmov);
        __os->write(ymov);
        __os->write(zmov);
        __os->write(side);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::MoveXY(::Ice::Double xmov, ::Ice::Double ymov, const ::std::string& ope, const ::std::string& side, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__MoveXY_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(xmov);
        __os->write(ymov);
        __os->write(ope);
        __os->write(side);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::Move(::Ice::Double x, ::Ice::Double y, ::Ice::Double z, ::Ice::Double rx, ::Ice::Double ry, ::Ice::Double rz, ::Ice::Int dflag, const ::std::string& movetype, const ::std::string& OPE, ::Ice::Int time, const ::std::string& side, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__Move_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(x);
        __os->write(y);
        __os->write(z);
        __os->write(rx);
        __os->write(ry);
        __os->write(rz);
        __os->write(dflag);
        __os->write(movetype);
        __os->write(OPE);
        __os->write(time);
        __os->write(side);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::SetTarget(::Ice::Double coord1, ::Ice::Double coord2, const ::std::string& system, ::Ice::Double epoch, ::Ice::Float wavelength, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__SetTarget_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(coord1);
        __os->write(coord2);
        __os->write(system);
        __os->write(epoch);
        __os->write(wavelength);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::SetStars(const ::lbto::SeqPos& stars, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__SetStars_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        if(stars.size() == 0)
        {
            __os->writeSize(0);
        }
        else
        {
            ::lbto::__writeSeqPos(__os, &stars[0], &stars[0] + stars.size());
        }
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::SetStarsNew(const ::lbto::SeqNewPos& stars, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__SetStarsNew_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        if(stars.size() == 0)
        {
            __os->writeSize(0);
        }
        else
        {
            ::lbto::__writeSeqNewPos(__os, &stars[0], &stars[0] + stars.size());
        }
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::ClearStars(const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__ClearStars_name, ::Ice::Normal, __context);
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::SetHotspot(::Ice::Double coord1, ::Ice::Double coord2, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__SetHotspot_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(coord1);
        __os->write(coord2);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::ClearHotspot(const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__ClearHotspot_name, ::Ice::Normal, __context);
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::SetOffset(::Ice::Double coord1, ::Ice::Double coord2, const ::std::string& system, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__SetOffset_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(coord1);
        __os->write(coord2);
        __os->write(system);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::ClearOffset(const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__ClearOffset_name, ::Ice::Normal, __context);
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::SetNonSidereal(const ::lbto::nonsidereal& target, bool override, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__SetNonSidereal_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        target.__write(__os);
        __os->write(override);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::ClearNonSidereal(bool override, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__ClearNonSidereal_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(override);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::UpdateNonSiderealTarget(::Ice::Double RArate, ::Ice::Double DECrate, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__UpdateNonSiderealTarget_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(RArate);
        __os->write(DECrate);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::PresetTelescope(::Ice::Double rotangle, const ::std::string& rotmode, const ::std::string& mode, const ::std::string& side, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__PresetTelescope_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(rotangle);
        __os->write(rotmode);
        __os->write(mode);
        __os->write(side);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::BinocularControl(const ::std::string& flag, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__BinocularControl_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(flag);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::GetParameter(const ::lbto::SeqDD& list, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__GetParameter_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        if(list.size() == 0)
        {
            __os->writeSize(0);
        }
        else
        {
            ::lbto::__writeSeqDD(__os, &list[0], &list[0] + list.size());
        }
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::SetParameter(const ::lbto::SeqDD& list, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__SetParameter_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        if(list.size() == 0)
        {
            __os->writeSize(0);
        }
        else
        {
            ::lbto::__writeSeqDD(__os, &list[0], &list[0] + list.size());
        }
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::RotateCommon(::Ice::Double z, ::Ice::Double angle, ::Ice::Double direction, const ::std::string& side, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__RotateCommon_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(z);
        __os->write(angle);
        __os->write(direction);
        __os->write(side);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::RotateZ(::Ice::Double angle, const ::std::string& movetype, const ::std::string& side, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__RotateZ_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(angle);
        __os->write(movetype);
        __os->write(side);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::RotatePrimary(::Ice::Double distance, ::Ice::Double angle, ::Ice::Double direction, const ::std::string& side, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__RotatePrimary_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(distance);
        __os->write(angle);
        __os->write(direction);
        __os->write(side);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::SendWavefront(const ::lbto::SeqWF& wfs, const ::std::string& OPE, const ::std::string& side, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__SendWavefront_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        if(wfs.size() == 0)
        {
            __os->writeSize(0);
        }
        else
        {
            __os->write(&wfs[0], &wfs[0] + wfs.size());
        }
        __os->write(OPE);
        __os->write(side);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::UpdateTargetWavelength(::Ice::Float wavelength, const ::std::string& side, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__UpdateTargetWavelength_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(wavelength);
        __os->write(side);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::LogEvent(const ::std::string& event, const ::std::string& description, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__LogEvent_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(event);
        __os->write(description);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::GetRotatorTrajectory(::Ice::Double seconds, ::Ice::Double interval, ::Ice::Double start, const ::std::string& side, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__GetRotatorTrajectory_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(seconds);
        __os->write(interval);
        __os->write(start);
        __os->write(side);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::GetRotatorPolynomials(::Ice::Double start, ::Ice::Int count, ::Ice::Double interval, ::Ice::Int order, const ::std::string& side, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__GetRotatorPolynomials_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(start);
        __os->write(count);
        __os->write(interval);
        __os->write(order);
        __os->write(side);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::Standby(::Ice::Int level, const ::std::string& side, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__Standby_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(level);
        __os->write(side);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::MaximizeWrapTime(bool azflag, bool rotflag, const ::std::string& side, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__MaximizeWrapTime_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(azflag);
        __os->write(rotflag);
        __os->write(side);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::PauseGuiding(const ::std::string& side, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__PauseGuiding_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(side);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::ResumeGuiding(const ::std::string& side, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__ResumeGuiding_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(side);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::SetGuidingHotspot(::Ice::Double coord1, ::Ice::Double coord2, const ::std::string& movetype, const ::std::string& side, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__SetGuidingHotspot_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(coord1);
        __os->write(coord2);
        __os->write(movetype);
        __os->write(side);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::RotReady(bool enable, const ::std::string& side, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__RotReady_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(enable);
        __os->write(side);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::RotHold(const ::std::string& side, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__RotHold_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(side);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::RotTrack(const ::std::string& side, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__RotTrack_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(side);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::RotServicePosition(::Ice::Double angle, const ::std::string& side, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__RotServicePosition_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(angle);
        __os->write(side);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::PauseAO(const ::std::string& side, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__PauseAO_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(side);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::ResumeAO(const ::std::string& side, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__ResumeAO_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(side);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::StartAO(const ::std::string& side, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__StartAO_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(side);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::StopAO(const ::std::string& reason, const ::std::string& side, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__StopAO_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(reason);
        __os->write(side);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::AcquireRefAO(bool repointFlag, const ::std::string& side, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__AcquireRefAO_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(repointFlag);
        __os->write(side);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::CheckRefAO(const ::std::string& side, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__CheckRefAO_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(side);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::RefineAO(const ::std::string& method, const ::std::string& side, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__RefineAO_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(method);
        __os->write(side);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::RunAO(const ::std::string& type, const ::std::string& side, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__RunAO_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(type);
        __os->write(side);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::PresetFlatAO(const ::std::string& flat, const ::std::string& side, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__PresetFlatAO_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(flat);
        __os->write(side);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::CorrectModesAO(const ::lbto::SeqModes& modes, const ::std::string& side, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__CorrectModesAO_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        if(modes.size() == 0)
        {
            __os->writeSize(0);
        }
        else
        {
            __os->write(&modes[0], &modes[0] + modes.size());
        }
        __os->write(side);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::ModifyAO(::Ice::Int NModes, ::Ice::Double Freq, ::Ice::Int Nbins, ::Ice::Double TTMod, const ::std::string& F1spec, const ::std::string& F2spec, const ::std::string& side, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__ModifyAO_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(NModes);
        __os->write(Freq);
        __os->write(Nbins);
        __os->write(TTMod);
        __os->write(F1spec);
        __os->write(F2spec);
        __os->write(side);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::PresetAO(const ::std::string& AOMode, const ::std::string& side, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__PresetAO_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(AOMode);
        __os->write(side);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::OffsetXYAO(::Ice::Double DeltaX, ::Ice::Double DeltaY, const ::std::string& side, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__OffsetXYAO_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(DeltaX);
        __os->write(DeltaY);
        __os->write(side);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::OffsetZAO(::Ice::Double DeltaZ, const ::std::string& side, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__OffsetZAO_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(DeltaZ);
        __os->write(side);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::SetReference(const ::lbto::SeqPos& stars, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__SetReference_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        if(stars.size() == 0)
        {
            __os->writeSize(0);
        }
        else
        {
            ::lbto::__writeSeqPos(__os, &stars[0], &stars[0] + stars.size());
        }
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::SetReferenceNew(const ::lbto::SeqNewPos& stars, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__SetReferenceNew_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        if(stars.size() == 0)
        {
            __os->writeSize(0);
        }
        else
        {
            ::lbto::__writeSeqNewPos(__os, &stars[0], &stars[0] + stars.size());
        }
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::ClearReference(const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__ClearReference_name, ::Ice::Normal, __context);
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::GetKFPCoordinates(const ::lbto::SeqPos& stars, const ::std::string& side, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__GetKFPCoordinates_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        if(stars.size() == 0)
        {
            __os->writeSize(0);
        }
        else
        {
            ::lbto::__writeSeqPos(__os, &stars[0], &stars[0] + stars.size());
        }
        __os->write(side);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::GetKFPCoordinatesNew(const ::lbto::SeqNewPos& stars, const ::std::string& side, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__GetKFPCoordinatesNew_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        if(stars.size() == 0)
        {
            __os->writeSize(0);
        }
        else
        {
            ::lbto::__writeSeqNewPos(__os, &stars[0], &stars[0] + stars.size());
        }
        __os->write(side);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::lbto::result
IceDelegateM::lbto::IIFServer::SetAGWFilter(::Ice::Int filterNumber, const ::std::string& side, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__SetAGWFilter_name, ::Ice::Normal, __context);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        __os->write(filterNumber);
        __os->write(side);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    ::lbto::result __ret;
    try
    {
        if(!__ok)
        {
            try
            {
                __og.throwUserException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                throw __uue;
            }
        }
        ::IceInternal::BasicStream* __is = __og.is();
        __is->startReadEncaps();
        __ret.__read(__is);
        __is->endReadEncaps();
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

void
IceDelegateM::lbto::IIFServer::remove(const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__handler.get(), __lbto__IIFServer__remove_name, ::Ice::Normal, __context);
    bool __ok = __og.invoke();
    if(!__og.is()->b.empty())
    {
        try
        {
            if(!__ok)
            {
                try
                {
                    __og.throwUserException();
                }
                catch(const ::Ice::UserException& __ex)
                {
                    ::Ice::UnknownUserException __uue(__FILE__, __LINE__, __ex.ice_name());
                    throw __uue;
                }
            }
            __og.is()->skipEmptyEncaps();
        }
        catch(const ::Ice::LocalException& __ex)
        {
            throw ::IceInternal::LocalExceptionWrapper(__ex, false);
        }
    }
}

::lbto::result
IceDelegateD::lbto::IIFServer::Authorize(const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->Authorize(_current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__Authorize_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::StepFocus(::Ice::Double relPos, const ::std::string& ope, const ::std::string& side, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, ::Ice::Double relPos, const ::std::string& ope, const ::std::string& side, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_relPos(relPos),
            _m_ope(ope),
            _m_side(side)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->StepFocus(_m_relPos, _m_ope, _m_side, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        ::Ice::Double _m_relPos;
        const ::std::string& _m_ope;
        const ::std::string& _m_side;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__StepFocus_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, relPos, ope, side, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::MoveFocus(::Ice::Double absPos, const ::std::string& ope, const ::std::string& side, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, ::Ice::Double absPos, const ::std::string& ope, const ::std::string& side, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_absPos(absPos),
            _m_ope(ope),
            _m_side(side)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->MoveFocus(_m_absPos, _m_ope, _m_side, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        ::Ice::Double _m_absPos;
        const ::std::string& _m_ope;
        const ::std::string& _m_side;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__MoveFocus_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, absPos, ope, side, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::OffsetPointing(::Ice::Double rotangle, ::Ice::Double offx, ::Ice::Double offy, const ::std::string& coordsys, const ::std::string& ope, const ::std::string& np, const ::std::string& movetype, const ::std::string& side, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, ::Ice::Double rotangle, ::Ice::Double offx, ::Ice::Double offy, const ::std::string& coordsys, const ::std::string& ope, const ::std::string& np, const ::std::string& movetype, const ::std::string& side, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_rotangle(rotangle),
            _m_offx(offx),
            _m_offy(offy),
            _m_coordsys(coordsys),
            _m_ope(ope),
            _m_np(np),
            _m_movetype(movetype),
            _m_side(side)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->OffsetPointing(_m_rotangle, _m_offx, _m_offy, _m_coordsys, _m_ope, _m_np, _m_movetype, _m_side, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        ::Ice::Double _m_rotangle;
        ::Ice::Double _m_offx;
        ::Ice::Double _m_offy;
        const ::std::string& _m_coordsys;
        const ::std::string& _m_ope;
        const ::std::string& _m_np;
        const ::std::string& _m_movetype;
        const ::std::string& _m_side;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__OffsetPointing_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, rotangle, offx, offy, coordsys, ope, np, movetype, side, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::OffsetGuiding(::Ice::Double rotangle, ::Ice::Double offx, ::Ice::Double offy, const ::std::string& coordsys, const ::std::string& side, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, ::Ice::Double rotangle, ::Ice::Double offx, ::Ice::Double offy, const ::std::string& coordsys, const ::std::string& side, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_rotangle(rotangle),
            _m_offx(offx),
            _m_offy(offy),
            _m_coordsys(coordsys),
            _m_side(side)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->OffsetGuiding(_m_rotangle, _m_offx, _m_offy, _m_coordsys, _m_side, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        ::Ice::Double _m_rotangle;
        ::Ice::Double _m_offx;
        ::Ice::Double _m_offy;
        const ::std::string& _m_coordsys;
        const ::std::string& _m_side;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__OffsetGuiding_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, rotangle, offx, offy, coordsys, side, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::UpdatePointingReference(const ::std::string& offsetcoord, const ::std::string& side, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, const ::std::string& offsetcoord, const ::std::string& side, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_offsetcoord(offsetcoord),
            _m_side(side)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->UpdatePointingReference(_m_offsetcoord, _m_side, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        const ::std::string& _m_offsetcoord;
        const ::std::string& _m_side;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__UpdatePointingReference_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, offsetcoord, side, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::TipTilt(::Ice::Double xrot, ::Ice::Double yrot, const ::std::string& ope, const ::std::string& side, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, ::Ice::Double xrot, ::Ice::Double yrot, const ::std::string& ope, const ::std::string& side, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_xrot(xrot),
            _m_yrot(yrot),
            _m_ope(ope),
            _m_side(side)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->TipTilt(_m_xrot, _m_yrot, _m_ope, _m_side, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        ::Ice::Double _m_xrot;
        ::Ice::Double _m_yrot;
        const ::std::string& _m_ope;
        const ::std::string& _m_side;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__TipTilt_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, xrot, yrot, ope, side, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::SetPMTerm(const ::std::string& name, ::Ice::Double value, const ::std::string& side, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, const ::std::string& name, ::Ice::Double value, const ::std::string& side, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_name(name),
            _m_value(value),
            _m_side(side)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->SetPMTerm(_m_name, _m_value, _m_side, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        const ::std::string& _m_name;
        ::Ice::Double _m_value;
        const ::std::string& _m_side;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__SetPMTerm_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, name, value, side, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::MoveXYZ(::Ice::Double xmov, ::Ice::Double ymov, ::Ice::Double zmov, const ::std::string& side, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, ::Ice::Double xmov, ::Ice::Double ymov, ::Ice::Double zmov, const ::std::string& side, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_xmov(xmov),
            _m_ymov(ymov),
            _m_zmov(zmov),
            _m_side(side)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->MoveXYZ(_m_xmov, _m_ymov, _m_zmov, _m_side, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        ::Ice::Double _m_xmov;
        ::Ice::Double _m_ymov;
        ::Ice::Double _m_zmov;
        const ::std::string& _m_side;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__MoveXYZ_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, xmov, ymov, zmov, side, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::MoveXY(::Ice::Double xmov, ::Ice::Double ymov, const ::std::string& ope, const ::std::string& side, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, ::Ice::Double xmov, ::Ice::Double ymov, const ::std::string& ope, const ::std::string& side, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_xmov(xmov),
            _m_ymov(ymov),
            _m_ope(ope),
            _m_side(side)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->MoveXY(_m_xmov, _m_ymov, _m_ope, _m_side, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        ::Ice::Double _m_xmov;
        ::Ice::Double _m_ymov;
        const ::std::string& _m_ope;
        const ::std::string& _m_side;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__MoveXY_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, xmov, ymov, ope, side, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::Move(::Ice::Double x, ::Ice::Double y, ::Ice::Double z, ::Ice::Double rx, ::Ice::Double ry, ::Ice::Double rz, ::Ice::Int dflag, const ::std::string& movetype, const ::std::string& OPE, ::Ice::Int time, const ::std::string& side, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, ::Ice::Double x, ::Ice::Double y, ::Ice::Double z, ::Ice::Double rx, ::Ice::Double ry, ::Ice::Double rz, ::Ice::Int dflag, const ::std::string& movetype, const ::std::string& OPE, ::Ice::Int time, const ::std::string& side, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_x(x),
            _m_y(y),
            _m_z(z),
            _m_rx(rx),
            _m_ry(ry),
            _m_rz(rz),
            _m_dflag(dflag),
            _m_movetype(movetype),
            _m_OPE(OPE),
            _m_time(time),
            _m_side(side)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->Move(_m_x, _m_y, _m_z, _m_rx, _m_ry, _m_rz, _m_dflag, _m_movetype, _m_OPE, _m_time, _m_side, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        ::Ice::Double _m_x;
        ::Ice::Double _m_y;
        ::Ice::Double _m_z;
        ::Ice::Double _m_rx;
        ::Ice::Double _m_ry;
        ::Ice::Double _m_rz;
        ::Ice::Int _m_dflag;
        const ::std::string& _m_movetype;
        const ::std::string& _m_OPE;
        ::Ice::Int _m_time;
        const ::std::string& _m_side;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__Move_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, x, y, z, rx, ry, rz, dflag, movetype, OPE, time, side, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::SetTarget(::Ice::Double coord1, ::Ice::Double coord2, const ::std::string& system, ::Ice::Double epoch, ::Ice::Float wavelength, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, ::Ice::Double coord1, ::Ice::Double coord2, const ::std::string& system, ::Ice::Double epoch, ::Ice::Float wavelength, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_coord1(coord1),
            _m_coord2(coord2),
            _m_system(system),
            _m_epoch(epoch),
            _m_wavelength(wavelength)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->SetTarget(_m_coord1, _m_coord2, _m_system, _m_epoch, _m_wavelength, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        ::Ice::Double _m_coord1;
        ::Ice::Double _m_coord2;
        const ::std::string& _m_system;
        ::Ice::Double _m_epoch;
        ::Ice::Float _m_wavelength;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__SetTarget_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, coord1, coord2, system, epoch, wavelength, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::SetStars(const ::lbto::SeqPos& stars, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, const ::lbto::SeqPos& stars, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_stars(stars)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->SetStars(_m_stars, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        const ::lbto::SeqPos& _m_stars;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__SetStars_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, stars, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::SetStarsNew(const ::lbto::SeqNewPos& stars, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, const ::lbto::SeqNewPos& stars, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_stars(stars)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->SetStarsNew(_m_stars, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        const ::lbto::SeqNewPos& _m_stars;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__SetStarsNew_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, stars, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::ClearStars(const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->ClearStars(_current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__ClearStars_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::SetHotspot(::Ice::Double coord1, ::Ice::Double coord2, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, ::Ice::Double coord1, ::Ice::Double coord2, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_coord1(coord1),
            _m_coord2(coord2)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->SetHotspot(_m_coord1, _m_coord2, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        ::Ice::Double _m_coord1;
        ::Ice::Double _m_coord2;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__SetHotspot_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, coord1, coord2, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::ClearHotspot(const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->ClearHotspot(_current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__ClearHotspot_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::SetOffset(::Ice::Double coord1, ::Ice::Double coord2, const ::std::string& system, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, ::Ice::Double coord1, ::Ice::Double coord2, const ::std::string& system, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_coord1(coord1),
            _m_coord2(coord2),
            _m_system(system)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->SetOffset(_m_coord1, _m_coord2, _m_system, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        ::Ice::Double _m_coord1;
        ::Ice::Double _m_coord2;
        const ::std::string& _m_system;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__SetOffset_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, coord1, coord2, system, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::ClearOffset(const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->ClearOffset(_current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__ClearOffset_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::SetNonSidereal(const ::lbto::nonsidereal& target, bool override, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, const ::lbto::nonsidereal& target, bool override, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_target(target),
            _m_override(override)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->SetNonSidereal(_m_target, _m_override, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        const ::lbto::nonsidereal& _m_target;
        bool _m_override;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__SetNonSidereal_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, target, override, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::ClearNonSidereal(bool override, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, bool override, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_override(override)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->ClearNonSidereal(_m_override, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        bool _m_override;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__ClearNonSidereal_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, override, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::UpdateNonSiderealTarget(::Ice::Double RArate, ::Ice::Double DECrate, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, ::Ice::Double RArate, ::Ice::Double DECrate, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_RArate(RArate),
            _m_DECrate(DECrate)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->UpdateNonSiderealTarget(_m_RArate, _m_DECrate, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        ::Ice::Double _m_RArate;
        ::Ice::Double _m_DECrate;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__UpdateNonSiderealTarget_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, RArate, DECrate, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::PresetTelescope(::Ice::Double rotangle, const ::std::string& rotmode, const ::std::string& mode, const ::std::string& side, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, ::Ice::Double rotangle, const ::std::string& rotmode, const ::std::string& mode, const ::std::string& side, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_rotangle(rotangle),
            _m_rotmode(rotmode),
            _m_mode(mode),
            _m_side(side)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->PresetTelescope(_m_rotangle, _m_rotmode, _m_mode, _m_side, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        ::Ice::Double _m_rotangle;
        const ::std::string& _m_rotmode;
        const ::std::string& _m_mode;
        const ::std::string& _m_side;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__PresetTelescope_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, rotangle, rotmode, mode, side, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::BinocularControl(const ::std::string& flag, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, const ::std::string& flag, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_flag(flag)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->BinocularControl(_m_flag, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        const ::std::string& _m_flag;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__BinocularControl_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, flag, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::GetParameter(const ::lbto::SeqDD& list, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, const ::lbto::SeqDD& list, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_list(list)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->GetParameter(_m_list, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        const ::lbto::SeqDD& _m_list;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__GetParameter_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, list, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::SetParameter(const ::lbto::SeqDD& list, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, const ::lbto::SeqDD& list, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_list(list)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->SetParameter(_m_list, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        const ::lbto::SeqDD& _m_list;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__SetParameter_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, list, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::RotateCommon(::Ice::Double z, ::Ice::Double angle, ::Ice::Double direction, const ::std::string& side, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, ::Ice::Double z, ::Ice::Double angle, ::Ice::Double direction, const ::std::string& side, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_z(z),
            _m_angle(angle),
            _m_direction(direction),
            _m_side(side)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->RotateCommon(_m_z, _m_angle, _m_direction, _m_side, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        ::Ice::Double _m_z;
        ::Ice::Double _m_angle;
        ::Ice::Double _m_direction;
        const ::std::string& _m_side;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__RotateCommon_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, z, angle, direction, side, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::RotateZ(::Ice::Double angle, const ::std::string& movetype, const ::std::string& side, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, ::Ice::Double angle, const ::std::string& movetype, const ::std::string& side, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_angle(angle),
            _m_movetype(movetype),
            _m_side(side)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->RotateZ(_m_angle, _m_movetype, _m_side, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        ::Ice::Double _m_angle;
        const ::std::string& _m_movetype;
        const ::std::string& _m_side;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__RotateZ_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, angle, movetype, side, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::RotatePrimary(::Ice::Double distance, ::Ice::Double angle, ::Ice::Double direction, const ::std::string& side, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, ::Ice::Double distance, ::Ice::Double angle, ::Ice::Double direction, const ::std::string& side, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_distance(distance),
            _m_angle(angle),
            _m_direction(direction),
            _m_side(side)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->RotatePrimary(_m_distance, _m_angle, _m_direction, _m_side, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        ::Ice::Double _m_distance;
        ::Ice::Double _m_angle;
        ::Ice::Double _m_direction;
        const ::std::string& _m_side;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__RotatePrimary_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, distance, angle, direction, side, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::SendWavefront(const ::lbto::SeqWF& wfs, const ::std::string& OPE, const ::std::string& side, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, const ::lbto::SeqWF& wfs, const ::std::string& OPE, const ::std::string& side, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_wfs(wfs),
            _m_OPE(OPE),
            _m_side(side)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->SendWavefront(_m_wfs, _m_OPE, _m_side, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        const ::lbto::SeqWF& _m_wfs;
        const ::std::string& _m_OPE;
        const ::std::string& _m_side;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__SendWavefront_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, wfs, OPE, side, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::UpdateTargetWavelength(::Ice::Float wavelength, const ::std::string& side, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, ::Ice::Float wavelength, const ::std::string& side, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_wavelength(wavelength),
            _m_side(side)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->UpdateTargetWavelength(_m_wavelength, _m_side, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        ::Ice::Float _m_wavelength;
        const ::std::string& _m_side;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__UpdateTargetWavelength_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, wavelength, side, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::LogEvent(const ::std::string& event, const ::std::string& description, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, const ::std::string& event, const ::std::string& description, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_event(event),
            _m_description(description)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->LogEvent(_m_event, _m_description, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        const ::std::string& _m_event;
        const ::std::string& _m_description;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__LogEvent_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, event, description, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::GetRotatorTrajectory(::Ice::Double seconds, ::Ice::Double interval, ::Ice::Double start, const ::std::string& side, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, ::Ice::Double seconds, ::Ice::Double interval, ::Ice::Double start, const ::std::string& side, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_seconds(seconds),
            _m_interval(interval),
            _m_start(start),
            _m_side(side)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->GetRotatorTrajectory(_m_seconds, _m_interval, _m_start, _m_side, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        ::Ice::Double _m_seconds;
        ::Ice::Double _m_interval;
        ::Ice::Double _m_start;
        const ::std::string& _m_side;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__GetRotatorTrajectory_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, seconds, interval, start, side, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::GetRotatorPolynomials(::Ice::Double start, ::Ice::Int count, ::Ice::Double interval, ::Ice::Int order, const ::std::string& side, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, ::Ice::Double start, ::Ice::Int count, ::Ice::Double interval, ::Ice::Int order, const ::std::string& side, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_start(start),
            _m_count(count),
            _m_interval(interval),
            _m_order(order),
            _m_side(side)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->GetRotatorPolynomials(_m_start, _m_count, _m_interval, _m_order, _m_side, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        ::Ice::Double _m_start;
        ::Ice::Int _m_count;
        ::Ice::Double _m_interval;
        ::Ice::Int _m_order;
        const ::std::string& _m_side;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__GetRotatorPolynomials_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, start, count, interval, order, side, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::Standby(::Ice::Int level, const ::std::string& side, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, ::Ice::Int level, const ::std::string& side, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_level(level),
            _m_side(side)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->Standby(_m_level, _m_side, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        ::Ice::Int _m_level;
        const ::std::string& _m_side;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__Standby_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, level, side, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::MaximizeWrapTime(bool azflag, bool rotflag, const ::std::string& side, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, bool azflag, bool rotflag, const ::std::string& side, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_azflag(azflag),
            _m_rotflag(rotflag),
            _m_side(side)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->MaximizeWrapTime(_m_azflag, _m_rotflag, _m_side, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        bool _m_azflag;
        bool _m_rotflag;
        const ::std::string& _m_side;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__MaximizeWrapTime_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, azflag, rotflag, side, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::PauseGuiding(const ::std::string& side, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, const ::std::string& side, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_side(side)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->PauseGuiding(_m_side, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        const ::std::string& _m_side;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__PauseGuiding_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, side, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::ResumeGuiding(const ::std::string& side, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, const ::std::string& side, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_side(side)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->ResumeGuiding(_m_side, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        const ::std::string& _m_side;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__ResumeGuiding_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, side, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::SetGuidingHotspot(::Ice::Double coord1, ::Ice::Double coord2, const ::std::string& movetype, const ::std::string& side, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, ::Ice::Double coord1, ::Ice::Double coord2, const ::std::string& movetype, const ::std::string& side, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_coord1(coord1),
            _m_coord2(coord2),
            _m_movetype(movetype),
            _m_side(side)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->SetGuidingHotspot(_m_coord1, _m_coord2, _m_movetype, _m_side, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        ::Ice::Double _m_coord1;
        ::Ice::Double _m_coord2;
        const ::std::string& _m_movetype;
        const ::std::string& _m_side;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__SetGuidingHotspot_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, coord1, coord2, movetype, side, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::RotReady(bool enable, const ::std::string& side, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, bool enable, const ::std::string& side, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_enable(enable),
            _m_side(side)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->RotReady(_m_enable, _m_side, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        bool _m_enable;
        const ::std::string& _m_side;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__RotReady_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, enable, side, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::RotHold(const ::std::string& side, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, const ::std::string& side, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_side(side)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->RotHold(_m_side, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        const ::std::string& _m_side;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__RotHold_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, side, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::RotTrack(const ::std::string& side, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, const ::std::string& side, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_side(side)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->RotTrack(_m_side, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        const ::std::string& _m_side;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__RotTrack_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, side, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::RotServicePosition(::Ice::Double angle, const ::std::string& side, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, ::Ice::Double angle, const ::std::string& side, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_angle(angle),
            _m_side(side)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->RotServicePosition(_m_angle, _m_side, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        ::Ice::Double _m_angle;
        const ::std::string& _m_side;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__RotServicePosition_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, angle, side, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::PauseAO(const ::std::string& side, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, const ::std::string& side, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_side(side)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->PauseAO(_m_side, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        const ::std::string& _m_side;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__PauseAO_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, side, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::ResumeAO(const ::std::string& side, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, const ::std::string& side, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_side(side)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->ResumeAO(_m_side, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        const ::std::string& _m_side;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__ResumeAO_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, side, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::StartAO(const ::std::string& side, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, const ::std::string& side, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_side(side)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->StartAO(_m_side, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        const ::std::string& _m_side;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__StartAO_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, side, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::StopAO(const ::std::string& reason, const ::std::string& side, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, const ::std::string& reason, const ::std::string& side, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_reason(reason),
            _m_side(side)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->StopAO(_m_reason, _m_side, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        const ::std::string& _m_reason;
        const ::std::string& _m_side;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__StopAO_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, reason, side, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::AcquireRefAO(bool repointFlag, const ::std::string& side, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, bool repointFlag, const ::std::string& side, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_repointFlag(repointFlag),
            _m_side(side)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->AcquireRefAO(_m_repointFlag, _m_side, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        bool _m_repointFlag;
        const ::std::string& _m_side;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__AcquireRefAO_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, repointFlag, side, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::CheckRefAO(const ::std::string& side, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, const ::std::string& side, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_side(side)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->CheckRefAO(_m_side, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        const ::std::string& _m_side;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__CheckRefAO_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, side, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::RefineAO(const ::std::string& method, const ::std::string& side, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, const ::std::string& method, const ::std::string& side, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_method(method),
            _m_side(side)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->RefineAO(_m_method, _m_side, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        const ::std::string& _m_method;
        const ::std::string& _m_side;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__RefineAO_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, method, side, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::RunAO(const ::std::string& type, const ::std::string& side, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, const ::std::string& type, const ::std::string& side, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_type(type),
            _m_side(side)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->RunAO(_m_type, _m_side, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        const ::std::string& _m_type;
        const ::std::string& _m_side;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__RunAO_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, type, side, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::PresetFlatAO(const ::std::string& flat, const ::std::string& side, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, const ::std::string& flat, const ::std::string& side, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_flat(flat),
            _m_side(side)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->PresetFlatAO(_m_flat, _m_side, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        const ::std::string& _m_flat;
        const ::std::string& _m_side;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__PresetFlatAO_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, flat, side, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::CorrectModesAO(const ::lbto::SeqModes& modes, const ::std::string& side, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, const ::lbto::SeqModes& modes, const ::std::string& side, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_modes(modes),
            _m_side(side)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->CorrectModesAO(_m_modes, _m_side, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        const ::lbto::SeqModes& _m_modes;
        const ::std::string& _m_side;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__CorrectModesAO_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, modes, side, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::ModifyAO(::Ice::Int NModes, ::Ice::Double Freq, ::Ice::Int Nbins, ::Ice::Double TTMod, const ::std::string& F1spec, const ::std::string& F2spec, const ::std::string& side, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, ::Ice::Int NModes, ::Ice::Double Freq, ::Ice::Int Nbins, ::Ice::Double TTMod, const ::std::string& F1spec, const ::std::string& F2spec, const ::std::string& side, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_NModes(NModes),
            _m_Freq(Freq),
            _m_Nbins(Nbins),
            _m_TTMod(TTMod),
            _m_F1spec(F1spec),
            _m_F2spec(F2spec),
            _m_side(side)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->ModifyAO(_m_NModes, _m_Freq, _m_Nbins, _m_TTMod, _m_F1spec, _m_F2spec, _m_side, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        ::Ice::Int _m_NModes;
        ::Ice::Double _m_Freq;
        ::Ice::Int _m_Nbins;
        ::Ice::Double _m_TTMod;
        const ::std::string& _m_F1spec;
        const ::std::string& _m_F2spec;
        const ::std::string& _m_side;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__ModifyAO_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, NModes, Freq, Nbins, TTMod, F1spec, F2spec, side, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::PresetAO(const ::std::string& AOMode, const ::std::string& side, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, const ::std::string& AOMode, const ::std::string& side, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_AOMode(AOMode),
            _m_side(side)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->PresetAO(_m_AOMode, _m_side, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        const ::std::string& _m_AOMode;
        const ::std::string& _m_side;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__PresetAO_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, AOMode, side, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::OffsetXYAO(::Ice::Double DeltaX, ::Ice::Double DeltaY, const ::std::string& side, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, ::Ice::Double DeltaX, ::Ice::Double DeltaY, const ::std::string& side, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_DeltaX(DeltaX),
            _m_DeltaY(DeltaY),
            _m_side(side)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->OffsetXYAO(_m_DeltaX, _m_DeltaY, _m_side, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        ::Ice::Double _m_DeltaX;
        ::Ice::Double _m_DeltaY;
        const ::std::string& _m_side;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__OffsetXYAO_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, DeltaX, DeltaY, side, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::OffsetZAO(::Ice::Double DeltaZ, const ::std::string& side, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, ::Ice::Double DeltaZ, const ::std::string& side, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_DeltaZ(DeltaZ),
            _m_side(side)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->OffsetZAO(_m_DeltaZ, _m_side, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        ::Ice::Double _m_DeltaZ;
        const ::std::string& _m_side;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__OffsetZAO_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, DeltaZ, side, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::SetReference(const ::lbto::SeqPos& stars, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, const ::lbto::SeqPos& stars, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_stars(stars)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->SetReference(_m_stars, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        const ::lbto::SeqPos& _m_stars;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__SetReference_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, stars, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::SetReferenceNew(const ::lbto::SeqNewPos& stars, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, const ::lbto::SeqNewPos& stars, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_stars(stars)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->SetReferenceNew(_m_stars, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        const ::lbto::SeqNewPos& _m_stars;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__SetReferenceNew_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, stars, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::ClearReference(const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->ClearReference(_current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__ClearReference_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::GetKFPCoordinates(const ::lbto::SeqPos& stars, const ::std::string& side, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, const ::lbto::SeqPos& stars, const ::std::string& side, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_stars(stars),
            _m_side(side)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->GetKFPCoordinates(_m_stars, _m_side, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        const ::lbto::SeqPos& _m_stars;
        const ::std::string& _m_side;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__GetKFPCoordinates_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, stars, side, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::GetKFPCoordinatesNew(const ::lbto::SeqNewPos& stars, const ::std::string& side, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, const ::lbto::SeqNewPos& stars, const ::std::string& side, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_stars(stars),
            _m_side(side)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->GetKFPCoordinatesNew(_m_stars, _m_side, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        const ::lbto::SeqNewPos& _m_stars;
        const ::std::string& _m_side;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__GetKFPCoordinatesNew_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, stars, side, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

::lbto::result
IceDelegateD::lbto::IIFServer::SetAGWFilter(::Ice::Int filterNumber, const ::std::string& side, const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(::lbto::result& __result, ::Ice::Int filterNumber, const ::std::string& side, const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current),
            _result(__result),
            _m_filterNumber(filterNumber),
            _m_side(side)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            _result = servant->SetAGWFilter(_m_filterNumber, _m_side, _current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
        ::lbto::result& _result;
        ::Ice::Int _m_filterNumber;
        const ::std::string& _m_side;
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__SetAGWFilter_name, ::Ice::Normal, __context);
    ::lbto::result __result;
    try
    {
        _DirectI __direct(__result, filterNumber, side, __current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
    return __result;
}

void
IceDelegateD::lbto::IIFServer::remove(const ::Ice::Context* __context)
{
    class _DirectI : public ::IceInternal::Direct
    {
    public:

        _DirectI(const ::Ice::Current& __current) : 
            ::IceInternal::Direct(__current)
        {
        }
        
        virtual ::Ice::DispatchStatus
        run(::Ice::Object* object)
        {
            ::lbto::IIFServer* servant = dynamic_cast< ::lbto::IIFServer*>(object);
            if(!servant)
            {
                throw ::Ice::OperationNotExistException(__FILE__, __LINE__, _current.id, _current.facet, _current.operation);
            }
            servant->remove(_current);
            return ::Ice::DispatchOK;
        }
        
    private:
        
    };
    
    ::Ice::Current __current;
    __initCurrent(__current, __lbto__IIFServer__remove_name, ::Ice::Normal, __context);
    try
    {
        _DirectI __direct(__current);
        try
        {
            __direct.servant()->__collocDispatch(__direct);
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
    }
    catch(const ::Ice::SystemException&)
    {
        throw;
    }
    catch(const ::IceInternal::LocalExceptionWrapper&)
    {
        throw;
    }
    catch(const ::std::exception& __ex)
    {
        ::IceInternal::LocalExceptionWrapper::throwWrapper(__ex);
    }
    catch(...)
    {
        throw ::IceInternal::LocalExceptionWrapper(::Ice::UnknownException(__FILE__, __LINE__, "unknown c++ exception"), false);
    }
}

::Ice::ObjectPtr
lbto::IIFServer::ice_clone() const
{
    throw ::Ice::CloneNotImplementedException(__FILE__, __LINE__);
    return 0; // to avoid a warning with some compilers
}

static const ::std::string __lbto__IIFServer_ids[2] =
{
    "::Ice::Object",
    "::lbto::IIFServer"
};

bool
lbto::IIFServer::ice_isA(const ::std::string& _s, const ::Ice::Current&) const
{
    return ::std::binary_search(__lbto__IIFServer_ids, __lbto__IIFServer_ids + 2, _s);
}

::std::vector< ::std::string>
lbto::IIFServer::ice_ids(const ::Ice::Current&) const
{
    return ::std::vector< ::std::string>(&__lbto__IIFServer_ids[0], &__lbto__IIFServer_ids[2]);
}

const ::std::string&
lbto::IIFServer::ice_id(const ::Ice::Current&) const
{
    return __lbto__IIFServer_ids[1];
}

const ::std::string&
lbto::IIFServer::ice_staticId()
{
    return __lbto__IIFServer_ids[1];
}

::Ice::DispatchStatus
lbto::IIFServer::___Authorize(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    __inS.is()->skipEmptyEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = Authorize(__current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___StepFocus(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::Ice::Double relPos;
    ::std::string ope;
    ::std::string side;
    __is->read(relPos);
    __is->read(ope);
    __is->read(side);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = StepFocus(relPos, ope, side, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___MoveFocus(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::Ice::Double absPos;
    ::std::string ope;
    ::std::string side;
    __is->read(absPos);
    __is->read(ope);
    __is->read(side);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = MoveFocus(absPos, ope, side, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___OffsetPointing(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::Ice::Double rotangle;
    ::Ice::Double offx;
    ::Ice::Double offy;
    ::std::string coordsys;
    ::std::string ope;
    ::std::string np;
    ::std::string movetype;
    ::std::string side;
    __is->read(rotangle);
    __is->read(offx);
    __is->read(offy);
    __is->read(coordsys);
    __is->read(ope);
    __is->read(np);
    __is->read(movetype);
    __is->read(side);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = OffsetPointing(rotangle, offx, offy, coordsys, ope, np, movetype, side, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___OffsetGuiding(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::Ice::Double rotangle;
    ::Ice::Double offx;
    ::Ice::Double offy;
    ::std::string coordsys;
    ::std::string side;
    __is->read(rotangle);
    __is->read(offx);
    __is->read(offy);
    __is->read(coordsys);
    __is->read(side);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = OffsetGuiding(rotangle, offx, offy, coordsys, side, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___UpdatePointingReference(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::std::string offsetcoord;
    ::std::string side;
    __is->read(offsetcoord);
    __is->read(side);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = UpdatePointingReference(offsetcoord, side, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___TipTilt(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::Ice::Double xrot;
    ::Ice::Double yrot;
    ::std::string ope;
    ::std::string side;
    __is->read(xrot);
    __is->read(yrot);
    __is->read(ope);
    __is->read(side);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = TipTilt(xrot, yrot, ope, side, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___SetPMTerm(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::std::string name;
    ::Ice::Double value;
    ::std::string side;
    __is->read(name);
    __is->read(value);
    __is->read(side);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = SetPMTerm(name, value, side, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___MoveXYZ(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::Ice::Double xmov;
    ::Ice::Double ymov;
    ::Ice::Double zmov;
    ::std::string side;
    __is->read(xmov);
    __is->read(ymov);
    __is->read(zmov);
    __is->read(side);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = MoveXYZ(xmov, ymov, zmov, side, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___MoveXY(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::Ice::Double xmov;
    ::Ice::Double ymov;
    ::std::string ope;
    ::std::string side;
    __is->read(xmov);
    __is->read(ymov);
    __is->read(ope);
    __is->read(side);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = MoveXY(xmov, ymov, ope, side, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___Move(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::Ice::Double x;
    ::Ice::Double y;
    ::Ice::Double z;
    ::Ice::Double rx;
    ::Ice::Double ry;
    ::Ice::Double rz;
    ::Ice::Int dflag;
    ::std::string movetype;
    ::std::string OPE;
    ::Ice::Int time;
    ::std::string side;
    __is->read(x);
    __is->read(y);
    __is->read(z);
    __is->read(rx);
    __is->read(ry);
    __is->read(rz);
    __is->read(dflag);
    __is->read(movetype);
    __is->read(OPE);
    __is->read(time);
    __is->read(side);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = Move(x, y, z, rx, ry, rz, dflag, movetype, OPE, time, side, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___SetTarget(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::Ice::Double coord1;
    ::Ice::Double coord2;
    ::std::string system;
    ::Ice::Double epoch;
    ::Ice::Float wavelength;
    __is->read(coord1);
    __is->read(coord2);
    __is->read(system);
    __is->read(epoch);
    __is->read(wavelength);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = SetTarget(coord1, coord2, system, epoch, wavelength, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___SetStars(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::lbto::SeqPos stars;
    ::lbto::__readSeqPos(__is, stars);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = SetStars(stars, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___SetStarsNew(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::lbto::SeqNewPos stars;
    ::lbto::__readSeqNewPos(__is, stars);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = SetStarsNew(stars, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___ClearStars(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    __inS.is()->skipEmptyEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = ClearStars(__current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___SetHotspot(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::Ice::Double coord1;
    ::Ice::Double coord2;
    __is->read(coord1);
    __is->read(coord2);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = SetHotspot(coord1, coord2, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___ClearHotspot(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    __inS.is()->skipEmptyEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = ClearHotspot(__current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___SetOffset(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::Ice::Double coord1;
    ::Ice::Double coord2;
    ::std::string system;
    __is->read(coord1);
    __is->read(coord2);
    __is->read(system);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = SetOffset(coord1, coord2, system, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___ClearOffset(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    __inS.is()->skipEmptyEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = ClearOffset(__current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___SetNonSidereal(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::lbto::nonsidereal target;
    bool override;
    target.__read(__is);
    __is->read(override);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = SetNonSidereal(target, override, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___ClearNonSidereal(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    bool override;
    __is->read(override);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = ClearNonSidereal(override, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___UpdateNonSiderealTarget(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::Ice::Double RArate;
    ::Ice::Double DECrate;
    __is->read(RArate);
    __is->read(DECrate);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = UpdateNonSiderealTarget(RArate, DECrate, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___PresetTelescope(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::Ice::Double rotangle;
    ::std::string rotmode;
    ::std::string mode;
    ::std::string side;
    __is->read(rotangle);
    __is->read(rotmode);
    __is->read(mode);
    __is->read(side);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = PresetTelescope(rotangle, rotmode, mode, side, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___BinocularControl(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::std::string flag;
    __is->read(flag);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = BinocularControl(flag, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___GetParameter(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::lbto::SeqDD list;
    ::lbto::__readSeqDD(__is, list);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = GetParameter(list, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___SetParameter(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::lbto::SeqDD list;
    ::lbto::__readSeqDD(__is, list);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = SetParameter(list, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___RotateCommon(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::Ice::Double z;
    ::Ice::Double angle;
    ::Ice::Double direction;
    ::std::string side;
    __is->read(z);
    __is->read(angle);
    __is->read(direction);
    __is->read(side);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = RotateCommon(z, angle, direction, side, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___RotateZ(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::Ice::Double angle;
    ::std::string movetype;
    ::std::string side;
    __is->read(angle);
    __is->read(movetype);
    __is->read(side);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = RotateZ(angle, movetype, side, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___RotatePrimary(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::Ice::Double distance;
    ::Ice::Double angle;
    ::Ice::Double direction;
    ::std::string side;
    __is->read(distance);
    __is->read(angle);
    __is->read(direction);
    __is->read(side);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = RotatePrimary(distance, angle, direction, side, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___SendWavefront(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::lbto::SeqWF wfs;
    ::std::string OPE;
    ::std::string side;
    __is->read(wfs);
    __is->read(OPE);
    __is->read(side);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = SendWavefront(wfs, OPE, side, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___UpdateTargetWavelength(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::Ice::Float wavelength;
    ::std::string side;
    __is->read(wavelength);
    __is->read(side);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = UpdateTargetWavelength(wavelength, side, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___LogEvent(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::std::string event;
    ::std::string description;
    __is->read(event);
    __is->read(description);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = LogEvent(event, description, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___GetRotatorTrajectory(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::Ice::Double seconds;
    ::Ice::Double interval;
    ::Ice::Double start;
    ::std::string side;
    __is->read(seconds);
    __is->read(interval);
    __is->read(start);
    __is->read(side);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = GetRotatorTrajectory(seconds, interval, start, side, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___GetRotatorPolynomials(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::Ice::Double start;
    ::Ice::Int count;
    ::Ice::Double interval;
    ::Ice::Int order;
    ::std::string side;
    __is->read(start);
    __is->read(count);
    __is->read(interval);
    __is->read(order);
    __is->read(side);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = GetRotatorPolynomials(start, count, interval, order, side, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___Standby(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::Ice::Int level;
    ::std::string side;
    __is->read(level);
    __is->read(side);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = Standby(level, side, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___MaximizeWrapTime(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    bool azflag;
    bool rotflag;
    ::std::string side;
    __is->read(azflag);
    __is->read(rotflag);
    __is->read(side);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = MaximizeWrapTime(azflag, rotflag, side, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___PauseGuiding(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::std::string side;
    __is->read(side);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = PauseGuiding(side, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___ResumeGuiding(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::std::string side;
    __is->read(side);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = ResumeGuiding(side, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___SetGuidingHotspot(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::Ice::Double coord1;
    ::Ice::Double coord2;
    ::std::string movetype;
    ::std::string side;
    __is->read(coord1);
    __is->read(coord2);
    __is->read(movetype);
    __is->read(side);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = SetGuidingHotspot(coord1, coord2, movetype, side, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___RotReady(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    bool enable;
    ::std::string side;
    __is->read(enable);
    __is->read(side);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = RotReady(enable, side, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___RotHold(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::std::string side;
    __is->read(side);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = RotHold(side, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___RotTrack(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::std::string side;
    __is->read(side);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = RotTrack(side, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___RotServicePosition(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::Ice::Double angle;
    ::std::string side;
    __is->read(angle);
    __is->read(side);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = RotServicePosition(angle, side, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___PauseAO(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::std::string side;
    __is->read(side);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = PauseAO(side, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___ResumeAO(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::std::string side;
    __is->read(side);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = ResumeAO(side, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___StartAO(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::std::string side;
    __is->read(side);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = StartAO(side, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___StopAO(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::std::string reason;
    ::std::string side;
    __is->read(reason);
    __is->read(side);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = StopAO(reason, side, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___AcquireRefAO(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    bool repointFlag;
    ::std::string side;
    __is->read(repointFlag);
    __is->read(side);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = AcquireRefAO(repointFlag, side, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___CheckRefAO(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::std::string side;
    __is->read(side);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = CheckRefAO(side, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___RefineAO(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::std::string method;
    ::std::string side;
    __is->read(method);
    __is->read(side);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = RefineAO(method, side, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___RunAO(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::std::string type;
    ::std::string side;
    __is->read(type);
    __is->read(side);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = RunAO(type, side, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___PresetFlatAO(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::std::string flat;
    ::std::string side;
    __is->read(flat);
    __is->read(side);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = PresetFlatAO(flat, side, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___CorrectModesAO(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::lbto::SeqModes modes;
    ::std::string side;
    __is->read(modes);
    __is->read(side);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = CorrectModesAO(modes, side, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___ModifyAO(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::Ice::Int NModes;
    ::Ice::Double Freq;
    ::Ice::Int Nbins;
    ::Ice::Double TTMod;
    ::std::string F1spec;
    ::std::string F2spec;
    ::std::string side;
    __is->read(NModes);
    __is->read(Freq);
    __is->read(Nbins);
    __is->read(TTMod);
    __is->read(F1spec);
    __is->read(F2spec);
    __is->read(side);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = ModifyAO(NModes, Freq, Nbins, TTMod, F1spec, F2spec, side, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___PresetAO(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::std::string AOMode;
    ::std::string side;
    __is->read(AOMode);
    __is->read(side);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = PresetAO(AOMode, side, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___OffsetXYAO(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::Ice::Double DeltaX;
    ::Ice::Double DeltaY;
    ::std::string side;
    __is->read(DeltaX);
    __is->read(DeltaY);
    __is->read(side);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = OffsetXYAO(DeltaX, DeltaY, side, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___OffsetZAO(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::Ice::Double DeltaZ;
    ::std::string side;
    __is->read(DeltaZ);
    __is->read(side);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = OffsetZAO(DeltaZ, side, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___SetReference(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::lbto::SeqPos stars;
    ::lbto::__readSeqPos(__is, stars);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = SetReference(stars, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___SetReferenceNew(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::lbto::SeqNewPos stars;
    ::lbto::__readSeqNewPos(__is, stars);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = SetReferenceNew(stars, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___ClearReference(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    __inS.is()->skipEmptyEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = ClearReference(__current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___GetKFPCoordinates(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::lbto::SeqPos stars;
    ::std::string side;
    ::lbto::__readSeqPos(__is, stars);
    __is->read(side);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = GetKFPCoordinates(stars, side, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___GetKFPCoordinatesNew(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::lbto::SeqNewPos stars;
    ::std::string side;
    ::lbto::__readSeqNewPos(__is, stars);
    __is->read(side);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = GetKFPCoordinatesNew(stars, side, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___SetAGWFilter(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    __is->startReadEncaps();
    ::Ice::Int filterNumber;
    ::std::string side;
    __is->read(filterNumber);
    __is->read(side);
    __is->endReadEncaps();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::lbto::result __ret = SetAGWFilter(filterNumber, side, __current);
    __ret.__write(__os);
    return ::Ice::DispatchOK;
}

::Ice::DispatchStatus
lbto::IIFServer::___remove(::IceInternal::Incoming& __inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    __inS.is()->skipEmptyEncaps();
    remove(__current);
    return ::Ice::DispatchOK;
}

static ::std::string __lbto__IIFServer_all[] =
{
    "AcquireRefAO",
    "Authorize",
    "BinocularControl",
    "CheckRefAO",
    "ClearHotspot",
    "ClearNonSidereal",
    "ClearOffset",
    "ClearReference",
    "ClearStars",
    "CorrectModesAO",
    "GetKFPCoordinates",
    "GetKFPCoordinatesNew",
    "GetParameter",
    "GetRotatorPolynomials",
    "GetRotatorTrajectory",
    "LogEvent",
    "MaximizeWrapTime",
    "ModifyAO",
    "Move",
    "MoveFocus",
    "MoveXY",
    "MoveXYZ",
    "OffsetGuiding",
    "OffsetPointing",
    "OffsetXYAO",
    "OffsetZAO",
    "PauseAO",
    "PauseGuiding",
    "PresetAO",
    "PresetFlatAO",
    "PresetTelescope",
    "RefineAO",
    "ResumeAO",
    "ResumeGuiding",
    "RotHold",
    "RotReady",
    "RotServicePosition",
    "RotTrack",
    "RotateCommon",
    "RotatePrimary",
    "RotateZ",
    "RunAO",
    "SendWavefront",
    "SetAGWFilter",
    "SetGuidingHotspot",
    "SetHotspot",
    "SetNonSidereal",
    "SetOffset",
    "SetPMTerm",
    "SetParameter",
    "SetReference",
    "SetReferenceNew",
    "SetStars",
    "SetStarsNew",
    "SetTarget",
    "Standby",
    "StartAO",
    "StepFocus",
    "StopAO",
    "TipTilt",
    "UpdateNonSiderealTarget",
    "UpdatePointingReference",
    "UpdateTargetWavelength",
    "ice_id",
    "ice_ids",
    "ice_isA",
    "ice_ping",
    "remove"
};

::Ice::DispatchStatus
lbto::IIFServer::__dispatch(::IceInternal::Incoming& in, const ::Ice::Current& current)
{
    ::std::pair< ::std::string*, ::std::string*> r = ::std::equal_range(__lbto__IIFServer_all, __lbto__IIFServer_all + 68, current.operation);
    if(r.first == r.second)
    {
        throw ::Ice::OperationNotExistException(__FILE__, __LINE__, current.id, current.facet, current.operation);
    }

    switch(r.first - __lbto__IIFServer_all)
    {
        case 0:
        {
            return ___AcquireRefAO(in, current);
        }
        case 1:
        {
            return ___Authorize(in, current);
        }
        case 2:
        {
            return ___BinocularControl(in, current);
        }
        case 3:
        {
            return ___CheckRefAO(in, current);
        }
        case 4:
        {
            return ___ClearHotspot(in, current);
        }
        case 5:
        {
            return ___ClearNonSidereal(in, current);
        }
        case 6:
        {
            return ___ClearOffset(in, current);
        }
        case 7:
        {
            return ___ClearReference(in, current);
        }
        case 8:
        {
            return ___ClearStars(in, current);
        }
        case 9:
        {
            return ___CorrectModesAO(in, current);
        }
        case 10:
        {
            return ___GetKFPCoordinates(in, current);
        }
        case 11:
        {
            return ___GetKFPCoordinatesNew(in, current);
        }
        case 12:
        {
            return ___GetParameter(in, current);
        }
        case 13:
        {
            return ___GetRotatorPolynomials(in, current);
        }
        case 14:
        {
            return ___GetRotatorTrajectory(in, current);
        }
        case 15:
        {
            return ___LogEvent(in, current);
        }
        case 16:
        {
            return ___MaximizeWrapTime(in, current);
        }
        case 17:
        {
            return ___ModifyAO(in, current);
        }
        case 18:
        {
            return ___Move(in, current);
        }
        case 19:
        {
            return ___MoveFocus(in, current);
        }
        case 20:
        {
            return ___MoveXY(in, current);
        }
        case 21:
        {
            return ___MoveXYZ(in, current);
        }
        case 22:
        {
            return ___OffsetGuiding(in, current);
        }
        case 23:
        {
            return ___OffsetPointing(in, current);
        }
        case 24:
        {
            return ___OffsetXYAO(in, current);
        }
        case 25:
        {
            return ___OffsetZAO(in, current);
        }
        case 26:
        {
            return ___PauseAO(in, current);
        }
        case 27:
        {
            return ___PauseGuiding(in, current);
        }
        case 28:
        {
            return ___PresetAO(in, current);
        }
        case 29:
        {
            return ___PresetFlatAO(in, current);
        }
        case 30:
        {
            return ___PresetTelescope(in, current);
        }
        case 31:
        {
            return ___RefineAO(in, current);
        }
        case 32:
        {
            return ___ResumeAO(in, current);
        }
        case 33:
        {
            return ___ResumeGuiding(in, current);
        }
        case 34:
        {
            return ___RotHold(in, current);
        }
        case 35:
        {
            return ___RotReady(in, current);
        }
        case 36:
        {
            return ___RotServicePosition(in, current);
        }
        case 37:
        {
            return ___RotTrack(in, current);
        }
        case 38:
        {
            return ___RotateCommon(in, current);
        }
        case 39:
        {
            return ___RotatePrimary(in, current);
        }
        case 40:
        {
            return ___RotateZ(in, current);
        }
        case 41:
        {
            return ___RunAO(in, current);
        }
        case 42:
        {
            return ___SendWavefront(in, current);
        }
        case 43:
        {
            return ___SetAGWFilter(in, current);
        }
        case 44:
        {
            return ___SetGuidingHotspot(in, current);
        }
        case 45:
        {
            return ___SetHotspot(in, current);
        }
        case 46:
        {
            return ___SetNonSidereal(in, current);
        }
        case 47:
        {
            return ___SetOffset(in, current);
        }
        case 48:
        {
            return ___SetPMTerm(in, current);
        }
        case 49:
        {
            return ___SetParameter(in, current);
        }
        case 50:
        {
            return ___SetReference(in, current);
        }
        case 51:
        {
            return ___SetReferenceNew(in, current);
        }
        case 52:
        {
            return ___SetStars(in, current);
        }
        case 53:
        {
            return ___SetStarsNew(in, current);
        }
        case 54:
        {
            return ___SetTarget(in, current);
        }
        case 55:
        {
            return ___Standby(in, current);
        }
        case 56:
        {
            return ___StartAO(in, current);
        }
        case 57:
        {
            return ___StepFocus(in, current);
        }
        case 58:
        {
            return ___StopAO(in, current);
        }
        case 59:
        {
            return ___TipTilt(in, current);
        }
        case 60:
        {
            return ___UpdateNonSiderealTarget(in, current);
        }
        case 61:
        {
            return ___UpdatePointingReference(in, current);
        }
        case 62:
        {
            return ___UpdateTargetWavelength(in, current);
        }
        case 63:
        {
            return ___ice_id(in, current);
        }
        case 64:
        {
            return ___ice_ids(in, current);
        }
        case 65:
        {
            return ___ice_isA(in, current);
        }
        case 66:
        {
            return ___ice_ping(in, current);
        }
        case 67:
        {
            return ___remove(in, current);
        }
    }

    assert(false);
    throw ::Ice::OperationNotExistException(__FILE__, __LINE__, current.id, current.facet, current.operation);
}

void
lbto::IIFServer::__write(::IceInternal::BasicStream* __os) const
{
    __os->writeTypeId(ice_staticId());
    __os->startWriteSlice();
    __os->endWriteSlice();
#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
    Object::__write(__os);
#else
    ::Ice::Object::__write(__os);
#endif
}

void
lbto::IIFServer::__read(::IceInternal::BasicStream* __is, bool __rid)
{
    if(__rid)
    {
        ::std::string myId;
        __is->readTypeId(myId);
    }
    __is->startReadSlice();
    __is->endReadSlice();
#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
    Object::__read(__is, true);
#else
    ::Ice::Object::__read(__is, true);
#endif
}

void
lbto::IIFServer::__write(const ::Ice::OutputStreamPtr&) const
{
    Ice::MarshalException ex(__FILE__, __LINE__);
    ex.reason = "type lbto::IIFServer was not generated with stream support";
    throw ex;
}

void
lbto::IIFServer::__read(const ::Ice::InputStreamPtr&, bool)
{
    Ice::MarshalException ex(__FILE__, __LINE__);
    ex.reason = "type lbto::IIFServer was not generated with stream support";
    throw ex;
}

void 
lbto::__patch__IIFServerPtr(void* __addr, ::Ice::ObjectPtr& v)
{
    ::lbto::IIFServerPtr* p = static_cast< ::lbto::IIFServerPtr*>(__addr);
    assert(p);
    *p = ::lbto::IIFServerPtr::dynamicCast(v);
    if(v && !*p)
    {
        IceInternal::Ex::throwUOE(::lbto::IIFServer::ice_staticId(), v->ice_id());
    }
}

bool
lbto::operator==(const ::lbto::IIFServer& l, const ::lbto::IIFServer& r)
{
    return static_cast<const ::Ice::Object&>(l) == static_cast<const ::Ice::Object&>(r);
}

bool
lbto::operator<(const ::lbto::IIFServer& l, const ::lbto::IIFServer& r)
{
    return static_cast<const ::Ice::Object&>(l) < static_cast<const ::Ice::Object&>(r);
}

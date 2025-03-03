/*
 *
 *    Copyright (c) 2020 Project CHIP Authors
 *    All rights reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#pragma once

#include <app/util/attribute-storage.h>

#include <stdbool.h>
#include <stdint.h>

#include <functional>

class Device
{
public:
    static const int kDeviceNameSize     = 32;
    static const int kDeviceLocationSize = 32;

    enum Changed_t
    {
        kChanged_Reachable = 1u << 0,
        kChanged_Location  = 1u << 1,
        kChanged_Name      = 1u << 2,
        kChanged_Last      = kChanged_Name,
    } Changed;

    Device(const char * szDeviceName, const char * szLocation);
    virtual ~Device() {}

    bool IsReachable();
    void SetReachable(bool aReachable);
    void SetName(const char * szDeviceName);
    void SetLocation(const char * szLocation);
    inline void SetEndpointId(chip::EndpointId id) { mEndpointId = id; };
    inline chip::EndpointId GetEndpointId() { return mEndpointId; };
    inline char * GetName() { return mName; };
    inline char * GetLocation() { return mLocation; };

private:
    virtual void HandleDeviceChange(Device * device, Device::Changed_t changeMask) = 0;

protected:
    bool mReachable;
    char mName[kDeviceNameSize];
    char mLocation[kDeviceLocationSize];
    chip::EndpointId mEndpointId;
};

class DeviceOnOff : public Device
{
public:
    enum Changed_t
    {
        kChanged_OnOff = kChanged_Last << 1,
    } Changed;

    DeviceOnOff(const char * szDeviceName, const char * szLocation);

    bool IsOn();
    void SetOnOff(bool aOn);
    void Toggle();

    using DeviceCallback_fn = std::function<void(DeviceOnOff *, DeviceOnOff::Changed_t)>;
    void SetChangeCallback(DeviceCallback_fn aChanged_CB);

private:
    void HandleDeviceChange(Device * device, Device::Changed_t changeMask);

private:
    bool mOn;
    DeviceCallback_fn mChanged_CB;
};

class DeviceSwitch : public Device
{
public:
    enum Changed_t
    {
        kChanged_NumberOfPositions = kChanged_Last << 1,
        kChanged_CurrentPosition   = kChanged_Last << 2,
        kChanged_MultiPressMax     = kChanged_Last << 3,
    } Changed;

    DeviceSwitch(const char * szDeviceName, const char * szLocation, uint32_t aFeatureMap);

    void SetNumberOfPositions(uint8_t aNumberOfPositions);
    void SetCurrentPosition(uint8_t aCurrentPosition);
    void SetMultiPressMax(uint8_t aMultiPressMax);

    inline uint8_t GetNumberOfPositions() { return mNumberOfPositions; };
    inline uint8_t GetCurrentPosition() { return mCurrentPosition; };
    inline uint8_t GetMultiPressMax() { return mMultiPressMax; };
    inline uint32_t GetFeatureMap() { return mFeatureMap; };

    using DeviceCallback_fn = std::function<void(DeviceSwitch *, DeviceSwitch::Changed_t)>;
    void SetChangeCallback(DeviceCallback_fn aChanged_CB);

private:
    void HandleDeviceChange(Device * device, Device::Changed_t changeMask);

private:
    uint8_t mNumberOfPositions;
    uint8_t mCurrentPosition;
    uint8_t mMultiPressMax;
    uint32_t mFeatureMap;
    DeviceCallback_fn mChanged_CB;
};

class ComposedDevice : public Device
{
public:
    ComposedDevice(const char * szDeviceName, const char * szLocation) : Device(szDeviceName, szLocation){};

    using DeviceCallback_fn = std::function<void(ComposedDevice *, ComposedDevice::Changed_t)>;

    void SetChangeCallback(DeviceCallback_fn aChanged_CB);

private:
    void HandleDeviceChange(Device * device, Device::Changed_t changeMask);

private:
    DeviceCallback_fn mChanged_CB;
};

class DevicePowerSource : public Device
{
public:
    enum Changed_t
    {
        kChanged_BatLevel    = kChanged_Last << 1,
        kChanged_Description = kChanged_Last << 2,
    } Changed;

    DevicePowerSource(const char * szDeviceName, const char * szLocation, uint32_t aFeatureMap) :
        Device(szDeviceName, szLocation), mFeatureMap(aFeatureMap){};

    using DeviceCallback_fn = std::function<void(DevicePowerSource *, DevicePowerSource::Changed_t)>;
    void SetChangeCallback(DeviceCallback_fn aChanged_CB) { mChanged_CB = aChanged_CB; }

    void SetBatChargeLevel(uint8_t aBatChargeLevel);
    void SetDescription(std::string aDescription);

    inline uint32_t GetFeatureMap() { return mFeatureMap; };
    inline uint8_t GetBatChargeLevel() { return mBatChargeLevel; };
    inline uint8_t GetOrder() { return mOrder; };
    inline uint8_t GetStatus() { return mStatus; };
    inline std::string GetDescription() { return mDescription; };

private:
    void HandleDeviceChange(Device * device, Device::Changed_t changeMask);

private:
    uint8_t mBatChargeLevel  = 0;
    uint8_t mOrder           = 0;
    uint8_t mStatus          = 0;
    std::string mDescription = "Primary Battery";
    uint32_t mFeatureMap;
    DeviceCallback_fn mChanged_CB;
};

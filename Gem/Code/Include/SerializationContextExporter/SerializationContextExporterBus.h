/*
* Copyright (c) Galib Arrieta (aka 'lumbermixalot@github', aka 'galibzon@github').
* For complete copyright and license terms please see the LICENSE at the root of this distribution.
*
* SPDX-License-Identifier: Apache-2.0 OR MIT
*
*/
#pragma once

#include <AzCore/EBus/EBus.h>
#include <AzCore/Interface/Interface.h>

namespace SerializationContextExporter
{
    class SerializationContextExporterRequests
    {
    public:
        AZ_RTTI(SerializationContextExporterRequests, "{3AD84F22-E9A7-455E-9377-5E974B117CA7}");
        virtual ~SerializationContextExporterRequests() = default;
        
        //! Returns a list of UUIDs for all classes whose name match the regular expression in @classNameRegexFilter.
        //! If @fetchDependencies is true, it also adds the UUIDs of all the classes that are being referenced by the initial
        // list of classes that matched the regular expression. The dependecies are returned regardless of their name.
        virtual AZStd::vector<AZ::Uuid> GetUuidsFromClassNameFilter(const AZStd::string& classNameRegexFilter, bool fetchDependencies) = 0;

        //! Give a class Uuid, returns a string, in json format, with all the data that is needed to serialize
        //! deserialize a particular class.
        virtual AZStd::string GetClassInfoAsJson(const AZ::Uuid& classUuid) = 0;

        //! Given a class Uuid, returns a string, with the class name.
        virtual AZStd::string GetClassName(const AZ::Uuid& classUuid) = 0;

        // Dummy function, left here for debugging purposes. Accepts
        // a string as input and returns its 32bits CRC as Calculated by AZ::Crc32.
        virtual uint32_t GetMySerializedObject(const AZStd::string& forCrc32) = 0;
    };
    
    class SerializationContextExporterBusTraits
        : public AZ::EBusTraits
    {
    public:
        //////////////////////////////////////////////////////////////////////////
        // EBusTraits overrides
        static constexpr AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
        static constexpr AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
        //////////////////////////////////////////////////////////////////////////
    };

    using SerializationContextExporterRequestBus = AZ::EBus<SerializationContextExporterRequests, SerializationContextExporterBusTraits>;
    using SerializationContextExporterInterface = AZ::Interface<SerializationContextExporterRequests>;

} // namespace SerializationContextExporter

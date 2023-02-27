
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
        
        virtual AZStd::vector<AZ::Uuid> GetUuidsFromClassNameFilter(const AZStd::string& classNameRegexFilter, bool fetchDependencies) = 0;
        virtual AZStd::string GetClassName(const AZ::Uuid& classUuid) = 0;
        virtual AZStd::string GetClassInfoAsJson(const AZ::Uuid& classUuid) = 0;

        virtual uint32_t GetMySerializedObject(const AZStd::string& forCrc32) = 0; // dummy function
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

/*
* Copyright (c) Galib Arrieta (aka 'lumbermixalot@github', aka 'galibzon@github').
* For complete copyright and license terms please see the LICENSE at the root of this distribution.
*
* SPDX-License-Identifier: Apache-2.0 OR MIT
*
*/
#include <AzCore/Memory/SystemAllocator.h>
#include <AzCore/Module/Module.h>
#include <Clients/SerializationContextExporterSystemComponent.h>

namespace SerializationContextExporter
{
    class SerializationContextExporterModuleInterface
        : public AZ::Module
    {
    public:
        AZ_RTTI(SerializationContextExporterModuleInterface, "{CC820B54-EFC6-4F19-9913-A30D857A0C9F}", AZ::Module);
        AZ_CLASS_ALLOCATOR(SerializationContextExporterModuleInterface, AZ::SystemAllocator, 0);

        SerializationContextExporterModuleInterface()
        {
            // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
            // Add ALL components descriptors associated with this gem to m_descriptors.
            // This will associate the AzTypeInfo information for the components with the the SerializeContext, BehaviorContext and EditContext.
            // This happens through the [MyComponent]::Reflect() function.
            m_descriptors.insert(m_descriptors.end(), {
                SerializationContextExporterSystemComponent::CreateDescriptor(),
                });
        }

        /**
         * Add required SystemComponents to the SystemEntity.
         */
        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList{
                azrtti_typeid<SerializationContextExporterSystemComponent>(),
            };
        }
    };
}// namespace SerializationContextExporter

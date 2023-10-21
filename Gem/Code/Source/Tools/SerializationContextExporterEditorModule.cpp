/*
* Copyright (c) Galib Arrieta (aka 'lumbermixalot@github', aka 'galibzon@github').
* For complete copyright and license terms please see the LICENSE at the root of this distribution.
*
* SPDX-License-Identifier: Apache-2.0 OR MIT
*
*/

#include <SerializationContextExporterModuleInterface.h>
#include "SerializationContextExporterEditorSystemComponent.h"

void InitSerializationContextExporterResources()
{
    // We must register our Qt resources (.qrc file) since this is being loaded from a separate module (gem)
    Q_INIT_RESOURCE(SerializationContextExporter);
}

namespace SerializationContextExporter
{
    class SerializationContextExporterEditorModule
        : public SerializationContextExporterModuleInterface
    {
    public:
        AZ_RTTI(SerializationContextExporterEditorModule, "{8C8CF667-6884-4F95-B514-7FAD59EA17C5}", SerializationContextExporterModuleInterface);
        AZ_CLASS_ALLOCATOR(SerializationContextExporterEditorModule, AZ::SystemAllocator, 0);

        SerializationContextExporterEditorModule()
        {
            InitSerializationContextExporterResources();

            // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
            // Add ALL components descriptors associated with this gem to m_descriptors.
            // This will associate the AzTypeInfo information for the components with the the SerializeContext, BehaviorContext and EditContext.
            // This happens through the [MyComponent]::Reflect() function.
            m_descriptors.insert(m_descriptors.end(), {
                SerializationContextExporterEditorSystemComponent::CreateDescriptor(),
            });
        }

        /**
         * Add required SystemComponents to the SystemEntity.
         * Non-SystemComponents should not be added here
         */
        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList {
                azrtti_typeid<SerializationContextExporterEditorSystemComponent>(),
            };
        }
    };
}// namespace SerializationContextExporter

AZ_DECLARE_MODULE_CLASS(Gem_SerializationContextExporter, SerializationContextExporter::SerializationContextExporterEditorModule)

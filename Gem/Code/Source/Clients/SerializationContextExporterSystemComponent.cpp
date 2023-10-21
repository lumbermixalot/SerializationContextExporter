/*
* Copyright (c) Galib Arrieta (aka 'lumbermixalot@github', aka 'galibzon@github').
* For complete copyright and license terms please see the LICENSE at the root of this distribution.
*
* SPDX-License-Identifier: Apache-2.0 OR MIT
*
*/
#include "SerializationContextExporterSystemComponent.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/EditContextConstants.inl>

namespace SerializationContextExporter
{
    void SerializationContextExporterSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (AZ::SerializeContext* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<SerializationContextExporterSystemComponent, AZ::Component>()
                ->Version(0)
                ;

            if (AZ::EditContext* ec = serialize->GetEditContext())
            {
                ec->Class<SerializationContextExporterSystemComponent>("SerializationContextExporter", "[Description of functionality provided by this System Component]")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                        ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("System"))
                        ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                    ;
            }
        }
    }

    void SerializationContextExporterSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("SerializationContextExporterService"));
    }

    void SerializationContextExporterSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("SerializationContextExporterService"));
    }

    void SerializationContextExporterSystemComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
    }

    void SerializationContextExporterSystemComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }

    SerializationContextExporterSystemComponent::SerializationContextExporterSystemComponent()
    {
    }

    SerializationContextExporterSystemComponent::~SerializationContextExporterSystemComponent()
    {
    }

    void SerializationContextExporterSystemComponent::Init()
    {
    }

    void SerializationContextExporterSystemComponent::Activate()
    {
        //AZ::TickBus::Handler::BusConnect();
    }

    void SerializationContextExporterSystemComponent::Deactivate()
    {
        //AZ::TickBus::Handler::BusDisconnect();
    }

    void SerializationContextExporterSystemComponent::OnTick([[maybe_unused]] float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
    {
    }

} // namespace SerializationContextExporter


#pragma once

#include <Clients/SerializationContextExporterSystemComponent.h>

#include <AzToolsFramework/Entity/EditorEntityContextBus.h>

#include <SerializationContextExporter/SerializationContextExporterBus.h>

namespace SerializationContextExporter
{
    /// System component for SerializationContextExporter editor
    class SerializationContextExporterEditorSystemComponent
        : public SerializationContextExporterSystemComponent
        , private AzToolsFramework::EditorEvents::Bus::Handler
        , private SerializationContextExporterRequestBus::Handler
    {
        using BaseSystemComponent = SerializationContextExporterSystemComponent;
    public:
        AZ_COMPONENT(SerializationContextExporterEditorSystemComponent, "{329E27B1-6A9C-4873-8EE5-854D474C12F2}", BaseSystemComponent);
        static void Reflect(AZ::ReflectContext* context);

        SerializationContextExporterEditorSystemComponent();
        ~SerializationContextExporterEditorSystemComponent();

    private:
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

        // AZ::Component
        void Activate() override;
        void Deactivate() override;

        // AzToolsFramework::EditorEventsBus overrides ...
        void NotifyRegisterViews() override;

        // SerializationContextExporterRequestBus overrides ...
        AZStd::vector<AZ::Uuid> GetUuidsFromClassNameFilter(const AZStd::string& classNameRegexFilter, bool fetchDependencies) override;
        AZStd::string GetClassName(const AZ::Uuid& classUuid) override;
        AZStd::string GetClassInfoAsJson(const AZ::Uuid& classUuid) override;
        uint32_t GetMySerializedObject(const AZStd::string& forCrc32) override; // REMOVEME
    };
} // namespace SerializationContextExporter

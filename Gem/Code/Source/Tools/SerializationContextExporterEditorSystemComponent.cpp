/*
* Copyright (c) Galib Arrieta (aka 'lumbermixalot@github', aka 'galibzon@github').
* For complete copyright and license terms please see the LICENSE at the root of this distribution.
*
* SPDX-License-Identifier: Apache-2.0 OR MIT
*
*/

#include <AzCore/RTTI/BehaviorContext.h>
#include <AzCore/std/string/regex.h>
#include <AzCore/JSON/document.h>
#include <AzCore/JSON/stringbuffer.h>
#include <AzCore/JSON/prettywriter.h>


// REMOVEME
#include <AzFramework/Script/ScriptDebugMsgReflection.h>

#include <AzToolsFramework/API/ViewPaneOptions.h>

#include "SerializationContextExporterWidget.h"
#include "SerializationContextExporterEditorSystemComponent.h"


namespace SerializationContextExporter
{
    static constexpr char LogName[] = "SerializationContextExporter";

    void SerializationContextExporterEditorSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<SerializationContextExporterEditorSystemComponent, SerializationContextExporterSystemComponent>()
                ->Version(0);
        }

        if (AZ::BehaviorContext* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context))
        {
            behaviorContext->EBus<SerializationContextExporterRequestBus>("SerializationContextExporterRequestBus")
                ->Attribute(AZ::Script::Attributes::Scope, AZ::Script::Attributes::ScopeFlags::Automation)
                ->Attribute(AZ::Script::Attributes::Module, "script")
                ->Event("GetUuidsFromClassNameFilter", &SerializationContextExporterRequests::GetUuidsFromClassNameFilter)
                ->Event("GetClassName", &SerializationContextExporterRequests::GetClassName)
                ->Event("GetClassInfoAsJson", &SerializationContextExporterRequests::GetClassInfoAsJson)
                ->Event("GetMySerializedObject", &SerializationContextExporterRequests::GetMySerializedObject)
                ;
        }
    }

    SerializationContextExporterEditorSystemComponent::SerializationContextExporterEditorSystemComponent() = default;

    SerializationContextExporterEditorSystemComponent::~SerializationContextExporterEditorSystemComponent() = default;

    void SerializationContextExporterEditorSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        BaseSystemComponent::GetProvidedServices(provided);
        provided.push_back(AZ_CRC_CE("SerializationContextExporterEditorService"));
    }

    void SerializationContextExporterEditorSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        BaseSystemComponent::GetIncompatibleServices(incompatible);
        incompatible.push_back(AZ_CRC_CE("SerializationContextExporterEditorService"));
    }

    void SerializationContextExporterEditorSystemComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        BaseSystemComponent::GetRequiredServices(required);
    }

    void SerializationContextExporterEditorSystemComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
        BaseSystemComponent::GetDependentServices(dependent);
    }

    void SerializationContextExporterEditorSystemComponent::Activate()
    {
        SerializationContextExporterSystemComponent::Activate();
        AzToolsFramework::EditorEvents::Bus::Handler::BusConnect();
        SerializationContextExporterRequestBus::Handler::BusConnect();
    }

    void SerializationContextExporterEditorSystemComponent::Deactivate()
    {
        SerializationContextExporterRequestBus::Handler::BusDisconnect();
        AzToolsFramework::EditorEvents::Bus::Handler::BusDisconnect();
        SerializationContextExporterSystemComponent::Deactivate();
    }

    void SerializationContextExporterEditorSystemComponent::NotifyRegisterViews()
    {
        AzToolsFramework::ViewPaneOptions options;
        options.paneRect = QRect(100, 100, 500, 400);
        options.showOnToolsToolbar = true;
        options.toolbarIcon = ":/SerializationContextExporter/toolbar_icon.svg";

        // Register our custom widget as a dockable tool with the Editor under an Examples sub-menu
        AzToolsFramework::RegisterViewPane<SerializationContextExporterWidget>("SerializationContextExporter", "Examples", options);
    }

    static void GetClassDependenciesRecursive(const AZ::SerializeContext* serializeContext, size_t uuidStartIndex, size_t uuidStopIndexNonInclusive, AZStd::vector<AZ::Uuid>& retList, AZStd::unordered_set<AZ::Uuid>& dependencies)
    {
        if (uuidStartIndex >= uuidStopIndexNonInclusive)
        {
            return;
        }

        const auto classUuid = retList[uuidStartIndex];

        //static auto testUuid = AZ::Uuid("{2E2BD168-1805-43D6-8602-FDE14CED8E53}");
        //if (testUuid == classUuid)
        //{
        //    AZ_TracePrintf(LogName, "Found the bastard!\n");
        //}

        if (dependencies.count(classUuid))
        {
            return;
        }
        dependencies.emplace(classUuid);

        auto classData = serializeContext->FindClassData(classUuid);
        if (!classData)
        {
            AZStd::string typeIdStr;
            classUuid.ToString(typeIdStr);
            AZ_Error(LogName, false, "Failed to fetch ClassData for uuid: %s.", typeIdStr.c_str());
            return;
        }

        for (auto eltIt = classData->m_elements.begin(); eltIt != classData->m_elements.end(); ++eltIt)
        {
            auto elementUuid = eltIt->m_typeId;

            if (dependencies.count(elementUuid))
            {
                continue;
            }
            retList.emplace_back(AZStd::move(elementUuid));
            GetClassDependenciesRecursive(serializeContext, retList.size() - 1, retList.size(), retList, dependencies);

            if (eltIt->m_genericClassInfo)
            {
                //virtual size_t GetNumTemplatedArguments() = 0;
                //virtual AZ::TypeId GetTemplatedTypeId(size_t element) = 0;
                const auto numTemplatedArgs = eltIt->m_genericClassInfo->GetNumTemplatedArguments();
                for (size_t i = 0; i < numTemplatedArgs; i++)
                {
                    auto templatedTypeId = eltIt->m_genericClassInfo->GetTemplatedTypeId(i);
                    if (!dependencies.count(templatedTypeId))
                    {
                        retList.emplace_back(AZStd::move(templatedTypeId));
                        GetClassDependenciesRecursive(serializeContext, retList.size() - 1, retList.size(), retList, dependencies);
                    }
                }

                auto specializedUuid = eltIt->m_genericClassInfo->GetSpecializedTypeId();
                if (!dependencies.count(specializedUuid))
                {
                    retList.emplace_back(AZStd::move(specializedUuid));
                    GetClassDependenciesRecursive(serializeContext, retList.size() - 1, retList.size(), retList, dependencies);
                }

                auto genericUuid = eltIt->m_genericClassInfo->GetGenericTypeId();
                if (!dependencies.count(genericUuid))
                {
                    retList.emplace_back(AZStd::move(genericUuid));
                    GetClassDependenciesRecursive(serializeContext, retList.size() - 1, retList.size(), retList, dependencies);
                }

            }
        }

        for (auto i = uuidStartIndex + 1; i < uuidStopIndexNonInclusive; i++)
        {
            GetClassDependenciesRecursive(serializeContext, i, uuidStopIndexNonInclusive, retList, dependencies);
        }

    }


    static void GetClassDependenciesNonRecursive(const AZ::SerializeContext* serializeContext, AZStd::vector<AZ::Uuid>& retList)
    {
        //AZStd::unordered_set<AZ::Uuid> dependencies;
        //GetClassDependenciesRecursive(serializeContext, 0, retList.size(), retList, dependencies);

        AZStd::stack<AZ::Uuid> stackOfUuids;
        for (const auto uuid : retList)
        {
            stackOfUuids.push(uuid);
        }
        retList.clear();
        AZStd::unordered_set<AZ::Uuid> dependencies;

        while (!stackOfUuids.empty())
        {
            const AZ::Uuid classUuid = stackOfUuids.top();
            stackOfUuids.pop();

            //static auto testUuid = AZ::Uuid("{2E2BD168-1805-43D6-8602-FDE14CED8E53}");
            //if (testUuid == classUuid)
            //{
            //    AZ_TracePrintf(LogName, "Found the bastard!\n");
            //}

            if (dependencies.count(classUuid))
            {
                continue;
            }
            dependencies.emplace(classUuid);

            auto classData = serializeContext->FindClassData(classUuid);
            if (!classData)
            {
                AZStd::string typeIdStr;
                classUuid.ToString(typeIdStr);
                AZ_Error(LogName, false, "Failed to fetch ClassData for uuid: %s.", typeIdStr.c_str());
                continue;
            }
            retList.push_back(classUuid);

            if (classData->m_container)
            {
                auto enumTypeCB = [&](const AZ::Uuid& elementClassId, [[maybe_unused]] const AZ::SerializeContext::ClassElement* genericClassElement) -> bool {
                    if (!dependencies.count(elementClassId))
                    {
                        stackOfUuids.push(elementClassId);
                    }
                    return true;
                };
                classData->m_container->EnumTypes(enumTypeCB);
            }

            for (auto eltIt = classData->m_elements.begin(); eltIt != classData->m_elements.end(); ++eltIt)
            {
                auto elementUuid = eltIt->m_typeId;

                if (!dependencies.count(elementUuid))
                {
                    stackOfUuids.push(elementUuid);
                }

                if (eltIt->m_genericClassInfo)
                {
                    const auto numTemplatedArgs = eltIt->m_genericClassInfo->GetNumTemplatedArguments();
                    for (size_t i = 0; i < numTemplatedArgs; i++)
                    {
                        auto templatedTypeId = eltIt->m_genericClassInfo->GetTemplatedTypeId(i);
                        if (!dependencies.count(templatedTypeId))
                        {
                            stackOfUuids.push(templatedTypeId);
                        }
                    }

                    auto specializedUuid = eltIt->m_genericClassInfo->GetSpecializedTypeId();
                    if (!dependencies.count(specializedUuid))
                    {
                        stackOfUuids.push(specializedUuid);
                    }

                    auto genericUuid = eltIt->m_genericClassInfo->GetGenericTypeId();
                    if (!dependencies.count(genericUuid))
                    {
                        stackOfUuids.push(genericUuid);
                    }

                }
            } // For loop elements

        } // main stack loop

    }

    // SerializationContextExporterRequestBus overrides ...
    AZStd::vector<AZ::Uuid> SerializationContextExporterEditorSystemComponent::GetUuidsFromClassNameFilter(const AZStd::string& classNameRegexFilter, bool fetchDependencies)
    {
        AZ::SerializeContext* serializeContext = nullptr;
        AZ::ComponentApplicationBus::BroadcastResult(serializeContext, &AZ::ComponentApplicationRequests::GetSerializeContext);
        AZ_Error("python", serializeContext, "Serialization context not available");
        if (!serializeContext)
        {
            return { {AZ::Uuid::CreateName("Dummy")}};
        }

        const char* regex = ".*+";
        if (!classNameRegexFilter.empty())
        {
            regex = classNameRegexFilter.c_str();
        }
        auto includeRegex = AZStd::regex(regex, AZStd::regex::ECMAScript);

        AZStd::vector<AZ::Uuid> retList;
        auto onClassInfo = [&](const AZ::SerializeContext::ClassData* classData, const AZ::Uuid& classUuid) -> bool
        {
            if (classNameRegexFilter.empty())
            {
                retList.push_back(classUuid);
                return true;
            }
            if (AZStd::regex_match(classData->m_name, includeRegex))
            {
                retList.push_back(classUuid);
            }
            return true;
        };
        serializeContext->EnumerateAll(onClassInfo, true);

        if (fetchDependencies)
        {
            //AZStd::unordered_set<AZ::Uuid> dependencies;
            //GetClassDependenciesRecursive(serializeContext, 0, retList.size(), retList, dependencies);
            GetClassDependenciesNonRecursive(serializeContext, retList);
        }

        return retList;
    }

    AZStd::string SerializationContextExporterEditorSystemComponent::GetClassName(const AZ::Uuid& classUuid)
    {
        AZ::SerializeContext* serializeContext = nullptr;
        AZ::ComponentApplicationBus::BroadcastResult(serializeContext, &AZ::ComponentApplicationRequests::GetSerializeContext);
        AZ_Error("python", serializeContext, "Serialization context not available");
        if (!serializeContext)
        {
            return { "ClassNotFound" };
        }
        auto classData = serializeContext->FindClassData(classUuid);
        return AZStd::string(classData->m_name);
    }


    static uint32_t GetStartingElementIndexRecursive(const AZ::SerializeContext* sc, uint32_t baseIndex, const AZ::SerializeContext::ClassData* classData)
    {
        AZ_Assert(classData, "Null classData");

        if (classData->m_elements.empty())
        {
            return baseIndex;
        }

        const auto& classElement = classData->m_elements[0];
        const bool isSubclass = ((classElement.m_flags & AZ::SerializeContext::ClassElement::Flags::FLG_BASE_CLASS) != 0);
        if (!isSubclass)
        {
            return baseIndex;
        }

        // Get the count of elements from the superClass.
        const auto& superClassUuid = classData->m_elements[0].m_typeId;
        auto superClassData = sc->FindClassData(superClassUuid);
        AZ_Assert(superClassData, "Failed to find ClassData for UUID %s", superClassUuid.ToString<AZStd::string>().c_str());
        return GetStartingElementIndexRecursive(sc, superClassData->m_elements.size() - 1 + baseIndex, superClassData);
    }

    AZStd::string SerializationContextExporterEditorSystemComponent::GetClassInfoAsJson(const AZ::Uuid& classUuid)
    {
        AZ::SerializeContext* serializeContext = nullptr;
        AZ::ComponentApplicationBus::BroadcastResult(serializeContext, &AZ::ComponentApplicationRequests::GetSerializeContext);
        AZ_Error("python", serializeContext, "Serialization context not available");
        if (!serializeContext)
        {
            return { "{}" };
        }
        auto classData = serializeContext->FindClassData(classUuid);
        if (!classData)
        {
            AZStd::string uuidStr;
            classUuid.ToString(uuidStr);
            AZ_Error(LogName, false, "Failed to fetch ClassData for uuid: %s\n", uuidStr.c_str());
            return { "" };
        }

        rapidjson::Document docObj;
        rapidjson::Document::AllocatorType& allocator = docObj.GetAllocator();

        docObj.SetObject();

        docObj.AddMember("name", rapidjson::StringRef(classData->m_name), allocator);
        
        AZStd::string typeIdStr;
        classData->m_typeId.ToString(typeIdStr);
        docObj.AddMember("uuid", rapidjson::StringRef(typeIdStr.c_str()), allocator);

        docObj.AddMember("version", classData->m_version, allocator);

        const bool isContainer = classData->m_container != nullptr;
        rapidjson::Value types(rapidjson::kArrayType);
        if (isContainer)
        {
            auto enumTypeCB = [&](const AZ::Uuid& elementClassId, [[maybe_unused]] const AZ::SerializeContext::ClassElement* genericClassElement) -> bool {
                AZStd::string typeIdAsStr;
                elementClassId.ToString(typeIdAsStr);
                rapidjson::Value uuidValue;
                uuidValue.SetString(typeIdAsStr.c_str(), typeIdAsStr.size(), allocator);

                types.PushBack(AZStd::move(uuidValue), allocator);
                return true;
            };

            classData->m_container->EnumTypes(enumTypeCB);
        }
        docObj.AddMember("containerTypes", types, allocator);

        auto typeSize = classData->m_azRtti ? classData->m_azRtti->GetTypeSize() : 0;
        docObj.AddMember("typeSize", typeSize, allocator);

        rapidjson::Value elements(rapidjson::kArrayType);        
        int32_t elementIndex = GetStartingElementIndexRecursive(serializeContext, 0, classData);
        for (auto eltIt = classData->m_elements.begin(); eltIt != classData->m_elements.end(); ++eltIt, ++elementIndex)
        {
            //if (eltIt->m_flags != 0)
            //{
            //    if (!(eltIt->m_flags & AZ::SerializeContext::ClassElement::Flags::FLG_BASE_CLASS))
            //    {
            //        continue;
            //    }
            //}
            
            rapidjson::Value classElementInfo(rapidjson::kObjectType);
            const bool isBaseClass = ((eltIt->m_flags & AZ::SerializeContext::ClassElement::Flags::FLG_BASE_CLASS) != 0);
            classElementInfo.AddMember("name", rapidjson::StringRef(eltIt->m_name), allocator);
            classElementInfo.AddMember("nameCrc", eltIt->m_nameCrc, allocator);
            classElementInfo.AddMember("isBaseClass", isBaseClass, allocator);

            AZStd::string typeIdAsStr;
            eltIt->m_typeId.ToString(typeIdAsStr);
            rapidjson::Value uuidValue;
            uuidValue.SetString(typeIdAsStr.c_str(), typeIdAsStr.size(), allocator);
            classElementInfo.AddMember("uuid", uuidValue, allocator);

            classElementInfo.AddMember("cppOffset", eltIt->m_offset, allocator); // C++ offset.
            classElementInfo.AddMember("cppSize", eltIt->m_dataSize, allocator); // C++ size.
            classElementInfo.AddMember("elementIndex", isBaseClass ? -1: elementIndex, allocator); // Very useful for TypeScript, Python, etc

            elements.PushBack(AZStd::move(classElementInfo), allocator);
        }

        docObj.AddMember("elements", elements, allocator);
        rapidjson::StringBuffer strbuf;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(strbuf);
        docObj.Accept(writer);

        return AZStd::string(strbuf.GetString());
    }

    uint32_t SerializationContextExporterEditorSystemComponent::GetMySerializedObject(const AZStd::string& forCrc32)
    {
        AZ::SerializeContext* serializeContext = nullptr;
        AZ::ComponentApplicationBus::BroadcastResult(serializeContext, &AZ::ComponentApplicationRequests::GetSerializeContext);
        AZ_Error("python", serializeContext, "Serialization context not available");
        if (!serializeContext)
        {
            return false;
        }

        //AzFramework::ScriptUserPropertyInfo msg;
        //msg.m_isRead = true;
        //msg.m_isWrite = false;
        //msg.m_name = "galib";

        //AzFramework::ScriptDebugRequest msg(AZ_CRC_CE("AttachDebugger"), "Default");

        AzFramework::ScriptDebugEnumLocalsResult msg;
        msg.m_names.push_back("Galib");
        msg.m_names.push_back("Tania");


        AZStd::vector<char> msgBuffer;
        AZ::IO::ByteContainerStream<AZStd::vector<char>> outMsg(&msgBuffer);

        AZ::ObjectStream* objStream = AZ::ObjectStream::Create(&outMsg, *serializeContext, AZ::ObjectStream::ST_BINARY);
        objStream->WriteClass(&msg);
        if (!objStream->Finalize())
        {
            //AZ_Error(LogName, false, "ObjectStream failed to serialize outbound TmMsg! msg.m_name=%s", msg.m_name.c_str());
            //AZ_Error(LogName, false, "ObjectStream failed to serialize outbound TmMsg! msg.m_name=%s", msg.m_context.c_str());
            AZ_Error(LogName, false, "ObjectStream failed to serialize outbound TmMsg! size=%zu", msg.m_names.size());
            return false;
        }

        AZ_Trace(LogName, "Size in bytes=%zu\n",  msgBuffer.size());

        AZ::Crc32 var(forCrc32);
        //uint32_t currentCrc = 0xFFFFFFFF;
        //int crcTmp = static_cast<int>(currentCrc);
        //int xorTet = crcTmp ^ (uint8_t)'s';
        //[[maybe_unused]]uint32_t index = xorTet & 0xff;
        //[[maybe_unused]]uint32_t crcShifted = currentCrc >> 8;
        //[[maybe_unused]] unsigned int tableValue = 3826175755;// [140] = 3826175755
        //[[maybe_unused]] uint32_t crc = tableValue ^ crcShifted;
        if ((AZ::u32)var == 0)
        {
            return static_cast<uint32_t>(msgBuffer.size());
        }
        return var;
    }

} // namespace SerializationContextExporter

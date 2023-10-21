/*
* Copyright (c) Galib Arrieta (aka 'lumbermixalot@github', aka 'galibzon@github').
* For complete copyright and license terms please see the LICENSE at the root of this distribution.
*
* SPDX-License-Identifier: Apache-2.0 OR MIT
*
*/

#include <SerializationContextExporterModuleInterface.h>
#include "SerializationContextExporterSystemComponent.h"

namespace SerializationContextExporter
{
    class SerializationContextExporterModule
        : public SerializationContextExporterModuleInterface
    {
    public:
        AZ_RTTI(SerializationContextExporterModule, "{8C8CF667-6884-4F95-B514-7FAD59EA17C5}", SerializationContextExporterModuleInterface);
        AZ_CLASS_ALLOCATOR(SerializationContextExporterModule, AZ::SystemAllocator, 0);
    };
}// namespace SerializationContextExporter

AZ_DECLARE_MODULE_CLASS(Gem_SerializationContextExporter, SerializationContextExporter::SerializationContextExporterModule)

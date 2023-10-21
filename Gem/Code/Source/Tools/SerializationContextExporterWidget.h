/*
* Copyright (c) Galib Arrieta (aka 'lumbermixalot@github', aka 'galibzon@github').
* For complete copyright and license terms please see the LICENSE at the root of this distribution.
*
* SPDX-License-Identifier: Apache-2.0 OR MIT
*
*/

#pragma once

#if !defined(Q_MOC_RUN)
#include <AzToolsFramework/API/ToolsApplicationAPI.h>

#include <QWidget>
#endif

namespace SerializationContextExporter
{
    class SerializationContextExporterWidget
        : public QWidget
    {
        Q_OBJECT
    public:
        explicit SerializationContextExporterWidget(QWidget* parent = nullptr);
    };
} 

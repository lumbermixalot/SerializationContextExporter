
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

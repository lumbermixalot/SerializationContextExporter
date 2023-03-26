# SerializationContextExporter
This is a Gem you can install to use with the O3DE Game Engine.
Provides an EBus with Python bindings (SerializationContextExporterRequestBus) to extract the decription
of any O3DE Class that is registered in the SerializationContext.

In particular, the attached script: Gem/Editor/Scripts/o3de_classes_extractor.py
was used to generate the `o3de_class.json` that is being used by  
https://github.com/lumbermixalot/vscode-dbg-ext-o3de-lua/blob/main/O3DESerializationContextAutogen/src/main.ts  
to generate:
https://github.com/lumbermixalot/vscode-dbg-ext-o3de-lua/blob/main/src/o3deNetClasses/o3deClasses.ts




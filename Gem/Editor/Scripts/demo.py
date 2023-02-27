import os
import json

import azlmbr.bus as azbus
import azlmbr.script as azscript
import azlmbr.legacy.general as azgeneral

def DemoFunc():
    class_uuids = azscript.SerializationContextExporterRequestBus(azbus.Broadcast,
                                           "GetUuidsFromClassNameFilter", "ScriptDebugAckBreakpoint", False)#"ScriptDebug.*+")
    #sorted_classes_by_named = sorted(class_symbols, key=lambda class_symbol: class_symbol.name)
    for uuid in class_uuids:
        class_name = azscript.SerializationContextExporterRequestBus(azbus.Broadcast,
                                           "GetClassName", uuid)
        print(f"For class uuid {uuid}, name is: {class_name}")


def DemoFunc2():
    class_uuids = azscript.SerializationContextExporterRequestBus(azbus.Broadcast,
                                           "GetUuidsFromClassNameFilter", "int", False)#"ScriptDebug.*+")
    #sorted_classes_by_named = sorted(class_symbols, key=lambda class_symbol: class_symbol.name)
    for uuid in class_uuids:
        class_info_str = azscript.SerializationContextExporterRequestBus(azbus.Broadcast,
                                           "GetClassInfoAsJson", uuid)
        print(f"For class uuid {uuid}, json info is: {class_info_str}")
        class_info_obj = json.loads(class_info_str)
        class_name = class_info_obj["name"]
        print(f"Class name from obj={class_name}")


def DemoFunc3():
    crc32Value = azscript.SerializationContextExporterRequestBus(azbus.Broadcast,
                                           "GetMySerializedObject", "ScriptDebugAgent")
    print(f"result={crc32Value}={hex(crc32Value)}")
    return crc32Value



def DumpScriptDebugClassesToFile(outputFileName: str):
    game_root_path = os.path.normpath(azgeneral.get_game_folder())
    outputFileName = os.path.join(game_root_path, outputFileName)
    try:
        fileObj = open(outputFileName, "wt")
    except:
        print(f"Failed to create output file = {outputFileName}")
        return
    # Get all UUIDs
    class_uuids = azscript.SerializationContextExporterRequestBus(azbus.Broadcast,
                                           "GetUuidsFromClassNameFilter", "ScriptDebug.*+", True)
    fileObj.write('{"classes":\n[\n\n')
    firstObj = True
    for uuid in class_uuids:
        class_info_str = azscript.SerializationContextExporterRequestBus(azbus.Broadcast,
                                           "GetClassInfoAsJson", uuid)
        #print(f"For class uuid {uuid}, json info is: {class_info_str}")
        #class_info_obj = json.loads(class_info_str)
        if class_info_str == "":
            continue
        if firstObj:
            firstObj = False
        else:
            fileObj.write(", ")
        fileObj.write(f"{class_info_str}\n")
        #print(f"Class name from obj={class_name}")
    fileObj.write('\n\n]\n}\n')
    fileObj.close();
    print(f"Successfully dumped all classes at: {outputFileName}")

if __name__ == "__main__":
    #DemoFunc()
    #DemoFunc2()
    #DemoFunc3()
    DumpScriptDebugClassesToFile("o3de_classes.json")
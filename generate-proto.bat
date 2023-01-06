cd ScriptExtender\Extender\Shared
..\..\..\External\x64-windows\tools\protobuf\protoc --cpp_out=. ScriptExtensions.proto

cd ..\..\Lua\Debugger
..\..\..\External\x64-windows\tools\protobuf\protoc --cpp_out=. LuaDebug.proto

cd ..\..\Osiris\Debugger
..\..\..\External\x64-windows\tools\protobuf\protoc --cpp_out=. osidebug.proto

pause
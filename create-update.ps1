# Force a build date refresh
(gci ScriptExtender/Extender/Shared/Console.cpp).LastWriteTime = Get-Date

# MSVC is broken and reuses old structure definitions from previous compilations.
# Force a full recompile each time
Remove-Item x64\Release -Recurse -ErrorAction SilentlyContinue
Remove-Item x64\ReleaseExtensionsOnly -Recurse -ErrorAction SilentlyContinue

msbuild OsiTools.sln /p:Configuration=Release /t:Build /m
msbuild OsiTools.sln /p:Configuration=ReleaseExtensionsOnly /t:Build /m

Remove-Item LatestBuild -Recurse -ErrorAction SilentlyContinue

New-Item LatestBuild -ItemType "directory"

git show -s --format="OsiTools Version: Commit %H, %cD" > LatestBuild\version.txt

Copy-Item x64\Release\CrashReporter.exe -Destination LatestBuild\CrashReporter.exe
Copy-Item x64\Release\OsiExtenderEoCApp.dll -Destination LatestBuild\OsiExtenderEoCApp.dll
Copy-Item x64\ReleaseExtensionsOnly\OsiExtenderEoCPlugin.dll -Destination LatestBuild\OsiExtenderEoCPlugin.dll
Copy-Item External\x64-windows\bin\libprotobuf-lite.dll -Destination LatestBuild\libprotobuf-lite.dll

Copy-Item x64\Release\CrashReporter.pdb -Destination PDB\Latest\CrashReporter.pdb
Copy-Item x64\Release\OsiExtenderEoCApp.pdb -Destination PDB\Latest\OsiExtenderEoCApp.pdb
Copy-Item x64\ReleaseExtensionsOnly\OsiExtenderEoCPlugin.pdb -Destination PDB\Latest\OsiExtenderEoCPlugin.pdb
Copy-Item x64\Release\CrashReporter.exe -Destination PDB\Latest\CrashReporter.exe
Copy-Item x64\Release\OsiExtenderEoCApp.dll -Destination PDB\Latest\OsiExtenderEoCApp.dll
Copy-Item x64\ReleaseExtensionsOnly\OsiExtenderEoCPlugin.dll -Destination PDB\Latest\OsiExtenderEoCPlugin.dll
Copy-Item External\x64-windows\bin\libprotobuf-lite.dll -Destination PDB\Latest\libprotobuf-lite.dll

Remove-Item Latest.zip -ErrorAction SilentlyContinue
Compress-Archive -Path LatestBuild\* -DestinationPath Latest.zip -CompressionLevel Optimal

# Force a build date refresh
(gci OsiInterface/Utils.cpp).LastWriteTime = Get-Date

msbuild OsiTools.sln /p:Configuration=Release /t:Build /m
msbuild OsiTools.sln /p:Configuration=ReleaseExtensionsOnly /t:Build /m

Remove-Item LatestBuild -Recurse -ErrorAction SilentlyContinue

New-Item LatestBuild -ItemType "directory"

git show -s --format="OsiTools Version: Commit %H, %cD" > LatestBuild\version.txt

Copy-Item x64\Release\CrashReporter.exe -Destination LatestBuild\CrashReporter.exe
Copy-Item x64\Release\DXGI.dll -Destination LatestBuild\OsiExtenderEoCApp.dll
Copy-Item x64\ReleaseExtensionsOnly\DXGI.dll -Destination LatestBuild\OsiExtenderEoCPlugin.dll
Copy-Item x64\ReleaseExtensionsOnly\libprotobuf-lite.dll -Destination LatestBuild\libprotobuf-lite.dll

Copy-Item x64\Release\CrashReporter.pdb -Destination LatestBuildPDB\CrashReporter.pdb
Copy-Item x64\Release\DXGI.pdb -Destination LatestBuildPDB\OsiExtenderEoCApp.pdb
Copy-Item x64\ReleaseExtensionsOnly\DXGI.pdb -Destination LatestBuildPDB\OsiExtenderEoCPlugin.pdb
Copy-Item x64\Release\CrashReporter.exe -Destination LatestBuildPDB\CrashReporter.exe
Copy-Item x64\Release\DXGI.dll -Destination LatestBuildPDB\OsiExtenderEoCApp.dll
Copy-Item x64\ReleaseExtensionsOnly\DXGI.dll -Destination LatestBuildPDB\OsiExtenderEoCPlugin.dll
Copy-Item x64\ReleaseExtensionsOnly\libprotobuf-lite.dll -Destination LatestBuildPDB\libprotobuf-lite.dll

Remove-Item Latest.zip -ErrorAction SilentlyContinue
Compress-Archive -Path LatestBuild\* -DestinationPath Latest.zip -CompressionLevel Optimal

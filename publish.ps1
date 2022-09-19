param($Channel)

if ($Channel -ne "Devel" -And $Channel -ne "Release" -And $Channel -ne "Nightly") {
	Write-Error -Message "Usage: publish.ps1 <Channel>"
	Exit 1
}

$GameChannel = $Channel
$EditorChannel = $Channel + "Editor"
$S3Bucket = "nb-stor"
$S3RootPath = "dos2-updater-v2/Channels"
$S3LegacyRootPath = "dos/OsiExtender"
$CloudFrontRootPath = "/Channels"
$CloudFrontRootURL = "https://dbn4nit5dt5fw.cloudfront.net/Channels"
$CloudFrontDistributionID = "E3P2WRK1Q5QRSZ"
$CloudFrontLegacyDistributionID = "E34KSY7UYCK5B3"

$S3ManifestPath = "Manifest2.json"

$PublishingRoot = Join-Path "C:\Dev\LS\DOS2PublishingRoot" "$Channel"
$BuildRoot = Join-Path "$PublishingRoot" "Build"
$PDBRoot = Join-Path "$PublishingRoot" "PDB"
$RootPath = (Get-Location).Path
$GameManifestPath = Join-Path $PublishingRoot "GameManifest2.json"
$EditorManifestPath = Join-Path $PublishingRoot "EditorManifest2.json"
$GameBuildZipPath = Join-Path $BuildRoot "GameLatest.zip"
$EditorBuildZipPath = Join-Path $BuildRoot "EditorLatest.zip"
$LegacyBuildZipPath = Join-Path $BuildRoot "LegacyLatest.zip"
$GameDllPath = Join-Path $RootPath "x64\Game Release\OsiExtenderEoCApp.dll"
$EditorDllPath = Join-Path $RootPath "x64\Editor Release\OsiExtenderEoCPlugin.dll"

$GameBuildDir = Join-Path "$BuildRoot" "TempGameBuild"
$EditorBuildDir = Join-Path "$BuildRoot" "TempEditorBuild"
$LegacyBuildDir = Join-Path "$BuildRoot" "TempLegacyBuild"
$PDBDir = Join-Path "$PDBRoot" "TempPDB"
	
$EditorMinVersion = "3.6.51.0"
$EditorMaxVersion = "-"
	
$GameMinVersion = "3.6.117.0"
$GameMaxVersion = "-"

function Build-Extender
{
	# Force a build date refresh
	(gci ScriptExtender/Extender/Shared/Console.cpp).LastWriteTime = Get-Date

	# MSVC is broken and sometimes reuses old struct definitions from previous compilations.
	# Force a full recompile each time
	# Remove-Item "x64\Game Release" -Recurse -ErrorAction SilentlyContinue
	# Remove-Item "x64\Editor Release" -Recurse -ErrorAction SilentlyContinue

	Write-Output " ===== BUILDING GAME EXTENDER ===== "
	msbuild OsiTools.sln "/p:Configuration=Game Release" /t:Build /m /nologo /verbosity:quiet /consoleloggerparameters:summary
	
	x64\Release\SymbolTableGenerator.exe "x64\Game Release\OsiExtenderEoCApp.pdb" "ScriptExtender\GameHooks\OsiExtenderEoCApp.symtab"
	
	msbuild OsiTools.sln "/p:Configuration=Game Release" /t:Build /m /nologo /verbosity:quiet /consoleloggerparameters:summary
	
	Write-Output " ===== BUILDING EDITOR EXTENDER ===== "
	msbuild OsiTools.sln "/p:Configuration=Editor Release" /t:Build /m /nologo /verbosity:quiet /consoleloggerparameters:summary
}

function Create-PDBDir
{
	Remove-Item $PDBDir -Recurse -ErrorAction SilentlyContinue
	New-Item $PDBDir -ItemType "directory"

	Copy-Item "x64\Release\CrashReporter.pdb" -Destination $PDBDir\CrashReporter.pdb
	Copy-Item "x64\Game Release\OsiExtenderEoCApp.pdb" -Destination $PDBDir\OsiExtenderEoCApp.pdb
	Copy-Item "x64\Editor Release\OsiExtenderEoCPlugin.pdb" -Destination $PDBDir\OsiExtenderEoCPlugin.pdb
	Copy-Item "x64\Release\CrashReporter.exe" -Destination $PDBDir\CrashReporter.exe
	Copy-Item "x64\Game Release\OsiExtenderEoCApp.dll" -Destination $PDBDir\OsiExtenderEoCApp.dll
	Copy-Item "x64\Editor Release\OsiExtenderEoCPlugin.dll" -Destination $PDBDir\OsiExtenderEoCPlugin.dll
	Copy-Item External\x64-windows\bin\libprotobuf-lite.dll -Destination $PDBDir\libprotobuf-lite.dll
}

function Create-Update-Package ($BuildDir, $ZipPath, $HasEditor, $HasGame)
{
	Remove-Item $BuildDir -Recurse -ErrorAction SilentlyContinue
	New-Item $BuildDir -ItemType "directory"

	git show -s --format="D:OS2 Extender Version: Commit %H, %cD" > $BuildDir\Version.txt

	Copy-Item "x64\Release\CrashReporter.exe" -Destination $BuildDir\CrashReporter.exe
	Copy-Item External\x64-windows\bin\libprotobuf-lite.dll -Destination $BuildDir\libprotobuf-lite.dll
	
	if ($HasEditor)
	{
		Copy-Item "x64\Editor Release\OsiExtenderEoCPlugin.dll" -Destination $BuildDir\OsiExtenderEoCPlugin.dll
	}
	
	if ($HasGame)
	{
		Copy-Item "x64\Game Release\OsiExtenderEoCApp.dll" -Destination $BuildDir\OsiExtenderEoCApp.dll
	}
	
	Remove-Item $ZipPath -ErrorAction SilentlyContinue
	Compress-Archive -Path $BuildDir\* -DestinationPath $ZipPath -CompressionLevel Optimal
}

function Archive-Build ($Digest, $ZipPath)
{
	Write-Output $Digest
	Write-Output $ZipPath
	$ArchivePDBDir = Join-Path "$PDBRoot" "$Digest"
	Write-Output "------------------------------------Digest:" $Digest
	New-Item $ArchivePDBDir -ItemType directory -ErrorAction SilentlyContinue
	Copy-Item $PDBDir\* -Destination $ArchivePDBDir
	Copy-Item $ZipPath -Destination $BuildRoot\$DigestPath.zip
}

function Build-Package ($BuildDir, $ZipPath, $DllPath, $Channel, $IsGame, $IsEditor)
{
	Create-Update-Package $BuildDir $ZipPath $IsEditor $IsGame

	x64\Release\UpdateSigner.exe sign package-signer.key $ZipPath
	x64\Release\UpdateSigner.exe verify $ZipPath

	if ($IsEditor)
	{
		x64\Release\UpdateSigner.exe update-manifest "$EditorManifestPath" ScriptExtender "$ZipPath" "$DllPath" $EditorMinVersion $EditorMaxVersion "$CloudFrontRootURL/$Channel/Packages/"
	}
	
	if ($IsGame)
	{
		x64\Release\UpdateSigner.exe update-manifest "$GameManifestPath" ScriptExtender "$ZipPath" "$DllPath" $GameMinVersion $GameMaxVersion "$CloudFrontRootURL/$Channel/Packages/"
	}
}

function Build-Legacy-Package ($BuildDir, $ZipPath)
{
	Create-Update-Package $BuildDir $ZipPath 1 1

	x64\Release\UpdateSigner.exe sign package-signer.key $ZipPath
	x64\Release\UpdateSigner.exe verify $ZipPath
}

function Publish-Legacy-Package
{
	aws s3 cp $LegacyBuildZipPath s3://$S3Bucket/$S3LegacyRootPath/$GameChannel/Latest.zip
	aws s3api put-object-acl --bucket $S3Bucket --key $S3LegacyRootPath/$GameChannel/Latest.zip --acl public-read
	aws cloudfront create-invalidation --distribution-id $CloudFrontLegacyDistributionID --paths /$GameChannel/Latest.zip
}

function Publish-Package ($ZipPath, $DigestPath, $Channel, $ManifestPath)
{
	Write-Output "S3 path: s3://$S3Bucket/$S3RootPath/$Channel/Packages/$DigestPath"
	Write-Output "CloudFront path: $CloudFrontRootPath/$Channel/Packages/$DigestPath"
	Write-Output "URL: $CloudFrontRootURL/$Channel/Packages/$DigestPath"

	aws s3 cp $ZipPath "s3://$S3Bucket/$S3RootPath/$Channel/Packages/$DigestPath"
	aws s3api put-object-acl --bucket nb-stor --key "$S3RootPath/$Channel/Packages/$DigestPath" --acl public-read
	aws s3 cp $ManifestPath "s3://$S3Bucket/$S3RootPath/$Channel/$S3ManifestPath"
	aws s3api put-object-acl --bucket nb-stor --key "$S3RootPath/$Channel/$S3ManifestPath" --acl public-read
	aws cloudfront create-invalidation --distribution-id $CloudFrontDistributionID --paths "$CloudFrontRootPath/$Channel/$S3ManifestPath" "$CloudFrontRootPath/$Channel/Packages/$DigestPath"
}

Build-Extender

Build-Package $GameBuildDir $GameBuildZipPath $GameDllPath $GameChannel 1 0
$GameDigestPath = (x64\Release\UpdateSigner.exe compute-path "$GameBuildZipPath" "$GameDllPath")
Archive-Build $GameDigestPath $GameBuildZipPath

Build-Package $EditorBuildDir $EditorBuildZipPath $EditorDllPath $EditorChannel 0 1
$EditorDigestPath = (x64\Release\UpdateSigner.exe compute-path "$EditorBuildZipPath" "$EditorDllPath")
Archive-Build $EditorDigestPath $EditorBuildZipPath

Build-Legacy-Package $LegacyBuildDir $LegacyBuildZipPath

Write-Output "Build completed."
Write-Output "."

Write-Output "*********************************************************************************"
Write-Output ""
Write-Output "                      ABOUT TO PUBLISH ON CHANNEL $Channel!"
Write-Output ""
Write-Output "*********************************************************************************"
Read-Host

Write-Output " ===== UPLOADING PACKAGES ===== "
Publish-Package $GameBuildZipPath $GameDigestPath $GameChannel $GameManifestPath
Publish-Package $EditorBuildZipPath $EditorDigestPath $EditorChannel $EditorManifestPath
Publish-Legacy-Package

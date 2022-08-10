param($Channel)

if ($Channel -ne "Devel" -And $Channel -ne "Release" -And $Channel -ne "Nightly") {
	Write-Error -Message "Usage: publish.ps1 <Channel>"
	Exit 1
}

$S3Bucket = "nb-stor"
$S3RootPath = "dos2-updater-v2/Channels/$Channel"
$S3LegacyRootPath = "dos/OsiExtender/$Channel"
$CloudFrontRootPath = "/Channels/$Channel"
$CloudFrontRootURL = "https://dbn4nit5dt5fw.cloudfront.net/Channels/$Channel"
$CloudFrontDistributionID = "E3P2WRK1Q5QRSZ"
$CloudFrontLegacyDistributionID = "E34KSY7UYCK5B3"

$S3ManifestPath = "Manifest2.json"

$PublishingRoot = Join-Path "C:\Dev\LS\DOS2PublishingRoot" "$Channel"
$BuildRoot = Join-Path "$PublishingRoot" "Build"
$PDBRoot = Join-Path "$PublishingRoot" "PDB"
$RootPath = (Get-Location).Path
$ManifestPath = Join-Path $PublishingRoot "Manifest2.json"
$BuildPackagePath = Join-Path $BuildRoot "Latest.zip"
$DllPath = Join-Path $RootPath "x64\Game Release\OsiExtenderEoCApp.dll"

$BuildDir = Join-Path "$BuildRoot" "TempBuild"
$PDBDir = Join-Path "$PDBRoot" "TempPDB"
	
$GameMinVersion = "3.6.54.0"
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

function Create-Update-Package ($ZipPath)
{
	Remove-Item $BuildDir -Recurse -ErrorAction SilentlyContinue
	Remove-Item $PDBDir -Recurse -ErrorAction SilentlyContinue

	New-Item $BuildDir -ItemType "directory"
	New-Item $PDBDir -ItemType "directory"

	git show -s --format="D:OS2 Extender Version: Commit %H, %cD" > $BuildDir\Version.txt

	Copy-Item "x64\Release\CrashReporter.exe" -Destination $BuildDir\CrashReporter.exe
	Copy-Item "x64\Game Release\OsiExtenderEoCApp.dll" -Destination $BuildDir\OsiExtenderEoCApp.dll
	Copy-Item "x64\Editor Release\OsiExtenderEoCPlugin.dll" -Destination $BuildDir\OsiExtenderEoCPlugin.dll
	Copy-Item External\x64-windows\bin\libprotobuf-lite.dll -Destination $BuildDir\libprotobuf-lite.dll

	Copy-Item "x64\Release\CrashReporter.pdb" -Destination $PDBDir\CrashReporter.pdb
	Copy-Item "x64\Game Release\OsiExtenderEoCApp.pdb" -Destination $PDBDir\OsiExtenderEoCApp.pdb
	Copy-Item "x64\Editor Release\OsiExtenderEoCPlugin.pdb" -Destination $PDBDir\OsiExtenderEoCPlugin.pdb
	Copy-Item "x64\Release\CrashReporter.exe" -Destination $PDBDir\CrashReporter.exe
	Copy-Item "x64\Game Release\OsiExtenderEoCApp.dll" -Destination $PDBDir\OsiExtenderEoCApp.dll
	Copy-Item "x64\Editor Release\OsiExtenderEoCPlugin.dll" -Destination $PDBDir\OsiExtenderEoCPlugin.dll
	Copy-Item External\x64-windows\bin\libprotobuf-lite.dll -Destination $PDBDir\libprotobuf-lite.dll
	
	Remove-Item $ZipPath -ErrorAction SilentlyContinue
	Compress-Archive -Path $BuildDir\* -DestinationPath $ZipPath -CompressionLevel Optimal
}

function Archive-Build ($Digest)
{
	$ArchivePDBDir = Join-Path "$PDBRoot" "$Digest"
	New-Item $ArchivePDBDir -ItemType directory -ErrorAction SilentlyContinue
	Copy-Item $PDBDir\* -Destination $ArchivePDBDir
	Copy-Item $BuildPackagePath -Destination $BuildRoot\$DigestPath.zip
}

function Build-Release
{
	Build-Extender
	Create-Update-Package $BuildPackagePath
	Archive-Build Latest

	x64\Release\UpdateSigner.exe sign package-signer.key $BuildPackagePath
	x64\Release\UpdateSigner.exe verify $BuildPackagePath
}

Build-Release

x64\Release\UpdateSigner.exe update-manifest "$ManifestPath" ScriptExtender "$BuildPackagePath" "$DllPath" $GameMinVersion $GameMaxVersion "$CloudFrontRootURL/Packages/"
$DigestPath = (x64\Release\UpdateSigner.exe compute-path "$BuildPackagePath" "$DllPath")

Archive-Build $DigestPath

Write-Output "Build completed."
Write-Output "."
Write-Output "S3 path: s3://$S3Bucket/$S3RootPath/Packages/$DigestPath"
Write-Output "CloudFront path: $CloudFrontRootPath/Packages/$DigestPath"
Write-Output "URL: $CloudFrontRootURL/Packages/$DigestPath"

Write-Output "*********************************************************************************"
Write-Output ""
Write-Output "                      ABOUT TO PUBLISH ON CHANNEL $Channel!"
Write-Output ""
Write-Output "*********************************************************************************"
Read-Host


Write-Output " ===== UPLOADING PACKAGE ===== "
aws s3 cp $BuildPackagePath "s3://$S3Bucket/$S3RootPath/Packages/$DigestPath"
aws s3api put-object-acl --bucket nb-stor --key "$S3RootPath/Packages/$DigestPath" --acl public-read
aws s3 cp $ManifestPath "s3://$S3Bucket/$S3RootPath/$S3ManifestPath"
aws s3api put-object-acl --bucket nb-stor --key "$S3RootPath/$S3ManifestPath" --acl public-read
aws cloudfront create-invalidation --distribution-id $CloudFrontDistributionID --paths "$CloudFrontRootPath/$S3ManifestPath" "$CloudFrontRootPath/Packages/$DigestPath"

aws s3 cp $BuildPackagePath s3://$S3Bucket/$S3LegacyRootPath/Latest.zip
aws s3api put-object-acl --bucket $S3Bucket --key $S3LegacyRootPath/Latest.zip --acl public-read
aws cloudfront create-invalidation --distribution-id $CloudFrontLegacyDistributionID --paths /$Channel/Latest.zip

# MSIX packaging

`install/msix` builds an MSIX package of ObjectiveFrame alongside the existing
Inno Setup installer in `install/objectiveframe_v2.iss`. The two ship the same
payload. The MSIX installs and uninstalls without touching the registry, which
is what `winget` and sideloaded builds want - but it is *not* what the Store
listing takes; see below.

## Building

```powershell
cmake -S . -B build -G"Visual Studio 17 2022"
cmake --build build --target msix --config Release
```

The result lands in `packages/windows/ObjectiveFrame-<version>-x64.msix`.

The target is never part of the default build, and it refuses to run against a
Debug configuration - the manifest names `objframe.exe` as its entry point and a
Debug build produces `objframed.exe`.

## What goes in the package

| Payload | Source |
| ------- | ------ |
| `objframe.exe` and its DLLs | the built `bin/<config>` directory |
| MSVC runtime DLLs | `InstallRequiredSystemLibraries` |
| `tetgen.exe` | `external/` |
| `fonts` `images` `maps` `plugins` `python` `examples` `ai` | `bin/` |
| `Assets/*.png` | rendered from `images/logo.png` by `generate_assets.ps1` |

A package cannot run `VC_redist.exe` the way the Inno installer does, so the
MSVC runtime is deployed app-local instead. The UCRT is left out - it is part of
Windows 10 and later.

The sample models the Inno installer drops in the user's Documents folder are
not included: an MSIX has no install-time script to copy them with. They remain
available from `bin/examples` inside the package.

## Version

`OF_MSIX_VERSION` defaults to the `project()` version in the top-level
`CMakeLists.txt` with `.0` appended, because an MSIX version needs four parts
and the fourth is reserved by the Store. Bump the version in one place:

```cmake
project(objframe2 VERSION 2.5.4)
```

`MyAppVersion` in `objectiveframe_v2.iss` is still maintained by hand and needs
to be kept in step.

## This package is not the Store artifact

ObjectiveFrame's Store listing (`XPFPKPPND1DK4T`) is an **EXE/MSI app**, which
distributes the Inno Setup installer from a URL we host. It does not take an
MSIX. A Partner Center product's type is fixed when its name is reserved and
cannot be converted, so shipping MSIX through the Store would mean reserving a
new name and retiring the existing listing. See
[../STORE_SUBMISSION.md](../STORE_SUBMISSION.md) for the route actually in use.

That leaves two jobs for this package:

- **winget**, whose manifests accept an MSIX and get clean install and uninstall
  from it.
- **Sideloaded builds**, where a single signed file is easier to hand someone
  than an installer.

Both need the package signed with our own certificate - see below. None of the
Partner Center identity values (`Package/Identity/Name`, a `CN=<guid>`
publisher, `PublisherDisplayName`) apply, because those exist only for MSIX
products.

If the Store route is ever revisited, what changes is the identity triple:
reserve a new name, set `OF_MSIX_IDENTITY_NAME`, `OF_MSIX_PUBLISHER` and
`OF_MSIX_PUBLISHER_DISPLAY_NAME` from the new product's Product identity page,
and leave `OF_MSIX_SIGN` off, because the Store signs its own submissions. The
configure step detects a Partner Center GUID publisher and reports which route
it is on.

## Signing (sideloading only)

Not needed for the Store route above. This applies to test builds handed to
people directly, which Windows refuses to install unsigned.

The certificate is the Sectigo EV one that `objectiveframe_v2.iss` signs with,
held on the SafeNet eToken:

| | |
| --- | --- |
| Subject | `CN=Lunds Universitet, O=Lunds Universitet, S=Skane lan, C=SE, OID.2.5.4.15=Government Entity, OID.1.3.6.1.4.1.311.60.2.1.3=SE, SERIALNUMBER=Government Entity` |
| Thumbprint | `9E352CE2635133F53A8C3B5795FAADCE80391F14` |
| Store | `Cert:\CurrentUser\My` |
| Provider | eToken Base Cryptographic Provider |

The private key is on the token and is not exportable, so the `.pfx` route
(`OF_MSIX_SIGN_CERT`) does not apply - select it by thumbprint. That subject is
already the default for `OF_MSIX_PUBLISHER`, because `Identity/@Publisher` must
match the signing certificate's subject character for character.

Plug in the eToken, then:

```powershell
cmake -S . -B build -DOF_MSIX_SIGN=ON `
    -DOF_MSIX_SIGN_SHA1=9E352CE2635133F53A8C3B5795FAADCE80391F14
cmake --build build --target msix --config Release
```

SafeNet prompts for the token PIN unless single-logon is enabled. Verify with
`Get-AuthenticodeSignature`; `Status` must be `Valid`.

If the publisher and certificate disagree, signtool refuses the package and
names both strings in its `0x8007000b` error - copy its version into
`OF_MSIX_PUBLISHER` verbatim, as it is more reliable than transcribing the
subject out of the certificate dialog.

Note that `OF_MSIX_PUBLISHER` is a cache variable: changing its default in
`CMakeLists.txt` does not affect an already-configured build tree. Pass
`-DOF_MSIX_PUBLISHER=...` to update one.

### Testing without the token

A self-signed certificate is enough to exercise install and uninstall locally.
It produces a package nobody else can install, so never ship one.

```powershell
$cert = New-SelfSignedCertificate -Type Custom -Subject "CN=ObjectiveFrame Test" `
    -KeyUsage DigitalSignature -FriendlyName "ObjectiveFrame test signing" `
    -CertStoreLocation "Cert:\CurrentUser\My" `
    -TextExtension @("2.5.29.37={text}1.3.6.1.5.5.7.3.3", "2.5.29.19={text}")

# Trust it locally - needs an elevated shell
Export-Certificate -Cert $cert -FilePath of-test.cer
Import-Certificate -FilePath of-test.cer -CertStoreLocation Cert:\LocalMachine\TrustedPeople

cmake -S . -B build-msixtest "-DOF_MSIX_PUBLISHER=CN=ObjectiveFrame Test" `
    -DOF_MSIX_SIGN=ON "-DOF_MSIX_SIGN_SHA1=$($cert.Thumbprint)" -DOF_MSIX_SIGN_TIMESTAMP=
cmake --build build-msixtest --target msix --config Release

Add-AppxPackage packages\windows\ObjectiveFrame-2.5.4.0-x64.msix
```

## Debugging the package

A packaged build behaves differently from `bin\Release\objframe.exe` in ways
that only show up once it is packaged: the install directory is read-only, the
working directory is not what you expect, and the process is started by the
shell rather than by you. Test the package itself, not the loose build.

### Enable Developer Mode first

Everything below needs it, and it is currently off on this machine. Settings ▸
System ▸ For developers ▸ Developer Mode, or from an elevated shell:

```powershell
New-Item -Path HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\AppModelUnlock -Force
Set-ItemProperty -Path HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\AppModelUnlock `
    -Name AllowDevelopmentWithoutDevLicense -Value 1
```

### Run the staged layout - the fast loop

A Store-identity package cannot be signed locally, because its publisher is a
Partner Center GUID that no certificate you hold matches. Registering the loose
layout sidesteps signing entirely:

```powershell
cmake --build build --target msix_register --config Release
```

This packs, then runs `Add-AppxPackage -Register` against
`build\install\msix\layout\AppxManifest.xml`. The app runs from that
directory, so files stay readable and a rebuild is picked up by re-running the
target. Note that the layout is *not* read-only the way a real install is, so
this will not reproduce the settings problem below - it is the loop for
everything else.

Undo with:

```powershell
Get-AppxPackage -Name '*ObjectiveFrame*' | Remove-AppxPackage
```

### Install the real package - the faithful test

To get the genuine read-only install directory, install the `.msix` itself.
Developer Mode allows this without a signature:

```powershell
Add-AppxPackage -Path packages\windows\ObjectiveFrame-2.5.4.0-x64.msix -AllowUnsigned
```

This is the one that reproduces what a Store user gets. Anything that writes
next to the executable fails here and nowhere else.

### Launching and attaching a debugger

A packaged app cannot be started by running its `.exe` - activation has to go
through the shell:

```powershell
$pfn = (Get-AppxPackage -Name '*ObjectiveFrame*').PackageFamilyName
explorer.exe "shell:AppsFolder\$pfn!ObjectiveFrame"
```

`ObjectiveFrame` there is the `Application Id` from the manifest.

To debug from the start of `main()` rather than attaching late, use Visual
Studio's **Debug ▸ Other Debug Targets ▸ Debug Installed App Package**, pick the
registered package and check *Do not launch, but debug my code when it starts*.
The SDK equivalent is `plmdebug.exe`, in
`C:\Program Files (x86)\Windows Kits\Debuggersd`:

```powershell
plmdebug.exe /enabledebug <PackageFullName> "C:\path	o\windbg.exe"
```

Attaching to a running process works normally and needs none of this.

### When deployment or activation fails

Deployment errors from `Add-AppxPackage` are usually truncated. The full reason
is in the deployment log:

```powershell
Get-AppxLog -ActivityId <the ActivityId from the error message>
```

Otherwise, Event Viewer ▸ Applications and Services Logs ▸ Microsoft ▸ Windows:

- **AppXDeployment-Server ▸ Operational** - install and registration failures
- **AppModel-Runtime ▸ Admin** - activation failures, i.e. the tile does nothing

An app that installs but exits immediately on launch is usually a missing
payload file: a DLL that was next to the exe in `bin\Release` but never made it
into the layout. Check what actually shipped with:

```powershell
makeappx unpack /p packages\windows\ObjectiveFrame-2.5.4.0-x64.msix /d unpacked
```

### Inspecting the installed package

```powershell
Get-AppxPackage -Name '*ObjectiveFrame*' | Format-List `
    Name, Version, Publisher, PackageFullName, InstallLocation, Status
```

`InstallLocation` is under `C:\Program Files\WindowsApps`, which is ACL'd -
open it from an elevated shell if you need to look inside.

### Package Support Framework

If a write-to-install-directory problem turns out to be awkward to fix in the
code, the Package Support Framework's `FileRedirectionFixup` is Microsoft's
supported shim for exactly that: it intercepts the writes and redirects them to
a per-user location, without source changes. It is the right tool for a
dependency you cannot change, and the wrong one for code you own - for
ObjectiveFrame's own settings, fixing the path is cleaner and one less moving
part in the package.

## Known issue: the install directory is read-only

A packaged application cannot write into its own install folder, and MSIX does
not virtualise those writes the way it does the registry. Two places in
ObjectiveFrame write relative to the current directory and will therefore fail
silently inside a package:

- `ofutil::AppSettings::save()` writes `<appName>.json` to the working directory
  (`src/util/app_settings.cpp`), so settings - including the Claude API key - are
  not persisted between runs.
- ImGui writes `imgui.ini` to the working directory, so window layout is not
  persisted either.

Both want a per-user location such as `%LOCALAPPDATA%\ObjectiveFrame` before the
package is handed to real users. Everything else - fonts, images, maps, plugins,
examples, the AI system prompt - is read-only and resolves correctly from
`argv[0]`, which points into the package folder.

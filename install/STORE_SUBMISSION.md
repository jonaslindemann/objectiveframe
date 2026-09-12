# Microsoft Store submission

ObjectiveFrame is listed in the Microsoft Store as an **EXE/MSI app**, which
distributes the Inno Setup installer built from `objectiveframe_v2.iss`. The
Store links to a binary you host; it does not host or re-sign it.

| | |
| --- | --- |
| Store ID | `XPFPKPPND1DK4T` |
| Partner Center ID | `36c55140-a864-4e42-9d63-1ad04542ecda` |
| Deep link | `ms-windows-store://pdp/?productid=XPFPKPPND1DK4T` |
| Web listing | <https://apps.microsoft.com/store/detail/XPFPKPPND1DK4T> |

> An EXE/MSI product has no package identity, no `AppxManifest.xml`, and no
> Product identity page - those are MSIX concepts. A product's type is fixed
> when its name is reserved and Partner Center cannot convert between them, so
> moving to MSIX would mean reserving a new name and retiring this listing.
> The MSIX built by `install/msix` is for winget and sideloading, not for this.

## Release procedure

### 1. Bump the version

`project(objframe2 VERSION x.y.z)` in the top-level `CMakeLists.txt` is the
source. `objectiveframe_v2.iss` picks it up when passed on the command line, and
otherwise falls back to the `#define` near the top of the script - keep that
fallback in step.

### 2. Build the installer

From the Inno Setup IDE, or:

```powershell
& "C:\Program Files\Inno Setup 7\ISCC.exe" /DMyAppVersion=2.5.4 install\objectiveframe_v2.iss
```

Run this from PowerShell or cmd, not Git Bash - MSYS rewrites the leading `/D`
into a path and ISCC then reports "You may not specify more than one script
filename".

The eToken must be plugged in: the script's `SignTool=signtool_lu` signs the
installer, the uninstaller and the payload binaries. Output is
`packages\windows\objectiveframe_setup_2.5.4.exe`.

The filename carries the version deliberately. The Store requires that the
binary behind a submitted URL never change, so every release must be a distinct
file at a distinct URL - see step 3.

### 3. Publish it at a versioned HTTPS URL

The Store downloads the installer from a URL you control, so it has to be
stable, public and reachable in every market you list. A GitHub release asset
works:

```
https://github.com/jonaslindemann/objectiveframe/releases/download/v2.5.4/objectiveframe_setup_2.5.4.exe
```

Never replace the file behind a URL that has already been submitted. The Store
keeps a copy and compares; if it finds the binary swapped it pulls the new one
into certification on its own, and notifies you to resubmit properly if that
fails.

### 4. Fill in the Packages page

| Field | Value |
| ----- | ----- |
| Package URL | the versioned URL from step 3 |
| Architecture | `x64` |
| App type | `EXE` |
| Installer parameters | `/VERYSILENT /SUPPRESSMSGBOXES /NORESTART /SP-` |
| Language | `en-us` |

The Store runs the installer unattended, so the silent switches are required.
`/NORESTART` matters: without it the installer may restart the machine out from
under the user, and the Store expects a return code instead.

### 5. Map the return codes

Inno Setup's documented exit codes, mapped onto the scenarios the Packages page
offers:

| Store scenario | Inno exit code |
| -------------- | -------------- |
| Installation successful | `0` |
| Installation cancelled by user | `2`, `5` |
| Reboot required | `8` |
| Installation already in progress | `1` |

Codes `3`, `4` and `7` are fatal errors during install, preparation and
preflight respectively. They have no matching Store scenario, so list them as
custom failure codes pointing at
<https://jrsoftware.org/ishelp/index.php?topic=setupexitcodes>.

Inno does not distinguish "application already exists" or "disk space is full" -
it reports a generic failure - so leave those two rows unmapped rather than
guessing. "Network failure" and "package rejected" do not apply: this is an
offline installer.

### 6. Properties page

Answer **yes** to *Does this product access, collect, or transmit personal
information?* and supply a privacy policy URL. `ofai::StructureGenerator` posts
the user's prompt text to `https://api.anthropic.com/v1/messages`
(`src/ofai/structure_generator.cpp:239`), which is transmission to a third
party. The policy needs to state what is sent, that it goes to Anthropic, and
that the user supplies their own API key.

Worth putting in *Notes for certification*: the app embeds an HTTP server on
port 8081 and launches `tetgen.exe` as a child process, both of which look
unusual to an automated scan.

### 7. Updates

Use **Update submission** in Partner Center and give it the new versioned URL.
The previous release's URL stays valid for anyone mid-download; do not delete
old release assets.

## Requirements this installer already satisfies

Checked against the EXE/MSI packaging rules, so they do not have to be
rediscovered each release:

- **Standalone, offline installer.** A downloader stub is rejected, because the
  Store certifies the exact bytes that get installed. `VC_redist.x64.exe` is
  embedded in the installer rather than fetched at install time.
- **Silent install.** `/VERYSILENT` suppresses the UI; the `[Run]` entry that
  launches the app after install carries `skipifsilent`, so an unattended
  install does not try to open a window.
- **Per-user install.** `PrivilegesRequired=lowest` means no elevation prompt,
  which an unattended Store install cannot answer.
- **Signed.** The Sectigo EV certificate on the eToken, thumbprint
  `9E352CE2635133F53A8C3B5795FAADCE80391F14`, via the `signtool_lu` entry
  configured in the Inno Setup IDE.

# macOS packaging

`install/macos` builds `ObjectiveFrame.app` and wraps it in an installer
package (`.pkg`) that installs into `/Applications`. Signed and notarized, it
can be downloaded and opened without Gatekeeper warnings.

## Building

```sh
cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release
cmake --build build-release --target pkg
```

The result lands in `packages/macos/ObjectiveFrame-<version>-arm64.pkg`.

The target is never part of the default build and is not offered for a Debug
configuration. Without signing identities the app is ad-hoc signed and the
package unsigned, which is enough to test locally but not to distribute.

## Signing and notarization

Distributing outside the Mac App Store needs a paid **Apple Developer Program**
membership and two **Developer ID** certificates. The *Apple Development*
certificate Xcode creates for any Apple ID is only for running on your own
devices; Gatekeeper rejects software signed with it.

| Certificate | Signs | Cache variable |
| ----------- | ----- | -------------- |
| Developer ID Application | the app and the bundled tetgen | `OF_PKG_SIGN_APP` |
| Developer ID Installer | the `.pkg` | `OF_PKG_SIGN_INSTALLER` |

Only the team's Account Holder can create Developer ID certificates (in Xcode:
Settings → Accounts → Manage Certificates, or on developer.apple.com). Once they
are in the login keychain, list their exact names with:

```sh
security find-identity -v -p basic
```

Notarization uploads the package to Apple for an automated malware check and
staples the ticket to it so it also verifies offline. Store credentials once,
using an app-specific password from appleid.apple.com:

```sh
xcrun notarytool store-credentials objectiveframe-notary \
    --apple-id <apple id> --team-id <TEAMID> --password <app-specific password>
```

Then configure and build:

```sh
cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release \
    -DOF_PKG_SIGN_APP="Developer ID Application: <Name> (<TEAMID>)" \
    -DOF_PKG_SIGN_INSTALLER="Developer ID Installer: <Name> (<TEAMID>)" \
    -DOF_PKG_NOTARY_PROFILE=objectiveframe-notary
cmake --build build-release --target pkg
```

Check the result the way Gatekeeper will:

```sh
spctl --assess --type install -vv packages/macos/ObjectiveFrame-*.pkg
```

If notarization is rejected, the build stops and prints the `notarytool log`
command that shows Apple's reasons.

## What goes in the package

| Payload | Source |
| ------- | ------ |
| `Contents/MacOS/objframe` | the Release build |
| `Contents/MacOS/tetgen` | built by `src/tetgen`, or `OF_PKG_TETGEN` |
| `Contents/Resources/{fonts,images,maps,plugins,python,examples,ai}` | `bin/` |
| `Contents/Resources/ObjectiveFrame.icns` | rendered from `images/logo.png` |
| `Contents/Info.plist` | `Info.plist.in` |

Everything is linked statically except system frameworks, so no libraries are
bundled. `FemViewWindow::setProgramPath()` finds the data in
`Contents/Resources` when the executable sits in a bundle and next to the
executable otherwise, so the plain development build is unaffected.

**tetgen.** Meshing runs `tetgen` as a separate program, which the mesher
looks for next to the executable. `src/tetgen` downloads the pinned TetGen
1.6.1 release from github.com/TetGen/Tetgen at configure time and builds it
alongside `objframe` (so the first configure needs network access), and the
package bundles and signs that binary. `OF_PKG_TETGEN` bundles a different
binary instead; `-DOF_BUILD_TETGEN=OFF` skips the download and build, and
without either the package has no mesher - an app started from Finder does not
see a Homebrew `tetgen` on `PATH`.

TetGen is licensed under the AGPLv3, and the package ships it as a separate
program, as the Windows installer already does with `tetgen.exe`. Its licence
terms apply to that binary, including those on making its source available.

The sample models the Windows installer copies into Documents are not
included: a `.pkg` installs as root and has no clean way to write into the
user's home folder.

## Minimum macOS version

The app runs on macOS 13.3 and later. Without a deployment target everything
would be built for the build machine's macOS and the package would refuse to
start anywhere older. The value is set in three places that must agree:

- `CMAKE_OSX_DEPLOYMENT_TARGET` in the top-level `CMakeLists.txt`
- `VCPKG_OSX_DEPLOYMENT_TARGET` in `triplets/arm64-osx.cmake`
- the ivf++ build, configured with `-DCMAKE_OSX_DEPLOYMENT_TARGET=13.3`

The linker warns "built for newer 'macOS' version" when ivf++ is behind.

## Architecture

The package is Apple Silicon only (`arm64`), matching the vcpkg triplet. An
Intel or universal build needs the dependencies and ivf++ built for `x86_64`
as well; set `OF_PKG_ARCH` to match so Installer refuses unsupported Macs
up front.

## Identity and version

`OF_PKG_BUNDLE_ID` (default `se.lu.byggmek.objectiveframe`) is how macOS and
Installer recognise the app across updates. Choose it before the first
release and never change it afterwards.

`OF_PKG_VERSION` defaults to the `project()` version in the top-level
`CMakeLists.txt`, the same source `install/msix` uses.

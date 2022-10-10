; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "ObjectiveFrame"
#define MyAppVersion "2.0.0-b9"
#define MyAppPublisher "Division of Structural Mechanics"
#define MyAppURL "https://jonaslindemann.github.io/objectiveframe/"
#define MyAppExeName "objframe.exe"
#define MyAppAssocName MyAppName + " File"
#define MyAppAssocExt ".df3"
#define MyAppAssocKey StringChange(MyAppAssocName, " ", "") + MyAppAssocExt

[Setup]
; NOTE: The value of AppId uniquely identifies this application. Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{6EA504BD-D534-40C4-9201-867509276D20}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={autopf}\Structural Mechanics\{#MyAppName}
ChangesAssociations=yes
DisableProgramGroupPage=yes
; Uncomment the following line to run in non administrative install mode (install for current user only.)
;PrivilegesRequired=lowest
OutputDir=D:\Users\Jonas\Development\objectiveframe\packages\windows
OutputBaseFilename=objectiveframe_setup
SetupIconFile=D:\Users\Jonas\Development\objectiveframe\images\logo.ico
Compression=lzma
SolidCompression=yes
WizardStyle=modern
SignTool=signtool_lu
SignedUninstaller=yes
ArchitecturesInstallIn64BitMode=x64

[Components]
Name: main; Description: Main Files; Types: full compact; Flags: fixed
Name: samples; Description: Samples; Types: full compact; Flags: fixed

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "..\bin\*.df3"; DestDir: "{userdocs}\ObjectiveFrame Samples"; Flags: ignoreversion; Components: samples
Source: "vc_redist\VC_redist.x64.exe"; DestDir: "{tmp}"; Flags: deleteafterinstall; Components: main
Source: "D:\Users\Jonas\Development\objectiveframe\bin\Release\{#MyAppExeName}"; DestDir: "{app}"; Flags: ignoreversion; Components: main
Source: "D:\Users\Jonas\Development\objectiveframe\bin\fonts\*"; DestDir: "{app}\fonts"; Flags: ignoreversion recursesubdirs createallsubdirs; Components: main
Source: "D:\Users\Jonas\Development\objectiveframe\bin\images\*"; DestDir: "{app}\images"; Flags: ignoreversion recursesubdirs createallsubdirs; Components: main
Source: "D:\Users\Jonas\Development\objectiveframe\bin\maps\*"; DestDir: "{app}\maps"; Flags: ignoreversion recursesubdirs createallsubdirs; Components: main
Source: "D:\Users\Jonas\Development\objectiveframe\bin\plugins\*"; DestDir: "{app}\plugins"; Flags: ignoreversion recursesubdirs createallsubdirs; Components: main
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Registry]
Root: HKA; Subkey: "Software\Classes\{#MyAppAssocExt}\OpenWithProgids"; ValueType: string; ValueName: "{#MyAppAssocKey}"; ValueData: ""; Flags: uninsdeletevalue
Root: HKA; Subkey: "Software\Classes\{#MyAppAssocKey}"; ValueType: string; ValueName: ""; ValueData: "{#MyAppAssocName}"; Flags: uninsdeletekey
Root: HKA; Subkey: "Software\Classes\{#MyAppAssocKey}\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"
Root: HKA; Subkey: "Software\Classes\{#MyAppAssocKey}\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""
Root: HKA; Subkey: "Software\Classes\Applications\{#MyAppExeName}\SupportedTypes"; ValueType: string; ValueName: ".myp"; ValueData: ""

[Icons]
Name: "{autoprograms}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: "{tmp}\VC_redist.x64.exe"; Parameters: "/install /passive";
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent


[Files]
Source: ..\Evan\*.*; Excludes: "dEvan.exe,EvanDebug.exe,*.bat,*d4.dll,qwtd5.dll,qwtplot3dd.dll"; DestDir: {app}\Evan\
Source: ..\Evan\Nodes\*.*; DestDir: {app}\Evan\Nodes\
Source: ..\Evan\Shaders\*.*; DestDir: {app}\Evan\Shaders\
Source: ..\Evan\Images\*.*; DestDir: {app}\Evan\Images\
[Dirs]
Name: {app}\Evan
Name: {app}\Evan\Examples
Name: {app}\Evan\Shaders
Name: {app}\Evan\Nodes
Name: {app}\Evan\Images
[Setup]
AppName=Evan Toolbox
;AppVerName=Evan Toolbox 1.64 (revision 1664)
AppVerName=Evan Toolbox 1.64 (git revision 38)
AppPublisher=EVAN (European Virtual Anthropology Network)
AppPublisherURL=http://www.evan.at/
AppUpdatesURL=http://www.evan-society.org/
DefaultDirName={pf}\Evan Toolbox
OutputBaseFilename=EvanToolboxSetup
DefaultGroupName=Evan Toolbox
LicenseFile=..\Evan\license\evantoolbox_public_license.txt
[Run]
Filename: "{app}\Evan\Evan.exe"; Description: "{cm:LaunchProgram,Evan Toolkit}"; Flags: nowait postinstall skipifsilent
[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
[Icons]
Name: {userdesktop}\Evan Toolbox; Filename: {app}\Evan\Evan.exe; WorkingDir: {app}\Evan\; Tasks: desktopicon
Name: {userappdata}\Microsoft\Internet Explorer\Quick Launch\Evan Toolbox; Filename: {app}\Evan\Evan.exe; WorkingDir: {app}\Evan\; Tasks: quicklaunchicon



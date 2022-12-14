[Files]
;Source: ..\Evan\*.*; Excludes: "dEvan.exe,EvanDebug.exe,EvanQTC.exe,NodeWizard.exe,*.bat,*.qrc,*d4.dll,qwtd5.dll,qwtplot3dd.dll,*.h,*.cpp,Makefile*,*.ltb,*.dat,*.txt,.cproject,.project,Evan.cbp,Evan.depend,Evan.layout,Evan.pro*,Evan.rc,Evan.workspace,*.rar,object_script.*,package-osx"; DestDir: {app}\Evan\
;Source: ..\Evan\Evan.exe; DestDir: {app}\Evan\
Source: ..\Evan\release\Evan.exe; DestDir: {app}\Evan\
;Source: ..\Evan\startEvan.bat; DestDir: {app}\Evan\
Source: ..\Evan\*.dll; Excludes: "Qt*d4.dll,qwtd5.dll,qwtplot3d_d.dll"; DestDir: {app}\Evan\
;Source: ..\Evan\Nodes\*.*; DestDir: {app}\Evan\Nodes\
Source: ..\Evan\Nodes\Nodes.xml; DestDir: {app}\Evan\Nodes\
Source: ..\Evan\Shaders\*.*; DestDir: {app}\Evan\Shaders\
Source: ..\Evan\Images\*.*; DestDir: {app}\Evan\Images\
;Source: ..\Evan\Examples\*.*; Excludes: "-*,Templand\-*,Templand\10LM&15sLMcurve"; DestDir: {app}\Evan\Examples\
Source: ..\Evan\license\*.txt; DestDir: {app}\Evan\
;Source: ..\Evan\Manuals\*.*; DestDir: {app}\Evan\Manuals
Source: ..\Evan\QtPlugins\*.*; Excludes: "*d4.dll"; DestDir: {app}\Evan\QtPlugins; Flags: recursesubdirs
[Dirs]
Name: {app}\Evan
Name: {app}\Evan\Examples
Name: {app}\Evan\Shaders
Name: {app}\Evan\Nodes
Name: {app}\Evan\Images
;Name: {app}\Evan\Manuals
Name: {app}\Evan\QtPlugins
[Setup]
;AppName=Evan Toolbox
AppName=Evan Toolbox 1.71
;AppVerName=Evan Toolbox 1.64 (revision 1664)
;AppVerName=Evan Toolbox 1.64; (git revision 38)
AppVerName=Evan Toolbox 1.71
AppPublisher=EVAN (European Virtual Anthropology Network)
AppPublisherURL=http://www.evan.at/
AppUpdatesURL=http://www.evan-society.org/
;DefaultDirName={pf}\Evan Toolbox
DefaultDirName={pf}\Evan Toolbox 1.71
OutputBaseFilename=EvanToolboxSetup
;DefaultGroupName=Evan Toolbox
DefaultGroupName=Evan Toolbox 1.71
LicenseFile=..\Evan\license\evantoolbox_public_license.txt
[Run]
Filename: "{app}\Evan\Evan.exe"; Description: "{cm:LaunchProgram,Evan Toolkit}"; Flags: nowait postinstall skipifsilent
[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
[Icons]
Name: {userdesktop}\Evan Toolbox; Filename: {app}\Evan\Evan.exe; WorkingDir: {app}\Evan\; Tasks: desktopicon
Name: {userappdata}\Microsoft\Internet Explorer\Quick Launch\Evan Toolbox; Filename: {app}\Evan\Evan.exe; WorkingDir: {app}\Evan\; Tasks: quicklaunchicon



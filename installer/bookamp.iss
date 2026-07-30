; bookamp.iss — instalator wtyczki DSP Bookamp dla Winampa (Inno Setup 6).
; Dwujezyczny (polski + angielski), wykrywa katalog Winampa z rejestru,
; instaluje dsp_bookamp.dll do podkatalogu Plugins. Dostepny dla czytnikow.
;
; Kompilacja: ISCC.exe bookamp.iss  (definiuje sie MyVersion i sciezke do DLL,
; np.: ISCC.exe /DMyVersion=1.0.0 /DPluginDll=..\build\dsp_bookamp.dll bookamp.iss)

#ifndef MyVersion
  #define MyVersion "1.0.0"
#endif
#ifndef PluginDll
  #define PluginDll "..\build\dsp_bookamp.dll"
#endif

[Setup]
AppId={{A7C3E2B1-9D4F-4E88-BB21-BE0A3D9C0001}
AppName=Bookamp
AppVersion={#MyVersion}
AppPublisher=Michał Dziwisz
AppSupportURL=https://github.com/michaldziwisz/bookamp
DefaultDirName={code:GetWinampDir}
DefaultGroupName=Bookamp
DisableProgramGroupPage=yes
DisableDirPage=no
OutputBaseFilename=bookamp-setup-v{#MyVersion}
OutputDir=.
Compression=lzma2
SolidCompression=yes
ArchitecturesInstallIn64BitMode=
WizardStyle=classic
; wtyczka jest 32-bit; instalujemy do katalogu Winampa (tez 32-bit)
UninstallDisplayName=Bookamp (wtyczka DSP Winampa)

[Languages]
Name: "pl"; MessagesFile: "compiler:Languages\Polish.isl"
Name: "en"; MessagesFile: "compiler:Default.isl"

[CustomMessages]
pl.PluginsComponent=Wtyczka DSP Bookamp (dsp_bookamp.dll)
en.PluginsComponent=Bookamp DSP plugin (dsp_bookamp.dll)
pl.WinampNotFound=Nie wykryto zainstalowanego Winampa. Wska\u017C r\u0119cznie katalog Winampa (ten, kt\u00F3ry zawiera podkatalog Plugins).
en.WinampNotFound=Winamp was not detected. Please select the Winamp folder manually (the one that contains the Plugins subfolder).
pl.DirPageDescription=Wybierz katalog Winampa. Wtyczka zostanie zainstalowana do podkatalogu Plugins.
en.DirPageDescription=Choose the Winamp folder. The plugin will be installed into its Plugins subfolder.

[Files]
Source: "{#PluginDll}"; DestDir: "{app}\Plugins"; Flags: ignoreversion
Source: "..\README.md"; DestDir: "{app}\Plugins"; DestName: "Bookamp-README.md"; Flags: ignoreversion
Source: "..\README.pl.md"; DestDir: "{app}\Plugins"; DestName: "Bookamp-README.pl.md"; Flags: ignoreversion
Source: "..\LICENSE"; DestDir: "{app}\Plugins"; DestName: "Bookamp-LICENSE.txt"; Flags: ignoreversion
Source: "..\NOTICE.md"; DestDir: "{app}\Plugins"; DestName: "Bookamp-NOTICE.md"; Flags: ignoreversion

[UninstallDelete]
Type: files; Name: "{app}\Plugins\dsp_bookamp.dll"
Type: files; Name: "{app}\Plugins\Bookamp-README.md"
Type: files; Name: "{app}\Plugins\Bookamp-README.pl.md"
Type: files; Name: "{app}\Plugins\Bookamp-LICENSE.txt"
Type: files; Name: "{app}\Plugins\Bookamp-NOTICE.md"

[Code]
{ Wykrycie katalogu instalacji Winampa z rejestru. Winamp (32-bit) zapisuje
  sciezke w kluczu ...\Winamp, wartosc domyslna = katalog programu. Na 64-bit
  Windows lezy pod WOW6432Node — RegQueryStringValue z HKLM32 trafia tam sam. }
function GetWinampDir(Param: String): String;
var
  Dir: String;
begin
  Result := '';
  if RegQueryStringValue(HKLM32, 'SOFTWARE\Winamp', '', Dir) and (Dir <> '') then
    Result := Dir
  else if RegQueryStringValue(HKCU32, 'SOFTWARE\Winamp', '', Dir) and (Dir <> '') then
    Result := Dir
  else if DirExists(ExpandConstant('{autopf32}\Winamp')) then
    Result := ExpandConstant('{autopf32}\Winamp')
  else
    Result := ExpandConstant('{autopf32}\Winamp');  { rozsadny domyslny }
end;

procedure InitializeWizard();
begin
  { Podpowiedz na stronie wyboru katalogu (dostepne dla czytnika). }
  WizardForm.SelectDirLabel.Caption := ExpandConstant('{cm:DirPageDescription}');
end;

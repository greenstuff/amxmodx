;(C)2004 AMX Mod X Development Team
; Licensed under the GNU General Public License
; Written by -=HaXoMaTiC=-
!define PRODUCT_NAME "AMX Mod X Installer"
!define PRODUCT_VERSION "1.0"
!define PRODUCT_PUBLISHER "AMX Mod X Dev Team"
!define PRODUCT_WEB_SITE "http://www.amxmodx.org/"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\amxmodx.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
!define PRODUCT_STARTMENU_REGVAL "NSIS:StartMenuDir"

SetCompressor lzma

!include "MUI.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "installer\amxmodx.ico"
!define MUI_UNICON "installer\amxmodx.ico"

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page
!define MUI_LICENSEPAGE_RADIOBUTTONS
!insertmacro MUI_PAGE_LICENSE "installer\gpl.txt"
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Start menu page
var ICONS_GROUP
!define MUI_STARTMENUPAGE_NODISABLE
!define MUI_STARTMENUPAGE_DEFAULTFOLDER "AMX Mod X"
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "${PRODUCT_UNINST_ROOT_KEY}"
!define MUI_STARTMENUPAGE_REGISTRY_KEY "${PRODUCT_UNINST_KEY}"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "${PRODUCT_STARTMENU_REGVAL}"
!insertmacro MUI_PAGE_STARTMENU Application $ICONS_GROUP
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
!define MUI_FINISHPAGE_RUN "$INSTDIR\amxmodx.exe"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"

; Reserve files
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS

; MUI end ------

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "amxmodx-installer-${PRODUCT_VERSION}.exe"
InstallDir "$PROGRAMFILES\AMX Mod X"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

Section "MainSection" SEC01
  SetOutPath "$INSTDIR"
  CreateDirectory "$INSTDIR\temp"
  SetOverwrite try
  File "installer\amxmodx.exe"
  SetOutPath "$INSTDIR\files\base\configs"
  File "installer\files\base\configs\amxx.cfg"
  File "installer\files\base\configs\clcmds.ini"
  File "installer\files\base\configs\cmds.ini"
  File "installer\files\base\configs\configs.ini"
  File "installer\files\base\configs\conmotd.txt"
  File "installer\files\base\configs\core.ini"
  File "installer\files\base\configs\custommenuitems.cfg"
  File "installer\files\base\configs\cvars.ini"
  File "installer\files\base\configs\maps.ini"
  File "installer\files\base\configs\modules.ini"
  File "installer\files\base\configs\plugins.ini"
  File "installer\files\base\configs\speech.ini"
  File "installer\files\base\configs\sql.cfg"
  File "installer\files\base\configs\users.ini"
  SetOutPath "$INSTDIR\files\base\data"
  File "installer\files\base\data\GeoIP.dat"
  SetOutPath "$INSTDIR\files\base\data\lang"
  File "installer\files\base\data\lang\admin.txt"
  File "installer\files\base\data\lang\adminchat.txt"
  File "installer\files\base\data\lang\admincmd.txt"
  File "installer\files\base\data\lang\adminhelp.txt"
  File "installer\files\base\data\lang\adminslots.txt"
  File "installer\files\base\data\lang\adminvote.txt"
  File "installer\files\base\data\lang\antiflood.txt"
  File "installer\files\base\data\lang\cmdmenu.txt"
  File "installer\files\base\data\lang\common.txt"
  File "installer\files\base\data\lang\imessage.txt"
  File "installer\files\base\data\lang\languages.txt"
  File "installer\files\base\data\lang\mapchooser.txt"
  File "installer\files\base\data\lang\mapsmenu.txt"
  File "installer\files\base\data\lang\menufront.txt"
  File "installer\files\base\data\lang\miscstats.txt"
  File "installer\files\base\data\lang\multilingual.txt"
  File "installer\files\base\data\lang\nextmap.txt"
  File "installer\files\base\data\lang\pausecfg.txt"
  File "installer\files\base\data\lang\plmenu.txt"
  File "installer\files\base\data\lang\restmenu.txt"
  File "installer\files\base\data\lang\scrollmsg.txt"
  File "installer\files\base\data\lang\statscfg.txt"
  File "installer\files\base\data\lang\statsx.txt"
  File "installer\files\base\data\lang\stats_dod.txt"
  File "installer\files\base\data\lang\telemenu.txt"
  File "installer\files\base\data\lang\timeleft.txt"
  SetOutPath "$INSTDIR\files\base\dlls"
  File "installer\files\base\dlls\amxmodx_mm.dll"
  File "installer\files\base\dlls\amxmodx_mm_amd64.so"
  File "installer\files\base\dlls\amxmodx_mm_i386.so"
  File "installer\files\base\dlls\metamod.dll"
  File "installer\files\base\dlls\metamod_amd64.so"
  File "installer\files\base\dlls\metamod_i386.so"
  SetOutPath "$INSTDIR\files\base\doc"
  File "installer\files\base\doc\amxmodx-doc.chm"
  SetOutPath "$INSTDIR\files\base\modules"
  File "installer\files\base\modules\array_amxx.dll"
  File "installer\files\base\modules\array_amxx_amd64.so"
  File "installer\files\base\modules\array_amxx_i386.so"
  File "installer\files\base\modules\engine_amxx.dll"
  File "installer\files\base\modules\engine_amxx_amd64.so"
  File "installer\files\base\modules\engine_amxx_i386.so"
  File "installer\files\base\modules\fakemeta_amxx.dll"
  File "installer\files\base\modules\fakemeta_amxx_amd64.so"
  File "installer\files\base\modules\fakemeta_amxx_i386.so"
  File "installer\files\base\modules\fun_amxx.dll"
  File "installer\files\base\modules\fun_amxx_amd64.so"
  File "installer\files\base\modules\fun_amxx_i386.so"
  File "installer\files\base\modules\geoip_amxx.dll"
  File "installer\files\base\modules\geoip_amxx_amd64.so"
  File "installer\files\base\modules\geoip_amxx_i386.so"
  File "installer\files\base\modules\mssql_amxx.dll"
  File "installer\files\base\modules\mysql_amxx.dll"
  File "installer\files\base\modules\mysql_amxx_amd64.so"
  File "installer\files\base\modules\mysql_amxx_i386.so"
  File "installer\files\base\modules\pgsql_amxx.dll"
  File "installer\files\base\modules\pgsql_amxx_i386.so"
  File "installer\files\base\modules\regex_amxx.dll"
  File "installer\files\base\modules\regex_amxx_amd64.so"
  File "installer\files\base\modules\regex_amxx_i386.so"
  File "installer\files\base\modules\sockets_amxx.dll"
  File "installer\files\base\modules\sockets_amxx_amd64.so"
  File "installer\files\base\modules\sockets_amxx_i386.so"
  SetOutPath "$INSTDIR\files\base\plugins"
  File "installer\files\base\plugins\admin.amxx"
  File "installer\files\base\plugins\adminchat.amxx"
  File "installer\files\base\plugins\admincmd.amxx"
  File "installer\files\base\plugins\adminhelp.amxx"
  File "installer\files\base\plugins\adminslots.amxx"
  File "installer\files\base\plugins\adminvote.amxx"
  File "installer\files\base\plugins\admin_sql.amxx"
  File "installer\files\base\plugins\antiflood.amxx"
  File "installer\files\base\plugins\cmdmenu.amxx"
  File "installer\files\base\plugins\imessage.amxx"
  File "installer\files\base\plugins\mapchooser.amxx"
  File "installer\files\base\plugins\mapsmenu.amxx"
  File "installer\files\base\plugins\menufront.amxx"
  File "installer\files\base\plugins\multilingual.amxx"
  File "installer\files\base\plugins\nextmap.amxx"
  File "installer\files\base\plugins\pausecfg.amxx"
  File "installer\files\base\plugins\plmenu.amxx"
  File "installer\files\base\plugins\scrollmsg.amxx"
  File "installer\files\base\plugins\statscfg.amxx"
  File "installer\files\base\plugins\telemenu.amxx"
  File "installer\files\base\plugins\timeleft.amxx"
  SetOutPath "$INSTDIR\files\base\scripting"
  File "installer\files\base\scripting\admin.sma"
  File "installer\files\base\scripting\adminchat.sma"
  File "installer\files\base\scripting\admincmd.sma"
  File "installer\files\base\scripting\adminhelp.sma"
  File "installer\files\base\scripting\adminslots.sma"
  File "installer\files\base\scripting\adminvote.sma"
  File "installer\files\base\scripting\amxxsc"
  File "installer\files\base\scripting\amxxsc.exe"
  File "installer\files\base\scripting\amxxsc32.dll"
  File "installer\files\base\scripting\amxxsc32.so"
  File "installer\files\base\scripting\amxxsc64.dll"
  File "installer\files\base\scripting\amxxsc64.so"
  File "installer\files\base\scripting\antiflood.sma"
  File "installer\files\base\scripting\cmdmenu.sma"
  File "installer\files\base\scripting\compile.exe"
  File "installer\files\base\scripting\compile.sh"
  File "installer\files\base\scripting\dlsym"
  File "installer\files\base\scripting\dlsym64"
  File "installer\files\base\scripting\imessage.sma"
  SetOutPath "$INSTDIR\files\base\scripting\include"
  File "installer\files\base\scripting\include\amxconst.inc"
  File "installer\files\base\scripting\include\amxmisc.inc"
  File "installer\files\base\scripting\include\amxmod.inc"
  File "installer\files\base\scripting\include\amxmodx.inc"
  File "installer\files\base\scripting\include\array.inc"
  File "installer\files\base\scripting\include\core.inc"
  File "installer\files\base\scripting\include\csstats.inc"
  File "installer\files\base\scripting\include\cstrike.inc"
  File "installer\files\base\scripting\include\csx.inc"
  File "installer\files\base\scripting\include\dbi.inc"
  File "installer\files\base\scripting\include\dodconst.inc"
  File "installer\files\base\scripting\include\dodfun.inc"
  File "installer\files\base\scripting\include\dodstats.inc"
  File "installer\files\base\scripting\include\dodx.inc"
  File "installer\files\base\scripting\include\engine.inc"
  File "installer\files\base\scripting\include\engine_const.inc"
  File "installer\files\base\scripting\include\engine_stocks.inc"
  File "installer\files\base\scripting\include\fakemeta.inc"
  File "installer\files\base\scripting\include\fakemeta_const.inc"
  File "installer\files\base\scripting\include\file.inc"
  File "installer\files\base\scripting\include\float.inc"
  File "installer\files\base\scripting\include\fun.inc"
  File "installer\files\base\scripting\include\geoip.inc"
  File "installer\files\base\scripting\include\jghg2.inc"
  File "installer\files\base\scripting\include\lang.inc"
  File "installer\files\base\scripting\include\mysql.inc"
  File "installer\files\base\scripting\include\ns.inc"
  File "installer\files\base\scripting\include\ns2amx.inc"
  File "installer\files\base\scripting\include\ns_const.inc"
  File "installer\files\base\scripting\include\regex.inc"
  File "installer\files\base\scripting\include\sockets.inc"
  File "installer\files\base\scripting\include\string.inc"
  File "installer\files\base\scripting\include\tfcconst.inc"
  File "installer\files\base\scripting\include\tfcstats.inc"
  File "installer\files\base\scripting\include\tfcx.inc"
  File "installer\files\base\scripting\include\tsconst.inc"
  File "installer\files\base\scripting\include\tsstats.inc"
  File "installer\files\base\scripting\include\tsx.inc"
  File "installer\files\base\scripting\include\vault.inc"
  File "installer\files\base\scripting\include\Vexd_Utilities.inc"
  File "installer\files\base\scripting\include\xtrafun.inc"
  SetOutPath "$INSTDIR\files\base\scripting"
  File "installer\files\base\scripting\mapchooser.sma"
  File "installer\files\base\scripting\mapsmenu.sma"
  File "installer\files\base\scripting\menufront.sma"
  File "installer\files\base\scripting\multilingual.sma"
  File "installer\files\base\scripting\nextmap.sma"
  File "installer\files\base\scripting\pausecfg.sma"
  File "installer\files\base\scripting\plmenu.sma"
  File "installer\files\base\scripting\scrollmsg.sma"
  File "installer\files\base\scripting\statscfg.sma"
  File "installer\files\base\scripting\telemenu.sma"
  File "installer\files\base\scripting\timeleft.sma"
  SetOutPath "$INSTDIR\files\cstrike\configs"
  File "installer\files\cstrike\configs\amxx.cfg"
  File "installer\files\cstrike\configs\cmds.ini"
  File "installer\files\cstrike\configs\core.ini"
  File "installer\files\cstrike\configs\cvars.ini"
  File "installer\files\cstrike\configs\maps.ini"
  File "installer\files\cstrike\configs\modules.ini"
  File "installer\files\cstrike\configs\plugins.ini"
  File "installer\files\cstrike\configs\stats.ini"
  SetOutPath "$INSTDIR\files\cstrike\data"
  File "installer\files\cstrike\data\csstats.amxx"
  SetOutPath "$INSTDIR\files\cstrike\modules"
  File "installer\files\cstrike\modules\cstrike_amxx.dll"
  File "installer\files\cstrike\modules\cstrike_amxx_amd64.so"
  File "installer\files\cstrike\modules\cstrike_amxx_i386.so"
  File "installer\files\cstrike\modules\csx_amxx.dll"
  File "installer\files\cstrike\modules\csx_amxx_amd64.so"
  File "installer\files\cstrike\modules\csx_amxx_i386.so"
  SetOutPath "$INSTDIR\files\cstrike\plugins"
  File "installer\files\cstrike\plugins\miscstats.amxx"
  File "installer\files\cstrike\plugins\restmenu.amxx"
  File "installer\files\cstrike\plugins\statsx.amxx"
  File "installer\files\cstrike\plugins\stats_logging.amxx"
  SetOutPath "$INSTDIR\files\cstrike\scripting"
  File "installer\files\cstrike\scripting\csstats.sma"
  File "installer\files\cstrike\scripting\miscstats.sma"
  File "installer\files\cstrike\scripting\restmenu.sma"
  File "installer\files\cstrike\scripting\statsx.sma"
  File "installer\files\cstrike\scripting\stats_logging.sma"
  SetOutPath "$INSTDIR\files\dod\configs"
  File "installer\files\dod\configs\core.ini"
  File "installer\files\dod\configs\cvars.ini"
  File "installer\files\dod\configs\maps.ini"
  File "installer\files\dod\configs\modules.ini"
  File "installer\files\dod\configs\plugins.ini"
  SetOutPath "$INSTDIR\files\dod\data"
  File "installer\files\dod\data\dodstats.amxx"
  SetOutPath "$INSTDIR\files\dod\modules"
  File "installer\files\dod\modules\dodfun_amxx.dll"
  File "installer\files\dod\modules\dodfun_amxx_amd64.so"
  File "installer\files\dod\modules\dodfun_amxx_i386.so"
  File "installer\files\dod\modules\dodx_amxx.dll"
  File "installer\files\dod\modules\dodx_amxx_amd64.so"
  File "installer\files\dod\modules\dodx_amxx_i386.so"
  SetOutPath "$INSTDIR\files\dod\plugins"
  File "installer\files\dod\plugins\plmenu.amxx"
  File "installer\files\dod\plugins\stats.amxx"
  File "installer\files\dod\plugins\statssounds.amxx"
  File "installer\files\dod\plugins\stats_logging.amxx"
  SetOutPath "$INSTDIR\files\dod\scripting"
  File "installer\files\dod\scripting\dodstats.sma"
  File "installer\files\dod\scripting\plmenu.sma"
  File "installer\files\dod\scripting\stats.sma"
  File "installer\files\dod\scripting\statssounds.sma"
  File "installer\files\dod\scripting\stats_logging.sma"
  SetOutPath "$INSTDIR\files\ns\configs"
  File "installer\files\ns\configs\amxx.cfg"
  File "installer\files\ns\configs\clcmds.ini"
  File "installer\files\ns\configs\cmds.ini"
  File "installer\files\ns\configs\cvars.ini"
  File "installer\files\ns\configs\maps.ini"
  File "installer\files\ns\configs\modules.ini"
  File "installer\files\ns\configs\plugins.ini"
  File "installer\files\ns\configs\speech.ini"
  File "installer\files\ns\configs\users.ini"
  SetOutPath "$INSTDIR\files\ns\modules"
  File "installer\files\ns\modules\ns_amxx.dll"
  File "installer\files\ns\modules\ns_amxx_amd64.so"
  File "installer\files\ns\modules\ns_amxx_i386.so"
  SetOutPath "$INSTDIR\files\ns\plugins"
  File "installer\files\ns\plugins\idlekicker.amxx"
  File "installer\files\ns\plugins\mapchooser.amxx"
  File "installer\files\ns\plugins\nextmap.amxx"
  File "installer\files\ns\plugins\nscommands.amxx"
  File "installer\files\ns\plugins\timeleft.amxx"
  File "installer\files\ns\plugins\unstuck.amxx"
  SetOutPath "$INSTDIR\files\ns\scripting"
  File "installer\files\ns\scripting\idlekicker.sma"
  File "installer\files\ns\scripting\mapchooser.sma"
  File "installer\files\ns\scripting\nextmap.sma"
  File "installer\files\ns\scripting\nscommands.sma"
  File "installer\files\ns\scripting\timeleft.sma"
  File "installer\files\ns\scripting\unstuck.sma"
  SetOutPath "$INSTDIR\files\ns\root\gfx\vgui"
  File "installer\files\ns\root\gfx\vgui\640_armory.tga"
  File "installer\files\ns\root\gfx\vgui\640_command.tga"
  File "installer\files\ns\root\gfx\vgui\640_gorge.tga"
  File "installer\files\ns\root\gfx\vgui\640_greyalienres.tga"
  File "installer\files\ns\root\gfx\vgui\640_greyhive.tga"
  File "installer\files\ns\root\gfx\vgui\640_greyoc.tga"
  File "installer\files\ns\root\gfx\vgui\640_hive.tga"
  File "installer\files\ns\root\gfx\vgui\640_lerk.tga"
  File "installer\files\ns\root\gfx\vgui\640_medpack.tga"
  File "installer\files\ns\root\gfx\vgui\640_minihive.tga"
  File "installer\files\ns\root\gfx\vgui\640_minimine.tga"
  File "installer\files\ns\root\gfx\vgui\640_minioc.tga"
  File "installer\files\ns\root\gfx\vgui\640_onos.tga"
  File "installer\files\ns\root\gfx\vgui\640_pirate.tga"
  File "installer\files\ns\root\gfx\vgui\640_skulk.tga"
  File "installer\files\ns\root\gfx\vgui\640_turret.tga"
  SetOutPath "$INSTDIR\files\tfc\configs"
  File "installer\files\tfc\configs\core.ini"
  File "installer\files\tfc\configs\cvars.ini"
  File "installer\files\tfc\configs\maps.ini"
  File "installer\files\tfc\configs\modules.ini"
  File "installer\files\tfc\configs\plugins.ini"
  SetOutPath "$INSTDIR\files\tfc\data"
  File "installer\files\tfc\data\tfcstats.amxx"
  SetOutPath "$INSTDIR\files\tfc\modules"
  File "installer\files\tfc\modules\tfcx_amxx.dll"
  File "installer\files\tfc\modules\tfcx_amxx_amd64.so"
  File "installer\files\tfc\modules\tfcx_amxx_i386.so"
  SetOutPath "$INSTDIR\files\tfc\plugins"
  File "installer\files\tfc\plugins\plmenu.amxx"
  File "installer\files\tfc\plugins\stats.amxx"
  File "installer\files\tfc\plugins\statssounds.amxx"
  File "installer\files\tfc\plugins\stats_logging.amxx"
  SetOutPath "$INSTDIR\files\tfc\scripting"
  File "installer\files\tfc\scripting\plmenu.sma"
  File "installer\files\tfc\scripting\stats.sma"
  File "installer\files\tfc\scripting\statssounds.sma"
  File "installer\files\tfc\scripting\stats_logging.sma"
  File "installer\files\tfc\scripting\tfcstats.sma"
  SetOutPath "$INSTDIR\files\ts\configs"
  File "installer\files\ts\configs\core.ini"
  File "installer\files\ts\configs\maps.ini"
  File "installer\files\ts\configs\modules.ini"
  File "installer\files\ts\configs\plugins.ini"
  SetOutPath "$INSTDIR\files\ts\data"
  File "installer\files\ts\data\tsstats.amxx"
  SetOutPath "$INSTDIR\files\ts\modules"
  File "installer\files\ts\modules\tsx_amxx.dll"
  File "installer\files\ts\modules\tsx_amxx_amd64.so"
  File "installer\files\ts\modules\tsx_amxx_i386.so"
  SetOutPath "$INSTDIR\files\ts\plugins"
  File "installer\files\ts\plugins\stats.amxx"
  File "installer\files\ts\plugins\statssounds.amxx"
  File "installer\files\ts\plugins\stats_logging.amxx"
  SetOutPath "$INSTDIR\files\ts\scripting"
  File "installer\files\ts\scripting\stats.sma"
  File "installer\files\ts\scripting\statssounds.sma"
  File "installer\files\ts\scripting\stats_logging.sma"
  File "installer\files\ts\scripting\tsstats.sma"
  SetOutPath "$INSTDIR\"
  File "installer\gpl.txt"

; Shortcuts
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  CreateDirectory "$SMPROGRAMS\$ICONS_GROUP"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\AMX Mod X.lnk" "$INSTDIR\amxmodx.exe"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\Documentation.lnk" "$INSTDIR\files\base\doc\amxmodx-doc.chm"
  CreateShortCut "$DESKTOP.lnk" "$INSTDIR\files\base\doc\amxmodx-doc.chm"
  !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

Section -AdditionalIcons
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\Website.lnk" "$INSTDIR\${PRODUCT_NAME}.url"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\Uninstall.lnk" "$INSTDIR\uninst.exe"
  !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\amxmodx.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\amxmodx.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd


Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer."
FunctionEnd

Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components?" IDYES +2
  Abort
FunctionEnd

Section Uninstall
  !insertmacro MUI_STARTMENU_GETFOLDER "Application" $ICONS_GROUP
  Delete "$INSTDIR\${PRODUCT_NAME}.url"
  Delete "$INSTDIR\uninst.exe"
  Delete "$INSTDIR\gpl.txt"
  Delete "$INSTDIR\files\ts\scripting\tsstats.sma"
  Delete "$INSTDIR\files\ts\scripting\stats_logging.sma"
  Delete "$INSTDIR\files\ts\scripting\statssounds.sma"
  Delete "$INSTDIR\files\ts\scripting\stats.sma"
  Delete "$INSTDIR\files\ts\plugins\stats_logging.amxx"
  Delete "$INSTDIR\files\ts\plugins\statssounds.amxx"
  Delete "$INSTDIR\files\ts\plugins\stats.amxx"
  Delete "$INSTDIR\files\ts\modules\tsx_amxx_i386.so"
  Delete "$INSTDIR\files\ts\modules\tsx_amxx_amd64.so"
  Delete "$INSTDIR\files\ts\modules\tsx_amxx.dll"
  Delete "$INSTDIR\files\ts\data\tsstats.amxx"
  Delete "$INSTDIR\files\ts\configs\plugins.ini"
  Delete "$INSTDIR\files\ts\configs\modules.ini"
  Delete "$INSTDIR\files\ts\configs\maps.ini"
  Delete "$INSTDIR\files\ts\configs\core.ini"
  Delete "$INSTDIR\files\tfc\scripting\tfcstats.sma"
  Delete "$INSTDIR\files\tfc\scripting\stats_logging.sma"
  Delete "$INSTDIR\files\tfc\scripting\statssounds.sma"
  Delete "$INSTDIR\files\tfc\scripting\stats.sma"
  Delete "$INSTDIR\files\tfc\scripting\plmenu.sma"
  Delete "$INSTDIR\files\tfc\plugins\stats_logging.amxx"
  Delete "$INSTDIR\files\tfc\plugins\statssounds.amxx"
  Delete "$INSTDIR\files\tfc\plugins\stats.amxx"
  Delete "$INSTDIR\files\tfc\plugins\plmenu.amxx"
  Delete "$INSTDIR\files\tfc\modules\tfcx_amxx_i386.so"
  Delete "$INSTDIR\files\tfc\modules\tfcx_amxx_amd64.so"
  Delete "$INSTDIR\files\tfc\modules\tfcx_amxx.dll"
  Delete "$INSTDIR\files\tfc\data\tfcstats.amxx"
  Delete "$INSTDIR\files\tfc\configs\plugins.ini"
  Delete "$INSTDIR\files\tfc\configs\modules.ini"
  Delete "$INSTDIR\files\tfc\configs\maps.ini"
  Delete "$INSTDIR\files\tfc\configs\cvars.ini"
  Delete "$INSTDIR\files\tfc\configs\core.ini"
  Delete "$INSTDIR\files\ns\scripting\unstuck.sma"
  Delete "$INSTDIR\files\ns\scripting\timeleft.sma"
  Delete "$INSTDIR\files\ns\scripting\nscommands.sma"
  Delete "$INSTDIR\files\ns\scripting\nextmap.sma"
  Delete "$INSTDIR\files\ns\scripting\mapchooser.sma"
  Delete "$INSTDIR\files\ns\scripting\idlekicker.sma"
  Delete "$INSTDIR\files\ns\plugins\unstuck.amxx"
  Delete "$INSTDIR\files\ns\plugins\timeleft.amxx"
  Delete "$INSTDIR\files\ns\plugins\nscommands.amxx"
  Delete "$INSTDIR\files\ns\plugins\nextmap.amxx"
  Delete "$INSTDIR\files\ns\plugins\mapchooser.amxx"
  Delete "$INSTDIR\files\ns\plugins\idlekicker.amxx"
  Delete "$INSTDIR\files\ns\modules\ns_amxx_i386.so"
  Delete "$INSTDIR\files\ns\modules\ns_amxx_amd64.so"
  Delete "$INSTDIR\files\ns\modules\ns_amxx.dll"
  Delete "$INSTDIR\files\ns\configs\users.ini"
  Delete "$INSTDIR\files\ns\configs\speech.ini"
  Delete "$INSTDIR\files\ns\configs\plugins.ini"
  Delete "$INSTDIR\files\ns\configs\modules.ini"
  Delete "$INSTDIR\files\ns\configs\maps.ini"
  Delete "$INSTDIR\files\ns\configs\cvars.ini"
  Delete "$INSTDIR\files\ns\configs\cmds.ini"
  Delete "$INSTDIR\files\ns\configs\clcmds.ini"
  Delete "$INSTDIR\files\ns\configs\amxx.cfg"
  Delete "$INSTDIR\files\ns\root\gfx\vgui\640_armory.tga"
  Delete "$INSTDIR\files\ns\root\gfx\vgui\640_command.tga"
  Delete "$INSTDIR\files\ns\root\gfx\vgui\640_gorge.tga"
  Delete "$INSTDIR\files\ns\root\gfx\vgui\640_greyalienres.tga"
  Delete "$INSTDIR\files\ns\root\gfx\vgui\640_greyhive.tga"
  Delete "$INSTDIR\files\ns\root\gfx\vgui\640_greyoc.tga"
  Delete "$INSTDIR\files\ns\root\gfx\vgui\640_hive.tga"
  Delete "$INSTDIR\files\ns\root\gfx\vgui\640_lerk.tga"
  Delete "$INSTDIR\files\ns\root\gfx\vgui\640_medpack.tga"
  Delete "$INSTDIR\files\ns\root\gfx\vgui\640_minihive.tga"
  Delete "$INSTDIR\files\ns\root\gfx\vgui\640_minimine.tga"
  Delete "$INSTDIR\files\ns\root\gfx\vgui\640_minioc.tga"
  Delete "$INSTDIR\files\ns\root\gfx\vgui\640_onos.tga"
  Delete "$INSTDIR\files\ns\root\gfx\vgui\640_pirate.tga"
  Delete "$INSTDIR\files\ns\root\gfx\vgui\640_skulk.tga"
  Delete "$INSTDIR\files\ns\root\gfx\vgui\640_turret.tga"
  Delete "$INSTDIR\files\dod\scripting\stats_logging.sma"
  Delete "$INSTDIR\files\dod\scripting\statssounds.sma"
  Delete "$INSTDIR\files\dod\scripting\stats.sma"
  Delete "$INSTDIR\files\dod\scripting\plmenu.sma"
  Delete "$INSTDIR\files\dod\scripting\dodstats.sma"
  Delete "$INSTDIR\files\dod\plugins\stats_logging.amxx"
  Delete "$INSTDIR\files\dod\plugins\statssounds.amxx"
  Delete "$INSTDIR\files\dod\plugins\stats.amxx"
  Delete "$INSTDIR\files\dod\plugins\plmenu.amxx"
  Delete "$INSTDIR\files\dod\modules\dodx_amxx_i386.so"
  Delete "$INSTDIR\files\dod\modules\dodx_amxx_amd64.so"
  Delete "$INSTDIR\files\dod\modules\dodx_amxx.dll"
  Delete "$INSTDIR\files\dod\modules\dodfun_amxx_i386.so"
  Delete "$INSTDIR\files\dod\modules\dodfun_amxx_amd64.so"
  Delete "$INSTDIR\files\dod\modules\dodfun_amxx.dll"
  Delete "$INSTDIR\files\dod\data\dodstats.amxx"
  Delete "$INSTDIR\files\dod\configs\plugins.ini"
  Delete "$INSTDIR\files\dod\configs\modules.ini"
  Delete "$INSTDIR\files\dod\configs\maps.ini"
  Delete "$INSTDIR\files\dod\configs\cvars.ini"
  Delete "$INSTDIR\files\dod\configs\core.ini"
  Delete "$INSTDIR\files\cstrike\scripting\stats_logging.sma"
  Delete "$INSTDIR\files\cstrike\scripting\statsx.sma"
  Delete "$INSTDIR\files\cstrike\scripting\restmenu.sma"
  Delete "$INSTDIR\files\cstrike\scripting\miscstats.sma"
  Delete "$INSTDIR\files\cstrike\scripting\csstats.sma"
  Delete "$INSTDIR\files\cstrike\plugins\stats_logging.amxx"
  Delete "$INSTDIR\files\cstrike\plugins\statsx.amxx"
  Delete "$INSTDIR\files\cstrike\plugins\restmenu.amxx"
  Delete "$INSTDIR\files\cstrike\plugins\miscstats.amxx"
  Delete "$INSTDIR\files\cstrike\modules\csx_amxx_i386.so"
  Delete "$INSTDIR\files\cstrike\modules\csx_amxx_amd64.so"
  Delete "$INSTDIR\files\cstrike\modules\csx_amxx.dll"
  Delete "$INSTDIR\files\cstrike\modules\cstrike_amxx_i386.so"
  Delete "$INSTDIR\files\cstrike\modules\cstrike_amxx_amd64.so"
  Delete "$INSTDIR\files\cstrike\modules\cstrike_amxx.dll"
  Delete "$INSTDIR\files\cstrike\data\csstats.amxx"
  Delete "$INSTDIR\files\cstrike\configs\stats.ini"
  Delete "$INSTDIR\files\cstrike\configs\plugins.ini"
  Delete "$INSTDIR\files\cstrike\configs\modules.ini"
  Delete "$INSTDIR\files\cstrike\configs\maps.ini"
  Delete "$INSTDIR\files\cstrike\configs\cvars.ini"
  Delete "$INSTDIR\files\cstrike\configs\core.ini"
  Delete "$INSTDIR\files\cstrike\configs\cmds.ini"
  Delete "$INSTDIR\files\cstrike\configs\amxx.cfg"
  Delete "$INSTDIR\files\base\scripting\timeleft.sma"
  Delete "$INSTDIR\files\base\scripting\telemenu.sma"
  Delete "$INSTDIR\files\base\scripting\statscfg.sma"
  Delete "$INSTDIR\files\base\scripting\scrollmsg.sma"
  Delete "$INSTDIR\files\base\scripting\plmenu.sma"
  Delete "$INSTDIR\files\base\scripting\pausecfg.sma"
  Delete "$INSTDIR\files\base\scripting\nextmap.sma"
  Delete "$INSTDIR\files\base\scripting\multilingual.sma"
  Delete "$INSTDIR\files\base\scripting\menufront.sma"
  Delete "$INSTDIR\files\base\scripting\mapsmenu.sma"
  Delete "$INSTDIR\files\base\scripting\mapchooser.sma"
  Delete "$INSTDIR\files\base\scripting\include\xtrafun.inc"
  Delete "$INSTDIR\files\base\scripting\include\Vexd_Utilities.inc"
  Delete "$INSTDIR\files\base\scripting\include\vault.inc"
  Delete "$INSTDIR\files\base\scripting\include\tsx.inc"
  Delete "$INSTDIR\files\base\scripting\include\tsstats.inc"
  Delete "$INSTDIR\files\base\scripting\include\tsconst.inc"
  Delete "$INSTDIR\files\base\scripting\include\tfcx.inc"
  Delete "$INSTDIR\files\base\scripting\include\tfcstats.inc"
  Delete "$INSTDIR\files\base\scripting\include\tfcconst.inc"
  Delete "$INSTDIR\files\base\scripting\include\string.inc"
  Delete "$INSTDIR\files\base\scripting\include\sockets.inc"
  Delete "$INSTDIR\files\base\scripting\include\regex.inc"
  Delete "$INSTDIR\files\base\scripting\include\ns_const.inc"
  Delete "$INSTDIR\files\base\scripting\include\ns2amx.inc"
  Delete "$INSTDIR\files\base\scripting\include\ns.inc"
  Delete "$INSTDIR\files\base\scripting\include\mysql.inc"
  Delete "$INSTDIR\files\base\scripting\include\lang.inc"
  Delete "$INSTDIR\files\base\scripting\include\jghg2.inc"
  Delete "$INSTDIR\files\base\scripting\include\geoip.inc"
  Delete "$INSTDIR\files\base\scripting\include\fun.inc"
  Delete "$INSTDIR\files\base\scripting\include\float.inc"
  Delete "$INSTDIR\files\base\scripting\include\file.inc"
  Delete "$INSTDIR\files\base\scripting\include\fakemeta_const.inc"
  Delete "$INSTDIR\files\base\scripting\include\fakemeta.inc"
  Delete "$INSTDIR\files\base\scripting\include\engine_stocks.inc"
  Delete "$INSTDIR\files\base\scripting\include\engine_const.inc"
  Delete "$INSTDIR\files\base\scripting\include\engine.inc"
  Delete "$INSTDIR\files\base\scripting\include\dodx.inc"
  Delete "$INSTDIR\files\base\scripting\include\dodstats.inc"
  Delete "$INSTDIR\files\base\scripting\include\dodfun.inc"
  Delete "$INSTDIR\files\base\scripting\include\dodconst.inc"
  Delete "$INSTDIR\files\base\scripting\include\dbi.inc"
  Delete "$INSTDIR\files\base\scripting\include\csx.inc"
  Delete "$INSTDIR\files\base\scripting\include\cstrike.inc"
  Delete "$INSTDIR\files\base\scripting\include\csstats.inc"
  Delete "$INSTDIR\files\base\scripting\include\core.inc"
  Delete "$INSTDIR\files\base\scripting\include\array.inc"
  Delete "$INSTDIR\files\base\scripting\include\amxmodx.inc"
  Delete "$INSTDIR\files\base\scripting\include\amxmod.inc"
  Delete "$INSTDIR\files\base\scripting\include\amxmisc.inc"
  Delete "$INSTDIR\files\base\scripting\include\amxconst.inc"
  Delete "$INSTDIR\files\base\scripting\imessage.sma"
  Delete "$INSTDIR\files\base\scripting\dlsym64"
  Delete "$INSTDIR\files\base\scripting\dlsym"
  Delete "$INSTDIR\files\base\scripting\compile.sh"
  Delete "$INSTDIR\files\base\scripting\compile.exe"
  Delete "$INSTDIR\files\base\scripting\cmdmenu.sma"
  Delete "$INSTDIR\files\base\scripting\antiflood.sma"
  Delete "$INSTDIR\files\base\scripting\amxxsc64.so"
  Delete "$INSTDIR\files\base\scripting\amxxsc64.dll"
  Delete "$INSTDIR\files\base\scripting\amxxsc32.so"
  Delete "$INSTDIR\files\base\scripting\amxxsc32.dll"
  Delete "$INSTDIR\files\base\scripting\amxxsc.exe"
  Delete "$INSTDIR\files\base\scripting\amxxsc"
  Delete "$INSTDIR\files\base\scripting\adminvote.sma"
  Delete "$INSTDIR\files\base\scripting\adminslots.sma"
  Delete "$INSTDIR\files\base\scripting\adminhelp.sma"
  Delete "$INSTDIR\files\base\scripting\admincmd.sma"
  Delete "$INSTDIR\files\base\scripting\adminchat.sma"
  Delete "$INSTDIR\files\base\scripting\admin.sma"
  Delete "$INSTDIR\files\base\plugins\timeleft.amxx"
  Delete "$INSTDIR\files\base\plugins\telemenu.amxx"
  Delete "$INSTDIR\files\base\plugins\statscfg.amxx"
  Delete "$INSTDIR\files\base\plugins\scrollmsg.amxx"
  Delete "$INSTDIR\files\base\plugins\plmenu.amxx"
  Delete "$INSTDIR\files\base\plugins\pausecfg.amxx"
  Delete "$INSTDIR\files\base\plugins\nextmap.amxx"
  Delete "$INSTDIR\files\base\plugins\multilingual.amxx"
  Delete "$INSTDIR\files\base\plugins\menufront.amxx"
  Delete "$INSTDIR\files\base\plugins\mapsmenu.amxx"
  Delete "$INSTDIR\files\base\plugins\mapchooser.amxx"
  Delete "$INSTDIR\files\base\plugins\imessage.amxx"
  Delete "$INSTDIR\files\base\plugins\cmdmenu.amxx"
  Delete "$INSTDIR\files\base\plugins\antiflood.amxx"
  Delete "$INSTDIR\files\base\plugins\admin_sql.amxx"
  Delete "$INSTDIR\files\base\plugins\adminvote.amxx"
  Delete "$INSTDIR\files\base\plugins\adminslots.amxx"
  Delete "$INSTDIR\files\base\plugins\adminhelp.amxx"
  Delete "$INSTDIR\files\base\plugins\admincmd.amxx"
  Delete "$INSTDIR\files\base\plugins\adminchat.amxx"
  Delete "$INSTDIR\files\base\plugins\admin.amxx"
  Delete "$INSTDIR\files\base\modules\sockets_amxx_i386.so"
  Delete "$INSTDIR\files\base\modules\sockets_amxx_amd64.so"
  Delete "$INSTDIR\files\base\modules\sockets_amxx.dll"
  Delete "$INSTDIR\files\base\modules\regex_amxx_i386.so"
  Delete "$INSTDIR\files\base\modules\regex_amxx_amd64.so"
  Delete "$INSTDIR\files\base\modules\regex_amxx.dll"
  Delete "$INSTDIR\files\base\modules\pgsql_amxx_i386.so"
  Delete "$INSTDIR\files\base\modules\pgsql_amxx.dll"
  Delete "$INSTDIR\files\base\modules\mysql_amxx_i386.so"
  Delete "$INSTDIR\files\base\modules\mysql_amxx_amd64.so"
  Delete "$INSTDIR\files\base\modules\mysql_amxx.dll"
  Delete "$INSTDIR\files\base\modules\mssql_amxx.dll"
  Delete "$INSTDIR\files\base\modules\geoip_amxx_i386.so"
  Delete "$INSTDIR\files\base\modules\geoip_amxx_amd64.so"
  Delete "$INSTDIR\files\base\modules\geoip_amxx.dll"
  Delete "$INSTDIR\files\base\modules\fun_amxx_i386.so"
  Delete "$INSTDIR\files\base\modules\fun_amxx_amd64.so"
  Delete "$INSTDIR\files\base\modules\fun_amxx.dll"
  Delete "$INSTDIR\files\base\modules\fakemeta_amxx_i386.so"
  Delete "$INSTDIR\files\base\modules\fakemeta_amxx_amd64.so"
  Delete "$INSTDIR\files\base\modules\fakemeta_amxx.dll"
  Delete "$INSTDIR\files\base\modules\engine_amxx_i386.so"
  Delete "$INSTDIR\files\base\modules\engine_amxx_amd64.so"
  Delete "$INSTDIR\files\base\modules\engine_amxx.dll"
  Delete "$INSTDIR\files\base\modules\array_amxx_i386.so"
  Delete "$INSTDIR\files\base\modules\array_amxx_amd64.so"
  Delete "$INSTDIR\files\base\modules\array_amxx.dll"
  Delete "$INSTDIR\files\base\doc\amxmodx-doc.chm"
  Delete "$INSTDIR\files\base\dlls\metamod_i386.so"
  Delete "$INSTDIR\files\base\dlls\metamod_amd64.so"
  Delete "$INSTDIR\files\base\dlls\metamod.dll"
  Delete "$INSTDIR\files\base\dlls\amxmodx_mm_i386.so"
  Delete "$INSTDIR\files\base\dlls\amxmodx_mm_amd64.so"
  Delete "$INSTDIR\files\base\dlls\amxmodx_mm.dll"
  Delete "$INSTDIR\files\base\data\lang\timeleft.txt"
  Delete "$INSTDIR\files\base\data\lang\telemenu.txt"
  Delete "$INSTDIR\files\base\data\lang\stats_dod.txt"
  Delete "$INSTDIR\files\base\data\lang\statsx.txt"
  Delete "$INSTDIR\files\base\data\lang\statscfg.txt"
  Delete "$INSTDIR\files\base\data\lang\scrollmsg.txt"
  Delete "$INSTDIR\files\base\data\lang\restmenu.txt"
  Delete "$INSTDIR\files\base\data\lang\plmenu.txt"
  Delete "$INSTDIR\files\base\data\lang\pausecfg.txt"
  Delete "$INSTDIR\files\base\data\lang\nextmap.txt"
  Delete "$INSTDIR\files\base\data\lang\multilingual.txt"
  Delete "$INSTDIR\files\base\data\lang\miscstats.txt"
  Delete "$INSTDIR\files\base\data\lang\menufront.txt"
  Delete "$INSTDIR\files\base\data\lang\mapsmenu.txt"
  Delete "$INSTDIR\files\base\data\lang\mapchooser.txt"
  Delete "$INSTDIR\files\base\data\lang\languages.txt"
  Delete "$INSTDIR\files\base\data\lang\imessage.txt"
  Delete "$INSTDIR\files\base\data\lang\common.txt"
  Delete "$INSTDIR\files\base\data\lang\cmdmenu.txt"
  Delete "$INSTDIR\files\base\data\lang\antiflood.txt"
  Delete "$INSTDIR\files\base\data\lang\adminvote.txt"
  Delete "$INSTDIR\files\base\data\lang\adminslots.txt"
  Delete "$INSTDIR\files\base\data\lang\adminhelp.txt"
  Delete "$INSTDIR\files\base\data\lang\admincmd.txt"
  Delete "$INSTDIR\files\base\data\lang\adminchat.txt"
  Delete "$INSTDIR\files\base\data\lang\admin.txt"
  Delete "$INSTDIR\files\base\data\GeoIP.dat"
  Delete "$INSTDIR\files\base\configs\users.ini"
  Delete "$INSTDIR\files\base\configs\sql.cfg"
  Delete "$INSTDIR\files\base\configs\speech.ini"
  Delete "$INSTDIR\files\base\configs\plugins.ini"
  Delete "$INSTDIR\files\base\configs\modules.ini"
  Delete "$INSTDIR\files\base\configs\maps.ini"
  Delete "$INSTDIR\files\base\configs\cvars.ini"
  Delete "$INSTDIR\files\base\configs\custommenuitems.cfg"
  Delete "$INSTDIR\files\base\configs\core.ini"
  Delete "$INSTDIR\files\base\configs\conmotd.txt"
  Delete "$INSTDIR\files\base\configs\configs.ini"
  Delete "$INSTDIR\files\base\configs\cmds.ini"
  Delete "$INSTDIR\files\base\configs\clcmds.ini"
  Delete "$INSTDIR\files\base\configs\amxx.cfg"
  Delete "$INSTDIR\amxmodx.exe"

  Delete "$SMPROGRAMS\$ICONS_GROUP\Uninstall.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\Website.lnk"
  Delete "$DESKTOP.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\Documentation.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\AMX Mod X.lnk"

  RMDir "$SMPROGRAMS\$ICONS_GROUP"
  RMDir "$INSTDIR\files\ts\scripting"
  RMDir "$INSTDIR\files\ts\plugins"
  RMDir "$INSTDIR\files\ts\modules"
  RMDir "$INSTDIR\files\ts\data"
  RMDir "$INSTDIR\files\ts\configs"
  RMDir "$INSTDIR\files\tfc\scripting"
  RMDir "$INSTDIR\files\tfc\plugins"
  RMDir "$INSTDIR\files\tfc\modules"
  RMDir "$INSTDIR\files\tfc\data"
  RMDir "$INSTDIR\files\tfc\configs"
  RMDir "$INSTDIR\files\ns\scripting"
  RMDir "$INSTDIR\files\ns\plugins"
  RMDir "$INSTDIR\files\ns\modules"
  RMDir "$INSTDIR\files\ns\configs"
  RMDir "$INSTDIR\files\dod\scripting"
  RMDir "$INSTDIR\files\dod\plugins"
  RMDir "$INSTDIR\files\dod\modules"
  RMDir "$INSTDIR\files\dod\data"
  RMDir "$INSTDIR\files\dod\configs"
  RMDir "$INSTDIR\files\cstrike\scripting"
  RMDir "$INSTDIR\files\cstrike\plugins"
  RMDir "$INSTDIR\files\cstrike\modules"
  RMDir "$INSTDIR\files\cstrike\data"
  RMDir "$INSTDIR\files\cstrike\configs"
  RMDir "$INSTDIR\files\base\scripting\include"
  RMDir "$INSTDIR\files\base\scripting"
  RMDir "$INSTDIR\files\base\plugins"
  RMDir "$INSTDIR\files\base\modules"
  RMDir "$INSTDIR\files\base\doc"
  RMDir "$INSTDIR\files\base\dlls"
  RMDir "$INSTDIR\files\base\data\lang"
  RMDir "$INSTDIR\files\base\data"
  RMDir "$INSTDIR\files\base\configs"
  RMDir /r "$INSTDIR"
  RMDir ""

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd

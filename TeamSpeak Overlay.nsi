############################################################################################
#      NSIS Installation Script created by NSIS Quick Setup Script Generator v1.09.18
#               Entirely Edited with NullSoft Scriptable Installation System
#              by Vlasis K. Barkas aka Red Wine red_wine@freemail.gr Sep 2006
############################################################################################

!define APP_NAME "TeamSpeak Overlay"
!define COMP_NAME "PandaDex"
!define WEB_SITE "https://github.com/PandaDex/TeamSpeak-6-Overlay"
!define VERSION "0.1.0.3"
!define COPYRIGHT ""
!define DESCRIPTION "Application"
!define INSTALLER_NAME ".\build\TeamSpeak Overlay.exe"
!define MAIN_APP_EXE "TeamSpeak-Overlay.exe"
!define INSTALL_TYPE "SetShellVarContext all"
!define REG_ROOT "HKLM"
!define REG_APP_PATH "Software\Microsoft\Windows\CurrentVersion\App Paths\${MAIN_APP_EXE}"
!define UNINSTALL_PATH "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}"
!define REG_START_MENU "Start Menu Folder"
!define SOURCE_DIR ".\build\bin"
!define LICENSE_TXT ".\LICENSE"
var SM_Folder

######################################################################

VIProductVersion  "${VERSION}"
VIAddVersionKey "ProductName"  "${APP_NAME}"
VIAddVersionKey "CompanyName"  "${COMP_NAME}"
VIAddVersionKey "LegalCopyright"  "${COPYRIGHT}"
VIAddVersionKey "FileDescription"  "${DESCRIPTION}"
VIAddVersionKey "FileVersion"  "${VERSION}"

######################################################################

SetCompressor ZLIB
Name "${APP_NAME}"
Caption "${APP_NAME}"
OutFile "${INSTALLER_NAME}"
BrandingText "${APP_NAME}"
XPStyle on
InstallDirRegKey "${REG_ROOT}" "${REG_APP_PATH}" ""
InstallDir "$PROGRAMFILES\TeamSpeak Overlay"

######################################################################

!include "MUI.nsh"

!define MUI_ABORTWARNING
!define MUI_UNABORTWARNING

!insertmacro MUI_PAGE_WELCOME

!ifdef LICENSE_TXT
!insertmacro MUI_PAGE_LICENSE "${LICENSE_TXT}"
!endif

!ifdef REG_START_MENU
!define MUI_STARTMENUPAGE_DEFAULTFOLDER "TeamSpeak Overlay"
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "${REG_ROOT}"
!define MUI_STARTMENUPAGE_REGISTRY_KEY "${UNINSTALL_PATH}"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "${REG_START_MENU}"
!insertmacro MUI_PAGE_STARTMENU Application $SM_Folder
!endif

!insertmacro MUI_PAGE_INSTFILES

!define MUI_FINISHPAGE_RUN "$INSTDIR\${MAIN_APP_EXE}"
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM

!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_UNPAGE_FINISH

!insertmacro MUI_LANGUAGE "English"

######################################################################

Section -MainProgram
${INSTALL_TYPE}
SetOverwrite ifnewer
SetOutPath "$INSTDIR"
File "${SOURCE_DIR}\D3Dcompiler_47.dll"
File "${SOURCE_DIR}\libgcc_s_seh-1.dll"
File "${SOURCE_DIR}\libstdc++-6.dll"
File "${SOURCE_DIR}\libwinpthread-1.dll"
File "${SOURCE_DIR}\opengl32sw.dll"
File "${SOURCE_DIR}\Qt6Core.dll"
File "${SOURCE_DIR}\Qt6Gui.dll"
File "${SOURCE_DIR}\Qt6Network.dll"
File "${SOURCE_DIR}\Qt6Svg.dll"
File "${SOURCE_DIR}\Qt6WebSockets.dll"
File "${SOURCE_DIR}\Qt6Widgets.dll"
File "${SOURCE_DIR}\TeamSpeak-Overlay.exe"
SetOutPath "$INSTDIR\translations"
File "${SOURCE_DIR}\translations\qt_ar.qm"
File "${SOURCE_DIR}\translations\qt_bg.qm"
File "${SOURCE_DIR}\translations\qt_ca.qm"
File "${SOURCE_DIR}\translations\qt_cs.qm"
File "${SOURCE_DIR}\translations\qt_da.qm"
File "${SOURCE_DIR}\translations\qt_de.qm"
File "${SOURCE_DIR}\translations\qt_en.qm"
File "${SOURCE_DIR}\translations\qt_es.qm"
File "${SOURCE_DIR}\translations\qt_fa.qm"
File "${SOURCE_DIR}\translations\qt_fi.qm"
File "${SOURCE_DIR}\translations\qt_fr.qm"
File "${SOURCE_DIR}\translations\qt_gd.qm"
File "${SOURCE_DIR}\translations\qt_he.qm"
File "${SOURCE_DIR}\translations\qt_hr.qm"
File "${SOURCE_DIR}\translations\qt_hu.qm"
File "${SOURCE_DIR}\translations\qt_it.qm"
File "${SOURCE_DIR}\translations\qt_ja.qm"
File "${SOURCE_DIR}\translations\qt_ka.qm"
File "${SOURCE_DIR}\translations\qt_ko.qm"
File "${SOURCE_DIR}\translations\qt_lv.qm"
File "${SOURCE_DIR}\translations\qt_nl.qm"
File "${SOURCE_DIR}\translations\qt_nn.qm"
File "${SOURCE_DIR}\translations\qt_pl.qm"
File "${SOURCE_DIR}\translations\qt_pt_BR.qm"
File "${SOURCE_DIR}\translations\qt_ru.qm"
File "${SOURCE_DIR}\translations\qt_sk.qm"
File "${SOURCE_DIR}\translations\qt_tr.qm"
File "${SOURCE_DIR}\translations\qt_uk.qm"
File "${SOURCE_DIR}\translations\qt_zh_CN.qm"
File "${SOURCE_DIR}\translations\qt_zh_TW.qm"
SetOutPath "$INSTDIR\tls"
File "${SOURCE_DIR}\tls\qcertonlybackend.dll"
File "${SOURCE_DIR}\tls\qschannelbackend.dll"
SetOutPath "$INSTDIR\styles"
File "${SOURCE_DIR}\styles\qmodernwindowsstyle.dll"
SetOutPath "$INSTDIR\platforms"
File "${SOURCE_DIR}\platforms\qwindows.dll"
SetOutPath "$INSTDIR\networkinformation"
File "${SOURCE_DIR}\networkinformation\qnetworklistmanager.dll"
SetOutPath "$INSTDIR\imageformats"
File "${SOURCE_DIR}\imageformats\qgif.dll"
File "${SOURCE_DIR}\imageformats\qico.dll"
File "${SOURCE_DIR}\imageformats\qjpeg.dll"
File "${SOURCE_DIR}\imageformats\qsvg.dll"
SetOutPath "$INSTDIR\iconengines"
File "${SOURCE_DIR}\iconengines\qsvgicon.dll"
SetOutPath "$INSTDIR\generic"
File "${SOURCE_DIR}\generic\qtuiotouchplugin.dll"
SectionEnd

######################################################################

Section -Icons_Reg
SetOutPath "$INSTDIR"
WriteUninstaller "$INSTDIR\uninstall.exe"

!ifdef REG_START_MENU
!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
CreateDirectory "$SMPROGRAMS\$SM_Folder"
CreateShortCut "$SMPROGRAMS\$SM_Folder\${APP_NAME}.lnk" "$INSTDIR\${MAIN_APP_EXE}"
CreateShortCut "$DESKTOP\${APP_NAME}.lnk" "$INSTDIR\${MAIN_APP_EXE}"
!ifdef WEB_SITE
WriteIniStr "$INSTDIR\${APP_NAME} website.url" "InternetShortcut" "URL" "${WEB_SITE}"
CreateShortCut "$SMPROGRAMS\$SM_Folder\${APP_NAME} Website.lnk" "$INSTDIR\${APP_NAME} website.url"
!endif
!insertmacro MUI_STARTMENU_WRITE_END
!endif

!ifndef REG_START_MENU
CreateDirectory "$SMPROGRAMS\TeamSpeak Overlay"
CreateShortCut "$SMPROGRAMS\TeamSpeak Overlay\${APP_NAME}.lnk" "$INSTDIR\${MAIN_APP_EXE}"
CreateShortCut "$DESKTOP\${APP_NAME}.lnk" "$INSTDIR\${MAIN_APP_EXE}"
!ifdef WEB_SITE
WriteIniStr "$INSTDIR\${APP_NAME} website.url" "InternetShortcut" "URL" "${WEB_SITE}"
CreateShortCut "$SMPROGRAMS\TeamSpeak Overlay\${APP_NAME} Website.lnk" "$INSTDIR\${APP_NAME} website.url"
!endif
!endif

WriteRegStr ${REG_ROOT} "${REG_APP_PATH}" "" "$INSTDIR\${MAIN_APP_EXE}"
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "DisplayName" "${APP_NAME}"
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "UninstallString" "$INSTDIR\uninstall.exe"
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "DisplayIcon" "$INSTDIR\${MAIN_APP_EXE}"
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "DisplayVersion" "${VERSION}"
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "Publisher" "${COMP_NAME}"

!ifdef WEB_SITE
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "URLInfoAbout" "${WEB_SITE}"
!endif
SectionEnd

######################################################################

Section Uninstall
${INSTALL_TYPE}
Delete "$INSTDIR\D3Dcompiler_47.dll"
Delete "$INSTDIR\libgcc_s_seh-1.dll"
Delete "$INSTDIR\libstdc++-6.dll"
Delete "$INSTDIR\libwinpthread-1.dll"
Delete "$INSTDIR\opengl32sw.dll"
Delete "$INSTDIR\Qt6Core.dll"
Delete "$INSTDIR\Qt6Gui.dll"
Delete "$INSTDIR\Qt6Network.dll"
Delete "$INSTDIR\Qt6Sql.dll"
Delete "$INSTDIR\Qt6Svg.dll"
Delete "$INSTDIR\Qt6WebSockets.dll"
Delete "$INSTDIR\Qt6Widgets.dll"
Delete "$INSTDIR\TeamSpeak-Overlay.exe"
Delete "$INSTDIR\translations\qt_ar.qm"
Delete "$INSTDIR\translations\qt_bg.qm"
Delete "$INSTDIR\translations\qt_ca.qm"
Delete "$INSTDIR\translations\qt_cs.qm"
Delete "$INSTDIR\translations\qt_da.qm"
Delete "$INSTDIR\translations\qt_de.qm"
Delete "$INSTDIR\translations\qt_en.qm"
Delete "$INSTDIR\translations\qt_es.qm"
Delete "$INSTDIR\translations\qt_fa.qm"
Delete "$INSTDIR\translations\qt_fi.qm"
Delete "$INSTDIR\translations\qt_fr.qm"
Delete "$INSTDIR\translations\qt_gd.qm"
Delete "$INSTDIR\translations\qt_he.qm"
Delete "$INSTDIR\translations\qt_hr.qm"
Delete "$INSTDIR\translations\qt_hu.qm"
Delete "$INSTDIR\translations\qt_it.qm"
Delete "$INSTDIR\translations\qt_ja.qm"
Delete "$INSTDIR\translations\qt_ka.qm"
Delete "$INSTDIR\translations\qt_ko.qm"
Delete "$INSTDIR\translations\qt_lv.qm"
Delete "$INSTDIR\translations\qt_nl.qm"
Delete "$INSTDIR\translations\qt_nn.qm"
Delete "$INSTDIR\translations\qt_pl.qm"
Delete "$INSTDIR\translations\qt_pt_BR.qm"
Delete "$INSTDIR\translations\qt_ru.qm"
Delete "$INSTDIR\translations\qt_sk.qm"
Delete "$INSTDIR\translations\qt_tr.qm"
Delete "$INSTDIR\translations\qt_uk.qm"
Delete "$INSTDIR\translations\qt_zh_CN.qm"
Delete "$INSTDIR\translations\qt_zh_TW.qm"
Delete "$INSTDIR\tls\qcertonlybackend.dll"
Delete "$INSTDIR\tls\qschannelbackend.dll"
Delete "$INSTDIR\styles\qmodernwindowsstyle.dll"
Delete "$INSTDIR\sqldrivers\qsqlibase.dll"
Delete "$INSTDIR\sqldrivers\qsqlite.dll"
Delete "$INSTDIR\sqldrivers\qsqlmimer.dll"
Delete "$INSTDIR\sqldrivers\qsqloci.dll"
Delete "$INSTDIR\sqldrivers\qsqlodbc.dll"
Delete "$INSTDIR\sqldrivers\qsqlpsql.dll"
Delete "$INSTDIR\platforms\qwindows.dll"
Delete "$INSTDIR\networkinformation\qnetworklistmanager.dll"
Delete "$INSTDIR\imageformats\qgif.dll"
Delete "$INSTDIR\imageformats\qico.dll"
Delete "$INSTDIR\imageformats\qjpeg.dll"
Delete "$INSTDIR\imageformats\qsvg.dll"
Delete "$INSTDIR\iconengines\qsvgicon.dll"
Delete "$INSTDIR\generic\qtuiotouchplugin.dll"

RmDir "$INSTDIR\generic"
RmDir "$INSTDIR\iconengines"
RmDir "$INSTDIR\imageformats"
RmDir "$INSTDIR\networkinformation"
RmDir "$INSTDIR\platforms"
RmDir "$INSTDIR\sqldrivers"
RmDir "$INSTDIR\styles"
RmDir "$INSTDIR\tls"
RmDir "$INSTDIR\translations"

Delete "$INSTDIR\uninstall.exe"
!ifdef WEB_SITE
Delete "$INSTDIR\${APP_NAME} website.url"
!endif

RmDir "$INSTDIR"

!ifdef REG_START_MENU
!insertmacro MUI_STARTMENU_GETFOLDER "Application" $SM_Folder
Delete "$SMPROGRAMS\$SM_Folder\${APP_NAME}.lnk"
!ifdef WEB_SITE
Delete "$SMPROGRAMS\$SM_Folder\${APP_NAME} Website.lnk"
!endif
Delete "$DESKTOP\${APP_NAME}.lnk"

RmDir "$SMPROGRAMS\$SM_Folder"
!endif

!ifndef REG_START_MENU
Delete "$SMPROGRAMS\TeamSpeak Overlay\${APP_NAME}.lnk"
!ifdef WEB_SITE
Delete "$SMPROGRAMS\TeamSpeak Overlay\${APP_NAME} Website.lnk"
!endif
Delete "$DESKTOP\${APP_NAME}.lnk"

RmDir "$SMPROGRAMS\TeamSpeak Overlay"
!endif

DeleteRegKey ${REG_ROOT} "${REG_APP_PATH}"
DeleteRegKey ${REG_ROOT} "${UNINSTALL_PATH}"
SectionEnd

######################################################################

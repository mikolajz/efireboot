QT += core gui widgets
TARGET = efireboot-gui
TEMPLATE = app
SOURCES += efirebootgui.cpp
HEADERS += efi.h \
           reboot.h

unix: {
    LIBS += -lefivar
    SOURCES += efi-linux.c \
               reboot-linux.c
}

win32: {
    SOURCES += efi-windows.c \
               reboot-windows.c \
               wincommon.c
    LIBS = "kernel32.lib" "user32.lib" "gdi32.lib" "winspool.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "odbc32.lib" "odbccp32.lib"
    HEADERS += wincommon.h
    RC_FILE = efireboot-gui.rc
}

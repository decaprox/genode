QUPZILLA = QupZilla-1.3.5

# identify the qt4 repository by searching for a file that is unique for qt4
QT4_REP_DIR := $(call select_from_repositories,lib/import/import-qt4.inc)

ifeq ($(QT4_REP_DIR),)
REQUIRES += qt4
endif

QT4_REP_DIR := $(realpath $(dir $(QT4_REP_DIR))../..)

-include $(QT4_REP_DIR)/src/app/tmpl/target_defaults.inc

QT_MAIN_STACK_SIZE = 768*1024

LIBS += libc_lwip libc_lwip_nic_dhcp

#
# Prevent contrib code from causing warnings with our toolchain compiler
#
CC_WARN += -Wno-unused-but-set-variable

vpath % $(PRG_DIR)

vpath % $(REP_DIR)/contrib/$(QUPZILLA)/src
vpath % $(REP_DIR)/contrib/$(QUPZILLA)/src/main
vpath % $(REP_DIR)/contrib/$(QUPZILLA)/src/lib
vpath % $(REP_DIR)/contrib/$(QUPZILLA)/src/lib/3rdparty
vpath % $(REP_DIR)/contrib/$(QUPZILLA)/src/lib/adblock
vpath % $(REP_DIR)/contrib/$(QUPZILLA)/src/lib/app
vpath % $(REP_DIR)/contrib/$(QUPZILLA)/src/lib/autofill
vpath % $(REP_DIR)/contrib/$(QUPZILLA)/src/lib/bookmarks
vpath % $(REP_DIR)/contrib/$(QUPZILLA)/src/lib/bookmarksimport
vpath % $(REP_DIR)/contrib/$(QUPZILLA)/src/lib/cookies
vpath % $(REP_DIR)/contrib/$(QUPZILLA)/src/lib/data
vpath % $(REP_DIR)/contrib/$(QUPZILLA)/src/lib/desktopnotifications
vpath % $(REP_DIR)/contrib/$(QUPZILLA)/src/lib/downloads
vpath % $(REP_DIR)/contrib/$(QUPZILLA)/src/lib/history
vpath % $(REP_DIR)/contrib/$(QUPZILLA)/src/lib/navigation
vpath % $(REP_DIR)/contrib/$(QUPZILLA)/src/lib/navigation/completer
vpath % $(REP_DIR)/contrib/$(QUPZILLA)/src/lib/network
vpath % $(REP_DIR)/contrib/$(QUPZILLA)/src/lib/network/schemehandlers
vpath % $(REP_DIR)/contrib/$(QUPZILLA)/src/lib/opensearch
vpath % $(REP_DIR)/contrib/$(QUPZILLA)/src/lib/other
vpath % $(REP_DIR)/contrib/$(QUPZILLA)/src/lib/plugins
vpath % $(REP_DIR)/contrib/$(QUPZILLA)/src/lib/popupwindow
vpath % $(REP_DIR)/contrib/$(QUPZILLA)/src/lib/preferences
vpath % $(REP_DIR)/contrib/$(QUPZILLA)/src/lib/rss
vpath % $(REP_DIR)/contrib/$(QUPZILLA)/src/lib/session
vpath % $(REP_DIR)/contrib/$(QUPZILLA)/src/lib/sidebar
vpath % $(REP_DIR)/contrib/$(QUPZILLA)/src/lib/tools
vpath % $(REP_DIR)/contrib/$(QUPZILLA)/src/lib/webview
vpath % $(REP_DIR)/contrib/$(QUPZILLA)/src/lib/data

-include $(QT4_REP_DIR)/src/app/tmpl/target_final.inc

INC_DIR += $(INCLUDEPATH)


include $(REP_DIR)/lib/import/import-qt_sql.mk

SHARED_LIB = yes

# extracted from src/script/Makefile
QT_DEFINES += -DQT_BUILD_SQL_LIB -DQT_NO_USING_NAMESPACE -DQT_NO_CAST_TO_ASCII -DQT_ASCII_CAST_WARNINGS -DQT_MOC_COMPAT -DQT_USE_FAST_OPERATOR_PLUS -DQT_USE_FAST_CONCATENATION -DQT_NO_DEBUG -DQT_CORE_LIB
# use default warning level to avoid noise when compiling contrib code
CC_WARN =

include $(REP_DIR)/lib/mk/qt_sql_generated.inc

# remove unneeded files to prevent moc warnings
#COMPILER_MOC_HEADER_MAKE_ALL_FILES_FILTER_OUT =

#COMPILER_MOC_SOURCE_MAKE_ALL_FILES_FILTER_OUT =

INC_DIR += $(REP_DIR)/contrib/$(QT4)/include/QtSql\
	$(REP_DIR)/contrib/$(QT4)/include/QtSql/private

LIBS += qt_core libc sqlite3

vpath % $(REP_DIR)/include/qt4/QtSql
vpath % $(REP_DIR)/include/qt4/QtSql/private

vpath % $(REP_DIR)/src/lib/qt4/src/sql/drivers/sqlite
vpath % $(REP_DIR)/src/lib/qt4/src/sql/kernel
vpath % $(REP_DIR)/src/lib/qt4/src/sql/models

vpath % $(REP_DIR)/contrib/$(QT4)/src/sql/drivers/sqlite
vpath % $(REP_DIR)/contrib/$(QT4)/src/sql/kernel
vpath % $(REP_DIR)/contrib/$(QT4)/src/sql/models

include $(REP_DIR)/lib/mk/qt.inc

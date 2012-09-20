include ports/sqlite3.inc

SQLITE_ZIP = $(SQLITE).zip
SQLITE_URL = http://www.sqlite.org/$(SQLITE_ZIP)

#
# Interface to top-level prepare Makefile
#
PORTS += $(SQLITE)

#
# Check for tools
#
$(call check_tool,unzip)

prepare-sqlite3: $(CONTRIB_DIR)/$(SQLITE) include/sqlite3

$(CONTRIB_DIR)/$(SQLITE): clean-sqlite3

#
# Port-specific local rules
#
$(DOWNLOAD_DIR)/$(SQLITE_ZIP):
	$(VERBOSE)wget -c -P $(DOWNLOAD_DIR) $(SQLITE_URL) && touch $@

$(CONTRIB_DIR)/$(SQLITE): $(DOWNLOAD_DIR)/$(SQLITE_ZIP)
	$(VERBOSE)unzip $< -d $(CONTRIB_DIR) && touch $@

include/sqlite3:
	$(VERBOSE)mkdir -p $@
	$(VERBOSE)ln -s $(addprefix ../../, $(wildcard $(CONTRIB_DIR)/$(SQLITE)/sqlite3.h)) -t $@

clean-sqlite3:
	$(VERBOSE)rm -rf $(CONTRIB_DIR)/$(SQLITE)
	$(VERBOSE)rm -rf include/sqlite3
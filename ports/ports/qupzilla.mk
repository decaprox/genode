QIPZILLA     = QupZilla-1.3.1
QUPZILLA_TGZ = $(QIPZILLA).tar.gz
QIPZILLA_URL = https://github.com/downloads/QupZilla/qupzilla/$(QUPZILLA_TGZ)

#
# Interface to top-level prepare Makefile
#
PORTS += $(QIPZILLA)

prepare:: $(CONTRIB_DIR)/$(QIPZILLA)

#
# Port-specific local rules
#
$(DOWNLOAD_DIR)/$(QUPZILLA_TGZ):
	$(VERBOSE)wget -c -P $(DOWNLOAD_DIR) $(QIPZILLA_URL) && touch $@

$(CONTRIB_DIR)/$(QIPZILLA): $(DOWNLOAD_DIR)/$(QUPZILLA_TGZ)
	$(VERBOSE)tar xfz $< -C $(CONTRIB_DIR) && touch $@
	$(VERBOSE)patch -d $@ -p1 -i ../../src/app/qupzilla/qupzilla_genode.patch
	$(VERBOSE)cp src/app/qupzilla/themes/default/theme.png $(CONTRIB_DIR)/$(QIPZILLA)/src/lib/data/themes/default/
	$(VERBOSE)cp -R src/app/qupzilla/themes/default/images $(CONTRIB_DIR)/$(QIPZILLA)/src/lib/data/themes/default/

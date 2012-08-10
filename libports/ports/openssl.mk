OPENSSL_VERSION = 1.0.1c
OPENSSL         = openssl-$(OPENSSL_VERSION)
OPENSSL_TGZ     = $(OPENSSL).tar.gz
OPENSSL_URL     = https://www.openssl.org/source/$(OPENSSL_TGZ)

# local openssl src
OPENSSL_SRC     = src/lib/openssl

#
# Interface to top-level prepare Makefile
#
PORTS += $(OPENSSL)

prepare-openssl: $(CONTRIB_DIR)/$(OPENSSL) include/openssl

#$(CONTRIB_DIR)/$(OPENSSL):

#
# Port-specific local rules
#
$(DOWNLOAD_DIR)/$(OPENSSL_TGZ):
	$(VERBOSE)wget -c -P $(DOWNLOAD_DIR) $(OPENSSL_URL) && touch $@

$(CONTRIB_DIR)/$(OPENSSL): $(DOWNLOAD_DIR)/$(OPENSSL_TGZ)
	$(VERBOSE)tar xfz $< -C $(CONTRIB_DIR) && touch $@
	$(VERBOSE)patch -d $(CONTRIB_DIR)/$(OPENSSL) -p1 -i ../../src/lib/openssl/openssl_conf.patch
	$(VERBOSE)patch -d $(CONTRIB_DIR)/$(OPENSSL) -p1 -i ../../src/lib/openssl/openssl_rand.patch

#
# Install openssl headers
#
include/openssl:
	$(VERBOSE)rm -rf $@
	$(VERBOSE)mkdir -p $@
	$(VERBOSE)cp src/lib/openssl/opensslconf.h $(CONTRIB_DIR)/$(OPENSSL)/crypto
	$(VERBOSE)for i in `find $(CONTRIB_DIR)/$(OPENSSL)/include -name *.h`; do \
		ln -fs ../../$$i include/openssl/; done
	$(VERBOSE)ln -fs ../../$(CONTRIB_DIR)/$(OPENSSL)/e_os.h include/openssl/

clean-openssl:
	$(VERBOSE)rm -rf include/openssl
	$(VERBOSE)rm -rf $(CONTRIB_DIR)/$(OPENSSL)

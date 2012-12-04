LIBGOST     = libgost-1.0.0
LIBOPENSSL_DIR = $(REP_DIR)/contrib/openssl-1.0.1c


LIBS      += libc libcrypto


SRC_C = engines/ccgost/e_gost_err.c \
		engines/ccgost/gost2001_keyx.c \
		engines/ccgost/gost2001.c \
		engines/ccgost/gost89.c \
		engines/ccgost/gost94_keyx.c \
		engines/ccgost/gost_ameth.c \
		engines/ccgost/gost_asn1.c \
		engines/ccgost/gost_crypt.c \
		engines/ccgost/gost_ctl.c \
		engines/ccgost/gost_eng.c \
		engines/ccgost/gosthash.c \
		engines/ccgost/gost_keywrap.c \
		engines/ccgost/gost_md.c \
		engines/ccgost/gost_params.c \
		engines/ccgost/gost_pmeth.c \
		engines/ccgost/gost_sign.c

vpath %.c $(LIBOPENSSL_DIR)/


SHARED_LIB = yes

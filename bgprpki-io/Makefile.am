ACLOCAL_AMFLAGS = -I m4

.PHONY: distclean-local rpms srcrpm

SUBDIRS = $(CUNIT_TESTS)
CLEAN_SUBDIRS  = .libs .deps
EXTRA_DIST = $(PACKAGE_TARNAME).spec.in

distclean-local:
	rm -f *.spec;            \
	rm -f bgprpki-*.rpm;    \
	rm -f bgprpki-*.tar.gz; \
	rm -rf autom4te.cache;

if LIB_VER_INFO_COND
  LIB_VER = $(LIB_VER_INFO)
else
  LIB_VER = 0:0:0
endif

################################################################################
##  TEMPORARY LIBRARIES
################################################################################
# Temporary Libraries

noinst_LTLIBRARIES = libantd_util.la libbrio_bgp.la libsrx_common.la

##=======================
## ANTD-UTIL
##=======================
libantd_util_la_SOURCES = \
          antd-util/log.c         \
          antd-util/prefix.c      \
          antd-util/linked_list.c \
          antd-util/stack.c       \
          antd-util/printer.c     

libantd_util_la_LDFLAGS = -version-info $(LIB_VER)

##=======================
## SRX-COMMON
##=======================
libsrx_common_la_SOURCES = \
          srx-common/debug.c         \
          srx-common/mutex.c         \
          srx-common/rwlock.c        \
          srx-common/slist.c         \
          srx-common/socket.c        \
          srx-common/str.c           \
          srx-common/timer.c

libsrx_common_la_CFLAGS  = -DBRIO
libsrx_common_la_LDFLAGS = -version-info $(LIB_VER)

##=======================
## BGP / BGPSEC Libraries
##=======================
libbrio_bgp_la_SOURCES = \
          bgpsec/Crypto.c                      \
          bgpsec/BGPSecPathBin.c               \
          bgp/BGPFinalStateMachine.c           \
          bgp/BGPHeader.c                      \
          bgp/printer/BGPPrinterUtil.c         \
          bgp/printer/BGPUpdatePrinter.c       \
          bgp/printer/BGPNotificationPrinter.c \
          bgp/printer/BGPOpenPrinter.c         \
          bgp/printer/BGPHeaderPrinter.c       \
          bgp/BGPSession.c

libbrio_bgp_la_CFLAGS = $(SCA_CFLAGS) $(OPENSSL_CFLAGS) $(OS_CFLAGS)
libbrio_bgp_la_LDFLAGS = $(LDFLAGS) $(SCA_LDFLAGS) $(OPENSSL_LDFLAGS)

################################################################################
##  PROGRAM FILES
################################################################################

briodir=$(bindir)
brio_PROGRAMS = brio_tg brio_rc 

AM_FLAGS = -DPRG_VERSION=@MAJOR_VER@.@MINOR_VER@

##=======================
## BRIO-RC
##=======================

brio_rc_SOURCES = \
          srx-common/server_socket.c      \
          rpki-rtr-protocol/rpkirtr_svr.c

brio_rc_LDADD = \
          libantd_util.la \
          libbrio_bgp.la  \
          libsrx_common.la

brio_rc_CFLAGS  = $(SCA_CFLAGS) $(OS_CFLAGS) -DBRIO

brio_rc_LDFLAGS = $(LDFLAGS) \
          -ldl           \
          -lrt           \
          -lconfig       \
          $(LIBS)

##=======================
## BRIO-TG
##=======================

brio_tg_SOURCES = \
          ASList.c            \
          ASNTokenizer.c      \
          updateStackUtil.c   \
          cfg/configuration.c \
          cfg/cfgFile.c       \
          player/player.c     \
          bgpsecio.c

brio_tg_LDADD = libantd_util.la libbrio_bgp.la

brio_tg_CFLAGS = $(SCA_CFLAGS) $(OPENSSL_CFLAGS) $(OS_CFLAGS)  -DBRIO

brio_tg_LDFLAGS = $(LDFLAGS) $(SCA_LDFLAGS) $(OPENSSL_LDFLAGS) \
          -ldl           \
          -lrt           \
          -lconfig       \
          @OPENSSL_LIBS@ \
          $(SCA_LIBS)    \
          $(LIBS)

################################################################################
##  HEADER FILE MANAGEMENT
################################################################################

noinst_HEADERS = \
          player/player.h                      \
          updateStackUtil.h                    \
          ASList.h                             \
          cfg/cfgFile.h                        \
          cfg/configuration.h                  \
          ASNTokenizer.h                       \
          bgpsec/Crypto.h                      \
          bgpsec/BGPSecPathBin.h               \
          bgp/BGPHeader.h                      \
          bgp/printer/BGPNotificationPrinter.h \
          bgp/printer/BGPHeaderPrinter.h       \
          bgp/printer/BGPPrinterUtil.h         \
          bgp/printer/BGPOpenPrinter.h         \
          bgp/printer/BGPUpdatePrinter.h       \
          bgp/BGPSession.h                     \
          bgp/BGPFinalStateMachine.h           \
          config.h                             \
          antd-util/printer.h                  \
          antd-util/prefix.h                   \
          antd-util/linked_list.h              \
          antd-util/log.h                      \
          antd-util/stack.h                    

# Crypto Key generation tools
dist_bin_SCRIPTS = tools/brio-traffic.sh \
                   tools/mrt_to_brio.sh

################################################################################
##  RPM Section
################################################################################


# RPM script and macros for BGPsec-IO
RPM = rpm

# RPM script and macros for BGPsec-IO
rpmspec = $(PACKAGE_TARNAME)-$(PACKAGE_VERSION).spec

rpmmacros =\
 --define="_rpmdir $${PWD}"\
 --define="_srcrpmdir $${PWD}"\
 --define="_sourcedir $${PWD}"\
 --define="_specdir $${PWD}"\
 --define="_builddir $${PWD}"

RPMBUILD = rpmbuild
#RPMFLAGS = --nodeps --buildroot="$${PWD}/_rpm"
RPMFLAGS = --buildroot="$${PWD}/_rpm"

rpmcheck:
	if [ which rpmbuild &> /dev/null ]; then \
	  echo "*** This make target requires an rpm-based linux distribution."; \
	  (exit 1); exit 1; \
	fi

srcrpm: rpmcheck $(rpmspec)
	($(RPMBUILD) $(RPMFLAGS) -bs $(rpmmacros) $(rpmspec)) || exit 1

rpms: dist rpmcheck $(rpmspec)
	($(RPMBUILD) $(RPMFLAGS) -bb $(rpmmacros) $(rpmspec)) || exit 1
	rpmarch=`$(RPM) --showrc | grep "^build arch" | sed 's/\(.*: \)\(.*\)/\2/'`; \
	test -z "$$rpmarch" || ( mv $$rpmarch/* . && rm -rf $$rpmarch ); \
	rm -rf $(distdir)

dist-hook:
	rm -rf `find $(distdir) -name .svn`

# Allows README.md in lieu of README. Another option is to declare the project
# as foreign in AUTOMAKE_OPTIONS in configure.ac or create a symbolic link 
# from README to README.md
README: README.md

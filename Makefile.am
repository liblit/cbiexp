AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I /usr/share/aclocal

SUBDIRS = src

if HAVE_MEX
SUBDIRS += correlations
endif

DIST_SUBDIRS = correlations sanity src
EXTRA_DIST = bin

check-local:
	$(MAKE) -C sanity check

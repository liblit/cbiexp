AM_CPPFLAGS = -W -Wall -Werror -Wformat=2 $(BOOST_CPPFLAGS)

noinst_PROGRAMS =				\
	make-location-data

make_location_data_SOURCES = make_location_data.cc
make_location_data_LDADD = ../../libanalyze.a -ldl

force:
.PHONY: force

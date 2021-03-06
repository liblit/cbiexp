# arch-tag: Rhythmbox RPM specification

%define gstreamer_version 0.6.0
%define libvorbis_version 1.0
%define gtk2_version 2.2.0
%define libgnomeui_version 2.2.0

Name:		rhythmbox
Summary:	Music Management Application 
Version:	0.6.5
Release:	1
License:	GPL
Group:		Applications/Multimedia
Source:		%{name}-%{version}.tar.gz
BuildRoot:	%{_tmppath}/%{name}-%{version}-root
Requires:	gtk2 >= %{gtk2_version}
Requires:	libgnomeui >= %{libgnomeui_version}
Requires:	gstreamer >= %{gstreamer_version}
Requires:	gstreamer-plugins >= %{gstreamer_version}
Requires:	libvorbis >= %{libvorbis_version}

Prereq:         GConf2
Prereq:         /usr/bin/gconftool-2

BuildRequires:  gstreamer-devel >= %{gstreamer_version}
BuildRequires:  gstreamer-plugins-devel >= %{gstreamer_version}
BuildRequires:  gtk2-devel >= %{gtk2_version}

%description
A program for organizing and playing back your music.  It supports
most popular music formats, also supports extracting CDs.

%prep
%setup -q

%build

%configure

make %{?_smp_mflags}

%install
rm -rf %{buildroot}

export GCONF_DISABLE_MAKEFILE_SCHEMA_INSTALL=1;
%makeinstall
unset GCONF_DISABLE_MAKEFILE_SCHEMA_INSTALL

%find_lang %name

%clean
rm -rf %{buildroot}

%post
/sbin/ldconfig
GCONF_CONFIG_SOURCE=`gconftool-2 --get-default-source` gconftool-2 --makefile-install-rule %{_sysconfdir}/gconf/schemas/rhythmbox.schemas > /dev/null;

%postun -p /sbin/ldconfig

%files -f %name.lang
%defattr(-, root, root)
%doc AUTHORS COPYING ChangeLog INSTALL README NEWS
%{_bindir}/rhythmbox
%{_sysconfdir}/gconf/schemas/rhythmbox.schemas
%{_datadir}/rhythmbox/*
%{_datadir}/applications/rhythmbox.desktop
%{_datadir}/application-registry/rhythmbox.applications
%{_datadir}/pixmaps/rhythmbox.png
%{_datadir}/locale/*
%{_datadir}/gnome-2.0/ui/*.xml
%{_datadir}/idl/Rhythmbox.idl
%{_libdir}/bonobo/librb-nautilus-context-menu.*
%{_libdir}/bonobo/servers/*
%{_libdir}/pkgconfig/*
%{_datadir}/gnome/help/rhythmbox/C/*
%{_datadir}/omf/rhythmbox/rhythmbox-C.omf
%{_datadir}/mime-info/rhythmbox.keys

%changelog
* Fri Nov 07 2003 Colin Walters <walters@verbum.org>
- Fix description, bump required gst version

* Wed Sep 10 2003 William Jon McCann <mccann@jhu.edu>
- Added missing files

* Thu Aug 14 2003 William Jon McCann <mccann@jhu.edu>
- Don't install Rhythmbox.h.

* Wed Aug 13 2003 William Jon McCann <mccann@jhu.edu>
- Updated dependencies and files

* Sun Oct 20 2002 Christian F.K. Schaller <Uraeus@linuxrising.org>
- Added documentation

* Thu Oct 10 2002 Christian F.K. Schaller <Uraeus@linuxrising.org>
- Added .applications file, rpmbuild complains about .mo files being listed twice, dont see how.

* Sat Jun 22 2002 Christian F.K. Schaller <Uraeus@linuxrising.org>
- Added gconf file
- Added i18n directory

* Sat Jun 15 2002 Christian F.K. Schaller <Uraeus@linuxrising.org>
- Updated for new rewrite of rhythmbox, thanks to Jeroen

* Mon Mar 18 2002 Jorn Baayen <jorn@nl.linux.org>
- removed bonobo dependency
* Sat Mar 02 2002 Christian Schaller <Uraeus@linuxrising.org>
- created new spec file

Name:		purple-telegram
Version:	1.2.0
Release:	3%{?dist}
Summary:	Adds support for Libpurple based messengers
Group:		Applications/Internet
License:	GPLv2+
URL:		https://github.com/majn/telegram-purple
Source0:	https://codeload.github.com/majn/telegram-purple/tar.gz/v%{version}.tar.gz

BuildRequires:	openssl-devel,glib2-devel,libpurple-devel,libwebp-devel
Requires:	openssl

%description
Adds support for Telegram to Pidgin, Adium, Finch 
and other Libpurple based messengers.

%prep
%autosetup
#Remove executable perm from soure files.
find . -type  f \( -name "*.c" -or -name "*.h" \)|xargs chmod -x 2>/dev/null

%build
%configure
make %{?_smp_mflags}

%install
%make_install
chmod 755 %{buildroot}/%{_libdir}/purple-2/telegram-purple.so

%files
%doc README* CHANGELOG* 
%{_libdir}/purple-2/telegram-purple.so
%config %{_sysconfdir}/telegram-purple/*
%{_datadir}/pixmaps/pidgin/protocols/16/telegram.png
%{_datadir}/pixmaps/pidgin/protocols/22/telegram.png
%{_datadir}/pixmaps/pidgin/protocols/48/telegram.png

%changelog
* Wed Sep 02 2015 tuxmaster 1.2.0-3
- fix rpmlint errors

* Mon Aug 31 2015 tuxmaster 1.2.0-2
- fix debug package

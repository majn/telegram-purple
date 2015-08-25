Name: 		purple-telegram
Version: 	1.2.0
Release:	1%{?dist}
Summary:	Adds support for Telegram to Pidgin, Adium, Finch and other Libpurple based messengers.
Group:		Internet/Messaging
License:	GPLv2+
URL:		https://github.com/majn/telegram-purple
Source0:	https://codeload.github.com/majn/telegram-purple/tar.gz/v%{version}.tar.gz

BuildRequires:	openssl-devel,glib2-devel,libpurple-devel,libwebp-devel
Requires:	openssl,glib2,libpurple,libwebp

%description

%prep
%autosetup

%build
%configure
make %{?_smp_mflags}

%install
%make_install

%files
%doc
%{_libdir}/purple-2/telegram-purple.so
%{_sysconfdir}/telegram-purple/*
%{_datadir}/pixmaps/pidgin/protocols/16/telegram.png
%{_datadir}/pixmaps/pidgin/protocols/22/telegram.png
%{_datadir}/pixmaps/pidgin/protocols/48/telegram.png

%changelog

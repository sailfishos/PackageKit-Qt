Summary:   Qt support library for PackageKit
Name:      PackageKit-Qt5
Version:   0.9.6+git
Release:   1
License:   LGPLv2+
Group:     System/Libraries
URL:       http://www.packagekit.org
Source0:   http://www.packagekit.org/releases/%{name}-%{version}.tar.xz

Requires: PackageKit >= 1.1.7
BuildRequires: pkgconfig(Qt5Core)
BuildRequires: pkgconfig(Qt5DBus)
BuildRequires: cmake >= 2.8.7
BuildRequires: PackageKit >= 1.1.7

Patch1:  0001-Avoid-deadlock-on-disconnecting-signals.patch
Patch2:  0002-Map-DBusError-AccessDenied-to-Transaction-ErrorNotAu.patch

%description
PackageKit-qt is a Qt support library for PackageKit

%package devel
Summary: Development headers for PackageKit-Qt
License: LGPLv2+
Group: Development/Libraries
Requires: %{name} = %{version}-%{release}
Requires: pkgconfig
# These requirements are because of the line:
#     Requires: Qt5Core, Qt5DBus, Qt5Xml
# in the pkg-config file from:
#     PackageKit-Qt/src/packagekit-qt5.pc.in
Requires: pkgconfig(Qt5Core)
Requires: pkgconfig(Qt5DBus)
Requires: pkgconfig(Qt5Xml)

%description devel
Development headers and libraries for PackageKit-Qt.

%prep
%setup -q -n %{name}-%{version}/upstream
%patch1 -p1
%patch2 -p1

%build
rm -f CMakeCache.txt && mkdir -p build && cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_INSTALL_LIBDIR:PATH=/usr/lib ..
make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
cd build
make DESTDIR=%{buildroot} install

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%doc COPYING
%{_libdir}/libpackagekitqt5.so.*

%files devel
%defattr(-,root,root,-)
%doc AUTHORS COPYING NEWS
%{_libdir}/libpackagekitqt*.so
%{_libdir}/pkgconfig/packagekitqt5.pc
%dir %{_includedir}/packagekitqt5/PackageKit/
%{_includedir}/packagekitqt5/PackageKit/*
%dir %{_libdir}/cmake/packagekitqt5
%{_libdir}/cmake/packagekitqt5/*.cmake

Summary:   Qt support library for PackageKit
Name:      PackageKit-Qt5
Version:   1.0.2
Release:   1
License:   LGPLv2+
URL:       http://www.packagekit.org
Source0:   %{name}-%{version}.tar.xz

Requires: PackageKit >= 1.1.7
BuildRequires: pkgconfig(Qt5Core)
BuildRequires: pkgconfig(Qt5DBus)
BuildRequires: cmake >= 2.8.7
BuildRequires: PackageKit >= 1.1.7

Patch1:  0001-Add-import-and-remove-gpg-key-commands.patch
Patch2:  0002-Add-compatibility-include-dir-for-projects-developed.patch

%description
PackageKit-qt is a Qt support library for PackageKit

%package devel
Summary: Development headers for PackageKit-Qt
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
%autosetup -p1 -n %{name}-%{version}/upstream

%build
mkdir -p build && pushd build
%cmake  ..
%make_build

%install
pushd build
%make_install

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%license COPYING
%{_libdir}/libpackagekitqt5.so.*

%files devel
%defattr(-,root,root,-)
%doc AUTHORS NEWS
%{_libdir}/libpackagekitqt*.so
%{_libdir}/pkgconfig/packagekitqt5.pc
%dir %{_includedir}/packagekitqt5/PackageKit/
%{_includedir}/packagekitqt5/PackageKit/*
%dir %{_libdir}/cmake/packagekitqt5
%{_libdir}/cmake/packagekitqt5/*.cmake

Summary:   Qt support library for PackageKit
Name:      PackageKit-Qt5
Version:   0.8.8+nemo5
Release:   1
License:   LGPLv2+
Group:     System/Libraries
URL:       http://www.packagekit.org
Source0:   http://www.packagekit.org/releases/%{name}-%{version}.tar.xz
Patch1:    001-initial-port-to-Qt5-compiles-some-cleaning-up-of-cma.patch
Patch2:    002-cleaned-include-directive.patch
Patch3:    003-edited-cmake-files-to-support-Qt4-and-Qt5.patch
Patch4:    004-dont-use-automoc-for-Qt5-as-it-would-not-work-with-c.patch
Patch5:    005-removed-unneccessary-line.patch
Patch6:    006-fixed-pkgconfig-file-for-Qt5-to-require-the-proper-m.patch
Patch7:    007-properly-link-dynamically-against-the-Qt5-modules-in.patch
Patch8:    008-changed-library-name-to-packagekitqt5-for-Qt5-build-.patch
Patch9:    009-added-method-for-retrieving-desktop-files-from-cache.patch
Patch10:   010-Added-method-for-retrieving-the-package-name-by-desk.patch
Patch11:   011-Dont-use-QSqlQuerysize-because-it-may-not-be-support.patch
Patch12:   012-packagekitqt-Build-with-Qt-56-Contributes-to-JB35409.patch

Requires: PackageKit >= 0.8.9
BuildRequires: pkgconfig(Qt5Core)
BuildRequires: pkgconfig(Qt5DBus)
BuildRequires: pkgconfig(Qt5Sql)
BuildRequires: cmake >= 2.8.7
BuildRequires: PackageKit >= 0.8.9

%description
PackageKit-qt is a Qt support library for PackageKit

%package devel
Summary: Development headers for PackageKit-Qt
License: LGPLv2+
Group: Development/Libraries
Requires: %{name} = %{version}-%{release}
Requires: pkgconfig
# These requirements are because of the line:
#     Requires: Qt5Core, Qt5DBus, Qt5Sql, Qt5Xml
# in the pkg-config file from:
#     PackageKit-Qt/src/packagekit-qt5.pc.in
Requires: pkgconfig(Qt5Core)
Requires: pkgconfig(Qt5DBus)
Requires: pkgconfig(Qt5Sql)
Requires: pkgconfig(Qt5Xml)

%description devel
Development headers and libraries for PackageKit-Qt.

%prep
%setup -q -n %{name}-%{version}/upstream
%patch1 -p1
%patch2 -p1
%patch3 -p1
%patch4 -p1
%patch5 -p1
%patch6 -p1
%patch7 -p1
%patch8 -p1
%patch9 -p1
%patch10 -p1
%patch11 -p1
%patch12 -p1

%build
rm -f CMakeCache.txt && mkdir -p build && cd build
cmake -DUSE_QT5=ON ..
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
%{_libdir}/*packagekit-qt5.so.*

%files devel
%defattr(-,root,root,-)
%doc AUTHORS COPYING NEWS
%{_libdir}/libpackagekit-qt*.so
%{_libdir}/pkgconfig/packagekit-qt5.pc
%dir %{_includedir}/PackageKit/packagekit-qt5
%{_includedir}/PackageKit/packagekit-qt5/*
%dir %{_libdir}/cmake/packagekit-qt5
%{_libdir}/cmake/packagekit-qt5/*.cmake

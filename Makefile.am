# Subdirectories to build
SUBDIRS = data docs po src

desktopdir = $(datarootdir)/applications

desktop_DATA = manaplus.desktop \
	     manaplustest.desktop

appdatadir = $(datarootdir)/metainfo

appdata_DATA = manaplus.metainfo.xml

# Extra files to include
EXTRA_DIST = $(desktop_DATA) \
	     $(appdata_DATA) \
	     CMakeLists.txt \
	     packaging/arch/PKGBUILD \
	     packaging/debian/manaplus.menu \
	     packaging/openSUSE_Build_Service/manaverse.spec \
	     po/CMakeLists.txt \
	     po/*.po \
	     CMake/Modules/FindLibIntl.cmake \
	     CMake/Modules/FindSDL_gfx.cmake \
	     Android.mk \
	     AUTHORS \
	     ChangeLog \
	     INSTALL \
	     README

# Autopoint m4 stuff
ACLOCAL_AMFLAGS = -I m4

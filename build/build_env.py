#!/usr/bin/env python3
import argparse
import subprocess

from pyfastogt import system_info, build_utils
from check_plugins import check_plugins

# Script for building environment on clean machine

GLIB_SRC_ROOT = "http://ftp.acc.umu.se/pub/gnome/sources/glib"
GLIB_ARCH_COMP = "xz"
GLIB_ARCH_EXT = "tar." + GLIB_ARCH_COMP

GSTREAMER_SRC_ROOT = "https://gstreamer.freedesktop.org/src/"
GSTREAMER_ARCH_COMP = "xz"
GSTREAMER_ARCH_EXT = "tar." + GSTREAMER_ARCH_COMP

GST_PLUGINS_BASE_SRC_ROOT = GSTREAMER_SRC_ROOT
GST_PLUGINS_BASE_ARCH_COMP = "xz"
GST_PLUGINS_BASE_ARCH_EXT = "tar." + GST_PLUGINS_BASE_ARCH_COMP

GST_PLUGINS_GOOD_SRC_ROOT = GSTREAMER_SRC_ROOT
GST_PLUGINS_GOOD_ARCH_COMP = "xz"
GST_PLUGINS_GOOD_ARCH_EXT = "tar." + GST_PLUGINS_GOOD_ARCH_COMP

GST_PLUGINS_BAD_SRC_ROOT = GSTREAMER_SRC_ROOT
GST_PLUGINS_BAD_ARCH_COMP = "xz"
GST_PLUGINS_BAD_ARCH_EXT = "tar." + GST_PLUGINS_BAD_ARCH_COMP

GST_PLUGINS_UGLY_SRC_ROOT = GSTREAMER_SRC_ROOT
GST_PLUGINS_UGLY_ARCH_COMP = "xz"
GST_PLUGINS_UGLY_ARCH_EXT = "tar." + GST_PLUGINS_UGLY_ARCH_COMP

GST_LIBAV_SRC_ROOT = GSTREAMER_SRC_ROOT
GST_LIBAV_ARCH_COMP = "xz"
GST_LIBAV_ARCH_EXT = "tar." + GST_LIBAV_ARCH_COMP

GLIB_NETWORKING_SRC_ROOT = "https://ftp.gnome.org/pub/GNOME/sources/glib-networking"
GLIB_NETWORKING_ARCH_COMP = "xz"
GLIB_NETWORKING_ARCH_EXT = "tar." + GLIB_NETWORKING_ARCH_COMP


class BuildRequest(build_utils.BuildRequest):
    def __init__(self, platform, arch_name, dir_path, prefix_path):
        build_utils.BuildRequest.__init__(self, platform, arch_name, dir_path, prefix_path)

    def get_system_libs(self):
        platform = self.platform_
        platform_name = platform.name()
        dep_libs = []

        if platform_name == 'linux':
            distribution = system_info.linux_get_dist()
            if distribution == 'DEBIAN':
                dep_libs = ['gcc', 'g++', 'git', 'make', 'autoconf', 'libtool', 'pkg-config', 'gettext',
                            'libcairo2-dev', 'libssl-dev', 'libblkid-dev',
                            'libmount-dev', 'libdrm-dev', 'libsoup2.4-dev', 'libudev-dev', 'libjpeg-dev',
                            # 'freeglut3-dev',
                            # 'libegl1-mesa-dev',
                            'zlib1g-dev', 'libffi-dev', 'yasm', 'nasm', 'bison', 'flex', 'libxrandr-dev',
                            'libfaac-dev', 'libfaad-dev', 'libgdk-pixbuf2.0-dev',
                            'ninja-build', 'intltool', 'liborc-0.4-dev', 'libxml2-dev', 'libx264-dev',
                            'libmp3lame-dev', 'librtmp-dev', 'libproxy-dev']
            elif distribution == 'RHEL':
                dep_libs = ['gcc', 'gcc-c++', 'git', 'make', 'autoconf', 'libtool', 'cairo-gobject-devel',
                            'libmount-devel', 'openssl-devel',
                            'libxcb-devel', 'libdrm-devel', 'libsoup-devel', 'libx264-devel',  # 'libpciaccess-devel',
                            'libudev-devel', 'libjpeg-turbo-devel', 'zlib-devel', 'libffi-devel', 'pcre-devel', 'yasm',
                            'nasm', 'bison', 'flex', 'alsa-lib-devel', 'ninja-build', 'lame-devel', 'librtmp-devel',
                            'gdk-pixbuf2-devel',
                            'libproxy-devel']
        else:
            raise NotImplemented("Unknown platform '%s'" % platform_name)

        return dep_libs

    def install_system(self):
        dep_libs = self.get_system_libs()
        for lib in dep_libs:
            self._install_package(lib)

        # post install step
        platform = self.platform()
        platform_name = platform.name()
        if platform_name == 'linux':
            distribution = system_info.linux_get_dist()
            if distribution == 'RHEL':
                subprocess.call(['ln', '-sf', '/usr/bin/ninja-build', '/usr/bin/ninja'])

    def build_glib(self, version):
        glib_version_short = version[:version.rfind('.')]
        url = '{0}/{1}/glib-{2}.{3}'.format(GLIB_SRC_ROOT, glib_version_short,
                                            version, GLIB_ARCH_EXT)
        self._download_and_build_via_meson(url, ['--buildtype=release'])

    def build_glib_networking(self, version):
        glib_version_short = version[:version.rfind('.')]
        url = '{0}/{1}/glib-networking-{2}.{3}'.format(GLIB_NETWORKING_SRC_ROOT, glib_version_short,
                                                       version, GLIB_NETWORKING_ARCH_EXT)
        self._download_and_build_via_meson(url, ['--buildtype=release', '-Dopenssl=enabled'])

    def build_gstreamer(self, version):
        url = '{0}gstreamer/gstreamer-{1}.{2}'.format(GSTREAMER_SRC_ROOT, version, GSTREAMER_ARCH_EXT)
        self._download_and_build_via_meson(url, ['--buildtype=release'])

    def build_gst_plugins_base(self, version):
        url = '{0}gst-plugins-base/gst-plugins-base-{1}.{2}'.format(GST_PLUGINS_BASE_SRC_ROOT, version,
                                                                    GST_PLUGINS_BASE_ARCH_EXT)
        self._download_and_build_via_meson(url, ['--buildtype=release'])

    def build_gst_plugins_good(self, version):
        url = '{0}gst-plugins-good/gst-plugins-good-{1}.{2}'.format(GST_PLUGINS_GOOD_SRC_ROOT, version,
                                                                    GST_PLUGINS_GOOD_ARCH_EXT)
        self._download_and_build_via_meson(url, ['--buildtype=release'])

    def build_gst_plugins_bad(self, version):
        url = '{0}gst-plugins-bad/gst-plugins-bad-{1}.{2}'.format(GST_PLUGINS_BAD_SRC_ROOT, version,
                                                                  GST_PLUGINS_BAD_ARCH_EXT)
        self._download_and_build_via_meson(url, ['--buildtype=release'])

    def build_gst_plugins_ugly(self, version):
        url = '{0}gst-plugins-ugly/gst-plugins-ugly-{1}.{2}'.format(GST_PLUGINS_UGLY_SRC_ROOT, version,
                                                                    GST_PLUGINS_UGLY_ARCH_EXT)
        self._download_and_build_via_meson(url, ['--buildtype=release'])

    def build_gst_libav(self, version):
        compiler_flags = []
        url = build_utils.generate_fastogt_git_path('gst-libav')
        self._clone_and_build_via_autogen(url, compiler_flags)


if __name__ == "__main__":
    # openssl_default_version = '1.1.1b'
    glib_default_version = '2.60.2'
    cmake_default_version = '3.4.0'
    meson_default_version = '0.50.1'
    gstreamer_default_version = '1.16.0'
    gst_plugins_base_default_version = gstreamer_default_version
    gst_plugins_good_default_version = gstreamer_default_version
    gst_plugins_bad_default_version = gstreamer_default_version
    gst_plugins_ugly_default_version = gstreamer_default_version
    gst_libav_default_version = gstreamer_default_version

    host_os = system_info.get_os()
    arch_host_os = system_info.get_arch_name()

    parser = argparse.ArgumentParser(prog='build_env', usage='%(prog)s [options]')
    # system
    system_grp = parser.add_mutually_exclusive_group()
    system_grp.add_argument('--with-system', help='build with system dependencies (default)', dest='with_system',
                            action='store_true', default=True)
    system_grp.add_argument('--without-system', help='build without system dependencies', dest='with_system',
                            action='store_false', default=False)

    # cmake
    cmake_grp = parser.add_mutually_exclusive_group()
    cmake_grp.add_argument('--with-cmake', help='build cmake (default, version:{0})'.format(meson_default_version),
                           dest='with_cmake', action='store_true', default=True)
    cmake_grp.add_argument('--without-cmake', help='build without cmake', dest='with_cmake', action='store_false',
                           default=False)
    parser.add_argument('--cmake-version', help='cmake version (default: {0})'.format(cmake_default_version),
                        default=cmake_default_version)

    # meson
    meson_grp = parser.add_mutually_exclusive_group()
    meson_grp.add_argument('--with-meson', help='build meson (default, version:{0})'.format(meson_default_version),
                           dest='with_meson', action='store_true', default=True)
    meson_grp.add_argument('--without-meson', help='build without meson', dest='with_meson', action='store_false',
                           default=False)
    parser.add_argument('--meson-version', help='meson version (default: {0})'.format(meson_default_version),
                        default=meson_default_version)

    # json-c
    jsonc_grp = parser.add_mutually_exclusive_group()
    jsonc_grp.add_argument('--with-json-c', help='build json-c (default, version: git master)', dest='with_jsonc',
                           action='store_true', default=True)
    jsonc_grp.add_argument('--without-json-c', help='build without json-c', dest='with_jsonc', action='store_false',
                           default=False)

    # libev
    libev_grp = parser.add_mutually_exclusive_group()
    libev_grp.add_argument('--with-libev-c', help='build libev (default, version: git master)', dest='with_libev',
                           action='store_true', default=True)
    libev_grp.add_argument('--without-libev-c', help='build without libev', dest='with_libev', action='store_false',
                           default=False)

    # common
    common_grp = parser.add_mutually_exclusive_group()
    common_grp.add_argument('--with-common', help='build common (default, version: git master)', dest='with_common',
                            action='store_true', default=True)
    common_grp.add_argument('--without-common', help='build without common', dest='with_common',
                            action='store_false',
                            default=False)

    # fastotv_protocol
    fastotv_protocol_grp = parser.add_mutually_exclusive_group()
    fastotv_protocol_grp.add_argument('--with-fastotv-protocol',
                                      help='build fastotv_protocol (default, version: git master)',
                                      dest='with_fastotv_protocol',
                                      action='store_true', default=True)
    fastotv_protocol_grp.add_argument('--without-fastotv-protocol', help='build without fastotv_protocol',
                                      dest='with_fastotv_protocol',
                                      action='store_false',
                                      default=False)

    # glib
    glib_grp = parser.add_mutually_exclusive_group()
    glib_grp.add_argument('--with-glib', help='build glib (default, version:{0})'.format(glib_default_version),
                          dest='with_glib', action='store_true', default=True)
    glib_grp.add_argument('--without-glib', help='build without glib', dest='with_glib', action='store_false',
                          default=False)
    parser.add_argument('--glib-version', help='glib version (default: {0})'.format(glib_default_version),
                        default=glib_default_version)

    # glib-networking
    glib_networking_grp = parser.add_mutually_exclusive_group()
    glib_networking_grp.add_argument('--with-glib-networking',
                                     help='build glib-networking (default, version:{0})'.format(glib_default_version),
                                     dest='with_glib_networking', action='store_true', default=True)
    glib_networking_grp.add_argument('--without-glib-networking', help='build without glib-networking',
                                     dest='with_glib_networking',
                                     action='store_false',
                                     default=False)
    parser.add_argument('--glib-networking-version',
                        help='glib networking version (default: {0})'.format(glib_default_version),
                        default=glib_default_version)

    # openssl
    # openssl_grp = parser.add_mutually_exclusive_group()
    # openssl_grp.add_argument('--with-openssl',
    #                         help='build openssl (default, version:{0})'.format(openssl_default_version),
    #                         dest='with_openssl', action='store_true', default=True)
    # openssl_grp.add_argument('--without-openssl', help='build without openssl', dest='with_openssl',
    #                         action='store_false',
    #                         default=False)
    # parser.add_argument('--openssl-version', help='openssl version (default: {0})'.format(openssl_default_version),
    #                    default=openssl_default_version)

    # gstreamer
    gstreamer_grp = parser.add_mutually_exclusive_group()
    gstreamer_grp.add_argument('--with-gstreamer',
                               help='build gstreamer (default, version:{0})'.format(gstreamer_default_version),
                               dest='with_gstreamer', action='store_true', default=True)
    gstreamer_grp.add_argument('--without-gstreamer', help='build without gstreamer', dest='with_gstreamer',
                               action='store_false',
                               default=False)
    parser.add_argument('--gstreamer-version',
                        help='gstreamer version (default: {0})'.format(gstreamer_default_version),
                        default=gstreamer_default_version)

    # gst-plugins-base
    gst_plugins_base_grp = parser.add_mutually_exclusive_group()
    gst_plugins_base_grp.add_argument('--with-gst-plugins-base',
                                      help='build gst-plugins-base (default, version:{0})'.format(
                                          gst_plugins_base_default_version),
                                      dest='with_gst_plugins_base', action='store_true', default=True)
    gst_plugins_base_grp.add_argument('--without-gst-plugins-base', help='build without gst-plugins-base',
                                      dest='with_gst_plugins_base',
                                      action='store_false',
                                      default=False)
    parser.add_argument('--gst-plugins-base-version',
                        help='gst-plugins-base version (default: {0})'.format(gst_plugins_base_default_version),
                        default=gst_plugins_base_default_version)

    # gst-plugins-good
    gst_plugins_good_grp = parser.add_mutually_exclusive_group()
    gst_plugins_good_grp.add_argument('--with-gst-plugins-good',
                                      help='build gst-plugins-good (default, version:{0})'.format(
                                          gst_plugins_good_default_version),
                                      dest='with_gst_plugins_good', action='store_true', default=True)
    gst_plugins_good_grp.add_argument('--without-gst-plugins-good', help='build without gst-plugins-good',
                                      dest='with_gst_plugins_good',
                                      action='store_false',
                                      default=False)
    parser.add_argument('--gst-plugins-good-version',
                        help='gst-plugins-good version (default: {0})'.format(gst_plugins_good_default_version),
                        default=gst_plugins_good_default_version)

    # gst-plugins-bad
    gst_plugins_bad_grp = parser.add_mutually_exclusive_group()
    gst_plugins_bad_grp.add_argument('--with-gst-plugins-bad',
                                     help='build gst-plugins-bad (default, version:{0})'.format(
                                         gst_plugins_bad_default_version),
                                     dest='with_gst_plugins_bad', action='store_true', default=True)
    gst_plugins_bad_grp.add_argument('--without-gst-plugins-bad', help='build without gst-plugins-bad',
                                     dest='with_gst_plugins_bad',
                                     action='store_false',
                                     default=False)
    parser.add_argument('--gst-plugins-bad-version',
                        help='gst-plugins-bad version (default: {0})'.format(gst_plugins_bad_default_version),
                        default=gst_plugins_bad_default_version)

    # gst-plugins-ugly
    gst_plugins_ugly_grp = parser.add_mutually_exclusive_group()
    gst_plugins_ugly_grp.add_argument('--with-gst-plugins-ugly',
                                      help='build gst-plugins-ugly (default, version:{0})'.format(
                                          gst_plugins_ugly_default_version),
                                      dest='with_gst_plugins_ugly', action='store_true', default=True)
    gst_plugins_ugly_grp.add_argument('--without-gst-plugins-ugly', help='build without gst-plugins-ugly',
                                      dest='with_gst_plugins_ugly',
                                      action='store_false',
                                      default=False)
    parser.add_argument('--gst-plugins-ugly-version',
                        help='gst-plugins-ugly version (default: {0})'.format(gst_plugins_ugly_default_version),
                        default=gst_plugins_ugly_default_version)

    # gst-libav
    gst_libav_grp = parser.add_mutually_exclusive_group()
    gst_libav_grp.add_argument('--with-gst-libav',
                               help='build gst-libav (default, version:{0})'.format(
                                   gst_libav_default_version),
                               dest='with_gst_libav', action='store_true', default=True)
    gst_libav_grp.add_argument('--without-gst-libav', help='build without gst-libav',
                               dest='with_gst_libav',
                               action='store_false',
                               default=False)
    parser.add_argument('--gst-libav-version',
                        help='gst-libav version (default: {0})'.format(gst_libav_default_version),
                        default=gst_libav_default_version)

    # other
    parser.add_argument('--platform', help='build for platform (default: {0})'.format(host_os), default=host_os)
    parser.add_argument('--architecture', help='architecture (default: {0})'.format(arch_host_os),
                        default=arch_host_os)
    parser.add_argument('--prefix_path', help='prefix_path (default: None)', default=None)

    argv = parser.parse_args()

    arg_platform = argv.platform
    arg_prefix_path = argv.prefix_path
    arg_architecture = argv.architecture

    request = BuildRequest(arg_platform, arg_architecture, 'build_' + arg_platform + '_env', arg_prefix_path)
    if argv.with_system:
        request.install_system()

    if argv.with_cmake:
        request.build_cmake(argv.cmake_version)

    if argv.with_meson:
        request.build_meson(argv.meson_version)

    if argv.with_jsonc:
        request.build_jsonc()
    if argv.with_libev:
        request.build_libev()
    if argv.with_common:
        request.build_common()

    if argv.with_fastotv_protocol:
        request.build_fastotv_protocol()

    # if argv.with_openssl:
    #    request.build_openssl(argv.openssl_version, True)
    if argv.with_glib:
        request.build_glib(argv.glib_version)
    if argv.with_glib_networking:
        request.build_glib_networking(argv.glib_version)

    if argv.with_gstreamer:
        request.build_gstreamer(argv.gstreamer_version)

    if argv.with_gst_plugins_base:
        request.build_gst_plugins_base(argv.gst_plugins_base_version)

    if argv.with_gst_plugins_good:
        request.build_gst_plugins_good(argv.gst_plugins_good_version)

    if argv.with_gst_plugins_bad:
        request.build_gst_plugins_bad(argv.gst_plugins_bad_version)

    if argv.with_gst_plugins_ugly:
        request.build_gst_plugins_ugly(argv.gst_plugins_ugly_version)

    if argv.with_gst_libav:
        request.build_gst_libav(argv.gst_libav_version)

    check_plugins()

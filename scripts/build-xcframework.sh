#!/usr/bin/env bash
#
# build-xcframework.sh — builds Objectively.xcframework for iOS device + simulator.
#
# Outputs:
#   Frameworks/libcurl.xcframework
#   Frameworks/Objectively.xcframework
#
# Usage:
#   scripts/build-xcframework.sh
#
# Optional env vars:
#   CURL_VERSION   — curl release to build (default: 8.10.1)
#   IOS_MIN        — iOS deployment target (default: 14.0)
#
set -e -o pipefail

# NOTE: This script runs `make distclean` in the source directory to allow
# out-of-tree cross-compilation. Re-run `./configure && make` in the repo
# root to restore your development build afterwards.

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
OBJECTIVELY_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_DIR="$OBJECTIVELY_DIR/.build-xcframework"
FRAMEWORKS_DIR="$OBJECTIVELY_DIR/Frameworks"

CURL_VERSION="${CURL_VERSION:-8.10.1}"
CURL_SHA256="${CURL_SHA256:-d15ebab765d793e2e96db090f0e172d127859d78ca6f6391d7eafecfd894bbc0}"
IOS_MIN="${IOS_MIN:-14.0}"

IPHONEOS_SDK=$(xcrun --sdk iphoneos --show-sdk-path)
IPHONESIMULATOR_SDK=$(xcrun --sdk iphonesimulator --show-sdk-path)
CLANG=$(xcrun --find clang)
LIPO=$(xcrun --find lipo)
NPROC=$(sysctl -n hw.logicalcpu)

mkdir -p "$BUILD_DIR" "$FRAMEWORKS_DIR"

# ---------------------------------------------------------------------------
# libcurl
# ---------------------------------------------------------------------------

CURL_SRC="$BUILD_DIR/curl-$CURL_VERSION"
CURL_TARBALL="$BUILD_DIR/curl-${CURL_VERSION}.tar.gz"

if [ ! -d "$CURL_SRC" ]; then
    : "${CURL_SHA256:?Set CURL_SHA256 to the expected sha256 for curl-${CURL_VERSION}.tar.gz}"
    echo "==> Downloading curl $CURL_VERSION"
    curl -fsSL "https://curl.se/download/curl-${CURL_VERSION}.tar.gz" -o "$CURL_TARBALL"
    echo "${CURL_SHA256}  ${CURL_TARBALL}" | shasum -a 256 -c -
    tar -xzf "$CURL_TARBALL" -C "$BUILD_DIR"
fi

build_curl_slice() {
    local name="$1"     # e.g. "iphoneos-arm64"
    local sdk="$2"
    local arch="$3"
    local min_flag="$4" # e.g. "-miphoneos-version-min=14.0"
    local host="$5"     # autotools host triple

    local prefix="$BUILD_DIR/curl-install-$name"
    local builddir="$BUILD_DIR/curl-build-$name"

    if [ -f "$prefix/lib/libcurl.a" ]; then
        echo "==> libcurl $name: cached"
        return 0
    fi

    echo "==> libcurl $name: configuring"
    mkdir -p "$builddir"
    pushd "$builddir" > /dev/null

    "$CURL_SRC/configure" \
        --host="$host" \
        --prefix="$prefix" \
        --disable-shared --enable-static \
        --with-secure-transport \
        --disable-ftp --disable-file --disable-ldap --disable-ldaps \
        --disable-rtsp --disable-dict --disable-telnet --disable-tftp \
        --disable-pop3 --disable-imap --disable-smtp --disable-gopher \
        --disable-manual --disable-debug \
        --without-libidn2 --without-libpsl --without-nghttp2 \
        CC="$CLANG" \
        CPPFLAGS="-arch $arch -isysroot $sdk $min_flag" \
        CFLAGS="-arch $arch -isysroot $sdk $min_flag" \
        LDFLAGS="-arch $arch -isysroot $sdk $min_flag" \
        > "$builddir/configure.log" 2>&1

    echo "==> libcurl $name: building"
    make -j"$NPROC" install >> "$builddir/make.log" 2>&1

    popd > /dev/null
}

build_curl_slice "iphoneos-arm64" \
    "$IPHONEOS_SDK" "arm64" "-miphoneos-version-min=$IOS_MIN" \
    "arm-apple-darwin"

build_curl_slice "iphonesimulator-arm64" \
    "$IPHONESIMULATOR_SDK" "arm64" "-mios-simulator-version-min=$IOS_MIN" \
    "arm-apple-darwin"

build_curl_slice "iphonesimulator-x86_64" \
    "$IPHONESIMULATOR_SDK" "x86_64" "-mios-simulator-version-min=$IOS_MIN" \
    "x86_64-apple-darwin"

echo "==> libcurl simulator: lipo arm64 + x86_64"
CURL_SIM_DIR="$BUILD_DIR/curl-install-iphonesimulator-fat"
mkdir -p "$CURL_SIM_DIR/lib"
"$LIPO" -create \
    "$BUILD_DIR/curl-install-iphonesimulator-arm64/lib/libcurl.a" \
    "$BUILD_DIR/curl-install-iphonesimulator-x86_64/lib/libcurl.a" \
    -output "$CURL_SIM_DIR/lib/libcurl.a"
cp -r "$BUILD_DIR/curl-install-iphoneos-arm64/include" "$CURL_SIM_DIR/"

echo "==> Creating libcurl.xcframework"
rm -rf "$FRAMEWORKS_DIR/libcurl.xcframework"
xcodebuild -create-xcframework \
    -library "$BUILD_DIR/curl-install-iphoneos-arm64/lib/libcurl.a" \
    -headers "$BUILD_DIR/curl-install-iphoneos-arm64/include" \
    -library "$CURL_SIM_DIR/lib/libcurl.a" \
    -headers "$CURL_SIM_DIR/include" \
    -output "$FRAMEWORKS_DIR/libcurl.xcframework"

# ---------------------------------------------------------------------------
# Objectively
# ---------------------------------------------------------------------------

echo "==> Preparing Objectively autotools"
pushd "$OBJECTIVELY_DIR" > /dev/null
make distclean > /dev/null 2>&1 || true
autoreconf -i > /dev/null 2>&1
popd > /dev/null

build_objectively_slice() {
    local name="$1"
    local sdk="$2"
    local arch="$3"
    local min_flag="$4"
    local host="$5"
    local curl_prefix="$6"

    local prefix="$BUILD_DIR/objectively-install-$name"
    local builddir="$BUILD_DIR/objectively-build-$name"

    if [ -f "$prefix/lib/libObjectively.a" ] && [ "$(wc -c < "$prefix/lib/libObjectively.a")" -gt 1000 ]; then
        echo "==> Objectively $name: cached"
        return 0
    fi

    echo "==> Objectively $name: configuring"
    mkdir -p "$builddir"
    pushd "$builddir" > /dev/null

    # CHECK_CFLAGS/LIBS set to empty strings so pkg-config is not invoked
    # for the unit test framework (not needed for the library itself).
    CURL_CFLAGS="-I$curl_prefix/include" \
    CURL_LIBS="$curl_prefix/lib/libcurl.a -framework Security -framework CoreFoundation -framework SystemConfiguration" \
    CHECK_CFLAGS="" CHECK_LIBS="" \
    "$OBJECTIVELY_DIR/configure" \
        --host="$host" \
        --prefix="$prefix" \
        --disable-shared --enable-static \
        CC="$CLANG" \
        CFLAGS="-arch $arch -isysroot $sdk $min_flag" \
        LDFLAGS="-arch $arch -isysroot $sdk $min_flag" \
        > "$builddir/configure.log" 2>&1

    echo "==> Objectively $name: building"
    make -j"$NPROC" -C Sources >> "$builddir/make.log" 2>&1

    # libtool cross-compilation doesn't reliably archive non-PIC objects,
    # so build the static archive directly from the compiled .o files.
    echo "==> Objectively $name: archiving"
    local objdir="$builddir/Sources/Objectively"
    mkdir -p "$prefix/lib" "$prefix/include/Objectively"

    ar cr "$prefix/lib/libObjectively.a" "$objdir"/*.o
    ranlib "$prefix/lib/libObjectively.a"

    # Install public headers (source + generated Config.h)
    cp "$OBJECTIVELY_DIR/Sources/Objectively/"*.h "$prefix/include/Objectively/"
    cp "$objdir/Config.h" "$prefix/include/Objectively/"

    popd > /dev/null
}

build_objectively_slice "iphoneos-arm64" \
    "$IPHONEOS_SDK" "arm64" "-miphoneos-version-min=$IOS_MIN" \
    "arm-apple-darwin" \
    "$BUILD_DIR/curl-install-iphoneos-arm64"

build_objectively_slice "iphonesimulator-arm64" \
    "$IPHONESIMULATOR_SDK" "arm64" "-mios-simulator-version-min=$IOS_MIN" \
    "arm-apple-darwin" \
    "$BUILD_DIR/curl-install-iphonesimulator-arm64"

build_objectively_slice "iphonesimulator-x86_64" \
    "$IPHONESIMULATOR_SDK" "x86_64" "-mios-simulator-version-min=$IOS_MIN" \
    "x86_64-apple-darwin" \
    "$BUILD_DIR/curl-install-iphonesimulator-x86_64"

echo "==> Objectively simulator: lipo arm64 + x86_64"
OBJ_SIM_DIR="$BUILD_DIR/objectively-install-iphonesimulator-fat"
mkdir -p "$OBJ_SIM_DIR/lib"
"$LIPO" -create \
    "$BUILD_DIR/objectively-install-iphonesimulator-arm64/lib/libObjectively.a" \
    "$BUILD_DIR/objectively-install-iphonesimulator-x86_64/lib/libObjectively.a" \
    -output "$OBJ_SIM_DIR/lib/libObjectively.a"
cp -r "$BUILD_DIR/objectively-install-iphoneos-arm64/include" "$OBJ_SIM_DIR/"

echo "==> Creating Objectively.xcframework"
rm -rf "$FRAMEWORKS_DIR/Objectively.xcframework"
xcodebuild -create-xcframework \
    -library "$BUILD_DIR/objectively-install-iphoneos-arm64/lib/libObjectively.a" \
    -headers "$BUILD_DIR/objectively-install-iphoneos-arm64/include" \
    -library "$OBJ_SIM_DIR/lib/libObjectively.a" \
    -headers "$OBJ_SIM_DIR/include" \
    -output "$FRAMEWORKS_DIR/Objectively.xcframework"

echo ""
echo "Done!"
echo "  $FRAMEWORKS_DIR/libcurl.xcframework"
echo "  $FRAMEWORKS_DIR/Objectively.xcframework"

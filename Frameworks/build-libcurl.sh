#!/usr/bin/env bash
#
# build-libcurl.sh — builds libcurl.xcframework (macOS + iOS device + iOS simulator)
# from official curl source.
#
# curl publishes no prebuilt Apple xcframework, so — unlike SDL3 — this third-party
# dependency must be compiled from source. This script does only that; the
# Objectively framework itself is built by `xcodebuild` (see .github/workflows).
# The result is cached under Frameworks/ (.gitignored) and is also published as a
# release asset for downstream iOS consumers. Bump CURL_VERSION (and CURL_SHA256)
# to upgrade; delete Frameworks/libcurl.xcframework to force a rebuild.
#
# Optional env vars:
#   CURL_VERSION   — curl release to build (default: 8.10.1)
#   CURL_SHA256    — expected sha256 of the curl tarball
#   IOS_MIN        — iOS deployment target (default: 16.0, matching the frameworks)
#   MACOS_MIN      — macOS deployment target (default: 13.0, matching the frameworks)
#
set -e -o pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
OBJECTIVELY_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_DIR="$OBJECTIVELY_DIR/.build-xcframework"
FRAMEWORKS_DIR="$OBJECTIVELY_DIR/Frameworks"

CURL_VERSION="${CURL_VERSION:-8.10.1}"
CURL_SHA256="${CURL_SHA256:-d15ebab765d793e2e96db090f0e172d127859d78ca6f6391d7eafecfd894bbc0}"
IOS_MIN="${IOS_MIN:-16.0}"
MACOS_MIN="${MACOS_MIN:-13.0}"

# Already cached? Nothing to do.
[ -d "$FRAMEWORKS_DIR/libcurl.xcframework" ] && exit 0

MACOSX_SDK=$(xcrun --sdk macosx --show-sdk-path)
IPHONEOS_SDK=$(xcrun --sdk iphoneos --show-sdk-path)
IPHONESIMULATOR_SDK=$(xcrun --sdk iphonesimulator --show-sdk-path)
CLANG=$(xcrun --find clang)
LIPO=$(xcrun --find lipo)
NPROC=$(sysctl -n hw.logicalcpu)

mkdir -p "$BUILD_DIR" "$FRAMEWORKS_DIR"

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

build_curl_slice "macos-arm64" \
    "$MACOSX_SDK" "arm64" "-mmacosx-version-min=$MACOS_MIN" \
    "aarch64-apple-darwin"

build_curl_slice "macos-x86_64" \
    "$MACOSX_SDK" "x86_64" "-mmacosx-version-min=$MACOS_MIN" \
    "x86_64-apple-darwin"

echo "==> libcurl macOS: lipo arm64 + x86_64"
CURL_MACOS_DIR="$BUILD_DIR/curl-install-macos-fat"
mkdir -p "$CURL_MACOS_DIR/lib"
"$LIPO" -create \
    "$BUILD_DIR/curl-install-macos-arm64/lib/libcurl.a" \
    "$BUILD_DIR/curl-install-macos-x86_64/lib/libcurl.a" \
    -output "$CURL_MACOS_DIR/lib/libcurl.a"
cp -r "$BUILD_DIR/curl-install-macos-arm64/include" "$CURL_MACOS_DIR/"

echo "==> Creating libcurl.xcframework"
rm -rf "$FRAMEWORKS_DIR/libcurl.xcframework"
xcodebuild -create-xcframework \
    -library "$BUILD_DIR/curl-install-iphoneos-arm64/lib/libcurl.a" \
    -headers "$BUILD_DIR/curl-install-iphoneos-arm64/include" \
    -library "$CURL_SIM_DIR/lib/libcurl.a" \
    -headers "$CURL_SIM_DIR/include" \
    -library "$CURL_MACOS_DIR/lib/libcurl.a" \
    -headers "$CURL_MACOS_DIR/include" \
    -output "$FRAMEWORKS_DIR/libcurl.xcframework"

echo "==> libcurl.xcframework ready at Frameworks/libcurl.xcframework"

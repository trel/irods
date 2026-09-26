# Building iRODS With Fil-C

This file records the current state of an experiment to build and run iRODS
with Fil-C 0.685. The goal is to give the next agent enough detail to avoid
rediscovering the same setup steps and early blockers.

## Goal

Build iRODS with the Fil-C compiler and as many Fil-C-built dependencies as
possible. Fil-C is not ABI-compatible with ordinary system C/C++ libraries, so
the useful path is not just changing `CC` and `CXX`; required libraries also
need to live in the Fil-C slice, normally `/opt/fil`.

## Environment

- Workspace: `/src/irods`
- Fil-C version: `0.685`
- Fil-C install prefix used: `/opt/fil`
- Fil-C C compiler: `/opt/fil/bin/filcc`
- Fil-C C++ compiler: `/opt/fil/bin/fil++`
- Fil-C target triple reported by default: `x86_64-unknown-linux-gnu`
- Parallelism authorized by user: up to 30 compile jobs
- Temporary source/build workspace used: `/tmp/opencode`

Do not put `/opt/fil/bin` first in `PATH` globally while running unrelated
system tools. The Fil-C `ld` can be selected accidentally and break normal
system compiler probes. Prefer explicit compiler and pkg-config paths:

```bash
CC=/opt/fil/bin/filcc
CXX=/opt/fil/bin/fil++
PKG_CONFIG=/opt/fil/bin/pkg-config
PKG_CONFIG_PATH=/opt/fil/lib/pkgconfig:/opt/fil/share/pkgconfig
```

## Fil-C Installation

Downloaded and installed the `/opt/fil` distribution, not the smaller `pizfix`
tarball. The `/opt/fil` distribution includes a glibc-based Fil-C slice and more
prebuilt Fil-C libraries, which removes several easy hurdles.

```bash
curl -L --fail --show-error \
  --output /tmp/opencode/optfil-0.685-linux-x86_64.tar.xz \
  https://github.com/pizlonator/fil-c/releases/download/v0.685/optfil-0.685-linux-x86_64.tar.xz
tar -xf /tmp/opencode/optfil-0.685-linux-x86_64.tar.xz -C /tmp/opencode
cd /tmp/opencode/optfil-0.685-linux-x86_64
./setup.sh --unattended
```

The unattended install extracted `/opt/fil` and skipped optional SSH/system
configuration.

Validated the compilers:

```bash
/opt/fil/bin/filcc --version
/opt/fil/bin/fil++ --version
```

Both report Clang 20.1.8 with Fil-C 0.685.

Validated simple C and C++ programs:

```bash
/opt/fil/bin/filcc -x c -O2 -g -o /tmp/opencode/filc-hello-c - <<'EOF'
#include <stdio.h>
int main(void) { puts("hello from fil-c"); return 0; }
EOF
/tmp/opencode/filc-hello-c

/opt/fil/bin/fil++ -x c++ -std=c++20 -O2 -g \
  -o /tmp/opencode/filc-hello-cxx - <<'EOF'
#include <iostream>
int main() { std::cout << "hello from fil-c++" << std::endl; }
EOF
/tmp/opencode/filc-hello-cxx
```

## iRODS Configure Hook

iRODS defaults to `/opt/irods-externals/clang16.0.6-0` via
`cmake/Modules/IrodsCXXCompiler.cmake`, but only if CMake compilers are not
already set. A Fil-C build can therefore use an isolated build tree and pass
explicit compilers without changing iRODS source.

Initial configure command:

```bash
cmake -S . -B build-filc -G Ninja \
  -DCMAKE_C_COMPILER=/opt/fil/bin/filcc \
  -DCMAKE_CXX_COMPILER=/opt/fil/bin/fil++ \
  -DCMAKE_BUILD_TYPE=Release \
  -DIRODS_BUILD_WITH_CLANG=OFF \
  -DCMAKE_PREFIX_PATH=/opt/fil
```

This got through compiler detection but failed at `fmt`, because `/opt/fil` did
not include it.

## Dependencies Built Into `/opt/fil`

The following dependencies have been built or installed into `/opt/fil` with
Fil-C compilers.

### fmt 8.1.1

```bash
curl -L --fail --show-error \
  --output /tmp/opencode/fmt-8.1.1.tar.gz \
  https://github.com/fmtlib/fmt/archive/refs/tags/8.1.1.tar.gz
tar -xf /tmp/opencode/fmt-8.1.1.tar.gz -C /tmp/opencode
cmake -S /tmp/opencode/fmt-8.1.1 \
  -B /tmp/opencode/fmt-8.1.1-build-filc \
  -G Ninja \
  -DCMAKE_C_COMPILER=/opt/fil/bin/filcc \
  -DCMAKE_CXX_COMPILER=/opt/fil/bin/fil++ \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=/opt/fil \
  -DBUILD_SHARED_LIBS=ON \
  -DFMT_TEST=OFF \
  -DFMT_DOC=OFF
cmake --build /tmp/opencode/fmt-8.1.1-build-filc --parallel 30
cmake --install /tmp/opencode/fmt-8.1.1-build-filc
```

### spdlog 1.12.0

```bash
curl -L --fail --show-error \
  --output /tmp/opencode/spdlog-1.12.0.tar.gz \
  https://github.com/gabime/spdlog/archive/refs/tags/v1.12.0.tar.gz
tar -xf /tmp/opencode/spdlog-1.12.0.tar.gz -C /tmp/opencode
cmake -S /tmp/opencode/spdlog-1.12.0 \
  -B /tmp/opencode/spdlog-1.12.0-build-filc \
  -G Ninja \
  -DCMAKE_C_COMPILER=/opt/fil/bin/filcc \
  -DCMAKE_CXX_COMPILER=/opt/fil/bin/fil++ \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=/opt/fil \
  -DCMAKE_PREFIX_PATH=/opt/fil \
  -DSPDLOG_BUILD_SHARED=ON \
  -DSPDLOG_FMT_EXTERNAL=ON \
  -DSPDLOG_BUILD_EXAMPLE=OFF \
  -DSPDLOG_BUILD_TESTS=OFF \
  -DSPDLOG_BUILD_BENCH=OFF
cmake --build /tmp/opencode/spdlog-1.12.0-build-filc --parallel 30
cmake --install /tmp/opencode/spdlog-1.12.0-build-filc
```

### nlohmann-json 3.11.3

```bash
curl -L --fail --show-error \
  --output /tmp/opencode/json-3.11.3.tar.gz \
  https://github.com/nlohmann/json/archive/refs/tags/v3.11.3.tar.gz
tar -xf /tmp/opencode/json-3.11.3.tar.gz -C /tmp/opencode
cmake -S /tmp/opencode/json-3.11.3 \
  -B /tmp/opencode/json-3.11.3-build-filc \
  -G Ninja \
  -DCMAKE_CXX_COMPILER=/opt/fil/bin/fil++ \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=/opt/fil \
  -DJSON_BuildTests=OFF
cmake --install /tmp/opencode/json-3.11.3-build-filc
```

### jsoncons 0.178.0

```bash
curl -L --fail --show-error \
  --output /tmp/opencode/jsoncons-0.178.0.tar.gz \
  https://github.com/danielaparker/jsoncons/archive/refs/tags/v0.178.0.tar.gz
tar -xf /tmp/opencode/jsoncons-0.178.0.tar.gz -C /tmp/opencode
cmake -S /tmp/opencode/jsoncons-0.178.0 \
  -B /tmp/opencode/jsoncons-0.178.0-build-filc \
  -G Ninja \
  -DCMAKE_CXX_COMPILER=/opt/fil/bin/fil++ \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=/opt/fil \
  -DJSONCONS_BUILD_TESTS=OFF \
  -DJSONCONS_BUILD_EXAMPLES=OFF
cmake --install /tmp/opencode/jsoncons-0.178.0-build-filc
```

### libarchive 3.7.7

`/opt/fil` does not ship libarchive, but it does ship useful dependencies such
as zlib, bzip2, xz, lz4, zstd, OpenSSL, and libxml2. libarchive configured,
built, and installed successfully with Fil-C.

```bash
curl -L --fail --show-error \
  --output /tmp/opencode/libarchive-3.7.7.tar.xz \
  https://github.com/libarchive/libarchive/releases/download/v3.7.7/libarchive-3.7.7.tar.xz
tar -xf /tmp/opencode/libarchive-3.7.7.tar.xz -C /tmp/opencode
PKG_CONFIG_EXECUTABLE=/opt/fil/bin/pkg-config \
PKG_CONFIG_PATH=/opt/fil/lib/pkgconfig:/opt/fil/share/pkgconfig \
cmake -S /tmp/opencode/libarchive-3.7.7 \
  -B /tmp/opencode/libarchive-3.7.7-build-filc \
  -G Ninja \
  -DCMAKE_C_COMPILER=/opt/fil/bin/filcc \
  -DCMAKE_CXX_COMPILER=/opt/fil/bin/fil++ \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=/opt/fil \
  -DCMAKE_PREFIX_PATH=/opt/fil \
  -DBUILD_SHARED_LIBS=ON \
  -DBUILD_TESTING=OFF \
  -DENABLE_TEST=OFF \
  -DENABLE_CNG=OFF \
  -DENABLE_NETTLE=OFF \
  -DENABLE_MBEDTLS=OFF \
  -DENABLE_OPENSSL=ON
cmake --build /tmp/opencode/libarchive-3.7.7-build-filc --parallel 30
cmake --install /tmp/opencode/libarchive-3.7.7-build-filc
```

The first build attempt used a 120 second timeout and was interrupted, but the
build resumed cleanly with a longer timeout and `--parallel 30`.

### Boost 1.81.0

iRODS directly links these Boost libraries:

- `chrono`
- `container`
- `filesystem`
- `program_options`
- `random`
- `regex`
- `system`
- `thread`

Boost.Build itself can be built with Fil-C only after working around missing
`vfork` in `/opt/fil/include/unistd.h`:

```bash
./tools/build/src/engine/build.sh \
  --cxx=/opt/fil/bin/fil++ \
  --cxxflags=-Dvfork=fork \
  clang
```

However, the Fil-C-built `b2` panicked on an error path when given a bad config
path:

```text
filc safety error: argument size mismatch (actual = 8, expected = 16).
```

To avoid spending time on the host tool, use a GCC-built `b2` and still build
the Boost target libraries with `fil++`:

```bash
cd /tmp/opencode/boost_1_81_0
./bootstrap.sh \
  --with-toolset=gcc \
  --with-libraries=chrono,container,filesystem,program_options,random,regex,system,thread \
  --prefix=/opt/fil
```

Create `/tmp/opencode/user-config-filc.jam`:

```jam
using clang : filc : /opt/fil/bin/fil++ ;
```

The default Boost `clang-linux` toolset adds `--target=x86_64-pc-linux`, which
breaks Fil-C libc++ include lookup because `__config_site` exists under
`/opt/fil/include/x86_64-unknown-linux-gnu/c++/v1`, not under
`x86_64-pc-linux`. Force the target triple back to Fil-C's default:

```bash
./b2 --user-config=/tmp/opencode/user-config-filc.jam \
  --with-chrono \
  --with-container \
  --with-filesystem \
  --with-program_options \
  --with-random \
  --with-regex \
  --with-system \
  --with-thread \
  toolset=clang-filc \
  cxxstd=20 \
  cxxflags=--target=x86_64-unknown-linux-gnu \
  linkflags=--target=x86_64-unknown-linux-gnu \
  variant=release \
  link=shared \
  runtime-link=shared \
  threading=multi \
  --prefix=/opt/fil \
  install \
  -j30
```

This successfully installed the required Boost libraries into `/opt/fil/lib`.

### unixODBC 2.3.12

`/opt/fil` does not ship ODBC. iRODS configure stops at
`plugins/database/CMakeLists.txt` if it cannot find `libodbc`.

Download and configure:

```bash
curl -L --fail --show-error \
  --output /tmp/opencode/unixODBC-2.3.12.tar.gz \
  https://www.unixodbc.org/unixODBC-2.3.12.tar.gz
tar -xf /tmp/opencode/unixODBC-2.3.12.tar.gz -C /tmp/opencode
cd /tmp/opencode/unixODBC-2.3.12
CC=/opt/fil/bin/filcc \
CXX=/opt/fil/bin/fil++ \
PKG_CONFIG=/opt/fil/bin/pkg-config \
PKG_CONFIG_PATH=/opt/fil/lib/pkgconfig:/opt/fil/share/pkgconfig \
./configure \
  --prefix=/opt/fil \
  --enable-shared \
  --disable-static \
  --disable-gui \
  --disable-drivers \
  --disable-driverc \
  --disable-dependency-tracking
make -j30
```

The full build fails while linking helper executables such as `dltest` and
`slencheck`. The driver-manager libraries build, but initially do not link
correctly against Fil-C callers because unixODBC uses libtool export-symbols
files for raw ODBC symbol names. Fil-C callers reference transformed symbols
such as `pizlonated_SQLAllocHandle`, so the export list hides the symbols Fil-C
needs.

Temporary source/build-tree changes applied outside the iRODS repository:

- In `DriverManager/Makefile`, remove `-export-symbols ./DriverManager.exp`
  from `libodbc_la_LDFLAGS`.
- In `odbcinst/Makefile`, remove `-export-symbols ./odbcinst.exp` from
  `libodbcinst_la_LDFLAGS`.
- In `libltdl/ltdl.c`, after the internal includes, add:

```c
const lt_dlsymlist lt_libltdlc_LTX_preloaded_symbols[] = {{0, 0}};
```

Then rebuild and install just the library pieces:

```bash
make -C libltdl clean
make -C odbcinst clean
make -C DriverManager clean
make -C libltdl -j30
make -C odbcinst -j30
make -C DriverManager -j30
make -C include install
make -C libltdl install
make -C odbcinst install
make -C DriverManager install
install -m 0644 /tmp/opencode/unixODBC-2.3.12/unixodbc.h \
  /opt/fil/include/unixodbc.h
```

After those changes, this direct Fil-C ODBC link test succeeds:

```bash
/opt/fil/bin/filcc \
  -I/opt/fil/include \
  -L/opt/fil/lib \
  -Wl,-rpath,/opt/fil/lib \
  -o /tmp/opencode/odbc-link-test \
  -x c \
  -lodbc \
  - <<'EOF'
#include <sql.h>
#include <sqlext.h>
int main(void) {
    SQLHENV env = SQL_NULL_HENV;
    return SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env) == SQL_SUCCESS ? 0 : 1;
}
EOF
/tmp/opencode/odbc-link-test
```

## Current nanodbc Blocker

Downloaded `nanodbc` 2.14.0:

```bash
curl -L --fail --show-error \
  --output /tmp/opencode/nanodbc-2.14.0.tar.gz \
  https://github.com/nanodbc/nanodbc/archive/refs/tags/v2.14.0.tar.gz
tar -xf /tmp/opencode/nanodbc-2.14.0.tar.gz -C /tmp/opencode
```

Initial configure/build command:

```bash
cmake -S /tmp/opencode/nanodbc-2.14.0 \
  -B /tmp/opencode/nanodbc-2.14.0-build-filc \
  -G Ninja \
  -DCMAKE_C_COMPILER=/opt/fil/bin/filcc \
  -DCMAKE_CXX_COMPILER=/opt/fil/bin/fil++ \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=/opt/fil \
  -DCMAKE_PREFIX_PATH=/opt/fil \
  -DODBC_INCLUDE_DIR=/opt/fil/include \
  -DODBC_LIBRARY=/opt/fil/lib/libodbc.so \
  -DNANODBC_ENABLE_TESTS=OFF \
  -DNANODBC_ENABLE_EXAMPLES=OFF \
  -DBUILD_SHARED_LIBS=ON
cmake --build /tmp/opencode/nanodbc-2.14.0-build-filc --parallel 30
```

This still built examples/tests because the option names were wrong for this
project version. The library compile also failed with libc++ because
`NANODBC_SQLCHAR` is `unsigned char`, and libc++ does not define
`std::char_traits<unsigned char>`:

```text
error: implicit instantiation of undefined template
'std::char_traits<unsigned char>'
```

The correct option names are `NANODBC_DISABLE_TESTS=ON` and
`NANODBC_DISABLE_EXAMPLES=ON`. A temporary patch was applied outside the iRODS
repository to avoid `std::char_traits<NANODBC_SQLCHAR>` and count ODBC SQLCHAR
buffers directly:

```diff
 template <std::size_t N>
 inline std::size_t size(NANODBC_SQLCHAR const (&array)[N]) noexcept
 {
-    auto const n = std::char_traits<NANODBC_SQLCHAR>::length(array);
+    std::size_t n = 0;
+    while (n < N && array[n] != 0)
+        ++n;
     NANODBC_ASSERT(n < N);
     return n < N ? n : N - 1;
 }
+
+template <std::size_t N>
+inline std::size_t sqlchar_length(NANODBC_SQLCHAR const (&array)[N]) noexcept
+{
+    return size(array);
+}
```

The call sites around datasource and driver names were changed from
`std::char_traits<NANODBC_SQLCHAR>::length(...)` to `sqlchar_length(...)`.

The working nanodbc build command is:

```bash
rm -rf /tmp/opencode/nanodbc-2.14.0-build-filc
cmake -S /tmp/opencode/nanodbc-2.14.0 \
  -B /tmp/opencode/nanodbc-2.14.0-build-filc \
  -G Ninja \
  -DCMAKE_C_COMPILER=/opt/fil/bin/filcc \
  -DCMAKE_CXX_COMPILER=/opt/fil/bin/fil++ \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=/opt/fil \
  -DCMAKE_PREFIX_PATH=/opt/fil \
  -DCMAKE_SHARED_LINKER_FLAGS="-L/opt/fil/lib -Wl,-rpath,/opt/fil/lib" \
  -DODBC_INCLUDE_DIR=/opt/fil/include \
  -DNANODBC_DISABLE_TESTS=ON \
  -DNANODBC_DISABLE_EXAMPLES=ON \
  -DBUILD_SHARED_LIBS=ON
cmake --build /tmp/opencode/nanodbc-2.14.0-build-filc \
  --target nanodbc \
  --parallel 30
cmake --install /tmp/opencode/nanodbc-2.14.0-build-filc
```

This installed `/opt/fil/lib/libnanodbc.so` and
`/opt/fil/include/nanodbc/nanodbc.h`.

## iRODS Configure Command

After rebuilding fmt, spdlog, nlohmann-json, jsoncons, libarchive, Boost,
unixODBC, and nanodbc, this command configures iRODS successfully:

```bash
cmake -S . -B build-filc -G Ninja \
  -DCMAKE_C_COMPILER=/opt/fil/bin/filcc \
  -DCMAKE_CXX_COMPILER=/opt/fil/bin/fil++ \
  -DCMAKE_BUILD_TYPE=Release \
  -DIRODS_BUILD_WITH_CLANG=OFF \
  -DIRODS_BUILD_WITH_WERROR=OFF \
  -DIRODS_USE_LIBSYSTEMD=OFF \
  -DCMAKE_CXX_FLAGS=-DFMT_USE_CONSTEVAL=0 \
  -DCMAKE_PREFIX_PATH=/opt/fil \
  -Dfmt_DIR=/opt/fil/lib/cmake/fmt \
  -Dspdlog_DIR=/opt/fil/lib/cmake/spdlog \
  -Dnlohmann_json_DIR=/opt/fil/share/cmake/nlohmann_json \
  -DPKG_CONFIG_EXECUTABLE=/opt/fil/bin/pkg-config \
  -DIRODS_EXTERNALS_FULLPATH_BOOST=/opt/fil \
  -DIRODS_EXTERNALS_FULLPATH_JSONCONS=/opt/fil \
  -DIRODS_EXTERNALS_FULLPATH_NANODBC=/opt/fil \
  -DODBC_LIBRARY=/opt/fil/lib/libodbc.so
```

Important configure notes:

- `IRODS_BUILD_WITH_WERROR=OFF` is needed because Fil-C/Clang 20 exposes new
  warnings as errors in existing iRODS code, including deprecated type use,
  missing designated initializers, VLA extensions, function-cast mismatches, and
  dangling pointer warnings.
- `DIRODS_USE_LIBSYSTEMD=OFF` avoids a dependency not present in `/opt/fil`.
- `CMAKE_CXX_FLAGS=-DFMT_USE_CONSTEVAL=0` alone does not avoid all fmt/spdlog
  consteval issues. See the spdlog note below.
- `nlohmann_json_DIR` must be pinned to `/opt/fil`. If CMake uses
  `/usr/share/cmake/nlohmann_json`, it injects `-isystem /usr/include`, causing
  Fil-C to mix system headers with `/opt/fil` libc++.

The command finds these `/opt/fil` dependencies successfully:

- `LibArchive`: `/opt/fil/lib/libarchive.so`
- `CURL`: `/opt/fil/lib/libcurl.so`
- `OpenSSL`: `/opt/fil/lib/libcrypto.so` and `libssl.so`
- `pam`: `/opt/fil/lib/libpam.so`
- `flex`: `/opt/fil/bin/flex`
- `bison`: `/opt/fil/bin/bison`
- `ODBC_LIBRARY`: `/opt/fil/lib/libodbc.so`

## spdlog/fmt Workaround

The iRODS build reached `lib/hasher/src/checksum.cpp` and failed inside
`/opt/fil/include/spdlog/details/fmt_helper.h` because `SPDLOG_FMT_STRING` uses
`FMT_STRING`, and Fil-C's Clang/libc++ rejected fmt's compile-time format parse
as a constant expression.

The working experiment patched the installed `/opt/fil` spdlog header:

```diff
 #if !defined(SPDLOG_USE_STD_FORMAT) && FMT_VERSION >= 80000
 #    define SPDLOG_FMT_RUNTIME(format_string) fmt::runtime(format_string)
-#    define SPDLOG_FMT_STRING(format_string) FMT_STRING(format_string)
+#    define SPDLOG_FMT_STRING(format_string) format_string
```

This is an `/opt/fil` experiment workaround, not an iRODS source change. A
cleaner long-term fix would be to rebuild spdlog for the Fil-C prefix with an
upstream-supported option or patch that disables compile-time format strings.

## iRODS Source Change Required

The only iRODS source change made so far is in
`plugins/microservices/src/json_parse.cpp`. The existing code passed an `int*`
range to `nlohmann::json::parse` for `IntArray_MS_T` input:

```c++
json::parse(int_array->value, int_array->value + buf_size)
```

nlohmann-json with libc++ then instantiates `std::char_traits<int>`, which is
undefined. The microservice documentation says the integer array represents a
byte sequence, so the code now copies the values into a `std::string` and parses
that byte string:

```c++
std::string json_text;
json_text.reserve(buf_size);

for (const auto v : s) {
    json_text.push_back(static_cast<char>(v));
}

handle = irods::process_stash::insert(json::parse(json_text));
```

## Build And Package Results

After the dependency rebuilds, spdlog workaround, and iRODS source fix, the
Fil-C iRODS build completed:

```bash
cmake --build build-filc --parallel 30
```

The package target also completed:

```bash
cmake --build build-filc --target package --parallel 30
```

Generated packages:

- `build-filc/irods-database-plugin-mysql_5.1.0-0~noble_amd64.deb`
- `build-filc/irods-database-plugin-oracle_5.1.0-0~noble_amd64.deb`
- `build-filc/irods-database-plugin-postgres_5.1.0-0~noble_amd64.deb`
- `build-filc/irods-dev_5.1.0-0~noble_amd64.deb`
- `build-filc/irods-icommands_5.1.0-0~noble_amd64.deb`
- `build-filc/irods-runtime_5.1.0-0~noble_amd64.deb`
- `build-filc/irods-server_5.1.0-0~noble_amd64.deb`

Lightweight runtime checks from the build tree passed:

```bash
LD_LIBRARY_PATH="/src/irods/build-filc/lib:/src/irods/build-filc/lib/core:/src/irods/build-filc/server:/opt/fil/lib" \
  ./build-filc/clients/icommands/ihelp

LD_LIBRARY_PATH="/src/irods/build-filc/lib:/src/irods/build-filc/lib/core:/src/irods/build-filc/server:/opt/fil/lib" \
  ./build-filc/server/main_server/irodsServer --version
```

`ihelp` printed the iCommands help text, and `irodsServer --version` printed
`irodsServer v5.1.0-7fb3557`.

Full package installation and server smoke testing have not been performed yet.
The generated Debian package metadata still describes normal system/iRODS
external dependencies, not the ad hoc `/opt/fil` dependency stack, so installing
these packages may require additional packaging changes or a controlled test
container.

## Known Pitfalls

- Do not globally put `/opt/fil/bin` first in `PATH` while running system CMake
  or GCC commands. It can select `/opt/fil/bin/ld` unintentionally.
- Fil-C is source-compatible, not ABI-compatible. Do not link iRODS against
  system `/usr/lib` C/C++ libraries unless the point is only to characterize a
  failure.
- Boost.Build's `clang-linux` toolset forces `--target=x86_64-pc-linux`; pass
  `cxxflags=--target=x86_64-unknown-linux-gnu` and the same link flag.
- `/opt/fil/include/unistd.h` does not declare `vfork`; use `-Dvfork=fork` for
  host tools that require it, or build the host tool with the system compiler.
- Some autotools/libtool projects use export-symbols lists. These can hide the
  `pizlonated_*` symbols that Fil-C callers need.
- `help2man` was not found in `/opt/fil`; iRODS configure did not stop there,
  but packaging or manpage targets may need attention later.
- The generated `.deb` packages are not yet proper redistributable Fil-C
  packages because their dependency metadata has not been adjusted for `/opt/fil`
  or Fil-C-built externals.

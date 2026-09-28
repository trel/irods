# Building iRODS With Fil-C

This document describes the known-good path for building iRODS from source with
Fil-C 0.685 using the `/opt/fil` distribution. It assumes the iRODS checkout is
at `/src/irods` and temporary dependency sources are unpacked under
`/tmp/opencode`.

Fil-C is not ABI-compatible with ordinary system C/C++ libraries. Do not build
iRODS with Fil-C while linking to normal `/usr/lib` dependencies. The required
libraries need to be in the Fil-C slice, `/opt/fil`, or the build will mix ABIs.

## Environment

- Workspace: `/src/irods`
- Fil-C version: `0.685`
- Fil-C install prefix: `/opt/fil`
- Fil-C C compiler: `/opt/fil/bin/filcc`
- Fil-C C++ compiler: `/opt/fil/bin/fil++`
- Fil-C target triple: `x86_64-unknown-linux-gnu`
- Build parallelism: up to 30 jobs
- Temporary source/build workspace: `/tmp/opencode`

Do not put `/opt/fil/bin` first in `PATH` globally while running unrelated
system tools. The Fil-C `ld` can be selected accidentally and break normal
system compiler probes. Prefer explicit tool paths:

```bash
CC=/opt/fil/bin/filcc
CXX=/opt/fil/bin/fil++
PKG_CONFIG=/opt/fil/bin/pkg-config
PKG_CONFIG_PATH=/opt/fil/lib/pkgconfig:/opt/fil/share/pkgconfig
```

## Install Fil-C

Use the `/opt/fil` distribution. It includes the Fil-C compiler, linker,
glibc-based headers, libc++, and several useful libraries already built with
Fil-C.

```bash
curl -L --fail --show-error \
  --output /tmp/opencode/optfil-0.685-linux-x86_64.tar.xz \
  https://github.com/pizlonator/fil-c/releases/download/v0.685/optfil-0.685-linux-x86_64.tar.xz
tar -xf /tmp/opencode/optfil-0.685-linux-x86_64.tar.xz -C /tmp/opencode
cd /tmp/opencode/optfil-0.685-linux-x86_64
./setup.sh --unattended
```

Validate the compilers:

```bash
/opt/fil/bin/filcc --version
/opt/fil/bin/fil++ --version
```

Both should report Clang 20.1.8 with Fil-C 0.685.

## Build Dependencies Into `/opt/fil`

Build these dependencies before configuring iRODS:

- `fmt 8.1.1`
- `spdlog 1.12.0`
- `nlohmann-json 3.11.3`
- `jsoncons 0.178.0`
- `libarchive 3.7.7`
- `Boost 1.81.0`
- `unixODBC 2.3.12`
- `psqlODBC 18.00.0004`
- `nanodbc 2.14.0`
- `Catch2 3.4.0`

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

Patch the installed spdlog header so it does not use fmt compile-time format
strings. Fil-C's Clang/libc++ rejected that consteval path while compiling
iRODS.

In `/opt/fil/include/spdlog/common.h`, change:

```c++
#    define SPDLOG_FMT_STRING(format_string) FMT_STRING(format_string)
```

to:

```c++
#    define SPDLOG_FMT_STRING(format_string) format_string
```

`CMAKE_CXX_FLAGS=-DFMT_USE_CONSTEVAL=0` is still used for iRODS, but that flag
alone is not enough because spdlog expands `SPDLOG_FMT_STRING` directly to
`FMT_STRING`.

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

### Boost 1.81.0

iRODS links these Boost libraries:

- `chrono`
- `container`
- `filesystem`
- `program_options`
- `random`
- `regex`
- `system`
- `thread`

Build the `b2` host tool with the system compiler, but build the target Boost
libraries with `/opt/fil/bin/fil++`:

```bash
curl -L --fail --show-error \
  --output /tmp/opencode/boost_1_81_0.tar.gz \
  https://archives.boost.io/release/1.81.0/source/boost_1_81_0.tar.gz
tar -xf /tmp/opencode/boost_1_81_0.tar.gz -C /tmp/opencode
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
`/opt/fil/include/x86_64-unknown-linux-gnu/c++/v1`. Force the target triple back
to Fil-C's default:

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

### unixODBC 2.3.12

`/opt/fil` does not ship ODBC. Build the driver-manager libraries into
`/opt/fil` and adjust the generated build files so Fil-C's transformed exported
symbols are not hidden by libtool export-symbols files.

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
```

Apply these changes to the generated source/build tree:

- In `DriverManager/Makefile`, remove `-export-symbols ./DriverManager.exp`
  from `libodbc_la_LDFLAGS`.
- In `odbcinst/Makefile`, remove `-export-symbols ./odbcinst.exp` from
  `libodbcinst_la_LDFLAGS`.
- In `libltdl/ltdl.c`, after the internal includes, add a preload table for
  the statically linked `dlopen` loader. The generated libtool preload table was
  not usable with Fil-C and caused `lt_dlinit()` or driver loading to fail.

```c
extern lt_dlvtable *dlopen_LTX_get_vtable (lt_user_data loader_data);

const lt_dlsymlist lt_libltdlc_LTX_preloaded_symbols[] = {
  {"libltdlc", 0},
  {"dlopen", 0},
  {"get_vtable", (void *) dlopen_LTX_get_vtable},
  {0, 0}
};
```
- In `DriverManager/SQLConnect.c`, do not hold `mutex_lib_entry()` while
  calling the driver's environment allocation and environment attribute
  functions. With Fil-C-built psqlODBC, calling `SQLAllocHandle` and
  `SQLSetEnvAttr` under that mutex can deadlock during driver initialization.

Build and install the ODBC headers and driver-manager libraries:

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

Verify ODBC links with a Fil-C caller:

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

### psqlODBC 18.00.0004

iRODS needs a PostgreSQL ODBC driver built with Fil-C. Build psqlODBC after
installing the Fil-C-built unixODBC libraries:

```bash
curl -L --fail --show-error \
  --output /tmp/opencode/psqlodbc-18.00.0004.tar.gz \
  https://github.com/postgresql-interfaces/psqlodbc/archive/refs/tags/REL-18_00_0004.tar.gz
tar -xf /tmp/opencode/psqlodbc-18.00.0004.tar.gz -C /tmp/opencode
cd /tmp/opencode/psqlodbc-REL-18_00_0004
CPPFLAGS="-I/opt/fil/include -DSQLCOLATTRIBUTE_SQLLEN" \
LDFLAGS="-L/opt/fil/lib -Wl,-rpath,/opt/fil/lib" \
./configure \
  --prefix=/opt/fil \
  --with-unixodbc=/opt/fil \
  --with-libpq=/opt/fil \
  --disable-dependency-tracking
```

Apply these changes to the generated `Makefile`:

- Remove `-export-symbols-regex '^SQL'` from `AM_LDFLAGS`, otherwise Fil-C's
  transformed exported symbols are hidden.
- Add `-Wl,-Bsymbolic-functions` to `LDFLAGS`, otherwise ODBC entry-point
  symbol interposition can route psqlODBC calls back through `libodbc` and cause
  recursive driver-manager connection setup.

Then build and install:

```bash
make -j30
make install
```

Configure the Fil-C ODBC files:

```ini
# /opt/fil/etc/odbcinst.ini
[PostgreSQL ANSI]
Description=PostgreSQL ODBC driver (ANSI version)
Driver=/opt/fil/lib/psqlodbca.so
Driver64=/opt/fil/lib/psqlodbca.so
Debug=0
CommLog=1
DisableGetFunctions=1

[PostgreSQL Unicode]
Description=PostgreSQL ODBC driver (Unicode version)
Driver=/opt/fil/lib/psqlodbcw.so
Driver64=/opt/fil/lib/psqlodbcw.so
Debug=0
CommLog=1
DisableGetFunctions=1
```

```ini
# /opt/fil/etc/odbc.ini
[iRODS Catalog]
Driver=PostgreSQL ANSI
Description=iRODS Catalog
Trace=No
Debug=0
CommLog=0
TraceFile=
Database=ICAT
Servername=localhost
Port=5432
ReadOnly=No
Ksqo=0
RowVersioning=No
ShowSystemTables=No
ShowOidColumn=No
FakeOidIndex=No
ConnSettings=
```

Verify the driver stack with a Fil-C ODBC caller:

```bash
ODBCINI=/opt/fil/etc/odbc.ini \
ODBCSYSINI=/opt/fil/etc \
LD_LIBRARY_PATH=/opt/fil/lib \
  /tmp/opencode/odbc-connect-test
```

Expected result:

```text
connect rc=0
```

### nanodbc 2.14.0

```bash
curl -L --fail --show-error \
  --output /tmp/opencode/nanodbc-2.14.0.tar.gz \
  https://github.com/nanodbc/nanodbc/archive/refs/tags/v2.14.0.tar.gz
tar -xf /tmp/opencode/nanodbc-2.14.0.tar.gz -C /tmp/opencode
```

Patch `/tmp/opencode/nanodbc-2.14.0/nanodbc/nanodbc.cpp` so it does not use
`std::char_traits<NANODBC_SQLCHAR>`. libc++ does not define
`std::char_traits<unsigned char>`, and `NANODBC_SQLCHAR` is `SQLCHAR`, which is
`unsigned char`, when Unicode support is disabled.

Change the helper around `size(NANODBC_SQLCHAR const (&array)[N])` from:

```c++
auto const n = std::char_traits<NANODBC_SQLCHAR>::length(array);
```

to:

```c++
std::size_t n = 0;
while (n < N && array[n] != 0)
    ++n;
```

Add this helper nearby:

```c++
template <std::size_t N>
inline std::size_t sqlchar_length(NANODBC_SQLCHAR const (&array)[N]) noexcept
{
    return size(array);
}
```

Change the datasource and driver-name call sites from
`std::char_traits<NANODBC_SQLCHAR>::length(...)` to `sqlchar_length(...)`.

Then build and install nanodbc:

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

### Catch2 3.4.0

Build Catch2 into `/opt/fil` before enabling iRODS unit tests. The system
Catch2 package is not suitable for Fil-C unit tests because it is built for the
system ABI.

Disable Catch2's POSIX signal handling. Fil-C 0.685 reports `sigaltstack` as
unsupported, and Catch2's fatal-condition handler uses `sigaltstack` unless
`CATCH_CONFIG_NO_POSIX_SIGNALS` is defined when Catch2 is built.

```bash
curl -L --fail --show-error \
  --output /tmp/opencode/Catch2-3.4.0.tar.gz \
  https://github.com/catchorg/Catch2/archive/refs/tags/v3.4.0.tar.gz
tar -xf /tmp/opencode/Catch2-3.4.0.tar.gz -C /tmp/opencode
cmake -S /tmp/opencode/Catch2-3.4.0 \
  -B /tmp/opencode/Catch2-3.4.0-build-filc \
  -G Ninja \
  -DCMAKE_CXX_COMPILER=/opt/fil/bin/fil++ \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=/opt/fil \
  -DCMAKE_CXX_FLAGS=-DCATCH_CONFIG_NO_POSIX_SIGNALS \
  -DBUILD_TESTING=OFF \
  -DCATCH_INSTALL_DOCS=OFF \
  -DCATCH_INSTALL_EXTRAS=ON
cmake --build /tmp/opencode/Catch2-3.4.0-build-filc --parallel 30
cmake --install /tmp/opencode/Catch2-3.4.0-build-filc
```

## iRODS Source Requirement

Use a source tree containing the `plugins/microservices/src/json_parse.cpp`
fix for `IntArray_MS_T` input. That code must copy integer byte values to a
string before calling `nlohmann::json::parse`. Without that change,
nlohmann-json instantiates `std::char_traits<int>` under libc++.

Use a source tree containing the `unit_tests/src/test_data_obj_stat_api.cpp`
fix for its local object-stat helper. The helper should not be named `stat`,
and its `std::unique_ptr` deleter should be `decltype(&freeRodsObjStat)` rather
than `decltype(freeRodsObjStat)&`. Without that change, libc++ instantiates
`std::unique_ptr<rodsObjStat, int (&)(rodsObjStat*)>`, which fails under Fil-C.

Use a source tree containing the `plugins/rule_engines/irods_rule_language/src/cache.cpp`
fix for copying pointer metadata into the shared-memory cache. The destination
for the pointer table must be calculated from the real shared-memory base, not
from a pointer inside the temporary malloc buffer after pointer relocation.
Without that change, Fil-C reports an out-of-bounds `memmove` during rule-engine
startup.

Use a source tree containing the `plugins/api/src/data_object_finalize.cpp` fix
for malformed optional `file_modified` keyword payloads. Some paths can provide
an empty serialized key-value payload that does not contain `key`/`value`
members; this should not abort data object finalization.

Use a source tree containing the `lib/core/include/irods/irods_query.hpp` fix
for query error messages. Avoid formatting `irods::query_type` with fmt in this
path because Fil-C has exposed fmt 8 range/formatter crashes in client and
server query fallback paths.

## Configure iRODS

iRODS normally selects `/opt/irods-externals/clang16.0.6-0` through
`cmake/Modules/IrodsCXXCompiler.cmake`, but only when CMake compilers are not
already set. Use an isolated build tree and pass explicit Fil-C compilers.

```bash
cd /src/irods
rm -rf build-filc
cmake -S . -B build-filc -G Ninja \
  -DCMAKE_C_COMPILER=/opt/fil/bin/filcc \
  -DCMAKE_CXX_COMPILER=/opt/fil/bin/fil++ \
  -DCMAKE_BUILD_TYPE=Release \
  -DIRODS_BUILD_WITH_CLANG=OFF \
  -DIRODS_BUILD_WITH_WERROR=OFF \
  -DIRODS_USE_LIBSYSTEMD=OFF \
  -DCMAKE_CXX_FLAGS="-DFMT_USE_CONSTEVAL=0 -UBOOST_STACKTRACE_USE_ADDR2LINE -DBOOST_STACKTRACE_USE_NOOP -DBOOST_INTERPROCESS_FORCE_GENERIC_EMULATION" \
  -DCMAKE_PREFIX_PATH=/opt/fil \
  -Dfmt_DIR=/opt/fil/lib/cmake/fmt \
  -Dspdlog_DIR=/opt/fil/lib/cmake/spdlog \
  -Dnlohmann_json_DIR=/opt/fil/share/cmake/nlohmann_json \
  -DCatch2_DIR=/opt/fil/lib/cmake/Catch2 \
  -DPKG_CONFIG_EXECUTABLE=/opt/fil/bin/pkg-config \
  -DIRODS_EXTERNALS_FULLPATH_BOOST=/opt/fil \
  -DIRODS_EXTERNALS_FULLPATH_JSONCONS=/opt/fil \
  -DIRODS_EXTERNALS_FULLPATH_NANODBC=/opt/fil \
  -DODBC_LIBRARY=/opt/fil/lib/libodbc.so
```

Important configure flags:

- `IRODS_BUILD_WITH_WERROR=OFF` avoids stopping on new Clang 20 warnings in
  existing iRODS code.
- `IRODS_USE_LIBSYSTEMD=OFF` avoids a dependency not present in `/opt/fil`.
- `nlohmann_json_DIR` must point at `/opt/fil`. If CMake uses the system
  package, it injects `/usr/include` into many compile commands.
- `fmt_DIR` and `spdlog_DIR` should also be pinned to `/opt/fil` to avoid system
  ABI contamination.
- `Catch2_DIR` must point at the Fil-C-built Catch2 package before enabling
  unit tests.

## Build iRODS

```bash
cmake --build build-filc --parallel 30
```

This completed successfully with the dependency stack and source state described
above.

## Package iRODS

```bash
cmake --build build-filc --target package --parallel 30
```

This produced:

- `build-filc/irods-database-plugin-mysql_5.1.0-0~noble_amd64.deb`
- `build-filc/irods-database-plugin-oracle_5.1.0-0~noble_amd64.deb`
- `build-filc/irods-database-plugin-postgres_5.1.0-0~noble_amd64.deb`
- `build-filc/irods-dev_5.1.0-0~noble_amd64.deb`
- `build-filc/irods-icommands_5.1.0-0~noble_amd64.deb`
- `build-filc/irods-runtime_5.1.0-0~noble_amd64.deb`
- `build-filc/irods-server_5.1.0-0~noble_amd64.deb`

## Build Unit Tests

Enable unit tests in the same build tree after installing Fil-C-built Catch2:

```bash
cmake -S . -B build-filc -G Ninja \
  -DCMAKE_C_COMPILER=/opt/fil/bin/filcc \
  -DCMAKE_CXX_COMPILER=/opt/fil/bin/fil++ \
  -DCMAKE_BUILD_TYPE=Release \
  -DIRODS_BUILD_WITH_CLANG=OFF \
  -DIRODS_BUILD_WITH_WERROR=OFF \
  -DIRODS_USE_LIBSYSTEMD=OFF \
  -DIRODS_UNIT_TESTS_BUILD=YES \
  -DCMAKE_CXX_FLAGS="-DFMT_USE_CONSTEVAL=0 -UBOOST_STACKTRACE_USE_ADDR2LINE -DBOOST_STACKTRACE_USE_NOOP -DBOOST_INTERPROCESS_FORCE_GENERIC_EMULATION" \
  -DCMAKE_PREFIX_PATH=/opt/fil \
  -Dfmt_DIR=/opt/fil/lib/cmake/fmt \
  -Dspdlog_DIR=/opt/fil/lib/cmake/spdlog \
  -Dnlohmann_json_DIR=/opt/fil/share/cmake/nlohmann_json \
  -DCatch2_DIR=/opt/fil/lib/cmake/Catch2 \
  -DPKG_CONFIG_EXECUTABLE=/opt/fil/bin/pkg-config \
  -DIRODS_EXTERNALS_FULLPATH_BOOST=/opt/fil \
  -DIRODS_EXTERNALS_FULLPATH_JSONCONS=/opt/fil \
  -DIRODS_EXTERNALS_FULLPATH_NANODBC=/opt/fil \
  -DODBC_LIBRARY=/opt/fil/lib/libodbc.so
cmake --build build-filc --target all-unit_tests --parallel 30
```

`ctest -N` still reported zero tests in this build, so run the unit-test
binaries directly.

```bash
rm -rf /tmp/opencode/irods-filc-unit-test-logs
mkdir -p /tmp/opencode/irods-filc-unit-test-logs
failures=0
total=0
passed=0
for t in build-filc/unit_tests/irods_*; do
  if [ -x "$t" ] && [ -f "$t" ]; then
    name=$(basename "$t")
    total=$((total + 1))
    log="/tmp/opencode/irods-filc-unit-test-logs/${name}.log"
    printf 'RUN %s\n' "$name"
    if LD_LIBRARY_PATH="/src/irods/build-filc/lib:/src/irods/build-filc/lib/core:/src/irods/build-filc/server:/opt/fil/lib" \
      timeout 120s "$t" >"$log" 2>&1; then
      printf 'PASS %s\n' "$name"
      passed=$((passed + 1))
    else
      rc=$?
      printf 'FAIL %s rc=%s log=%s\n' "$name" "$rc" "$log"
      failures=$((failures + 1))
    fi
  fi
done
printf 'SUMMARY total=%s passed=%s failed=%s\n' "$total" "$passed" "$failures"
exit "$failures"
```

Observed direct unit-test result after rebuilding Catch2 with
`CATCH_CONFIG_NO_POSIX_SIGNALS` and adding the Fil-C shared-memory fallbacks:

```text
SUMMARY total=73 passed=34 failed=39
```

The following tests passed:

- `irods_capped_memory_resource`
- `irods_client_server_negotiation`
- `irods_data_object_proxy`
- `irods_delay_hints_parser`
- `irods_environment_variables`
- `irods_file_object`
- `irods_fixed_buffer_resource`
- `irods_fully_qualified_username`
- `irods_generate_random_alphanumeric_string`
- `irods_getRodsEnv`
- `irods_hashers`
- `irods_host_list_context_string`
- `irods_hostname_cache`
- `irods_json_events`
- `irods_key_value_proxy`
- `irods_lifetime_manager`
- `irods_linked_list_iterator`
- `irods_packstruct`
- `irods_process_stash`
- `irods_rc_data_obj_repl`
- `irods_re_serialization`
- `irods_rerror_stack`
- `irods_scoped_privileged_client`
- `irods_server_utilities`
- `irods_server_properties`
- `irods_shared_memory_object`
- `irods_system_error`
- `irods_version`
- `irods_with_durability`

Most failures are not build failures. They fall into these categories:

- Missing installed iRODS runtime environment, such as
  `/root/.irods/irods_environment.json`, for tests that expect configured client
  or server state.
- Fil-C `Not implemented` panics when iRODS exception formatting calls
  `irods::stacktrace::dump()`, which reaches `_Unwind_Backtrace` through
  `boost::stacktrace`. Configure with `-UBOOST_STACKTRACE_USE_ADDR2LINE` and
  `-DBOOST_STACKTRACE_USE_NOOP` to avoid this path.
- Boost.Interprocess managed shared memory is not safe for Fil-C capabilities in
  several iRODS tests and server code paths. Use
  `-DBOOST_INTERPROCESS_FORCE_GENERIC_EMULATION` and the iRODS `__FILC__`
  in-process fallbacks for host cache, DNS cache, replica access table, and
  access-time queue behavior.
- Fil-C use-after-free/null-object reports in a smaller number of tests, for
  example `irods_data_object_finalize`, `irods_logical_locking`, and
  `irods_server_properties`.

Representative logs are under
`/tmp/opencode/irods-filc-unit-test-logs/<test-name>.log`.

## Verify Build Tree Binaries

Use `LD_LIBRARY_PATH` so binaries find the build-tree iRODS libraries and the
Fil-C dependency slice:

```bash
LD_LIBRARY_PATH="/src/irods/build-filc/lib:/src/irods/build-filc/lib/core:/src/irods/build-filc/server:/opt/fil/lib" \
  ./build-filc/clients/icommands/ihelp

LD_LIBRARY_PATH="/src/irods/build-filc/lib:/src/irods/build-filc/lib/core:/src/irods/build-filc/server:/opt/fil/lib" \
  ./build-filc/server/main_server/irodsServer --version
```

The verified output included the iCommands help text and:

```text
irodsServer v5.1.0-7fb3557
```

## Installed Server Smoke Test

The generated packages still declare normal system package dependencies. Install
the Fil-C-built packages with `dpkg --force-depends`, then patch ELF RPATHs so
installed iRODS binaries can find both packaged iRODS libraries and `/opt/fil`:

```bash
dpkg --force-depends -i \
  build-filc/irods-runtime_5.1.0-0~noble_amd64.deb \
  build-filc/irods-icommands_5.1.0-0~noble_amd64.deb \
  build-filc/irods-server_5.1.0-0~noble_amd64.deb \
  build-filc/irods-database-plugin-postgres_5.1.0-0~noble_amd64.deb
```

After installation, use `/opt/fil/bin/patchelf --set-rpath /usr/lib:/opt/fil/lib`
on the installed ELF binaries and shared objects from the iRODS packages. Do not
add `/opt/fil/lib` to the global loader cache because that breaks normal system
tools by loading Fil-C libraries into non-Fil-C processes.

This smoke test passed with the Fil-C-built server and PostgreSQL catalog stack:

```bash
su - irods -c 'irodsServer -d'
su - irods -c 'printf "rods\n" | iinit && ils'
```

Observed output:

```text
Connecting as rods#tempZone to cfec0e96ddc2:1247 ...
/tempZone/home/rods:
```

The packaged Python `test_ils` suite also passed after a full iRODS shutdown,
removing stale `/dev/shm/irods*` and `/dev/shm/sem.irods*` entries, restoring the
`ShowCollAcls` built-in specific query if needed, and running:

```bash
su - irods -c 'cd /var/lib/irods/scripts && python3 run_tests.py --run_specific_test test_ils --no_buffer'
```

Observed result:

```text
Ran 17 tests in 778.689s

OK
python_test_status=0
```

The packaged Python `test_access_time_updates` suite passed with the Fil-C-built
server after enabling `IRODS_ACCESS_TIME_QUEUE_FILE_BACKEND`, replacing the main
server listener probe with a POSIX socket check, and cleaning stale iRODS IPC
objects before the run:

```bash
su - irods -c 'cd /var/lib/irods/scripts && python3 run_tests.py --run_specific_test test_access_time_updates --no_buffer'
```

Observed result:

```text
Ran 6 tests in 238.132s

OK
```

The packaged Python `test_all_rules.Test_AllRules` class has been exercised one
method at a time under the Fil-C-built server. All discovered methods in the
class either passed or were skipped by the upstream test decorators. This
included the previously problematic `test_rulemsiDataObjRsync` case and the
forced-copy path it exercises.

```bash
su - irods -c 'cd /var/lib/irods/scripts && python3 run_tests.py --run_specific_test test_all_rules.Test_AllRules.<method> --no_buffer'
```

The rsync rule work is captured in these commits:

- `655cbecdd Fix Fil-C rsync rule blockers`
- `9d53aaf9f Use one query for copy resource checks`

The fixes were:

- Ignore null-only file-modified JSON input such as `[[null]]` when publishing
  replica state table entries. This prevents an empty serialized `KeyValPair`
  from erasing the in-flight RST entry during overwrite handling.
- Treat positive `msiDataObjRsync` statuses as successful microservice
  execution after copying the status into the output parameter.
- Avoid the GenQuery shorthand resource-hierarchy condition used by forced
  copy. Query destination replica resource names and hierarchies once, then
  check the requested resource in C++.

Observed focused verification:

```text
test_all_rules.Test_AllRules.test_rulemsiDataObjRsync ... ok
test_all_rules.Test_AllRules.test_str_2528 ... ok
test_all_rules.Test_AllRules.test_datetimef_3767 ... ok
test_all_rules.Test_AllRules.test_type_3575 ... ok
test_all_rules.Test_AllRules.test_pattern_3575 ... ok
test_all_rules.Test_AllRules.test_return_data_structure_non_null_2604 ... ok
test_all_rules.Test_AllRules.test_writeLine_config_last_3477 ... skipped
test_all_rules.Test_AllRules.test_writeLine_config_first_3477 ... skipped
test_all_rules.Test_AllRules.test_msiAddKeyValToMspStr_works_with_empty_string__issue_6918 ... ok
test_all_rules.Test_AllRules.test_msiRmColl_removes_trash__issue_6918 ... ok
test_all_rules.Test_AllRules.test_msiRmColl_removes_collection__issue_6918 ... ok
test_all_rules.Test_AllRules.test_msiRmColl_removes_collection_via_flag__issue_6918 ... ok
test_all_rules.Test_AllRules.test_msiCheckAccess_3309 ... ok
test_all_rules.Test_AllRules.test_msiTarFileExtract_big_file__issue_4118 ... ok
test_all_rules.Test_AllRules.test_msiRenameCollection_does_rename_collections__issue_4597 ... ok
test_all_rules.Test_AllRules.test_msiRenameCollection_does_not_support_renaming_data_objects__issue_5452 ... ok
test_all_rules.Test_AllRules.test_msiDataObjPhymv_to_resource_hierarchy__3234 ... ok
test_all_rules.Test_AllRules.test_msi_atomic_apply_metadata_operations__issue_4484 ... ok
test_all_rules.Test_AllRules.test_msi_atomic_apply_metadata_operations_considers_group_permissions__issue_6190 ... ok
test_all_rules.Test_AllRules.test_msi_atomic_apply_acl_operations__issue_5001 ... ok
test_all_rules.Test_AllRules.test_msi_atomic_apply_acl_operations_considers_group_permissions__issue_6191 ... ok
test_all_rules.Test_AllRules.test_msi_touch__issue_4669 ... ok
test_all_rules.Test_AllRules.test_msiExit_prints_user_provided_error_information_on_client_side__issue_4463 ... ok
test_all_rules.Test_AllRules.test_non_admins_are_not_allowed_to_rename_zone_collection__issue_5445 ... ok
test_all_rules.Test_AllRules.test_rename_to_current_zone_collection_is_a_no_op__issue_5445 ... ok
test_all_rules.Test_AllRules.test_rename_to_existing_collection_with_different_name_is_an_error__issue_5445 ... ok
test_all_rules.Test_AllRules.test_rename_local_zone__issue_5693 ... ok
test_all_rules.Test_AllRules.test_use_lowercase_select_in_genquery_conditions__issue_4697 ... ok
test_all_rules.Test_AllRules.test_msiGetValByKey_does_not_crash_the_agent_on_bad_input_arguments__issue_5420 ... ok
test_all_rules.Test_AllRules.test_data_obj_read_for_2100MB_file__5709 ... ok
test_all_rules.Test_AllRules.test_msiDataObjRead_does_not_generate_a_stacktrace_on_bad_input_arguments__issue_4550 ... ok
test_all_rules.Test_AllRules.test_msiGetStderrInExecCmdOut_does_not_segfault_when_using_failed_out_parameter_as_input__issue_5791 ... ok
test_all_rules.Test_AllRules.test_msiDataObjChksum_with_admin_keyword__issue_6118 ... ok
test_all_rules.Test_AllRules.test_adding_user_to_more_than_SQL_MAX_ROWS_groups_and_try_msiCheckAccess__issue_7050 ... ok
test_all_rules.Test_AllRules.test_msiRemoveUserFromGroup__issue_7165 ... ok
test_all_rules.Test_AllRules.test_msiRemoveUserFromGroup_supports_remote_users__issue_7165 ... ok
test_all_rules.Test_AllRules.test_msiRemoveUserFromGroup_correctly_handles_incorrect_arguments__issue_7165 ... ok
test_all_rules.Test_AllRules.test_msiRemoveUserFromGroup_returns_error_when_attempting_to_remove_user_from_group_which_they_are_not_a_member_of__issue_7165 ... ok
test_all_rules.Test_AllRules.test_msiSetKeyValuePairsToObj_does_not_crash__issue_7027 ... ok
test_all_rules.Test_AllRules.test_msiDataObjChksum_does_not_lead_to_segfault_on_good_verification_result__issue_7859 ... ok
test_all_rules.Test_AllRules.test_vault_path_random_scheme_customization_options__issue_8917 ... ok
test_all_rules.Test_AllRules.test_irule_cannot_modify_the_random_scheme_via_acSetVaultPathPolicy__issue_8917 ... ok
test_all_rules.Test_AllRules.test_delay_server_executes_delay_rule_as_the_user_who_scheduled_it__issue_9059 ... ok
```

The packaged Python `test_all_rules.Test_JSON_microservices` class passed after
removing remaining `fmt::join()` use from the JSON microservice debug paths:

```bash
su - irods -c 'cd /var/lib/irods/scripts && python3 run_tests.py --run_specific_test test_all_rules.Test_JSON_microservices --no_buffer'
```

Observed result:

```text
<__main__.RegisteredTestResult run=12 errors=0 failures=0>
```

The failing cases were `msi_json_names()` and then byte-list input to
`msi_json_parse()`. Both failures were due to Fil-C-sensitive `fmt::join()` use in
debug logging. Replacing those joins with straightforward string construction
allowed the full JSON microservice class to pass.

The remaining `test_all_rules` core entries also passed individually:

```text
test_all_rules.Test_msiDataObjRepl_checksum_keywords ... ok
test_all_rules.test_msi_replica_truncate ... ok
```

The packaged Python auth tests passed once the required local OS account existed:

```bash
env -u LD_LIBRARY_PATH useradd -m irodsauthuser
printf '%s:%s\n' 'irodsauthuser' ';=iamnotasecret' | env -u LD_LIBRARY_PATH chpasswd
su - irods -c 'cd /var/lib/irods/scripts && python3 run_tests.py --run_specific_test test_auth.Test_Auth --no_buffer'
su - irods -c 'cd /var/lib/irods/scripts && python3 run_tests.py --run_specific_test test_auth.test_iinit --no_buffer'
```

Observed results:

```text
test_auth.Test_Auth: rc=0
test_auth.test_iinit: rc=0
```

The packaged Python tests through `test_configuration` passed after the
`test_all_rules` and auth work:

```text
test_catalog: rc=0
test_client_hints: rc=0
test_collection_mtime: rc=0
test_configuration: rc=0
```

The delay queue class is not fully passing under Fil-C yet:

```bash
su - irods -c 'cd /var/lib/irods/scripts && python3 run_tests.py --run_specific_test test_delay_queue.Test_Delay_Queue --no_buffer'
su - irods -c 'cd /var/lib/irods/scripts && python3 run_tests.py --run_specific_test test_delay_queue.Test_Delay_Queue.test_delay_queue_with_long_job --no_buffer'
```

Observed failure:

```text
test_delay_queue.Test_Delay_Queue.test_delay_queue_with_long_job ... FAIL
AssertionError: 5 != 1
AssertionError: 5 != 2
```

The failure appears to be timing-related rather than a crash. The test schedules
five immediate jobs, five jobs due after 15 seconds, and one long-running job.
It waits for all immediate jobs to finish and then asserts that all five later
jobs are still queued. Under Fil-C, the immediate-job polling takes long enough
that some later jobs are already due and have started or completed before that
assertion. The observed queued later-rule count was one in the class run and two
when the failing method was run in isolation.

The next packaged Python modules passed individually after documenting the delay
queue timing failure:

```text
test_delay_queue.Test_Execution_Frequency: rc=0
test_dynamic_peps: rc=0
test_genquery2_microservices: rc=0
```

The packaged Python `test_iadmin.Test_Iadmin` class passed after addressing
Fil-C-sensitive replication rebalance and resource-modification paths:

```text
test_iadmin.Test_Iadmin.test_empty_data_mode_does_not_cause_INVALID_LEXICAL_CAST_on_rebalance__issue_5227: rc=0
test_iadmin.Test_Iadmin.test_modify_resource_changing_parent_context_string__issue__4022: rc=0
test_iadmin.Test_Iadmin: rc=0
test_iadmin.Test_Iadmin_Queries: rc=0
test_iadmin.Test_Iadmin_Resources: rc=0
test_iadmin.Test_Iadmin_modrepl: rc=0
test_iadmin.Test_Issue3862: rc=0
test_iadmin.test_making_groups: rc=0
test_iadmin.test_mkzone_conn_str_validation: rc=0
test_iadmin.test_moduser_group: rc=0
test_iadmin.test_moduser_remove_password__issue_2899: rc=0
test_iadmin.test_moduser_user: rc=0
test_iadmin.test_modzone_conn_str_validation: rc=0
test_iadmin_set_grid_configuration.test_get_grid_configuration: rc=0
test_iadmin_set_grid_configuration.test_set_grid_configuration: rc=0
test_ibun: rc=0
test_icd: rc=0
test_ichksum: rc=0
test_ichmod.Test_ichmod: rc=0
test_ichmod.test_collection_acl_inheritance: rc=0
test_icommands_file_operations.Test_ICommands_File_Operations_1: rc=0
test_icommands_file_operations.Test_ICommands_File_Operations_2: rc=0
test_icommands_file_operations.Test_ICommands_File_Operations_3: rc=0
test_icommands_file_operations.Test_ICommands_File_Operations_4: rc=0
test_icommands_file_operations.Test_ICommands_File_Operations_5: rc=0
test_icp.Test_Icp: rc=0
test_icp.test_overwriting: rc=0
test_iexit: rc=0
test_ifsck: rc=0
test_iget: rc=0
test_igroupadmin.Test_Igroupadmin: rc=0
test_igroupadmin.test_making_groups: rc=0
test_ihelp: rc=0
test_ilsresc: rc=0
test_imeta_admin_mode: rc=0
test_imeta_error_handling: rc=0
test_imeta_help: rc=0
test_imeta_set.Test_ImetaCp: rc=0
test_imeta_set.Test_ImetaLsLongmode: rc=0
test_imeta_set.Test_ImetaSet: rc=0
test_imiscsvrinfo: rc=0
test_imkdir: rc=0
test_imv.Test_Imv: rc=0
test_imv.test_moving_and_renaming_collections_with_multibyte_characters__issue_6239: rc=0
test_imv.test_renaming_collections_with_special_characters__issue_6239: rc=0
test_ipasswd: rc=0
test_iphymv.Test_iPhymv: rc=0
test_iphymv.test_invalid_parameters: rc=0
test_iphymv.test_iphymv_exit_codes: rc=0
test_iphymv.test_iphymv_repl_status: rc=0
test_iphymv.test_iphymv_with_two_basic_ufs_resources: rc=0
test_ips: rc=0
test_iput.Test_Iput: rc=0
test_iput.test_iput_with_checksums: rc=0
test_iput_options.Test_iPut_Options: rc=0
test_iput_options.Test_iPut_Options_Issue_3883: rc=0
test_ipwd: rc=0
test_iqmod: rc=0
test_iqstat: rc=0
test_iquery: rc=0
test_iquest.Test_Iquest: rc=0
test_iquest.test_iquest_logical_or_operator_with_data_resc_hier: rc=0
test_iquest.test_iquest_with_data_resc_hier: rc=0
test_ireg.Test_Ireg: rc=0
test_ireg.test_ireg_options: rc=0
test_ireg.test_ireg_replica: rc=0
test_irepl.Test_Irepl: rc=0
test_irepl.test_all_permission_levels__issue_7444_7465_7816: rc=0
test_irepl.test_invalid_parameters: rc=0
test_irepl.test_irepl_repl_status: rc=0
test_irepl.test_irepl_replication_hierarchy: rc=0
test_irepl.test_irepl_with_special_resource_configurations: rc=0
test_irepl.test_irepl_with_two_basic_ufs_resources: rc=0
test_irm: rc=0
test_irmdir: rc=0
test_irmtrash: rc=0
test_irsync: rc=0
test_irule: rc=0
test_iscan: rc=0
test_istream: rc=0
test_isysmeta: rc=0
test_iticket: rc=0
test_itouch: rc=0
test_itree: rc=0
test_itrim.Test_Itrim: rc=0
test_itrim.test_itrim_target_replica_selection_decision_making__issue_7515: rc=0
test_iunreg: rc=0
test_iuserinfo: rc=0
test_izonereport: rc=0
```

The fixes were:

- Normalize one-element array-wrapped key/value maps from `data_object_finalize`
  before converting them to `KeyValPair`, so `openType` reaches the replication
  resource during finalize.
- Avoid parsing an empty `in_pdmo` string while handling replication resource
  file-modified operations.
- Remove `fmt::join()` from replication rebalance SQL list generation, including
  the Postgres database plugin leaf-bundle query path.
- Set the new parent-context value passed to the modify-resource pre/post PEPs,
  instead of leaving the third rule argument unset for `iadmin modresc ...
  parent_context ...`.

The `test_ips` entry requires the `irods_test_issue_8733` helper. Build it by
configuring the Fil-C tree with `IRODS_ENABLE_ALL_TESTS=ON`, building the
`irods_test_issue_8733` target, and installing the generated `/usr/sbin` helper.
The enabled install rules also require the test microservice plugin targets to
exist before running `cmake --install`.

The `test_iquest.Test_Iquest` class needed another `fmt::join()` removal in the
GenQuery1 `DATA_RESC_HIER` condition translator used for resource-hierarchy
`LIKE` queries.

The `test_irepl.test_all_permission_levels__issue_7444_7465_7816` entry needed
the same `fmt::join()` avoidance in the group-permission query assembled by the
data object replication API.

## Remaining Caveats

- The installed server smoke test passes, but the packaging metadata still needs
  work before these packages can be installed normally with `apt`.
- Unit tests were built and executed directly from the build tree. Some failures
  are expected without an installed/configured iRODS server environment, but the
  Fil-C stacktrace and inline-assembly panics are real follow-up work.
- The generated `.deb` metadata still describes normal iRODS/system dependency
  packages. It does not yet describe the ad hoc `/opt/fil` dependency stack.
- These packages are build artifacts for exploration, not redistributable Fil-C
  packages yet.
- `help2man` was not found in `/opt/fil`; configure and packaging completed, but
  manpage-specific behavior may need more attention.
- The `/opt/fil` dependency changes are local machine state and are not captured
  by the iRODS repository except through these notes.

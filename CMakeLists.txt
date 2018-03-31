cmake_minimum_required(VERSION 3.9)
project(hinarin C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "-O0 -g")

if (TOOLCHAIN)
  set(TOOLCHAIN_HOST --host=${TOOLCHAIN})
endif(TOOLCHAIN)

include(ExternalProject)

ExternalProject_Add(moddable
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/moddable-cmake
        DOWNLOAD_COMMAND ""
        PATCH_COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR}/libs/moddable && patch -p1 < ../moddable-cmake/patch.diff
        CMAKE_ARGS
        "-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/libs"
        "-DCMAKE_RC_COMPILER=${BINPATH}${CMAKE_RC_COMPILER}"
        "-DCMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}"
        "-DCMAKE_C_COMPILER=${BINPATH}${CMAKE_C_COMPILER}"
        "-DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}"
        )

ExternalProject_Add(gumbo-parser
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/gumbo-parser
        DOWNLOAD_COMMAND ""
        PATCH_COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR}/libs/gumbo-parser && ./autogen.sh
        CONFIGURE_COMMAND
        export "CC=${BINPATH}${CMAKE_C_COMPILER}" &&
        export "CFLAGS=${CMAKE_C_FLAGS}" &&
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/gumbo-parser/configure
        --prefix=${CMAKE_BINARY_DIR}/libs
        ${TOOLCHAIN_HOST}
        )
ExternalProject_Add(fontconfig
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/fontconfig
        DOWNLOAD_COMMAND ""
        PATCH_COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR}/libs/fontconfig && ./autogen.sh && make distclean
        CONFIGURE_COMMAND
        export "CC=${BINPATH}${CMAKE_C_COMPILER}" &&
        export "CFLAGS=${CMAKE_C_FLAGS}" &&
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/fontconfig/configure
        --prefix=${CMAKE_BINARY_DIR}/libs
        --enable-static=yes
        --enable-shared=no
        --disable-libxml2
        --with-expat=${CMAKE_BINARY_DIR}/libs
        ${TOOLCHAIN_HOST}
        )
ExternalProject_Add(freetype2
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/freetype2
        DOWNLOAD_COMMAND ""
        PATCH_COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR}/libs/freetype2 && ./autogen.sh
        CONFIGURE_COMMAND
        export "CC=${BINPATH}${CMAKE_C_COMPILER}" &&
        export "CFLAGS=${CMAKE_C_FLAGS}" &&
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/freetype2/configure
        --prefix=${CMAKE_BINARY_DIR}/libs
        --enable-static=yes
        --enable-shared=no
        --without-png
        --without-bzip2
        --without-zlib
        --without-harfbuzz
        ${TOOLCHAIN_HOST}
        )
ExternalProject_Add(expat
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/libexpat/expat
        DOWNLOAD_COMMAND ""
        PATCH_COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR}/libs/libexpat/expat && ./buildconf.sh
        CONFIGURE_COMMAND
        export "CC=${BINPATH}${CMAKE_C_COMPILER}" &&
        export "CFLAGS=${CMAKE_C_FLAGS}" &&
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/libexpat/expat/configure
        --prefix=${CMAKE_BINARY_DIR}/libs
        --enable-static=yes
        --enable-shared=no
        --without-docbook
        ${TOOLCHAIN_HOST}
        BUILD_COMMAND ""
        INSTALL_COMMAND make install || true
        )
ExternalProject_Add(nuklear
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/nuklear
        DOWNLOAD_COMMAND ""
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND mkdir -p ${CMAKE_BINARY_DIR}/libs/include && cp ${CMAKE_CURRENT_SOURCE_DIR}/libs/nuklear/nuklear.h ${CMAKE_CURRENT_SOURCE_DIR}/libs/nuklear/demo/glfw_opengl3/nuklear_glfw_gl3.h ${CMAKE_BINARY_DIR}/libs/include
        )
ExternalProject_Add(glfw3
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/glfw
        DOWNLOAD_COMMAND ""
        CMAKE_ARGS
        -DGLFW_BUILD_EXAMPLES=OFF
        -DGLFW_BUILD_TESTS=OFF
        -DGLFW_BUILD_DOCS=OFF
        "-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/libs"
        "-DCMAKE_RC_COMPILER=${BINPATH}${CMAKE_RC_COMPILER}"
        "-DCMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}"
        "-DCMAKE_C_COMPILER=${BINPATH}${CMAKE_C_COMPILER}"
        "-DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}"
        )

if (WIN32)
  ExternalProject_Add(glew
          SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/glew
          DOWNLOAD_COMMAND ""
          PATCH_COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR}/libs/glew && make extensions
          CONFIGURE_COMMAND cmake ${CMAKE_CURRENT_SOURCE_DIR}/libs/glew/build/cmake
          "-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/libs"
          "-DCMAKE_RC_COMPILER=${BINPATH}${CMAKE_RC_COMPILER}"
          "-DCMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}"
          "-DCMAKE_C_COMPILER=${BINPATH}${CMAKE_C_COMPILER}"
          "-DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}"
          )
  ExternalProject_Add(zlib
          SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/zlib
          DOWNLOAD_COMMAND ""
          CMAKE_ARGS
          "-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/libs"
          "-DCMAKE_RC_COMPILER=${BINPATH}${CMAKE_RC_COMPILER}"
          "-DCMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}"
          "-DCMAKE_C_COMPILER=${BINPATH}${CMAKE_C_COMPILER}"
          "-DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}"
          )
  ExternalProject_Add(curl
          SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/curl
          DOWNLOAD_COMMAND ""
          PATCH_COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR}/libs/curl && ./buildconf
          CONFIGURE_COMMAND
          export "PKG_CONFIG_PATH=${CMAKE_BINARY_DIR}/libs/lib/pkgconfig" &&
          export "CC=${BINPATH}${CMAKE_C_COMPILER}" &&
          export "CFLAGS=${CMAKE_C_FLAGS}" &&
          ${CMAKE_CURRENT_SOURCE_DIR}/libs/curl/configure
          --disable-crypto-auth
          --disable-pthreads
          --disable-threaded-resolver
          --disable-manual
          --disable-gopher
          --disable-smtp
          --disable-smb
          --disable-imap
          --disable-pop3
          --disable-tftp
          --disable-telnet
          --disable-dict
          --disable-rtsp
          --disable-ldaps
          --disable-ldap
          --enable-static=yes
          --enable-shared=no
          --prefix=${CMAKE_BINARY_DIR}/libs
          --with-winssl
          --without-libidn2
          --without-libpsl
          --without-librtmp
          --without-nghttp2
          ${TOOLCHAIN_HOST}
          )
  ExternalProject_Add(pthread-win32
          SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/pthread-win32
          DOWNLOAD_COMMAND ""
          CONFIGURE_COMMAND ""
          BUILD_COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR}/libs/pthread-win32 && make clean GC GC-static CROSS=${TOOLCHAIN}-
          INSTALL_COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR}/libs/pthread-win32 && make install DESTROOT=${CMAKE_BINARY_DIR}/libs
          )
endif (WIN32)

if (NOT WIN32)
  find_package(OpenGL REQUIRED)
  find_package(OpenSSL REQUIRED)
  find_package(GLEW REQUIRED)
  find_package(ZLIB REQUIRED)
  find_package(CURL REQUIRED)
  find_package(X11 REQUIRED)
endif(NOT WIN32)

include_directories(${CMAKE_BINARY_DIR}/libs/include ${CMAKE_BINARY_DIR}/libs/include/xs ${OPENGL_INCLUDE_DIR} ${OPENSSL_INCLUDE_DIR} ${CURL_INCLUDE_DIRECTORIES} ${X11_INCLUDE_DIR})
link_directories(${CMAKE_BINARY_DIR}/libs/lib ${CMAKE_BINARY_DIR}/libs/lib64)

set(SOURCES src/main.c src/bind_nuklear.c src/bind_nuklear.h src/bind_root.c src/bind_root.h src/string.c src/string.h src/file.c src/file.h src/download.c src/download.h src/linkedmap.c src/linkedmap.h src/platform.c src/hinarin.h)
if (WIN32)
  add_executable(hinarin WIN32 ${SOURCES})
  target_link_libraries(hinarin ${OPENGL_gl_LIBRARY} glfw3.a gumbo.a xs.a fontconfig.a freetype.a expat.a libzlibstatic.a curl.a glew32.a pthread.a crypt32 ws2_32 opengl32)
  add_dependencies(hinarin moddable gumbo-parser expat fontconfig freetype2 nuklear glfw3 glew zlib curl pthread-win32)
else (WIN32)
  add_executable(hinarin ${SOURCES})
  target_link_libraries(hinarin ${OPENGL_gl_LIBRARY} glfw3.a gumbo.a xs.a fontconfig.a freetype.a expat.a ${ZLIB_LIBRARIES} ${CURL_LIBRARIES} ${OPENSSL_LIBRARIES} ${GLEW_LIBRARIES} ${X11_LIBRARIES} dl pthread m uuid)
  add_dependencies(hinarin moddable gumbo-parser expat fontconfig freetype2 nuklear glfw3)
endif (WIN32)
set_target_properties(hinarin PROPERTIES LINKER_LANGUAGE C)
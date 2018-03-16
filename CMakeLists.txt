cmake_minimum_required(VERSION 3.9)
project(hinarin C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s")

include(ExternalProject)

if (TOOLCHAIN)
  set(TOOLCHAIN_HOST --host=${TOOLCHAIN})
endif(TOOLCHAIN)

ExternalProject_Add(jerryscript SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/jerryscript" DOWNLOAD_COMMAND "" CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/libs" -DJERRY_CMDLINE=OFF -DJERRY_LIBC=OFF "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}")
ExternalProject_Add(gumbo-parser SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/gumbo-parser" DOWNLOAD_COMMAND "" CONFIGURE_COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR}/libs/gumbo-parser && ./autogen.sh COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/libs/gumbo-parser/configure --prefix=${CMAKE_BINARY_DIR}/libs ${TOOLCHAIN_HOST})
ExternalProject_Add(nuklear SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/nuklear" DOWNLOAD_COMMAND "" CONFIGURE_COMMAND "" BUILD_COMMAND "" INSTALL_COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/libs/nuklear/nuklear.h ${CMAKE_CURRENT_SOURCE_DIR}/libs/nuklear/demo/glfw_opengl3/nuklear_glfw_gl3.h ${CMAKE_BINARY_DIR}/libs/include)
ExternalProject_Add(glfw3 SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/glfw" DOWNLOAD_COMMAND "" CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/libs" -DGLFW_BUILD_EXAMPLES=OFF -DGLFW_BUILD_TESTS=OFF -DGLFW_BUILD_DOCS=OFF "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}")

if (WIN32)
  ExternalProject_Add(glew SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/glew" DOWNLOAD_COMMAND "" COMMAND cd "${CMAKE_CURRENT_SOURCE_DIR}/libs/glew" && make extensions CONFIGURE_COMMAND cmake "${CMAKE_CURRENT_SOURCE_DIR}/libs/glew/build/cmake" "-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/libs" "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}")
  ExternalProject_Add(zlib SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/zlib" DOWNLOAD_COMMAND "" CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/libs" "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}")
  ExternalProject_Add(curl SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/curl" DOWNLOAD_COMMAND "" COMMAND cd "${CMAKE_CURRENT_SOURCE_DIR}/libs/curl" && ./buildconf CONFIGURE_COMMAND PKG_CONFIG_PATH=${CMAKE_BINARY_DIR}/libs/lib/pkgconfig "${CMAKE_CURRENT_SOURCE_DIR}/libs/curl/configure" --disable-crypto-auth --disable-pthreads --disable-threaded-resolver --disable-manual --disable-gopher --disable-smtp --disable-smb --disable-imap --disable-pop3 --disable-tftp --disable-telnet --disable-dict --disable-rtsp --disable-ldaps --disable-ldap --enable-static=yes --enable-shared=no "--prefix=${CMAKE_BINARY_DIR}/libs" --with-winssl --without-libidn2 --without-libpsl --without-librtmp --without-nghttp2 ${TOOLCHAIN_HOST})
  ExternalProject_Add(pthread-win32 SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/pthread-win32" DOWNLOAD_COMMAND "" CONFIGURE_COMMAND "" BUILD_COMMAND cd "${CMAKE_CURRENT_SOURCE_DIR}/libs/pthread-win32" && make clean GC GC-static "CROSS=${TOOLCHAIN}-" INSTALL_COMMAND cd "${CMAKE_CURRENT_SOURCE_DIR}/libs/pthread-win32" && make install "DESTROOT=${CMAKE_BINARY_DIR}/libs")
endif (WIN32)

find_package(OpenGL REQUIRED)
if (NOT WIN32)
  find_package(OpenSSL REQUIRED)
  find_package(GLEW REQUIRED)
  find_package(ZLIB REQUIRED)
  find_package(CURL REQUIRED)
  find_package(X11 REQUIRED)
endif(NOT WIN32)

include_directories(${CMAKE_BINARY_DIR}/libs/include ${OPENGL_INCLUDE_DIR} ${OPENSSL_INCLUDE_DIR} ${CURL_INCLUDE_DIRECTORIES} ${X11_INCLUDE_DIR})
link_directories(${CMAKE_BINARY_DIR}/libs/lib ${CMAKE_BINARY_DIR}/libs/lib64)

set(SOURCES main.c nuklear_binding.c nuklear_binding.h util.c util.h system_binding.c system_binding.h modules_binding.c modules_binding.h)
if (WIN32)
  add_executable(hinarin WIN32 ${SOURCES})
  target_link_libraries(hinarin ${OPENGL_gl_LIBRARY} glfw3.a jerry-core.a jerry-ext.a jerry-libm.a jerry-port-default.a gumbo.a libzlibstatic.a curl.a glew32.a pthread.a crypt32 ws2_32)
  add_dependencies(hinarin jerryscript gumbo-parser nuklear glfw3 glew zlib curl pthread-win32)
else (WIN32)
  add_executable(hinarin ${SOURCES})
  target_link_libraries(hinarin ${OPENGL_gl_LIBRARY} glfw3.a jerry-core.a jerry-ext.a jerry-libm.a jerry-port-default.a gumbo.a ${ZLIB_LIBRARIES} ${CURL_LIBRARIES} ${OPENSSL_LIBRARIES} ${GLEW_LIBRARIES} ${X11_LIBRARIES} dl pthread m)
  add_dependencies(hinarin jerryscript gumbo-parser nuklear glfw3)
endif (WIN32)

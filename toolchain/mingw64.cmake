# the name of the target operating system
set(CMAKE_SYSTEM_NAME Windows)
set(TOOLCHAIN x86_64-w64-mingw32)
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)

# which compilers to use for C and C++
#set(CMAKE_C_FLAGS "--target=x86_64-w64-mingw32 -I${CMAKE_FIND_ROOT_PATH}/include -L${CMAKE_FIND_ROOT_PATH}/lib")
#set(CMAKE_CXX_FLAGS "--target=x86_64-w64-mingw32 -I${CMAKE_FIND_ROOT_PATH}/include -L${CMAKE_FIND_ROOT_PATH}/lib")

#set(CMAKE_C_COMPILER clang)
#set(CMAKE_CXX_COMPILER clang++)
#set(CMAKE_C_LINK_EXECUTABLE ${CMAKE_FIND_ROOT_PATH}/bin/llvm-link)
#set(CMAKE_CXX_LINK_EXECUTABLE ${CMAKE_FIND_ROOT_PATH}/bin/llvm-link)
#set(CMAKE_LINKER ${CMAKE_FIND_ROOT_PATH}/bin/llvm-link)
#set(CMAKE_C_LINK_EXECUTABLE llvm-link)
#set(CMAKE_C_LINK_EXECUTABLE llvm-link)
#set(CMAKE_CXX_LINK_EXECUTABLE llvm-link)


# here is the target environment located
# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search 
# programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

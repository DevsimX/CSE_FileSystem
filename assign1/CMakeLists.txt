cmake_minimum_required(VERSION 3.14)
project(assign1 C)

set(CMAKE_C_STANDARD 11)

include_directories(/usr/local/Cellar/openssl/1.0.2t/include)

link_directories(/usr/local/Cellar/openssl/1.0.2t/lib)

link_libraries(libcrypto.a)

add_executable(assign1
        chksumfile.c
        chksumfile.h
        directory.c
        directory.h
        direntv6.h
        diskimageaccess.c
        diskimg.c
        diskimg.h
        file.c
        file.h
        filsys.h
        ino.h
        inode.c
        inode.h
        pathname.c
        pathname.h
        unixfilesystem.c
        unixfilesystem.h)

target_link_libraries(assign1 libcrypto.a)

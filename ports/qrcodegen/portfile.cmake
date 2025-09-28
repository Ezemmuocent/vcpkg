vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO nayuki/QR-Code-generator
    REF v1.8.0
    SHA512 0cdf0873e71aed124fc7357da86fb26f23fd26432f94c9752fa5a044085b26e5aece2115134d0e50213ff24be7c55818e7dec31205a68751065bc82ab0c2c6ac
    HEAD_REF master
)

# Create library from source files
file(GLOB QR_SRC "${SOURCE_PATH}/cpp/*.cpp")
file(GLOB QR_HDR "${SOURCE_PATH}/cpp/*.hpp")

# Install headers
file(INSTALL ${QR_HDR} DESTINATION ${CURRENT_PACKAGES_DIR}/include/qrcodegen)

# Build static library
vcpkg_cmake_configure(
    SOURCE_PATH ${SOURCE_PATH}/cpp
    OPTIONS
        -DBUILD_SHARED_LIBS=OFF
)

vcpkg_cmake_build()

# Install the library
file(INSTALL ${CMAKE_BINARY_DIR}/qrcodegen.lib DESTINATION ${CURRENT_PACKAGES_DIR}/lib)
file(INSTALL ${CMAKE_BINARY_DIR}/qrcodegen.lib DESTINATION ${CURRENT_PACKAGES_DIR}/debug/lib)

# Generate CMake config
file(WRITE ${CURRENT_PACKAGES_DIR}/share/qrcodegen/qrcodegenConfig.cmake
"include(\"\${CMAKE_CURRENT_LIST_DIR}/../../include\")
add_library(qrcodegen STATIC IMPORTED)
set_target_properties(qrcodegen PROPERTIES
    IMPORTED_LOCATION \"\${CMAKE_CURRENT_LIST_DIR}/../../lib/qrcodegen.lib\"
    INTERFACE_INCLUDE_DIRECTORIES \"\${CMAKE_CURRENT_LIST_DIR}/../../include\"
)")

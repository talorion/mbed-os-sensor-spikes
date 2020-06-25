#########################################
## Cross Compile
#########################################
SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_PROCESSOR arm)
SET(CMAKE_CROSSCOMPILING TRUE)

#########################################
## force compiler settings
#########################################
SET(CMAKE_C_COMPILER_WORKS TRUE)
SET(CMAKE_CXX_COMPILER_WORKS TRUE)

#########################################
## force cmake compilers
#########################################
if(WIN32)
SET(CMAKE_ASM_COMPILER    "arm-none-eabi-gcc")
SET(CMAKE_C_COMPILER      "arm-none-eabi-gcc")
SET(CMAKE_CXX_COMPILER    "arm-none-eabi-g++")
SET(ELF2BIN               "arm-none-eabi-objcopy")
SET(CMAKE_LD_COMPILER     "arm-none-eabi-cpp")
SET(CMAKE_C_LINK_EXECUTABLE "arm-none-eabi-cpp")
endif(WIN32)

if(UNIX)
SET(CMAKE_ASM_COMPILER    "/opt/gcc-arm-none-eabi/bin/arm-none-eabi-gcc")
SET(CMAKE_C_COMPILER      "/opt/gcc-arm-none-eabi/bin/arm-none-eabi-gcc")
SET(CMAKE_CXX_COMPILER    "/opt/gcc-arm-none-eabi/bin/arm-none-eabi-g++")
SET(ELF2BIN               "/opt/gcc-arm-none-eabi/bin/arm-none-eabi-objcopy")
SET(CMAKE_LD_COMPILER     "/opt/gcc-arm-none-eabi/bin/arm-none-eabi-cpp")
SET(CMAKE_C_LINK_EXECUTABLE "/opt/gcc-arm-none-eabi/bin/arm-none-eabi-cpp")

#SET(CMAKE_ASM_COMPILER    "/opt/gcc-arm-none-eabi-9-2019-q4-major/bin/arm-none-eabi-gcc")
#SET(CMAKE_C_COMPILER      "/opt/gcc-arm-none-eabi-9-2019-q4-major/bin/arm-none-eabi-gcc")
#SET(CMAKE_CXX_COMPILER    "/opt/gcc-arm-none-eabi-9-2019-q4-major/bin/arm-none-eabi-g++")
#SET(ELF2BIN               "/opt/gcc-arm-none-eabi-9-2019-q4-major/bin/arm-none-eabi-objcopy")

#SET(CMAKE_LD_COMPILER     "/opt/gcc-arm-none-eabi-9-2019-q4-major/bin/arm-none-eabi-cpp")
endif(UNIX)

#########################################
## generate compile database
#########################################
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#########################################
#verbose build process
#########################################
SET(CMAKE_VERBOSE_MAKEFILE OFF )

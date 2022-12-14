cmake_minimum_required(VERSION 3.9)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake/Modules")
project(SOEM
    DESCRIPTION "mycython"
    VERSION 1.4.0
    LANGUAGES C)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  # Default to installing in SOEM source directory
  set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_LIST_DIR}/install)
endif()

set(SOEM_INCLUDE_INSTALL_DIR include/soem)
set(SOEM_LIB_INSTALL_DIR lib)

set(MOTION_INCLUDE_INSTALL_DIR include/motion)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  set(BUILD_TESTS TRUE)
else()
  message(STATUS "SOEM: not building tests when built as dependency")
  set(BUILD_TESTS FALSE)
endif()

if(WIN32)
  set(OS "win32")
  include_directories(oshw/win32/wpcap/Include)
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    link_directories(${CMAKE_CURRENT_LIST_DIR}/oshw/win32/wpcap/Lib/x64)
  elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    link_directories(${CMAKE_CURRENT_LIST_DIR}/oshw/win32/wpcap/Lib)
  endif()
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /D _CRT_SECURE_NO_WARNINGS")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  /WX")
  set(OS_LIBS wpcap.lib Packet.lib Ws2_32.lib Winmm.lib)
elseif(UNIX AND NOT APPLE)
  set(OS "linux")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")
  set(OS_LIBS pthread rt)
elseif(APPLE)
  # This must come *before* linux or MacOSX will identify as Unix.
  set(OS "macosx")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")
  set(OS_LIBS pthread pcap)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "rt-kernel")
  set(OS "rtk")
  message(STATUS "ARCH is ${ARCH}")
  message(STATUS "BSP is ${BSP}")
  include_directories(oshw/${OS}/${ARCH})
  file(GLOB OSHW_EXTRA_SOURCES oshw/${OS}/${ARCH}/*.c)
  set(OSHW_SOURCES "${OS_HW_SOURCES} ${OSHW_ARCHSOURCES}")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-but-set-variable")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-function")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-format")
  set(OS_LIBS "-Wl,--start-group -l${BSP} -l${ARCH} -lkern -ldev -lsio -lblock -lfs -lusb -llwip -leth -li2c -lrtc -lcan -lnand -lspi -lnor -lpwm -ladc -ltrace -lc -lm -Wl,--end-group")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "rtems")
  message(STATUS "Building for RTEMS")
  set(OS "rtems")
  set(SOEM_LIB_INSTALL_DIR ${LIB_DIR})
  set(BUILD_TESTS FALSE)
endif()

message(STATUS "OS is ${OS}")

file(GLOB SOEM_SOURCES soem/soem/*.c)
file(GLOB OSAL_SOURCES soem/osal/${OS}/*.c)
file(GLOB OSHW_SOURCES soem/oshw/${OS}/*.c)
file(GLOB MOTION_SOURCES motion/*.c)

file(GLOB SOEM_HEADERS soem/soem/*.h)
file(GLOB OSAL_HEADERS soem/osal/osal.h osal/${OS}/*.h)
file(GLOB OSHW_HEADERS soem/oshw/${OS}/*.h)
file(GLOB MOTION_HEADERS motion/*.h)

add_library(motion STATIC
  ${SOEM_SOURCES}
  ${OSAL_SOURCES}
  ${OSHW_SOURCES}
  ${OSHW_EXTRA_SOURCES}
  ${MOTION_SOURCES})

target_link_libraries(motion ${OS_LIBS})


target_include_directories(motion PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/soem/soem>
  $<INSTALL_INTERFACE:include/soem>)

target_include_directories(motion PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/soem/osal>
  $<INSTALL_INTERFACE:include/soem>)

target_include_directories(motion PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/soem/osal/${OS}>
  $<INSTALL_INTERFACE:include/soem>)

target_include_directories(motion
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/soem/oshw/${OS}>
  $<INSTALL_INTERFACE:include/soem>
  )

target_include_directories(motion
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/motion>
  $<INSTALL_INTERFACE:include/motion>
  )

message(STATUS "LIB_DIR: ${SOEM_LIB_INSTALL_DIR}")

install(TARGETS motion EXPORT soemConfig DESTINATION ${SOEM_LIB_INSTALL_DIR})

install(EXPORT soemConfig DESTINATION share/soem/cmake)

install(FILES
  ${SOEM_HEADERS}
  ${OSAL_HEADERS}
  ${OSHW_HEADERS}
  ${MOTION_HEADERS}
  DESTINATION ${SOEM_INCLUDE_INSTALL_DIR})

set(SOURCES main.c)
add_executable(main ${SOURCES})
target_link_libraries(main motion)

find_library(M_LIB m)
target_link_libraries(main ${M_LIB})

install(TARGETS main DESTINATION bin)

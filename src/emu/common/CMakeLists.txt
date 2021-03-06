add_library(common
    include/common/advstream.h
    include/common/algorithm.h
    include/common/armcommon.h
    include/common/armemitter.h
    include/common/buffer.h
    include/common/bytepair.h
    include/common/chunkyseri.h
    include/common/codeblock.h
    include/common/cpudetect.h
    include/common/cvt.h
    include/common/data_displayer.h
    include/common/dynamicfile.h
    include/common/fileutils.h
    include/common/flate.h
    include/common/hash.h
    include/common/ini.h
    include/common/log.h
    include/common/path.h
    include/common/platform.h
    include/common/queue.h
    include/common/random.h
    include/common/raw_bind.h
    include/common/resource.h
    include/common/time.h
    include/common/types.h
    include/common/virtualmem.h
    src/algorithm.cpp
    src/arm_cpudetect.cpp
    src/bytepair.cpp
    src/chunkyseri.cpp
    src/cvt.cpp
    src/data_displayer.cpp
    src/dynamicfile.cpp
    src/fileutils.cpp
    src/flate.cpp
    src/hash.cpp
    src/ini.cpp
    src/log.cpp
    src/path.cpp
    src/random.cpp
    src/time.cpp
    src/types.cpp
    src/virtualmem.cpp
)

target_include_directories(common PUBLIC include)
target_link_libraries(common PUBLIC fmt miniz spdlog)

if (UNIX OR APPLE)
    set_property(TARGET common PROPERTY POSITION_INDEPENDENT_CODE ON)
endif()
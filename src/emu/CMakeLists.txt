if (WIN32)
    add_definitions(-DNOMINMAX)
    message("NOMINMAX")
endif()

add_subdirectory(arm)
add_subdirectory(common)
add_subdirectory(console)
add_subdirectory(debugger)
add_subdirectory(disasm)
add_subdirectory(drivers)
add_subdirectory(epoc)
add_subdirectory(gdbstub)
add_subdirectory(hle)
add_subdirectory(manager)

if (ENABLE_SCRIPTING_ABILITY)
    add_subdirectory(scripting)
endif()
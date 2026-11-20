# Release
file(STRINGS sboxxd_version.txt SBX_VERSION)
add_compile_definitions(SBX_VERSION=\"${SBX_VERSION}\")

# Debug
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DSBX_CPP_DEBUG")

# Func
function(sbx_add_compile_definitions arg)
    message("[add_compile_definitions] ${ARGV}")
    add_compile_definitions(${ARGV})
endfunction()

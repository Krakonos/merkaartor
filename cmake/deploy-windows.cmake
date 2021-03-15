message("Running Windows deploy script.")
find_program(WINDEPLOYQT windeployqt)
find_program(CYGCHECK cygcheck)
message("Found windeployqt: ${WINDEPLOYQT}")
set(MERKAARTOR_BINARY "${CPACK_TEMPORARY_INSTALL_DIRECTORY}/bin/merkaartor.exe")
message("Working on binary: ${MERKAARTOR_BINARY}")
execute_process(COMMAND ${WINDEPLOYQT} --compiler-runtime ${MERKAARTOR_BINARY})
execute_process(COMMAND ${CYGCHECK} ${MERKAARTOR_BINARY} OUTPUT_VARIABLE MERKAARTOR_DLL_DEPS OUTPUT_STRIP_TRAILING_WHITESPACE)

string(REPLACE "\n" ";" MERKAARTOR_DLL_DEPS_LIST ${MERKAARTOR_DLL_DEPS})
message("Dependencies: ${MERKAARTOR_DLL_DEPS_LIST}")
foreach(DLL_FILE IN LISTS MERKAARTOR_DLL_DEPS_LIST)
  string(TOUPPER "${DLL_FILE}" DLL_FILE_UPPER)
  if (${DLL_FILE_UPPER} MATCHES "WINDOWS")
    message("skipping file '${DLL_FILE}'")
  else()
    set(DLL_FILE_DEST "${CPACK_TEMPORARY_INSTALL_DIRECTORY}/bin/")
    string(STRIP ${DLL_FILE} DLL_FILE)
    string(REPLACE "\\\\" "/" DLL_FILE ${DLL_FILE})
    message("copying new dependency '${DLL_FILE}' into ${DLL_FILE_DEST}")
    file( COPY ${DLL_FILE} DESTINATION ${DLL_FILE_DEST} )
  endif()
endforeach()
endif()

# Remove when done debugging
get_cmake_property(_variableNames VARIABLES)
list (SORT _variableNames)
foreach (_variableName ${_variableNames})
    message(STATUS "${_variableName}=${${_variableName}}")
endforeach()
#message("Prefix is: ${CMAKE_INSTALL_PREFIX}")
#execute_process(COMMAND ls ${CMAKE_INSTALL_PREFIX})

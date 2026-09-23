set(PROJECT_FOLDER_NAME Pathlines)
set(OUTPUT_UUID D319E2F3-0AFC-07AB-0B64-71EB19D7EE2F)

set(PROJECT_PATH ${ELEMENT_NAME})
set(OUTPUT_NAME "SE${PROJECT_FOLDER_NAME}")

message(STATUS "CREATING PROJECT OR MAKEFILE ${OUTPUT_UUID} : ${OUTPUT_NAME}")

file(GLOB_RECURSE ${OUTPUT_UUID}_headers CONFIGURE_DEPENDS include/*.hpp)
file(GLOB_RECURSE ${OUTPUT_UUID}_sources CONFIGURE_DEPENDS source/*.cpp)
file(GLOB_RECURSE ${OUTPUT_UUID}_resources CONFIGURE_DEPENDS resource/*.qrc)
file(GLOB_RECURSE ${OUTPUT_UUID}_form CONFIGURE_DEPENDS form/*.ui)
file(GLOB_RECURSE ${OUTPUT_UUID}_ts CONFIGURE_DEPENDS resource/translations/*.ts)

add_library(${OUTPUT_NAME} SHARED)

target_sources(${OUTPUT_NAME}
  PRIVATE
    ${${OUTPUT_UUID}_headers}
    ${${OUTPUT_UUID}_sources}
    ${${OUTPUT_UUID}_resources}
    ${${OUTPUT_UUID}_form}
)

target_compile_definitions(${OUTPUT_NAME}
  PRIVATE
    ELEMENT_NAME="${OUTPUT_NAME}"
    SB_ELEMENT_UUID="${OUTPUT_UUID}"
)

target_include_directories(${OUTPUT_NAME}
  PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  PRIVATE
    "${CMAKE_CURRENT_BINARY_DIR}"
)

# Qt6 use auto tools
set_target_properties(${OUTPUT_NAME} PROPERTIES
  AUTOMOC ON
  AUTOUIC ON
  AUTORCC ON
  AUTOUIC_SEARCH_PATHS
    "${CMAKE_CURRENT_SOURCE_DIR}/form"
)

# Qt6 translations
if(${OUTPUT_UUID}_ts)
  qt6_add_translations(${OUTPUT_NAME}
    TS_FILES ${${OUTPUT_UUID}_ts}
    QM_FILES_OUTPUT_VARIABLE ${OUTPUT_UUID}_QM
 )
endif()

source_group(include FILES ${${OUTPUT_UUID}_headers})
source_group(source FILES ${${OUTPUT_UUID}_sources})
source_group(resource FILES ${${OUTPUT_UUID}_resources})
source_group(form FILES ${${OUTPUT_UUID}_form})
source_group(translationFiles FILES ${${OUTPUT_UUID}_ts} ${${OUTPUT_UUID}_QM})

target_link_libraries (${OUTPUT_NAME}
  PRIVATE
    ${QT_LIBRARIES}
    ${OPENGL_LIBRARY}
    ${SAMSON_SDK_LIBRARIES}
)

set_target_properties(${OUTPUT_NAME} PROPERTIES OUTPUT_NAME ${OUTPUT_UUID})

if(UNIX)
  set_target_properties (${OUTPUT_NAME} PROPERTIES PREFIX "")
endif(UNIX)

set_property(TARGET ${OUTPUT_NAME} PROPERTY FOLDER "Elements")

SAMSON_INSTALL_ELEMENT_DIRECTORY(data/ ${OUTPUT_UUID}/Data)
SAMSON_INSTALL_ELEMENT_DIRECTORY(resource/ ${OUTPUT_UUID}/Resource)
SAMSON_INSTALL_ELEMENT_DIRECTORY(doc/ ${OUTPUT_UUID}/Documentation)

SAMSON_INSTALL_ELEMENT (${OUTPUT_NAME} ${OUTPUT_UUID})
# Use real production types for ownership regressions; no renderer or test subclass is needed.
enable_testing()
set(_samson_test_folder "Tests/${PROJECT_FOLDER_NAME}")
set(_samson_test_labels samson extension "${PROJECT_FOLDER_NAME}")
add_executable(SEPathlinesOwnershipTests tests/SEPathlinesOwnershipTests.cpp)
target_link_libraries(SEPathlinesOwnershipTests PRIVATE SEPathlines ${QT_LIBRARIES} ${SAMSON_SDK_LIBRARIES})
set_property(TARGET SEPathlinesOwnershipTests PROPERTY FOLDER "${_samson_test_folder}")
add_test(NAME SEPathlines.Ownership COMMAND $<TARGET_FILE:SEPathlinesOwnershipTests>)
set_property(TEST SEPathlines.Ownership APPEND PROPERTY LABELS ${_samson_test_labels})
get_filename_component(_pathlines_sdk_parent "${CMAKE_SAMSON_SDK_PATH}" DIRECTORY)
get_filename_component(_pathlines_install_root "${_pathlines_sdk_parent}" DIRECTORY)
get_filename_component(_pathlines_sdk_version "${CMAKE_SAMSON_SDK_PATH}" NAME)
if(SAMSON_TEST_RELEASE_RUNTIME_DIR)
  set(_pathlines_release_runtime "${SAMSON_TEST_RELEASE_RUNTIME_DIR}")
elseif(WIN32)
  set(_pathlines_release_runtime "${_pathlines_install_root}/SAMSON/${_pathlines_sdk_version}/Binaries")
else()
  set(_pathlines_release_runtime "${CMAKE_SAMSON_SDK_PATH}/libs")
endif()
if(SAMSON_TEST_DEBUG_RUNTIME_DIR)
  set(_pathlines_debug_runtime "${SAMSON_TEST_DEBUG_RUNTIME_DIR}")
else()
  set(_pathlines_debug_runtime "${CMAKE_SAMSON_SDK_PATH}/SAMSON-Debug/Binaries")
endif()
set(_pathlines_runtime "$<IF:$<CONFIG:Debug>,${_pathlines_debug_runtime},${_pathlines_release_runtime}>")
if(WIN32)
  set(_pathlines_test_environment
    "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>"
    "PATH=path_list_prepend:${_pathlines_runtime}")
elseif(APPLE)
  set(_pathlines_test_environment
    "DYLD_LIBRARY_PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>"
    "DYLD_LIBRARY_PATH=path_list_prepend:${_pathlines_runtime}"
    "DYLD_FRAMEWORK_PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>"
    "DYLD_FRAMEWORK_PATH=path_list_prepend:${_pathlines_runtime}")
else()
  set(_pathlines_test_environment
    "LD_LIBRARY_PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>"
    "LD_LIBRARY_PATH=path_list_prepend:${_pathlines_runtime}")
endif()
set_tests_properties(SEPathlines.Ownership PROPERTIES
  ENVIRONMENT_MODIFICATION "${_pathlines_test_environment}")

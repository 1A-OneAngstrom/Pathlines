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
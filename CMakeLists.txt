cmake_minimum_required(VERSION 3.9.0)
project(smblevelworkshop2)

#Export compile commands for editor autocomplete
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
if(APPLE)
    set(CMAKE_INSTALL_RPATH "@loader_path/../lib;@loader_path")
else(APPLE)
    set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib:$ORIGIN")
endif(APPLE)

#Add uninstall target
configure_file("cmake/uninstall.cmake" "cmake/uninstall.cmake" COPYONLY)
add_custom_target(uninstall "${CMAKE_COMMAND}" -P "cmake/uninstall.cmake")

#Add installprerequisites target
set(platformPlugins "")
find_package(Qt5Gui REQUIRED)
foreach(plugin ${Qt5Gui_PLUGINS})
    get_target_property(loc ${plugin} LOCATION)

    list(APPEND platformPlugins ${loc})
endforeach()
list(FILTER platformPlugins INCLUDE REGEX ".*platforms.*") #Only include platform plugins
string (REPLACE ";" "$<SEMICOLON>" platformPluginsStr "${platformPlugins}")

configure_file("cmake/installprerequisites.cmake" "cmake/installprerequisites.cmake" COPYONLY)
add_custom_target(installprerequisites "${CMAKE_COMMAND}" -DCMAKE_INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}" -DQT_PLATFORM_PLUGINS="${platformPluginsStr}" -P "cmake/installprerequisites.cmake")

#No need to add lang/lang_en_US.ts to the translation list, as the source already has the en_US translation
set(TRANSLATIONS
    lang/lang_en_US.ts
    #lang/lang_ja_JA.ts
    )

foreach(val ${TRANSLATIONS})
    message(STATUS "Translation: " ${val})
endforeach(val ${TRANSLATIONS})

add_subdirectory(./ws2common)
add_subdirectory(./ws2lz)
add_subdirectory(./ws2lzfrontend)
add_subdirectory(./ws2editor)

if(APPLE)
    set(CPACK_GENERATOR Bundle)
    set(CPACK_BUNDLE_NAME "ws2editor")
    set(CPACK_BUNDLE_PLIST ${CMAKE_SOURCE_DIR}/ws2editor/resources/Raw/Info.plist)
    set(CPACK_BUNDLE_ICON ${CMAKE_SOURCE_DIR}/ws2editor/resources/Raw/ws2editor.icns)
endif(APPLE)
include(CPack)


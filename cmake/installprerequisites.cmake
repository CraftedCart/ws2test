include(GetPrerequisites)

function(resolve_windows_prereqs var)
    #Darn it I have to resolve Windows paths myself
    if(WIN32)
        set(temp ${WS2EDITOR_PREREQS})
        set(newList "")

        foreach(file ${temp})
            #Iterate over PATH to try and find it
            foreach(pth $ENV{PATH})
                #if("${pth}" MATCHES ".*System32.*")
                    ##Drop System32 paths
                    #continue()
                #endif()

                if(EXISTS "${pth}/${file}")
                    list(APPEND newList "${pth}/${file}")
                endif()
            endforeach(pth)
        endforeach(file)

        unset(temp)
        set(${var} ${newList} PARENT_SCOPE)
    endif()
endfunction(resolve_windows_prereqs)

if(WIN32)
    #Windows has no concept of rpath, so just group all the exes/dlls in one big mess of a bin directory
    set(LIBPATH "${CMAKE_INSTALL_PREFIX}/bin")
else(WIN32)
    set(LIBPATH "${CMAKE_INSTALL_PREFIX}/lib")
endif(WIN32)

message(STATUS "Finding installed files")
file(GLOB INSTALL_FILES "${CMAKE_INSTALL_PREFIX}/bin/*" ${LIBPATH})

foreach(installedFile ${INSTALL_FILES})
    if(IS_DIRECTORY ${installedFile})
        continue()
    endif(IS_DIRECTORY ${installedFile})

    get_prerequisites(${installedFile} WS2EDITOR_PREREQS 0 1 "" "")
    resolve_windows_prereqs(WS2EDITOR_PREREQS)

    #Resolve symlinks
    set(resolvedFiles "")
    set(origFiles "")
    foreach(file ${WS2EDITOR_PREREQS})
        #Drop paths w/ "@rpath" (A macOS thing)
        #I can't be bothered to resolve them and most of the time it shouldn't matter
        if(${file} MATCHES "^@rpath.*")
            message(STATUS "Dropping file " ${file})
            continue()
        endif()

        get_filename_component(resolvedFile "${file}" REALPATH)
        list(APPEND resolvedFiles "${resolvedFile}")
        list(APPEND origFiles "${file}")
    endforeach(file)

    message(STATUS "Installing prerequisites for " ${installedFile})

    #Copy non-symlinked files
    file(COPY ${resolvedFiles} DESTINATION ${LIBPATH})
    #Also copy the symlinks (The original files)
    file(COPY ${origFiles} DESTINATION ${LIBPATH})
endforeach(installedFile)

#Also fetch the Qt platform plugins
foreach(plugin ${QT_PLATFORM_PLUGINS})
    if(IS_DIRECTORY ${plugin})
        continue()
    endif(IS_DIRECTORY ${plugin})

    message(STATUS "Installing Qt platform plugin " ${plugin})
    file(COPY ${plugin} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/platforms)

    get_prerequisites(${plugin} PLUGIN_PREREQS 0 1 "" "")
    resolve_windows_prereqs(PLUGIN_PREREQS)

    #Resolve symlinks
    set(resolvedFiles "")
    foreach(file ${PLUGIN_PREREQS})
        get_filename_component(resolvedFile "${file}" REALPATH)
        list(APPEND resolvedFiles "${resolvedFile}")
    endforeach(file)

    message(STATUS "Installing prerequisites for " ${plugin})

    #Copy non-symlinked files
    file(COPY ${resolvedFiles} DESTINATION ${LIBPATH})
    #Also copy the symlinks (The original files)
    file(COPY ${PLUGIN_PREREQS} DESTINATION ${LIBPATH})
endforeach()


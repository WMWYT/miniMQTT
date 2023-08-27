aux_source_directory(./file/ LIBFILEEXTEND)
aux_source_directory(./json/ LIBJSONEXTEND)
aux_source_directory(./test/ LIBTESTEXTEND)
aux_source_directory(./system/ LIBSYSTEMEXTEND)

add_library(fileextend SHARED ${LIBFILEEXTEND})
add_library(jsonextend SHARED ${LIBJSONEXTEND})
add_library(testextend SHARED ${LIBTESTEXTEND})
add_library(systemextend SHARED ${LIBSYSTEMEXTEND})

target_link_libraries(fileextend control)
target_link_libraries(jsonextend control)
target_link_libraries(testextend control)
target_link_libraries(systemextend control)
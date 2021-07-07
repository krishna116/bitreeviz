add_executable(${BITREEVIZ_APP_NAME} 
    main.cpp 
)

target_include_directories(${BITREEVIZ_APP_NAME}  
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

target_link_directories(${BITREEVIZ_APP_NAME}  
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(${BITREEVIZ_APP_NAME}
    PRIVATE 
        bitreevizLib
)

add_custom_command(TARGET ${BITREEVIZ_APP_NAME}
	POST_BUILD
	    COMMAND ${CMAKE_COMMAND} -E echo "$<TARGET_FILE:${BITREEVIZ_APP_NAME}>"
)


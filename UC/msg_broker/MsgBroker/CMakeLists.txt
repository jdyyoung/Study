###### Library ######
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s -Werror")
SET(LIB_SRC_LIST msg_broker.c)

IF(DEBUG_ENABLE)
	ADD_DEFINITIONS(-DDEBUG)
ELSE(DEBUG_ENABLE)
	ADD_DEFINITIONS(-DNDEBUG)
ENDIF(DEBUG_ENABLE)

SET(LIB_TARGET_NAME msgbroker)

ADD_LIBRARY(${LIB_TARGET_NAME} SHARED ${LIB_SRC_LIST})
SET_TARGET_PROPERTIES(${LIB_TARGET_NAME} PROPERTIES SOVERSION 1 VERSION 1.0.0.0)
INSTALL(TARGETS ${LIB_TARGET_NAME}
	LIBRARY DESTINATION ${INSTALL_DIR}/lib)

INSTALL(FILES msg_broker.h
		DESTINATION ${INSTALL_DIR}/include)
	

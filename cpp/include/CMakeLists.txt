project(Lib)
include_directories(headers)
include_directories(JSON/JSONHeaders)

set(SOURCES
    sources/Field.cpp
    JSON/JSONSources/JSONBase.cpp
    JSON/JSONSources/JSONChildren.cpp
    JSON/JSONSources/JSONParser.cpp
)
set(HEADERS
    headers/Constants.h
    headers/Field.h
    headers/JSON.h
    JSON/JSONHeaders/JSONBase.h
    JSON/JSONHeaders/JSONChildren.h
    JSON/JSONHeaders/JSONParser.h
)

set(INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/headers
    ${CMAKE_CURRENT_SOURCE_DIR}/JSON/JSONHeaders
)

add_library(lib ${SOURCES} ${HEADERS})
target_include_directories(lib PUBLIC ${INCLUDE_DIRS})

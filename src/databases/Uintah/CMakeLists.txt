# DO NOT EDIT THIS FILE! THIS FILE IS AUTOMATICALLY GENERATED BY xml2cmake
PROJECT(Uintah_database)

ADD_DATABASE_CODE_GEN_TARGETS(Uintah)

SET(COMMON_SOURCES
UintahPluginInfo.C
UintahCommonPluginInfo.C
)

SET(LIBI_SOURCES
UintahPluginInfo.C
)

SET(LIBM_SOURCES
UintahMDServerPluginInfo.C
${COMMON_SOURCES}
avtUintahFileFormat.C
avtUintahOptions.C
)

SET(LIBE_SOURCES
UintahEnginePluginInfo.C
${COMMON_SOURCES}
avtUintahFileFormat.C
avtUintahOptions.C
)

INCLUDE_DIRECTORIES(
${CMAKE_CURRENT_SOURCE_DIR}
${UINTAH_INCLUDE_DIR}
${VISIT_DATABASE_INCLUDES}
)

ADD_DEFINITIONS(-DUINTAH_LIBRARY_DIR="${UINTAH_LIBRARY_DIR}")
ADD_DEFINITIONS(-DUINTAH_INTERFACES_LIB="${UINTAH_INTERFACES_LIB}")
ADD_DEFINITIONS(-DUINTAH_VERSION_HEX=${UINTAH_VERSION_HEX})

LINK_DIRECTORIES(${VISIT_LIBRARY_DIR} ${UINTAH_LIBRARY_DIR} )

ADD_LIBRARY(IUintahDatabase ${LIBI_SOURCES})
TARGET_LINK_LIBRARIES(IUintahDatabase visitcommon)
SET(INSTALLTARGETS IUintahDatabase)

IF(NOT VISIT_ENGINE_ONLY AND NOT VISIT_DBIO_ONLY)
    ADD_LIBRARY(MUintahDatabase ${LIBM_SOURCES})
    TARGET_LINK_LIBRARIES(MUintahDatabase visitcommon avtdbatts avtdatabase_ser ${UINTAH_LIB} )
    ADD_TARGET_DEFINITIONS(MUintahDatabase MDSERVER)
    SET(INSTALLTARGETS ${INSTALLTARGETS} MUintahDatabase)
ENDIF(NOT VISIT_ENGINE_ONLY AND NOT VISIT_DBIO_ONLY)

ADD_LIBRARY(EUintahDatabase_ser ${LIBE_SOURCES})
TARGET_LINK_LIBRARIES(EUintahDatabase_ser visitcommon avtpipeline_ser avtdatabase_ser ${UINTAH_LIB} )
ADD_TARGET_DEFINITIONS(EUintahDatabase_ser ENGINE)
SET(INSTALLTARGETS ${INSTALLTARGETS} EUintahDatabase_ser)

IF(VISIT_PARALLEL)
    ADD_PARALLEL_LIBRARY(EUintahDatabase_par ${LIBE_SOURCES})
    TARGET_LINK_LIBRARIES(EUintahDatabase_par visitcommon avtpipeline_par avtdatabase_par ${UINTAH_LIB} )
    ADD_TARGET_DEFINITIONS(EUintahDatabase_par ENGINE)
    SET(INSTALLTARGETS ${INSTALLTARGETS} EUintahDatabase_par)
ENDIF(VISIT_PARALLEL)

VISIT_INSTALL_DATABASE_PLUGINS(${INSTALLTARGETS})
VISIT_PLUGIN_TARGET_OUTPUT_DIR(databases ${INSTALLTARGETS})
VISIT_PLUGIN_TARGET_FOLDER(databases Uintah ${INSTALLTARGETS})


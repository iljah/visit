# DO NOT EDIT THIS FILE! THIS FILE IS AUTOMATICALLY GENERATED BY xml2cmake
PROJECT(Stagger_operator)

ADD_OPERATOR_CODE_GEN_TARGETS(Stagger)

SET(COMMON_SOURCES
StaggerPluginInfo.C
StaggerCommonPluginInfo.C
StaggerAttributes.C
)

SET(LIBI_SOURCES
StaggerPluginInfo.C
)

SET(LIBG_SOURCES
StaggerGUIPluginInfo.C
QvisStaggerWindow.C
${COMMON_SOURCES}
)

SET(LIBV_SOURCES
StaggerViewerEnginePluginInfo.C
StaggerViewerPluginInfo.C
${COMMON_SOURCES}
)

SET(LIBE_SOURCES
StaggerViewerEnginePluginInfo.C
StaggerEnginePluginInfo.C
avtStaggerFilter.C
${COMMON_SOURCES}
)


INCLUDE_DIRECTORIES(
${CMAKE_CURRENT_SOURCE_DIR}
${VISIT_OPERATOR_INCLUDES}
)

LINK_DIRECTORIES(${VISIT_LIBRARY_DIR} )

ADD_LIBRARY(IStaggerOperator ${LIBI_SOURCES})
TARGET_LINK_LIBRARIES(IStaggerOperator visitcommon)
SET(INSTALLTARGETS IStaggerOperator)

IF(NOT VISIT_SERVER_COMPONENTS_ONLY AND NOT VISIT_ENGINE_ONLY AND NOT VISIT_DBIO_ONLY)
    ADD_LIBRARY(GStaggerOperator ${LIBG_SOURCES})
    set_target_properties(GStaggerOperator PROPERTIES AUTOMOC ON)
    TARGET_LINK_LIBRARIES(GStaggerOperator visitcommon gui )

    ADD_LIBRARY(VStaggerOperator ${LIBV_SOURCES})
    ADD_TARGET_DEFINITIONS(VStaggerOperator VIEWER)
    TARGET_LINK_LIBRARIES(VStaggerOperator visitcommon viewer )

    SET(INSTALLTARGETS ${INSTALLTARGETS} GStaggerOperator VStaggerOperator)

    IF(VISIT_PYTHON_SCRIPTING)
        SET(LIBS_SOURCES
            StaggerScriptingPluginInfo.C
            PyStaggerAttributes.C
            ${COMMON_SOURCES}
        )
        ADD_LIBRARY(SStaggerOperator ${LIBS_SOURCES})
        IF(WIN32)
            # This prevents python from #defining snprintf as _snprintf
            SET_TARGET_PROPERTIES(SStaggerOperator PROPERTIES COMPILE_DEFINITIONS HAVE_SNPRINTF)
        ENDIF()
        TARGET_LINK_LIBRARIES(SStaggerOperator visitcommon visitpy ${PYTHON_LIBRARY})
        SET(INSTALLTARGETS ${INSTALLTARGETS} SStaggerOperator)
    ENDIF(VISIT_PYTHON_SCRIPTING)

    IF(VISIT_JAVA)
        FILE(COPY StaggerAttributes.java DESTINATION ${JavaClient_BINARY_DIR}/src/operators)
        ADD_CUSTOM_TARGET(JavaStagger ALL ${Java_JAVAC_EXECUTABLE} ${VISIT_Java_FLAGS} -d ${JavaClient_BINARY_DIR} -classpath ${JavaClient_BINARY_DIR} -sourcepath ${JavaClient_BINARY_DIR} StaggerAttributes.java
            DEPENDS JavaClient
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    ENDIF(VISIT_JAVA)
ENDIF(NOT VISIT_SERVER_COMPONENTS_ONLY AND NOT VISIT_ENGINE_ONLY AND NOT VISIT_DBIO_ONLY)

ADD_LIBRARY(EStaggerOperator_ser ${LIBE_SOURCES})
TARGET_LINK_LIBRARIES(EStaggerOperator_ser visitcommon avtpipeline_ser avtexpressions_ser avtfilters_ser )
SET(INSTALLTARGETS ${INSTALLTARGETS} EStaggerOperator_ser)

IF(VISIT_PARALLEL)
    ADD_PARALLEL_LIBRARY(EStaggerOperator_par ${LIBE_SOURCES})
    TARGET_LINK_LIBRARIES(EStaggerOperator_par visitcommon avtpipeline_par avtexpressions_par avtfilters_par )
    SET(INSTALLTARGETS ${INSTALLTARGETS} EStaggerOperator_par)
ENDIF(VISIT_PARALLEL)

VISIT_INSTALL_OPERATOR_PLUGINS(${INSTALLTARGETS})
VISIT_PLUGIN_TARGET_OUTPUT_DIR(operators ${INSTALLTARGETS})
VISIT_PLUGIN_TARGET_FOLDER(operators Stagger ${INSTALLTARGETS})


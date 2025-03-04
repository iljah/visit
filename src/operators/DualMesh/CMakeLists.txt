# DO NOT EDIT THIS FILE! THIS FILE IS AUTOMATICALLY GENERATED BY xml2cmake
PROJECT(DualMesh_operator)

ADD_OPERATOR_CODE_GEN_TARGETS(DualMesh)

SET(COMMON_SOURCES
DualMeshPluginInfo.C
DualMeshCommonPluginInfo.C
DualMeshAttributes.C
)

SET(LIBI_SOURCES
DualMeshPluginInfo.C
)

SET(LIBG_SOURCES
DualMeshGUIPluginInfo.C
QvisDualMeshWindow.C
${COMMON_SOURCES}
)

SET(LIBV_SOURCES
DualMeshViewerEnginePluginInfo.C
DualMeshViewerPluginInfo.C
${COMMON_SOURCES}
)

SET(LIBE_SOURCES
DualMeshViewerEnginePluginInfo.C
DualMeshEnginePluginInfo.C
avtDualMeshFilter.C
${COMMON_SOURCES}
)


INCLUDE_DIRECTORIES(
${CMAKE_CURRENT_SOURCE_DIR}
${VISIT_OPERATOR_INCLUDES}
)

LINK_DIRECTORIES(${VISIT_LIBRARY_DIR} )

ADD_LIBRARY(IDualMeshOperator ${LIBI_SOURCES})
TARGET_LINK_LIBRARIES(IDualMeshOperator visitcommon)
SET(INSTALLTARGETS IDualMeshOperator)

IF(NOT VISIT_SERVER_COMPONENTS_ONLY AND NOT VISIT_ENGINE_ONLY AND NOT VISIT_DBIO_ONLY)
    ADD_LIBRARY(GDualMeshOperator ${LIBG_SOURCES})
    set_target_properties(GDualMeshOperator PROPERTIES AUTOMOC ON)
    TARGET_LINK_LIBRARIES(GDualMeshOperator visitcommon gui )

    ADD_LIBRARY(VDualMeshOperator ${LIBV_SOURCES})
    ADD_TARGET_DEFINITIONS(VDualMeshOperator VIEWER)
    TARGET_LINK_LIBRARIES(VDualMeshOperator visitcommon viewer )

    SET(INSTALLTARGETS ${INSTALLTARGETS} GDualMeshOperator VDualMeshOperator)

    IF(VISIT_PYTHON_SCRIPTING)
        SET(LIBS_SOURCES
            DualMeshScriptingPluginInfo.C
            PyDualMeshAttributes.C
            ${COMMON_SOURCES}
        )
        ADD_LIBRARY(SDualMeshOperator ${LIBS_SOURCES})
        IF(WIN32)
            # This prevents python from #defining snprintf as _snprintf
            SET_TARGET_PROPERTIES(SDualMeshOperator PROPERTIES COMPILE_DEFINITIONS HAVE_SNPRINTF)
        ENDIF()
        TARGET_LINK_LIBRARIES(SDualMeshOperator visitcommon visitpy ${PYTHON_LIBRARY})
        SET(INSTALLTARGETS ${INSTALLTARGETS} SDualMeshOperator)
    ENDIF(VISIT_PYTHON_SCRIPTING)

    IF(VISIT_JAVA)
        FILE(COPY DualMeshAttributes.java DESTINATION ${JavaClient_BINARY_DIR}/src/operators)
        ADD_CUSTOM_TARGET(JavaDualMesh ALL ${Java_JAVAC_EXECUTABLE} ${VISIT_Java_FLAGS} -d ${JavaClient_BINARY_DIR} -classpath ${JavaClient_BINARY_DIR} -sourcepath ${JavaClient_BINARY_DIR} DualMeshAttributes.java
            DEPENDS JavaClient
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    ENDIF(VISIT_JAVA)
ENDIF(NOT VISIT_SERVER_COMPONENTS_ONLY AND NOT VISIT_ENGINE_ONLY AND NOT VISIT_DBIO_ONLY)

ADD_LIBRARY(EDualMeshOperator_ser ${LIBE_SOURCES})
TARGET_LINK_LIBRARIES(EDualMeshOperator_ser visitcommon avtpipeline_ser avtexpressions_ser avtfilters_ser )
SET(INSTALLTARGETS ${INSTALLTARGETS} EDualMeshOperator_ser)

IF(VISIT_PARALLEL)
    ADD_PARALLEL_LIBRARY(EDualMeshOperator_par ${LIBE_SOURCES})
    TARGET_LINK_LIBRARIES(EDualMeshOperator_par visitcommon avtpipeline_par avtexpressions_par avtfilters_par )
    SET(INSTALLTARGETS ${INSTALLTARGETS} EDualMeshOperator_par)
ENDIF(VISIT_PARALLEL)

VISIT_INSTALL_OPERATOR_PLUGINS(${INSTALLTARGETS})
VISIT_PLUGIN_TARGET_OUTPUT_DIR(operators ${INSTALLTARGETS})
VISIT_PLUGIN_TARGET_FOLDER(operators DualMesh ${INSTALLTARGETS})


# basic source code CMakeLists.txt

art_make(
	MODULE_LIBRARIES
			ubana_PatternFilter_PMAlgs
			larcorealg_Geometry
			lardataalg_DetectorInfo
			lardataobj_RecoBase
                        ${ART_FRAMEWORK_CORE}
			${ART_FRAMEWORK_IO_ROOT}
			${ART_FRAMEWORK_MODULES}
                        ${ART_FRAMEWORK_PRINCIPAL}
                        ${ART_FRAMEWORK_SERVICES_REGISTRY}
                        ${ART_FRAMEWORK_SERVICES_OPTIONAL}
                        ${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
                        art_Persistency_Common
                        art_Persistency_Provenance
                        art_Utilities canvas
			${MF_MESSAGELOGGER}
                        ${BOOST_LIB}
                        ${FHICLCPP}
                        cetlib cetlib_except
			${ROOT_BASIC_LIB_LIST}
)							   

install_headers()
install_source()
install_fhicl()

# Libraries I didn't need
#			ub_RawDigitAdderAna
#
#			${ART_FRAMEWORK_IO_SOURCES}
#                        art_Persistency_Provenance canvas
#                        ${MF_MESSAGELOGGER}
#                        
#                        ${ROOT_BASIC_LIB_LIST}

cet_enable_asserts()
include_directories ( $ENV{GENIE_INC}/GENIE )
include_directories ( $ENV{DK2NUDATA_INC} )
link_directories( $ENV{DK2NUDATA_LIB})
file(GLOB INTERFACEFILES *Interface* )
simple_plugin( TreeReader
               "source"
	       TreeReader
               ${ART_FRAMEWORK_PERSISTENCY_COMMON}
               BASENAME_ONLY
             )

art_make_library(LIBRARY_NAME TreeInterface 
                 SOURCE	      ${INTERFACEFILES}
                 LIBRARIES
		 nusimdata_SimulationBase
#GENIE STUFF########################################
			xml2
			 ${LOG4CPP}
                 	 GFwEG
                 	 GFwAlg
                 	 GFwMsg
                 	 GFwGHEP
                 	 GTlFlx
                 	 dk2nuTree
                 	 #${DK2NUDATA}
                 	 ${DK2NUGENIE}
			${LOG4CPP}
			${GALGORITHM}
			${GBASE}
			${GEVGCORE}
			${GEVGDRIVERS}
                        ${GFLUXDRIVERS}
			${GINTERACTION}
			${GMESSENGER}
			${GHEP}
####################################################
		#$(CETLIB)
		      ${FHICLCPP}
		      cetlib cetlib_except
		      #${MF_MESSAGELOGGER}
		      ${CLHEP}
                      ${ROOT_BASIC_LIB_LIST}
                      ${Boost_SYSTEM_LIBRARY}
	        ) 

art_make( EXCLUDE TreeReader_source.cc ${INTERFACEFILES}
	  BASENAME_ONLY
          LIBRARY_NAME TreeReader
          LIB_LIBRARIES	nusimdata_SimulationBase
	  		TreeInterface
			larcoreobj_SummaryData
	  		${ART_FRAMEWORK_CORE}
                        ${ART_FRAMEWORK_IO_SOURCES}
                        ${ART_FRAMEWORK_PRINCIPAL}
                        ${ART_FRAMEWORK_SERVICES_REGISTRY}
                        ${ART_FRAMEWORK_SERVICES_OPTIONAL}
                        ${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
		      art_Persistency_Common canvas 
                      art_Persistency_Provenance canvas 
                      art_Utilities canvas
			${MF_MESSAGELOGGER}
                        ${MF_UTILITIES}
                        ${FHICLCPP}
			cetlib cetlib_except
                        ${Boost_SYSTEM_LIBRARY}
                        ${ROOT_BASIC_LIB_LIST}

          MODULE_LIBRARIES 
	  		TreeReader
	  		TreeInterface
                        nusimdata_SimulationBase
			larcoreobj_SummaryData
                        ${ART_FRAMEWORK_CORE}
                        ${ART_FRAMEWORK_IO_SOURCES}
                        ${ART_FRAMEWORK_PRINCIPAL}
                        ${ART_FRAMEWORK_SERVICES_REGISTRY}
                        ${ART_FRAMEWORK_SERVICES_OPTIONAL}
                        ${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
#GENIE STUFF########################################
			xml2
			GFwEG
                 	GFwAlg
                	GFwMsg
               		GFwGHEP
               		GTlFlx
			${LOG4CPP}
               		dk2nuTree
			${DK2NUGENIE}
			${LOG4CPP}
			${GALGORITHM}
			${GBASE}
			${GEVGCORE}
			${GEVGDRIVERS}
                        ${GFLUXDRIVERS}
			${GINTERACTION}
			${GMESSENGER}
			${GHEP}
####################################################
                        ${MF_MESSAGELOGGER}
                        ${MF_UTILITIES}
                        ${FHICLCPP}
			cetlib cetlib_except
                        ${Boost_SYSTEM_LIBRARY}
                        ${ROOT_BASIC_LIB_LIST}
        )                                                           

install_headers()
install_source()
install_fhicl()

add_subdirectory(job)

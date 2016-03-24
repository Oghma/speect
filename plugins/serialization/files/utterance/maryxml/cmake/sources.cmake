######################################################################################
##                                                                                  ##
## AUTHOR  : Matteo Lisotto                                                         ##
## DATE    : 21 September 2015                                                      ##
##                                                                                  ##
######################################################################################
##                                                                                  ##
## Source files for SMaryXMLUttFile plug-in                                         ##
##                                                                                  ##
##                                                                                  ##
######################################################################################


######## source files ##################

speect_plugin_sources(
  src/plugin.c
  src/utt_maryxml.c
  src/write.c
  src/read.c
  )
 

######## header files ##################

speect_plugin_headers(
  src/utt_maryxml.h
  )


######################################################################################
##                                                                                  ##
## AUTHOR  : Aby Louw                                                               ##
## DATE    : 11 June 2010                                                           ##
##                                                                                  ##
######################################################################################
##                                                                                  ##
## CMakeList for nlp utterance processors plug-ins                                  ##
##                                                                                  ##
##                                                                                  ##
######################################################################################

#------------------------------------------------------------------------------------#
#                        Plug-in directories                                         #
#------------------------------------------------------------------------------------#

add_subdirectory(tokenizer)
add_subdirectory(normalizer)
add_subdirectory(phrasify)
add_subdirectory(rule_based_features)
add_subdirectory(rule_based_endtone)
add_subdirectory(lexlookup)
add_subdirectory(pauses)
add_subdirectory(syltone)
add_subdirectory(morphdecomp)
add_subdirectory(uttbreak)

# Also see speect/plugins/cmake/pluginOptions.cmake
if(WANT_HUNPOS_POSTAGGER)
  add_subdirectory(hunpos)
endif(WANT_HUNPOS_POSTAGGER)

if(WANT_CRFSUITE_POSTAGGER)
  add_subdirectory(crfsuite_postagger)
endif(WANT_CRFSUITE_POSTAGGER)

if(WANT_TEXTEXP_NORMALIZER)
  add_subdirectory(textexp_normalizer)
endif(WANT_TEXTEXP_NORMALIZER)

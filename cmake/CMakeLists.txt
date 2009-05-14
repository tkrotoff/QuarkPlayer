# Warning!!! order matters

# Needed for the Phonon DS9 backend
include(MacroPushRequiredVars)

include(OWDefaultConfig)

include(OWGetGCCVersion)
include(OWDetectSystem)

include(OWDefaultCompilerFlags)

include(OWGetSvnRevision)
#include(SvnExport)
include(OWGetCurrentDateTime)
include(LocCounter)

include(OWGlobalVariables)

include(OWEnsureOutOfSourceBuild)
include(ReduceQtIncludeDirs)

include(AddQtTranslations)
include(Unix2Dos)

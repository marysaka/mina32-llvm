add_lldb_library(lldbPluginABIMINA32 PLUGIN
  ABISysV_MINA32.cpp

  LINK_LIBS
    lldbCore
    lldbSymbol
    lldbTarget
  LINK_COMPONENTS
    Support
  )

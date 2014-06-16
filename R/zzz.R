.onLoad <- function (lib, pkg) {
  library.dynam("Rrdrand", pkg, lib)

  if(hasRDRAND()==TRUE){
    RNGkind("user-supplied")
  }

  ## skip warning loading test time
  if(Sys.getenv("R_PACKAGE_NAME")=="Rrdrand")return()

  if(hasRDRAND()==FALSE){
     warning("This CPU does not support RDRAND:  RNGkind has not been changed.",call.=FALSE)
  }
}

.onUnload <- function (libpath) {
  RNGkind("default")
  library.dynam.unload("Rrdrand", libpath)
}

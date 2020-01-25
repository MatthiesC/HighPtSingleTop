LIBRARY := SUHH2HighPtSingleTop
LHAPDFLIB=$(shell scram tool tag LHAPDF LIBDIR)
LWTNNLIB=/nfs/dust/cms/user/matthies/102X/CMSSW_10_2_10/src/lwtnn/build/lib/
LWTNNINC=/nfs/dust/cms/user/matthies/102X/CMSSW_10_2_10/src/lwtnn/include/
USERCXXFLAGS := -I$(LWTNNINC)
USERLDFLAGS := -lSUHH2core -lSUHH2common -lSUHH2HOTVR -lGenVector -lSUHH2JetMETObjects -L${LHAPDFLIB} -lLHAPDF -lTMVA -L$(LWTNNLIB)
# enable par creation; this is necessary for all packages containing AnalysisModules
# to be loaded from by AnalysisModuleRunner.
PAR := 1
include ../Makefile.common

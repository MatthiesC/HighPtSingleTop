LIBRARY := SUHH2HighPtSingleTop
LHAPDFLIB=$(shell scram tool tag LHAPDF LIBDIR)
#LWTNNINC=/nfs/dust/cms/user/matthies/102X/CMSSW_10_2_10/src/lwtnn/include/
#LWTNNLIB=/nfs/dust/cms/user/matthies/102X/CMSSW_10_2_10/src/lwtnn/build/lib/
LWTNNINC=$(shell scram tool tag lwtnn INCLUDE)
LWTNNLIB=$(shell scram tool tag lwtnn LIBDIR)
LWTNNLIBNAME=$(shell scram tool tag lwtnn LIB)
USERCXXFLAGS := -I$(LWTNNINC)
USERLDFLAGS := -lSUHH2core -lSUHH2common -lSUHH2HOTVR -lSUHH2LegacyTopTagging -lGenVector -lSUHH2JetMETObjects -L${LHAPDFLIB} -lLHAPDF -L$(LWTNNLIB) -l$(LWTNNLIBNAME)
# enable par creation; this is necessary for all packages containing AnalysisModules
# to be loaded from by AnalysisModuleRunner.
PAR := 1
include ../Makefile.common

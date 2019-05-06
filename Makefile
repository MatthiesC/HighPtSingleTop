LIBRARY := SUHH2HighPtSingleTop
LHAPDFLIB=$(shell scram tool tag LHAPDF LIBDIR)
USERLDFLAGS := -lSUHH2core -lSUHH2common -lSUHH2HOTVR -lGenVector -lSUHH2JetMETObjects -L${LHAPDFLIB} -lLHAPDF -lTMVA
# enable par creation; this is necessary for all packages containing AnalysisModules
# to be loaded from by AnalysisModuleRunner.
PAR := 1
include ../Makefile.common

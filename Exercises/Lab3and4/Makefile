CC=g++ #Name of compiler
FLAGS=-std=c++20 -w #Compiler flags (the s makes it silent)
TARGET=CustomDistributions #Executable name
OBJECTS=CustomDistributions.o FiniteFunctions.o
LIBS=-I ../../GNUplot/ -lboost_iostreams

#First target in Makefile is default
${TARGET}:${OBJECTS} #Make target from objects
	@echo "Linking..."
	${CC} ${FLAGS} ${OBJECTS} ${LIBS} -o ${TARGET}
	@make clean

# TestFiniteFunctions.o : TestFiniteFunctions.cxx FiniteFunctions.h
# 	${CC} ${FLAGS} ${LIBS} -c TestFiniteFunctions.cxx

CustomDistributions.o : CustomDistributions.cxx CustomDistributions.h FiniteFunctions.h
	${CC} ${FLAGS} ${LIBS} -c CustomDistributions.cxx

FiniteFunctions.o : FiniteFunctions.cxx FiniteFunctions.h
	${CC} ${FLAGS} ${LIBS} -c FiniteFunctions.cxx

# CustomFunctions.o : CustomFunctions.cxx
# 	${CC} ${FLAGS} ${LIBS} -c CustomFunctions.cxx
	
CalculatePi.o : CalculatePi.cxx
	${CC} ${FLAGS} ${LIBS} -c CalculatePi.cxx

CalculatePi : CalculatePi.o
	@echo "Linking for CalculatePi..."
	${CC} ${FLAGS} CalculatePi.o -o CalculatePi
	@make clean
	
clean: #No targets just run shell command to rm object files and emacs backups
	@rm -f *.o *~

cleantarget: #Delete the exectuable
	@rm -f ${TARGET}

.PHONY: all

NAME=chvi
CMP=g++ -std=c++1z
WARN=-Wall -Wno-unused-result -Wno-deprecated-declarations -Wno-sign-compare -Wno-maybe-uninitialized -Wno-ignored-attributes -Wno-strict-aliasing -Wno-catch-value
OPTIM=-Ofast -march=native -funroll-loops -funsafe-loop-optimizations -falign-functions=16 -falign-loops=16 -fopenmp
NOOPTIM=-O0 -march=native -fopenmp
DBG=-g ${NOOPTIM}
PROF=-g -DWITHGPERFTOOLS ${OPTIM}

ARCH=x86-64_linux
INC=
LDIR=
LINK=-lpthread -lqhull

COBJSUBDIR=cobj
DEPSUBDIR=dep

ECHOCC=>&2 echo "[ CC ]"
ECHOLD=>&2 echo "[ LD ]"

ifeq ($(PROFILE),1)
OPT=${PROF}
LINK+=-lprofiler
else
OPT=${OPTIM} # Put desired optimisation level here
endif

define compilec
${ECHOCC} $(notdir $<) ;\
mkdir -p ${DEPSUBDIR} ;\
tmp=`mktemp` ;\
${CMP} ${DEFS} ${INC} -MM ${OPT} $< >> $$tmp ;\
if [ $$? -eq 0 ] ;\
then echo -n "${COBJSUBDIR}/" > ${DEPSUBDIR}/$(notdir $<).d ;\
cat $$tmp >> ${DEPSUBDIR}/$(notdir $<).d ;\
rm $$tmp ;\
mkdir -p ${COBJSUBDIR} ;\
cd ${COBJSUBDIR} ;\
${CMP} ${DEFS} -c ${INC} ${OPT} ${WARN} ../$< ;\
else \
ret=$$? ;\
rm $$tmp ;\
exit $$ret ;\
fi
endef

all: ${NAME}
	@true

-include ${DEPSUBDIR}/*.d

${NAME}: ${COBJSUBDIR}/main.o ${COBJSUBDIR}/ch.o
	@${ECHOLD} ${NAME}
	@${CMP} ${OPT} ${LDIR} $^ ${LINK} -o ${NAME}

${COBJSUBDIR}/main.o: main.cpp
	@$(compilec)

${COBJSUBDIR}/ch.o: ch.cpp
	@$(compilec)

clean:
	@echo "Removing subdirectories..."
	@rm -rf ${NAME} ${COBJSUBDIR} ${DEPSUBDIR} 

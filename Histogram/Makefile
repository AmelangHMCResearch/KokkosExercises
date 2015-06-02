# machine-specific paths
NVCC_WRAPPER  ?= /research/jeff/bin/nvcc_wrapper
CUDA_PATH     ?= /opt/cuda
TRILINOS_PATH ?= /research/jeff/trilinos-install-shadowfax-spring15
TBB_PATH      ?= /cs/cs181j/common/tbb-4.3

# include cuda
CC_INCLUDE      += -I$(CUDA_PATH)/include
LD_FLAGS        += -L$(CUDA_PATH)/lib64 -lcudart -lcufft -lcublas

# include kokkos
CC_INCLUDE        += -I$(TRILINOS_PATH)/include
LD_FLAGS          += -L$(TRILINOS_PATH)/lib -lkokkoscore -lhwloc -lgomp 

# include tbb
CC_INCLUDE        += -I$(TBB_PATH)/include
LD_FLAGS          += -L$(TBB_PATH)/lib -ltbb

# universal flags
CC_FLAGS   += -m64 -O3 -std=c++11 -fopenmp -Wall -Wextra

TARGETS = Histogram

all: $(TARGETS)

Histogram: Histogram.cc $(wildcard *.h)
	 $(NVCC_WRAPPER) $< -o $@ $(CC_FLAGS) $(CC_INCLUDE) $(LD_FLAGS)

clean:
	rm -f *.o $(TARGETS)

again: clean $(TARGETS)

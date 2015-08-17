# --------------------------------------------- #
# Makefile for Jet Images Framework             #
# Luke de Oliveira, July 1, 2015                # 
# lukedeo@stanford.edu                          # 
# --------------------------------------------- #

# --- set directories
TARGDIR      := event-gen

EXTERNALS  := simulations


all: $(EXTERNALS)

simulations:
	@echo "Building $@"
	@$(MAKE) -C $(TARGDIR)

# clean
.PHONY : clean rmdep
CLEANLIST     = *~ *.o *.o~ *.d core 

clean:
	rm -fr $(CLEANLIST) $(CLEANLIST:%=$(BIN)/%) $(CLEANLIST:%=$(DEP)/%)
	rm -fr $(BIN) 
	@$(MAKE) $@ -C $(TARGDIR)

purge:
	rm -fr $(CLEANLIST) $(CLEANLIST:%=$(BIN)/%) $(CLEANLIST:%=$(DEP)/%)
	rm -fr $(BIN) 
	rm -fr $(EXECUTABLE)
	@$(MAKE) $@ -C $(TARGDIR)

rmdep: 
	rm -f $(DEP)/*.d
	@$(MAKE) $@ -C $(TARGDIR)
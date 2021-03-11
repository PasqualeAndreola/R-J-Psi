#Compiler
CXX 	 := g++

#Executable program
TARGET 	 :=	Analysis

#Source, build and executable directories
SRCDIR    := SourceFiles
BUILDDIR  := BuildFiles
TARGETDIR := BinFiles
INPUTDIR  := InputFiles
OUTPUTDIR := OutputFiles

#Source and build file extension
SRCEXT := cpp
OBJEXT := o
DEPEXT := d

#Flags
CXXFLAGS 	 := -O3 -g -Wall $(shell root-config --cflags)

#Includes
INCROOT 	 := -I$(shell root-config --incdir)
INCLUDES     := $(INCROOT)

#Libraries that has to be linked
LIBSROOT 	 := $(shell root-config --libs)
LIBS 	 	 := $(LIBSROOT)

#Sources and objects research
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.$(OBJEXT)))	

#Default make execution
all: directories doxy $(TARGET)

#Remake
remake: cleaner all

#Making of directories
directories:
	@mkdir -p $(TARGETDIR)
	@mkdir -p $(BUILDDIR)
	@mkdir -p $(INPUTDIR)
	@mkdir -p $(OUTPUTDIR)

#Clean Build
clean:
	@$(RM) -rf $(BUILDDIR)

#Clean everything
cleaner: clean
	@$(RM) -rf $(TARGETDIR)

#Documenting
doxy:
	doxygen Documentation/doxy_config

#Testing
test:
	@echo 
	@echo To make tests, open the TestFiles directory and run the command "make"\n
	@echo
	
#Getting the dipendencies
-include $(OBJECTS:.$(OBJEXT)=.$(DEPEXT))

#Setting up the linker
$(TARGET): $(OBJECTS)
	$(CXX) -o $(TARGETDIR)/$(TARGET) $^ $(LIBS)

#Compiling
$(BUILDDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -MM $(SRCDIR)/$*.$(SRCEXT) > $(BUILDDIR)/$*.$(DEPEXT)
	@cp -f $(BUILDDIR)/$*.$(DEPEXT) $(BUILDDIR)/$*.$(DEPEXT).tmp
	@sed -e 's|.*:|$(BUILDDIR)/$*.$(OBJEXT):|' < $(BUILDDIR)/$*.$(DEPEXT).tmp > $(BUILDDIR)/$*.$(DEPEXT)
	@sed -e 's/.*://' -e 's/\\$$//' < $(BUILDDIR)/$*.$(DEPEXT).tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(BUILDDIR)/$*.$(DEPEXT)
	@rm -f $(BUILDDIR)/$*.$(DEPEXT).tmp

.PHONY: all remake clean cleaner doxy
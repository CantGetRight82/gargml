#Compiler and Linker
CC          := clang++

#The Target Binary Program
TARGET      := program

#The Directories, Source, Includes, Objects, Binary and Resources
SRCDIR      := src
INCDIR      := inc
BUILDDIR    := obj
TARGETDIR   := bin
RESDIR      := res
SRCEXT      := cpp
DEPEXT      := d
OBJEXT      := o

#Flags, Libraries and Includes
CFLAGS      := -Wall -O3 -g
CFLAGS      := -g -std=c++11
LIB         := -lgd
INC         := -I$(INCDIR) -I/usr/local/include
INCDEP      := -I$(INCDIR)

#---------------------------------------------------------------------------------
#DO NOT EDIT BELOW THIS LINE
#---------------------------------------------------------------------------------
SOURCES     := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS     := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.$(OBJEXT)))

testsol:
	clang++ test/testsol.cpp -std=c++11 -o bin/testsol
	./bin/testsol

testep:
	clang++ test/testep.cpp -std=c++11 -o bin/testep
	./bin/testep
	
all: resources $(TARGET)
	make examples

examples:
	./bin/program examples/box1.gar output/box1.png
	./bin/program examples/box2.gar output/box2.png
	./bin/program examples/box3.gar output/box3.png
	# ./bin/program examples/box4.gar output/box4.png


#Remake
remake: cleaner all

#Copy Resources from Resources Directory to Target Directory
resources: directories

#Make the Directories
directories:
	@mkdir -p $(TARGETDIR)
	@mkdir -p $(BUILDDIR)

#Clean only Objecst
clean:
	@$(RM) -rf $(BUILDDIR)

#Full Clean, Objects and Binaries
cleaner: clean
	@$(RM) -rf $(TARGETDIR)

#Pull in dependency info for *existing* .o files
-include $(OBJECTS:.$(OBJEXT)=.$(DEPEXT))

#Link
$(TARGET): $(OBJECTS)
	$(CC) -o $(TARGETDIR)/$(TARGET) $^ $(LIB)

#Compile
$(BUILDDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<
	@$(CC) $(CFLAGS) $(INCDEP) -MM $(SRCDIR)/$*.$(SRCEXT) > $(BUILDDIR)/$*.$(DEPEXT)
	@cp -f $(BUILDDIR)/$*.$(DEPEXT) $(BUILDDIR)/$*.$(DEPEXT).tmp
	@sed -e 's|.*:|$(BUILDDIR)/$*.$(OBJEXT):|' < $(BUILDDIR)/$*.$(DEPEXT).tmp > $(BUILDDIR)/$*.$(DEPEXT)
	@sed -e 's/.*://' -e 's/\\$$//' < $(BUILDDIR)/$*.$(DEPEXT).tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(BUILDDIR)/$*.$(DEPEXT)
	@rm -f $(BUILDDIR)/$*.$(DEPEXT).tmp

#Non-File Targets
.PHONY: all examples remake clean cleaner resources

#reference:
#	clang++ src/gd/main.cpp -lgd -lpng -lz -ljpeg -lfreetype -lm -o ./test/gdgarg
#	cd test && ./gdgarg ../examples/reference.gar

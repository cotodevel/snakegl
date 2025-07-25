#
#			Copyright (C) 2017  Coto
#This program is free software; you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation; either version 2 of the License, or
#(at your option) any later version.

#This program is distributed in the hope that it will be useful, but
#WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
#General Public License for more details.

#You should have received a copy of the GNU General Public License
#along with this program; if not, write to the Free Software
#Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301
#USA
#


#put here the custom stage_2 vram loader bits
#for 0x02380000 -> 0x06000000 arm7vram payload
export BINSTRIP_RULE_arm7vram =	arm7.bin
#for 0x02380000 -> 0x06000000 arm7vram payload
export TARGET_LIBRARY_CRT0_FILE_7custom = nds_arm_ld_crt0
export TARGET_LIBRARY_LINKER_FILE_7custom = ../$(TARGET_LIBRARY_CRT0_FILE_7custom).S

# relative path (if you are outside arm9 folder then $(DIR_ARM9), arm7 is $(DIR_ARM7))
RELATIVE_PATH =	

#Prepare ARM7 dirs
DIRLIBS_HDR = $(foreach dirres,$(DIRS_ARM7_HEADER),-I "$(RELATIVE_PATH)$(dirres)" )	$(DIRLIBS_ALLOWED)	$(TARGET_LIBRARY_HEADERS_7_INC)
DIRLIBS_SRC = $(foreach dirres,$(DIRS_ARM7_SRC),-I "$(CURDIR)/$(RELATIVE_PATH)$(dirres)" )

file_arm7_src_c=  $(foreach dir,$(DIRS_ARM7_SRC), $(subst $(CURDIR)/$(RELATIVE_PATH)/$(dir)/,,$(wildcard $(CURDIR)/$(RELATIVE_PATH)/$(dir)/*.c))  )
file_arm7_src_o=  $(foreach dir,$(BUILD), $(subst $(CURDIR)/$(RELATIVE_PATH)/$(dir)/,,$(wildcard $(CURDIR)/$(RELATIVE_PATH)/$(dir)/*.o))  )
file_arm7_src_o_out = $(filter-out $(TARGET_LIBRARY_CRT0_FILE_7custom).o, $(file_arm7_src_o))
file_arm7_src_s=  $(foreach dir,$(DIRS_ARM7_SRC), $(subst $(CURDIR)/$(RELATIVE_PATH)/$(dir)/,,$(wildcard $(CURDIR)/$(RELATIVE_PATH)/$(dir)/*.s))  )
file_arm7_src_SUPP=  $(foreach dir,$(DIRS_ARM7_SRC), $(subst $(CURDIR)/$(RELATIVE_PATH)/$(dir)/,,$(wildcard $(CURDIR)/$(RELATIVE_PATH)/$(dir)/*.S))  )
file_arm7_src_cpp=  $(foreach dir,$(DIRS_ARM7_SRC), $(subst $(CURDIR)/$(RELATIVE_PATH)/$(dir)/,,$(wildcard $(CURDIR)/$(RELATIVE_PATH)/$(dir)/*.cpp))  )
file_arm7_src_bin=  $(foreach dir,$(DIRS_ARM7_SRC), $(subst $(CURDIR)/$(RELATIVE_PATH)/$(dir)/,,$(wildcard $(CURDIR)/$(RELATIVE_PATH)/$(dir)/*.bin))  )
file_arm7_src_tga=  $(foreach dir,$(DIRS_ARM7_SRC), $(subst $(CURDIR)/$(RELATIVE_PATH)/$(dir)/,,$(wildcard $(CURDIR)/$(RELATIVE_PATH)/$(dir)/*.tga))  )

# Object Target
objs_arm7:=		${file_arm7_src_tga:.tga=.o}	${file_arm7_src_bin:.bin=.o}	${file_arm7_src_c:.c=.o}	${file_arm7_src_s:.s=.o}	${file_arm7_src_SUPP:.S=.o}		${file_arm7_src_cpp:.cpp=.o}
d_objs_arm7:=	${file_arm7_src_c:.c=.d}	${file_arm7_src_s:.s=.d}	${file_arm7_src_SUPP:.S=.d}	${file_arm7_src_cpp:.cpp=.d}

#ARM7 specific definitions.	-fno-common so we force initialized data to rodata
MAPFILE = $(DIR_ARM7).map
AS_ASFLAGS		=	$(GLOBAL_SHARED_FLAGS_7)	$(SPECIFIC_AS_COMPILER_FLAGS_7)		$(GLOBAL_AS_COMPILER_FLAGS)
CC_ASFLAGS		=	-march=armv4t	-mthumb-interwork	-marm	-mfloat-abi=soft	-O1	-DARM7	-fno-exceptions	-fno-unwind-tables -fno-asynchronous-unwind-tables -fno-tree-loop-distribute-patterns -ffunction-sections	-fdata-sections	-fno-common	-Dappver='$(EXECUTABLE_VERSION)'	-DARM7VRAMCUSTOMCORE
CPP_FLAGS		=	$(CC_ASFLAGS)	-nostdinc++	
BUILD			= 	$(BUILD_ARM7)
DIR_SRC			=	$(DIRS_ARM7_SRC)	$(DIRS_ARM7_HEADER)

#--gc-sections: allows relocatable sections linked (already) to be included, otherwise unused sections are discarded.
#check arm7.map "Discarded input sections" to see --gc-sections in action.
LDFLAGS	:=	-specs=96k_arm7_ld_stage1.specs	-Wl,--no-gc-sections,-Map,$(MAPFILE),--fix-v4bx,-z,defs
LDFLAGS_DSI	:=	-specs=96k_arm7_ld_stage1.specs	-Wl,--no-gc-sections,-Map,arm7-nonstripped_dsi.map,--fix-v4bx,-z,defs
LIBRARIES_NTR =	-lutils7	-l$(TARGET_LIBRARY_TGDS_NTR_7)	-lhardware7		-lstdc++	-lcnano7	-lgcc
LIBRARIES_TWL =	-lutils7i	-l$(TARGET_LIBRARY_TGDS_TWL_7)	-lhardware7i	-lstdc++	-lcnano7	-lgcc

################################################################################################

#Dont modify anything else if not necessary ..
OBJECTS:= 	$(objs_arm7)	$(file_arm7_src_o_out)
OBJDIR 	=	./$(BUILD)
VPATH 	=	$(DIR_SRC)
OBJPROG = 	$(addprefix $(OBJDIR)/, $(NONSTRIPELF_ARM7))


all: makebuild	$(TARGET_LIBRARY_CRT0_FILE_7custom).o	$(BINSTRIP_RULE_arm7vram)
#all: debug

makebuild:
	-mkdir -p $(OBJDIR)

#convert a TGA BMP16 into a native 3D texture unit format (GL_COLOR 16bpp)
$(OBJDIR)/%.o : %.tga
	-$(GRITTOOL) ${<:.o=.c} -ftc -o $@ -gu32 -gb -gB16
	$(CC)	$(CC_ASFLAGS)	$(DIRLIBS_HDR)	$(DIRLIBS_SRC)	-o $@ -c ${@:.o=.c}
	
#rule for reading *.bin in /data folder, converts to .C source code, which then gets compiled as object so it can be linked later
$(OBJDIR)/%.o : %.bin
	-@echo 'trying to convert .BIN ($<) -> .C output: ${@:.o=.c} : arrayName: $(subst build/,,${@:.o=})'
	$(BIN2C)	$<	${@:.o=.c}	$(subst build/,,${@:.o=})
	$(CC)	$(CC_ASFLAGS)	$(DIRLIBS_HDR)	$(DIRLIBS_SRC)	$(DIRLIBS_PROHIBITED)	-o $@ -c ${@:.o=.c}
	
#Do C Compile now!
$(OBJDIR)/%.o : %.c
	-@echo 'C Compile start: $@'
	$(CC)	$(CC_ASFLAGS)	$(DIRLIBS_HDR)	$(DIRLIBS_SRC)	-o $@ -c $<
	-@echo 'C Compile End: $@'

#Do C++ (CXX) Compile now!
$(OBJDIR)/%.o : %.cpp
	-@echo 'C++ Compile start: $@'
	$(CPP)	$(CPP_FLAGS)	$(DIRLIBS_HDR)	$(DIRLIBS_SRC)	-o	$@	-c	$<
	-@echo 'C Compile End: $@'

#Coto: Special "exception" rule. Fixes tricky projects such as SnemulDS's opcodes2.s where GCC AS is used, while compatibility with .S is required for newer projects.
#.S is GCC CC/CPP for compiling assembler with cpp headers
#.s is GCC AS Assembler for compiling assembler the usual way

#Do s (GCC AS) Compile now!
$(OBJDIR)/%.o : %.s
	-@echo 's Compile start: $@'
	$(AS)	$(DIRLIBS_HDR)	$(DIRLIBS_SRC)	$(AS_ASFLAGS)	-o $@ -c $<
	-@echo 's Compile end: $@'

#Do S (GCC CC) Compile now!
$(OBJDIR)/%.o : %.S
	-@echo 'S Compile start: $@'
	$(CPP)	-x assembler-with-cpp	$(DIRLIBS_HDR)	$(DIRLIBS_SRC)	$(CC_ASFLAGS)	-o $@ -c $<
	-@echo 'S Compile end: $@'

#Special rule: compile crt0 for arm7 entrypoint
$(TARGET_LIBRARY_CRT0_FILE_7custom).o:	$(TARGET_LIBRARY_LINKER_FILE_7custom)
	-@echo 'S Compile start: crt0s'
	rm -fr $(BUILD)/$(TARGET_LIBRARY_CRT0_FILE_7custom).o
	$(CPP)	-x assembler-with-cpp	$(DIRLIBS_HDR)	$(DIRLIBS_SRC)	$(CC_ASFLAGS)	-o	$(OBJDIR)/$@	-c	$<
	-@echo 'S Compile end: crt0s'

#Linkage. 
$(OBJPROG): $(addprefix $(OBJDIR)/, $(OBJECTS))
	-@echo 'Entering Linking phase'
	-$(CC) $^	$(LDFLAGS)	$(DIRLIBS_HDR)	$(DIRLIBS_SRC)	$(DIRLIBS_SHARED)	-L	$(TARGET_LIBRARY_PATH)/	-L	$(TOOLCHAIN_PATH)/		$(LIBRARIES_NTR)	-L	$(TARGET_LIBRARY_LINKER_PATH_LD)	-o $@
	-$(CC) $^	$(LDFLAGS_DSI)	$(DIRLIBS_HDR)	$(DIRLIBS_SRC)	$(DIRLIBS_SHARED)	-L	$(TARGET_LIBRARY_PATH)/	-L	$(TOOLCHAIN_PATH)/		$(LIBRARIES_TWL)	-L	$(TARGET_LIBRARY_LINKER_PATH_LD)	-o	${@:.elf=_dsi.elf}
	-@echo 'Leaving Linking phase. Output: $@'

#xxxx-nonstripped.elf -> xxxx.elf STRIP operation
$(ELF_ARM7): $(OBJPROG)
	-@echo '$@ Unstripped. STRIP Start!'
	-mv build/${@:.elf=-nonstripped_dsi.elf}	$(CURDIR)
	$(OBJSTRIP)	-s -R .comment -R .gnu.version	$<
	$(OBJSTRIP)	-s -R .comment -R .gnu.version	${@:.elf=-nonstripped_dsi.elf}
	-mv	$<	$@
	
$(BINSTRIP_RULE_arm7vram): $(ELF_ARM7)
	-@echo 'STRIP End: Remove ELF header and build baremetal Binary executable: $<	->	$@ !'
	$(OBJCOPY) 	-S -O binary	$<	$@
	$(OBJCOPY) 	-S -O binary	${<:.elf=-nonstripped_dsi.elf}	${@:.bin=_twl.bin}
	-rm	$<
################################################################################################
	
clean:
	-@echo 'cleanup $(DIR_ARM7)'
	-@rm -fr $(BINSTRIP_RULE_arm7vram)	$(MAPFILE)	${BINSTRIP_RULE_arm7vram:.bin=_twl.bin}	$(BUILD)	$(CURDIR)/arm7-nonstripped_dsi.elf $(CURDIR)/arm7-nonstripped_dsi.map


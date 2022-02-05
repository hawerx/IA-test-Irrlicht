#########################################################################
## 								MACROS						           ##
#########################################################################

## $(1) = COMPILADOR
## $(2) = FICHERO DE SALIDA
## $(3) = DEPENDENCIA 1
## $(4) = DEPENDENCIA 2
## $(5) = FLAGS DEL COMPILADOR

define COMP_OBJ
$(2) : $(3) $(4)
	$(1) $(5) -c $(3) -o $(2)
endef

## $(1) = FICHERO PARA GENERAR OBJ

define CPP_TO_OBJ
$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst $(SRC)%,$(OBJ)%,$(1))))
endef

define CPP_TO_HPP
$(patsubst %.cpp,%.hpp,$(1))
endef


#########################################################################
## 								CONFIGURACION						   ##
#########################################################################


# COMPILADOR A UTILIZAR. (C++)
CC				:= clang++

CC_VERSION 		:= 20

# FLAGS PARA EL COMPILADOR. (C++)
CXXFLAGS 		:= -g -Wextra -Wall -Wpedantic -fsanitize=address -std=c++$(CC_VERSION)

LIBS 			:= -lIrrlicht

# DIRECTORIO "SRC".
SRC				:= src
ALLSRC			:= src/*

INC 			:= include

GAME 			:= IAtest

# DIRECTORIO "OBJ".
OBJ				:= obj
ALLOBJ			:= obj/*

# FICHEROS ".cpp"
CPP_FILES 		:= $(shell find $(SRC)/ -type f -iname *.cpp)

# FICHEROS ".c" (POR SI HACE FALTA)
C_FILES			:= $(shell find $(SRC)/ -type f -iname *.c)

# FICHEROS ".hpp"
HPP_FILES		:= $(shell find $(INC)/ -type f -iname *.hpp)

# FICHEROS ".h"
H_FILES			:= $(shell find $(INC)/ -type f -iname *.h)

# FICHEROS ".o" TANTO DE C++ COMO DE C
OBJ_FILES		:= $(foreach F,$(CPP_FILES) $(C_FILES),$(call CPP_TO_OBJ,$(F)))

# SUBDIRECTORIOS DEL PROYECTO.
SUBDIRS		:= $(shell find $(SRC)/ -type d)   				# OBTIENE TODOS LOS DIRECTORIOS DE "SRC".
OBJSUBDIRS	:= $(patsubst $(SRC)%,$(OBJ)%,$(SUBDIRS))		# UNA VEZ OBTENIDOS, CAMBIA DONDE PONE "SRC" POR "OBJ".

# VARIABLES DE COMANDOS.
    
   #### COMANDO MKDIR ####
    
    MKDIR		:= mkdir -p		   # -p: Para poder crear todas las carpetas necesarias de golpe.

   #### COMANDO RM #######
    RM			:= rm -r -d -v -f  # -r: Elimina los directorios de forma recursiva. 
                                   # -d: Elimina directorios vacios. 
                                   # -v: Muestra los que esta haciendo.
                                   # -f: Ignora los errores producidos por archivos no existentes.


.PHONY: info  # COMO "INFO" NO GENERA NADA, SE PONE COMO PHONY. (Al hacer "make info", no se hace nada.)

all: $(GAME)

# CREACION DE LA CARPETA "OBJ"
$(OBJSUBDIRS): $(SUBDIRS)
	$(RM) $(ALLOBJ)
	$(MKDIR) $(OBJSUBDIRS)

# MUESTRA LOS DIRECTORIOS DEL SISTEMA
info:
	$(info $(SUBDIRS))
	$(info $(OBJSUBDIRS))
	$(info $(CPP_FILES))
	$(info $(OBJ_FILES))
	$(info $(HPP_FILES))

####### COMPILACION DE LOS FICHEROS ##########

$(foreach F,$(CPP_FILES), $(eval $(call COMP_OBJ,$(CC),$(call CPP_TO_OBJ,$(F)),$(F),,$(CXXFLAGS))))
$(foreach F,$(C_FILES),   $(eval $(call COMP_OBJ,$(CC),$(call CPP_TO_OBJ,$(F)),$(F),,$(CXXFLAGS))))


$(GAME):  $(OBJSUBDIRS) $(OBJ_FILES) $(CPP_FILES) $(HPP_FILES)
	$(CC) $(CXXFLAGS) -o $(GAME) $(OBJ_FILES) $(LIBS)

# ELIMINA LOS DIRECTORIOS "OBJ", "DOC" Y EL JUEGO FINAL COMPILADO. (SE PUEDE USAR "make clean" O "make clear". ES LO MISMO)
cleanall:
	$(RM) $(ALLOBJ) $(GAME)

# ELIMINA LOS DIRECTORIOS "OBJ", "DOC".
clean:
	$(RM) $(ALLOBJ)
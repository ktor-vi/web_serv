# Nom de l'exécutable
EXECUTABLE_NAME = web_serv.exe

# Compilateur et options
COMPILATOR = c++
FLAGS = -Wall -Wextra -g -std=c++98 #-Werror -Ofast -funroll-loops

# Répertoires
SRC_DIR = sources
OBJ_DIR = OBJ

# Fichiers sources et objets
SRC_FILES = $(shell find $(SRC_DIR) -name "*.cpp")
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC_FILES))

# Règle par défaut
all: $(EXECUTABLE_NAME)

# Création de l'exécutable
$(EXECUTABLE_NAME): $(OBJECTS)
	@echo "Linking..."
	@$(COMPILATOR) $(OBJECTS) -o $@
	@echo "Compilation et linking réussis!"

# Compilation des fichiers objets
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "Compiling $<..."
	@mkdir -p $(dir $@)
	$(COMPILATOR) $(FLAGS) -c $< -o $@

# Nettoyage
clean:
	@echo "Cleaning objects..."
	@rm -rf $(OBJ_DIR)

fclean: clean
	@echo "Cleaning binary..."
	@rm -f $(EXECUTABLE_NAME)

# Rebuild complet
re: fclean all

.PHONY: all clean fclean re

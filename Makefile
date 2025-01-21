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

# Couleurs pour le terminal
GREEN = \033[1;32m
YELLOW = \033[1;33m
BLUE = \033[1;34m
RED = \033[1;31m
NC = \033[0m

# Règle par défaut
all: $(EXECUTABLE_NAME)

# Création de l'exécutable
$(EXECUTABLE_NAME): $(OBJECTS)
	@echo "$(BLUE)Linking...$(NC)"
	@$(COMPILATOR) $(OBJECTS) -o $@
	@echo "$(GREEN)Compilation et linking réussis!$(NC)"

# Compilation des fichiers objets
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "$(YELLOW)Compiling $<...$(NC)"
	@mkdir -p $(dir $@)
	@$(COMPILATOR) $(FLAGS) -c $< -o $@

# Nettoyage
clean:
	@echo "$(RED)Cleaning objects...$(NC)"
	@rm -rf $(OBJ_DIR)

fclean: clean
	@echo "$(RED)Cleaning binaire...$(NC)"
	@rm -f $(EXECUTABLE_NAME)

# Rebuild complet
re: fclean all

.PHONY: all clean fclean re

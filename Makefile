# Nom de l'exécutable
TARGET = web_serv

# Compilateur et options de compilation
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -O2
LDFLAGS =

# Couleurs
GREEN = \033[1;32m
YELLOW = \033[1;33m
BLUE = \033[1;34m
RED = \033[1;31m
NC = \033[0m

# Dossiers
SRC_DIR = sources
BUILD_DIR = objs

# Fichiers sources, templates et objets
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
TEMPLATES = $(wildcard $(SRC_DIR)/*.tpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SOURCES))

# Nombre total de fichiers à compiler (pour la barre de progression)
TOTAL = $(words $(SOURCES))

# Compilation
all: $(TARGET)

$(TARGET): $(OBJECTS)
	@echo "$(BLUE)Linking...$(NC)"
	@$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "$(GREEN)Done!$(NC)"

# Compilation des fichiers objets avec une barre de progression
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(TEMPLATES) | $(BUILD_DIR)
	$(eval CURRENT := $(shell echo $$(($(PROGRESS)+1))))
	@echo "$(YELLOW)[$(CURRENT)/$(TOTAL)] Compiling $<... $(NC)"
	@$(CXX) $(CXXFLAGS) -c $< -o $@
	@echo "$(GREEN)OK$(NC)"
	$(eval PROGRESS := $(CURRENT))

# Crée le dossier build si nécessaire
$(BUILD_DIR):
	@mkdir -p $@

# Nettoyage
clean:
	@echo "$(RED)Cleaning objects...$(NC)"
	@rm -rf $(BUILD_DIR)/*.o
	@echo "$(GREEN)Cleaned!$(NC)"

fclean: clean
	@echo "$(RED)Cleaning binary...$(NC)"
	@rm -rf $(TARGET)
	@echo "$(GREEN)Cleaned!$(NC)"

re: fclean all

# Variables pour le suivi de la progression
PROGRESS := 0

.PHONY: all clean fclean re

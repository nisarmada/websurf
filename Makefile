
NAME            = webserv
CXX             = c++
CXXFLAGS        = -Wall -Wextra -Werror -std=c++17 -g

SRC_DIR         = src
INC_DIR         = includes
BUILD_DIR       = build

SRCS            = \
	$(SRC_DIR)/Cgi.cpp \
	$(SRC_DIR)/Client.cpp \
	$(SRC_DIR)/Chunked.cpp \
	$(SRC_DIR)/HttpRequest.cpp \
	$(SRC_DIR)/HttpRequestParsing.cpp \
	$(SRC_DIR)/HttpResponse.cpp \
	$(SRC_DIR)/LocationBlock.cpp \
	$(SRC_DIR)/Parsing.cpp \
	$(SRC_DIR)/Server.cpp \
	$(SRC_DIR)/ServerBlock.cpp \
	$(SRC_DIR)/Utils.cpp \
	$(SRC_DIR)/UtilsParsing.cpp \
	$(SRC_DIR)/main.cpp

# --- Object Files and Dependency Files Generation ---
# Replace SRC_DIR with BUILD_DIR for object files, and change .cpp to .o
OBJS            = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

# Replace .o with .d for dependency files (used by -MMD)
DEPS            = $(patsubst $(BUILD_DIR)/%.o,$(BUILD_DIR)/%.d,$(OBJS))

# --- Main Rules ---

# Default target: builds the executable
all: $(BUILD_DIR) $(NAME)

# Link the object files to create the executable
$(NAME): $(OBJS)
	@echo "Linking $(NAME)..."
	$(CXX) $(OBJS) -o $(NAME)
	@echo "Build complete: $(NAME) created."

# Compile .cpp files into .o files
# -MMD: Generate dependency files (.d)
# -MP: Add phony targets for header files
# -I$(INC_DIR): Add includes/ to the include path
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $< -o $@ -MMD -MP

# Ensure the build directory exists before compilation
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)
	@echo "Created build directory: $(BUILD_DIR)"

# --- Cleanup Rules ---

clean:
	@echo "Cleaning object files and dependencies..."
	@rm -rf $(BUILD_DIR)
	@echo "Clean complete."

fclean: clean
	@echo "Full clean (removing executable)..."
	@rm -f $(NAME)
	@echo "Full clean complete."

re: fclean all
	@echo "Rebuilding project."

.PHONY: all clean fclean re run

# --- Include Generated Dependencies ---
-include $(DEPS)

# --- Run the program with original.config ---
run: $(NAME)
	@echo "\033[0;32mRunning $(NAME) with original.config...\033[0m"
	@./$(NAME) original.config
#                           Webserv Project Makefile                           #
# ============================================================================ #

# --- Project Configuration ---
NAME            = webserv
CXX             = c++
CXXFLAGS        = -Wall -Wextra -Werror -std=c++17

# --- Directory Paths ---
SRC_DIR         = src
INC_DIR         = includes
BUILD_DIR       = build

# --- Source Files Discovery ---
# Find all .cpp files in the SRC_DIR and its subdirectories
SRCS            = $(shell find $(SRC_DIR) -name "*.cpp")

# --- Object Files and Dependency Files Generation ---
# Replace SRC_DIR with BUILD_DIR for object files, and change .cpp to .o
OBJS            = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

# Replace .o with .d for dependency files (used by -MMD)
DEPS            = $(patsubst $(BUILD_DIR)/%.o,$(BUILD_DIR)/%.d,$(OBJS))

# --- Main Rules ---

# Default target: builds the executable
all: $(NAME)

# Link the object files to create the executable
$(NAME): $(OBJS)
	@echo "Linking $(NAME)..."
	$(CXX) $(OBJS) -o $(NAME)
	@echo "Build complete: $(NAME) created."

# Compile .cpp files into .o files
# -MMD: Generate dependency files (.d)
# -MP: Add phony targets for header files (prevents errors if headers are removed)
# -I$(INC_DIR): Add includes/ to the include path
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	@mkdir -p $(@D) # Create parent directories for the object file if they don't exist
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $< -o $@ -MMD -MP

# Ensure the build directory exists before compilation
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)
	@echo "Created build directory: $(BUILD_DIR)"

# --- Cleanup Rules ---

# Remove object files and dependency files
clean:
	@echo "Cleaning object files and dependencies..."
	@rm -rf $(BUILD_DIR)
	@echo "Clean complete."

# Remove object files, dependency files, and the executable
fclean: clean
	@echo "Full clean (removing executable)..."
	@rm -f $(NAME)
	@echo "Full clean complete."

# Rebuild the project from scratch
re: fclean all
	@echo "Rebuilding project."

# --- Phony Targets ---
# Declare phony targets to prevent issues if files with the same names exist
.PHONY: all clean fclean re

# --- Include Generated Dependencies ---
# This line ensures that Make rebuilds targets when header files change.
# The '-' suppresses errors if .d files don't exist yet (e.g., on first build).
-include $(DEPS)

# --- Run the program with test.conf ---
run: all
	@echo "\033[0;32mRunning $(NAME) with test.conf...\033[0m"
	@./$(NAME) original.config


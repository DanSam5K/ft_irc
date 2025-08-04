# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dsamuel <dsamuel@student.42wolfsburg.de    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/07/31 17:46:39 by dsamuel           #+#    #+#              #
#    Updated: 2025/08/04 16:05:03 by dsamuel          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# **************************************************************************** #
#                                  ft_ircserv                                   #
# **************************************************************************** #

# Executable name
NAME := ircserv

# Directories
SRC_DIR := srcs
INC_DIR := includes
OBJ_DIR := objects

# File extensions
SRC_EXT := cpp
HDR_EXT := hpp h

# Compiler & Flags
CXX := c++
CXXFLAGS := -Wall -Wextra -Werror -std=c++98 -g3

# Automatically find all include subdirectories
INCLUDES := $(shell find $(INC_DIR) -type d -exec echo -I{} \;)

# Find all source files recursively
SRC := $(shell find $(SRC_DIR) -name '*.$(SRC_EXT)')

# Create corresponding object files in the OBJ_DIR hierarchy
OBJ := $(patsubst $(SRC_DIR)/%,$(OBJ_DIR)/%,$(SRC:.$(SRC_EXT)=.o))

# Libraries (none currently required, placeholder)
LIBS :=

# Default target
.PHONY: all
all: $(NAME)

# Link final executable
$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS) -lssl -lcrypto

# Compile source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.$(SRC_EXT)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDES)

# Clean object files
.PHONY: clean
clean:
	@rm -rf $(OBJ_DIR)

# Full clean
.PHONY: fclean
fclean: clean
	@rm -f $(NAME)

# Remake
.PHONY: re
re: fclean all

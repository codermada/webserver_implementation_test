NAME = webserv

CXX = c++
CXXFLAGS = -Wall -Werror -Wextra -std=c++98 \
	-IConfig -IValidator -IFile -ISignal -INetwork -IMultiplexing -IResponse -IRequest -I.

# Colors
GREEN  = \033[0;32m
YELLOW = \033[0;33m
BLUE   = \033[0;34m
RESET  = \033[0m

QUIET = @

# Sources
CONFIG_DIR = Config
CONFIG_SRCS = $(addprefix $(CONFIG_DIR)/, Config.cpp helpers.cpp)

VALIDATOR_DIR = Validator
VALIDATOR_SRCS = $(addprefix $(VALIDATOR_DIR)/, Validator.cpp)

FILE_DIR = File
FILE_SRCS = $(addprefix $(FILE_DIR)/, File.cpp)

NETWORK_DIR = Network
NETWORK_SRCS = $(addprefix $(NETWORK_DIR)/, Address.cpp SocketOperation.cpp)

SIGNAL_DIR = Signal
SIGNAL_SRCS = $(addprefix $(SIGNAL_DIR)/, Signal.cpp)

MULTIPLEXING_DIR = Multiplexing
MULTIPLEXING_SRCS = $(addprefix $(MULTIPLEXING_DIR)/, SelectMultiplexer.cpp)

RESPONSE_DIR = Response
RESPONSE_SRCS = $(addprefix $(RESPONSE_DIR)/, \
	HttpError.cpp HttpRedirection.cpp HttpSuccess.cpp \
	StaticFile.cpp DirectoryListing.cpp ExceptionHandler.cpp Upload.cpp CGI.cpp)

REQUEST_DIR = Request
REQUEST_SRCS = $(addprefix $(REQUEST_DIR)/, Header.cpp)

SRCS = main.cpp Webserv.cpp \
	$(CONFIG_SRCS) $(VALIDATOR_SRCS) $(FILE_SRCS) \
	$(NETWORK_SRCS) $(SIGNAL_SRCS) \
	$(MULTIPLEXING_SRCS) $(RESPONSE_SRCS) $(REQUEST_SRCS)

OBJS = $(SRCS:.cpp=.o)

TOTAL := $(words $(SRCS))
COUNT := 0
BAR_WIDTH := 30

.PHONY: all clean fclean re

# ===================== BUILD =====================

all:
	@echo "$(YELLOW)🚧 Building $(NAME)...$(RESET)"
	@$(MAKE) $(NAME)

$(NAME): $(OBJS)
	$(QUIET)$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@echo "\n$(GREEN)✔ Build successful: $(NAME) ready!$(RESET)"

# ===================== PROGRESS BAR =====================

define progress_bar
	$(eval COUNT=$(shell echo $$(($(COUNT)+1))))
	$(eval PERCENT=$(shell echo $$(($(COUNT)*100/$(TOTAL)))))
	$(eval FILLED=$(shell echo $$(($(COUNT)*$(BAR_WIDTH)/$(TOTAL)))))
	$(eval EMPTY=$(shell echo $$(($(BAR_WIDTH)-$(FILLED)))))

	$(QUIET)printf "\r$(BLUE)[%3d%%] [" $(PERCENT)

	$(QUIET)for i in $$(seq 1 $(FILLED)); do printf "#"; done
	$(QUIET)for i in $$(seq 1 $(EMPTY)); do printf "-"; done

	$(QUIET)printf "] %-30s$(RESET)" $(notdir $<)
endef

%.o: %.cpp
	$(QUIET)$(call progress_bar)
	$(QUIET)$(CXX) $(CXXFLAGS) -c $< -o $@

# ===================== CLEAN =====================

clean:
	@echo "$(YELLOW)🧹 Cleaning object files...$(RESET)"
	$(QUIET)rm -f $(OBJS)

fclean: clean
	@echo "$(YELLOW)🗑 Removing executable...$(RESET)"
	$(QUIET)rm -f $(NAME)

re:
	@echo "$(YELLOW)🔄 Rebuilding project...$(RESET)"
	@$(MAKE) fclean
	@$(MAKE) all

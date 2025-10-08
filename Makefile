BBLU		=	\033[1;34m
BGREEN		=	\033[1;32m
BRED		=	\033[1;31m
BLU			=	\033[0;34m
GRN			=	\033[0;32m
RED			=	\033[0;31m
RST			=	\033[0m

SRC_SERVER		=	$(shell find ./$(NAME_SERVER) -type f -name "*.cpp" | cut -c 10-)
HDR_SERVER		= 	$(shell find ./$(NAME_SERVER) -type f -name "*.hpp" | cut -c 3-)

CXX				=	g++
FLAGS			=	-std=c++20 -Wall -Werror -Wextra -O2 -Idaemon/include

NAME 			= 	MattDaemon
NAME_SERVER		=	daemon
OBJ_SERVER_REP	=	obj_$(NAME_SERVER)
OBJ_SERVER		=	$(addprefix ./$(OBJ_SERVER_REP)/, $(SRC_SERVER:.cpp=.o))

all: print_header $(NAME)

print_header:
	@echo "$(BBLU)==========================================================="
	@echo "$(BBLU)"
	@echo "$(BBLU)\033[5G  /@@      /@@             /@@     /@@     /@@@@@@@                                                       "
	@echo "$(BBLU)\033[5G | @@@    /@@@            | @@    | @@    | @@__  @@                                                      "
	@echo "$(BBLU)\033[5G | @@@@  /@@@@  /@@@@@@  /@@@@@@ /@@@@@@  | @@  \ @@  /@@@@@@   /@@@@@@  /@@@@@@/@@@@   /@@@@@@  /@@@@@@@ "
	@echo "$(BBLU)\033[5G | @@ @@/@@ @@ |____  @@|_  @@_/|_  @@_/  | @@  | @@ |____  @@ /@@__  @@| @@_  @@_  @@ /@@__  @@| @@__  @@"
	@echo "$(BBLU)\033[5G | @@  @@@| @@  /@@@@@@@  | @@    | @@    | @@  | @@  /@@@@@@@| @@@@@@@@| @@ \ @@ \ @@| @@  \ @@| @@  \ @@"
	@echo "$(BBLU)\033[5G | @@\  @ | @@ /@@__  @@  | @@ /@@| @@ /@@| @@  | @@ /@@__  @@| @@_____/| @@ | @@ | @@| @@  | @@| @@  | @@"
	@echo "$(BBLU)\033[5G | @@ \/  | @@|  @@@@@@@  |  @@@@/|  @@@@/| @@@@@@@/|  @@@@@@@|  @@@@@@@| @@ | @@ | @@|  @@@@@@/| @@  | @@"
	@echo "$(BBLU)\033[5G |__/     |__/ \_______/   \___/   \___/  |_______/  \_______/ \_______/|__/ |__/ |__/ \______/ |__/  |__/"
	@echo "$(BBLU)"
	@echo "$(BBLU)==================> by rgeral & dbarbry <=================="
	@echo "$(RST)"

./obj_$(NAME_SERVER)/%.o: ./$(NAME_SERVER)/%.cpp $(HDR_SERVER)
	mkdir -p $(OBJ_SERVER_REP) $(OBJ_SERVER_REP)/log
	$(CXX) $(FLAGS) -c $< -o $@
	echo "$(BBLU)[$(NAME) OBJ] :$(RST) $@ $(BGREEN)\033[56G[✔]$(RST)"

$(NAME): $(OBJ_SERVER)
	$(CXX) $(FLAGS) $(OBJ_SERVER) -o $(NAME)
	echo "$(BGREEN)[$(NAME) END] :$(RST)$(RST) ./$(NAME) $(BGREEN)\033[56G[✔]$(RST)"

server:
	if [ ! -f "./$(NAME)" ]; then \
		echo "$(RED)[ERROR] :$(RST) Compile the project first$(RED)\033[56G[✘]$(RST)"; \
		exit 1; \
	fi;
	@echo "$(GRN)[LOG]  :$(RST) Launching $(NAME)...$(BGREEN)\033[56G[✔]$(RST)"
	@sudo ./$(NAME) || true

clean:
	$(RM) $(OBJ_SERVER)
	$(RM) -r $(OBJ_SERVER_REP)
	echo "$(RED)[CLEAN]  :$(RST) Deleting objects...$(BGREEN)\033[56G[✔]$(RST)"

fclean: clean
	$(RM) $(NAME)
	echo "$(RED)[FCLEAN] :$(RST) Deleting executable...$(BGREEN)\033[56G[✔]$(RST)"

re: fclean all

logs:
	@sudo sh -c 'mkdir -p /var/log/matt_daemon && touch /var/log/matt_daemon/matt_daemon.log && tail -f /var/log/matt_daemon/matt_daemon.log'

client:
	@echo "Connecting to localhost:4242 (type or wait). If server replies 'server full', client will exit.";
	@(nc -N localhost 4242 2>/dev/null || nc localhost 4242) || true

kill:
	@sudo killall $(NAME) 2>/dev/null || true
	@sudo $(RM) -f /var/lock/matt_daemon.lock
	@echo "Daemon killed and lock removed."

.PHONY: all print_header server clean fclean re logs client kill
.SILENT:
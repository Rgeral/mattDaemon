# Makefile for Matt_daemon

CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Werror -O2

NAME := Matt_daemon

INCDIR := include
SRCDIR := src
OBJDIR := objs

SRC := \
	$(SRCDIR)/main.cpp \
	$(SRCDIR)/start_server.cpp \
	$(SRCDIR)/signal.cpp \
	$(SRCDIR)/tintin_reporter.cpp \
	$(SRCDIR)/server.cpp

OBJ := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRC))

INCLUDES := -I$(INCDIR) -I$(SRCDIR)

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -f $(OBJ)
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(NAME)
	rm -rf $(OBJDIR)


re: fclean all

# Helper target: tail the log file
logs:
	@sudo sh -c 'mkdir -p /var/log/matt_daemon && touch /var/log/matt_daemon/matt_daemon.log && tail -f /var/log/matt_daemon/matt_daemon.log'

# Helper target: minimal client using netcat (nc)
client:
	@echo "Connecting to localhost:4242 (type or wait). If server replies 'server full', client will exit.";
	@(nc -N localhost 4242 2>/dev/null || nc localhost 4242) || true

kill:
	@sudo killall Matt_daemon 2>/dev/null || true
	@sudo rm -f /var/lock/matt_daemon.lock
	@echo "Daemon killed and lock removed."

.PHONY: all clean fclean re logs client kill

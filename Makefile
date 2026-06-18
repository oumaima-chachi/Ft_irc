NAME        = ircserv
NAME_BONUS  = ircserv_bonus

CC          = c++
CPPFLAGS    = -Wall -Wextra -Werror -std=c++98

OBJ_DIR     = .obj/
OBJ_DIR_B   = .obj_bonus/

SRCS        = core_actions/cmd_invite.cpp \
              core_actions/cmd_join.cpp \
              core_actions/cmd_kick.cpp \
              core_actions/cmd_mode.cpp \
              core_actions/cmd_nick.cpp \
              core_actions/cmd_pass.cpp \
              core_actions/cmd_privmsg.cpp \
              core_actions/cmd_quit.cpp \
              core_actions/cmd_topic.cpp \
              core_actions/cmd_user.cpp \
              core_logic/channel.cpp \
              core_logic/client.cpp \
              core_logic/main.cpp \
              core_logic/server.cpp

BONUS_SRCS  = $(SRCS) bonus/bonus.cpp

OBJS        = $(SRCS:%.cpp=$(OBJ_DIR)%.o)
BONUS_OBJS  = $(BONUS_SRCS:%.cpp=$(OBJ_DIR_B)%.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CPPFLAGS) $(OBJS) -o $(NAME)

bonus: $(NAME_BONUS)

$(NAME_BONUS): $(BONUS_OBJS)
	$(CC) $(CPPFLAGS) $(BONUS_OBJS) -o $(NAME_BONUS)

$(OBJ_DIR)%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) -c $< -o $@ -MMD -MP

$(OBJ_DIR_B)%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) -DBONUS -c $< -o $@ -MMD -MP

clean:
	rm -rf $(OBJ_DIR) $(OBJ_DIR_B)

fclean: clean
	rm -f $(NAME) $(NAME_BONUS)

re: fclean all

run: all
	./$(NAME) 8080 okokok

run_bonus: bonus
	./$(NAME_BONUS) 8080 okokok

.PHONY: all bonus clean fclean re run run_bonus

-include $(OBJS:.o=.d)
-include $(BONUS_OBJS:.o=.d)
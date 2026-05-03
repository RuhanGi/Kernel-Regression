NAME = kernel.exe
SRCDIR = src
SRCS =	main.cpp Data.cpp Types.cpp export.cpp \
			Kernel.cpp GProcess.cpp

OBJS = $(addprefix $(OBJDIR)/, $(SRCS:.cpp=.o))

OBJDIR = obj
CXXFLAGS = -Wall -Wextra -Werror -g3 -MMD -MP

.SILENT:

all: $(OBJDIR) $(NAME)

$(OBJDIR):
	if not exist $(OBJDIR) mkdir $(OBJDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	c++ $(CXXFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	c++ $(CXXFLAGS) $(OBJS) -o $(NAME)

a: $(OBJDIR) $(NAME)
	-.\$(NAME) hw4_data/regression/wine_red.csv 

f: $(OBJDIR) $(NAME)
	-.\$(NAME) hw4_data/regression/forest_fires.csv 

car: $(OBJDIR) $(NAME)
	-.\$(NAME) hw4_data/regression/automobile.csv 

cub: $(OBJDIR) $(NAME)
	-.\$(NAME) hw4_data/regression/cubic_1d.csv 

q: $(OBJDIR) $(NAME)
	-.\$(NAME) hw4_data/regression/quadratic_1d.csv 

s: $(OBJDIR) $(NAME)
	-.\$(NAME) hw4_data/regression/saddle_2d.csv 

sin: $(OBJDIR) $(NAME)
	-.\$(NAME) hw4_data/regression/sin_1d.csv 

ss: $(OBJDIR) $(NAME)
	-.\$(NAME) hw4_data/regression/sinsurf_2d.csv 


clean:
	if exist $(OBJDIR) rmdir /s /q $(OBJDIR)

fclean: clean
	if exist $(NAME) del /q $(NAME)

re: fclean all

gpush: fclean
	git add .
	git commit -m "gaussian process"
	git push
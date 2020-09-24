#!/bin/bash
# The line above makes your script executable.
# You should write your tutorial in here.
# Include comments inline with your script explaining what is going on.

# THIS TUTORIAL CAN BE SHARED ON OUR CS5007 PIAZZA BOARD.

# You might start out by first saying the purpose of the script and
# demonstrating its usage.

# PURPOSE:
# This script will detect and print some basic system information, including
# disk usage, cpu information, memory usage, gpu information.
# The user can choose the information interatively.
# Also, a console-oriented weather forecast service is used at the beginning of the
# script because it's cool.

# Section 0 User information.

# The command whoami is used to get the username of the current user.
USER=$(whoami)
# Get the home folder of the current user.
HOMEDIR="/home/$USER/"

# Here is a trick of printf used to print some characters repeatedly.
# Use printf to printf a character repeatedly. Let's break it down step by step.
# 1. In "%s", s represents a string. it could be any length. S will be substituted by the
# argument following it.
# 2. In "%0s", Adding 0 to specify the length of the s string is zero. But if the following
# argument is longer than the specified length, printf will print the whole thing.
# 3. In "%0.s". Adding "." so printf will truncate the string longer than the specified length,
# which is zero. That means "%0.s" will print "" no matter what argument is given.
# 4. In "%).s#". '#' is the character to be printed after string s is printed.
# 5. {1..80} these means the arguments of printf, which are 1 2 3 4 ... 79 80.
# So we have 80 arguments here.
# Then we come to the important part. The default behavior of printf is that when you give it
# more arguments than there are specified in the format string. It will automatically iterate over
# the arguments with the format string.
# So
printf "%0.s#" {1..80}
printf "\n\n"

# As we may want to add a line multiple times, we can define a function to do this for us.
print_line() {
	# Here, $1 is the first argument of the function.
	printf "%0.s$1" {1..80}
	printf "\n"
}

# About terminal text color and format.
# For ANSI/VT100 terminals and terminal simulators, text's color and format can be adjusted by
# escape sequences. A escape sequence is composed of the ESCAPE character and some other
# characters. The format is "<ESC>[{Format_Code}m". In bash, <ESC> can be represented as \e, \033, \x1b.

YELLOW='\033[1;93m'
# No specified color.
NC='\x1b[0m'
printf "${YELLOW}"
# Before looking at our own system. Let's try something really cool.
# The wttr.in is a command line weather service.
# Most simple useage is : $curl wttr.in
# Here we specified the format with a string.
# In the string: l = location; c = weather condition icon; C = weather condition name
# t = temperature; w = wind; P = pressure; m = moon phase; M = moonday.
curl wttr.in/?format="%l:+%c+%C+%t+%w+%P+%m+%M"
printf "${NC}"

USERCOLOR='\033[1;104m'
# Using $VARIABLE_NAME in printf.
# Escape sequences are used to change the color and format of the text.
# e[3m is used to specify italic.
# e[4m is used to specify underline.
# e[5m is used to specify blink.
# e[0m is used to reset all the attributes.
# More colors and formats can be googled easily.
printf "\nHello! User \e[5m\e[3m\e[4m\e[97m${USERCOLOR}$USER${NC}\e[0m! Welcome to this command line tutorial!\n\n"

disk_usage() {
	# Section 1 DISK usage.
	printf "\033[1;33m"
	# Use the print_line function defined above to print some beautiful lines.
	print_line "_"
	print_line "+"
	printf "\n"
	# Here we are about to query disk usage of the current user.
	printf "DISK USAGE\n\n"
	printf "Please note disk usage is specifically coded according to my Khoury Server's directory strucuture."
	printf "It is very unlikely that it will work on other computer. Sorry for the inconvenience!\n\n"
	# df(stands for: disk filesystem) is a tool to display disk usage.
	# A very useful option is -h (--human-readable).
	# As its name implies, this option can give your a result easier to understand. You
	# won't want to read the size in bytes when you have a terabyte hard drive.

	# Use head -n 1 to print the first line of the ouput.
	df -h 2>/dev/null | head -n 1
	# Use grep to find out the user's home directory.
	df -h 2>/dev/null | grep $USER

	# Here I used IO redirection. There are 3 file descriptors, stdin, stdout and stderr in bash.
	# Number 1 stands for stdout and 2 stderr. Character '>' is used to redirect IO.
	# 'file1 > file2' means the output ot file1 is redirected to file2. File2 will be created or overwritten.
	# 'file1 >> file2' means append file1 to file2.

	# So, 2>/dev/null is used to redirect the stand error ouput to device null. which is
	# a way to suppress the unwanted error message. I put it here because the df command
	# on Khoury server always output an error message.
	print_line "="
	printf "\n"
	printf "$NC"
}

cpu_info() {
	#  Section 2 CPU information.
	printf "\033[1;91m"
	print_line "_"
	print_line "+"
	printf "\n"

	printf "CPU INFO\n\n"

	# lscpu is used to display information about the CPU architecture.
	# However, the output includes some advanced information. Thus, we use
	# grep to extract the information that an ordinary people like me cares about.
	lscpu | grep -i "archi\|model name\|mhz\|cache\|CPU(S)"

	print_line "="
	printf "\n"
	printf "$NC"
}

mem_usage() {
	# Section 3 memory information.
	printf "\033[1;92m"
	print_line "_"
	print_line "+"
	printf "\n"

	printf "MEMORY INFO\n"

	# free is used to display amount of free and used memory in the system.
	# The option -h (-human) is just like the option -h of df.  It will make
	# the result easy to understand. With this option, the output will automatically
	# scaled to shortest three digit unit and display the unit of print out.
	free -h
	print_line "="
	printf "\n"
	printf "$NC"
}

gpu_info() {
	# Section 4 GPU information.

	printf "\033[1;96m"
	print_line "_"
	print_line "+"
	printf "\n"

	printf "GPU INFO\n\n"
	printf "Find graphics card, and print detailed information about the card.\n\n"

	# Now, let's try to find the graphics card on this system.
	# Graphics card is connected through PCI bus, so we use lspci command
	# to list all the devices on the PCI bus.
	# Then we use grep to find any device include 'vga', '3d' or '2d', which are commmon
	# for graphics card name.
	# In the following grep command, -i meands ignore the case of the character.
	# Then let's focus on the pattern definition. a back slash is used to escape '|',
	# that is '\|' means logic operator 'or'.
	GRAPHICS=$(lspci | grep -i 'vga\|3d\|2d')

	# Since there could be more than one gpu. We use while to iterate over the lines in
	# the string "$GRAPHICS". Note <<< refirector is used here to accept a string as
	# the input of the while loop.
	# read -r line is used to read a line from the input stream. Here, each line represents a gpu.
	while IFS= read -r line; do
		# ARR=($line) is used to convert the string to an array which uses spaces as delimiter.
		ARR=($line)
		# For command lspci:
		# -v represents the degree of verbosity.
		# -s represents the selection of the device domain. Here we extract the device domain from
		# each line of gpu information.
		lspci -v -s ${ARR[0]}
	done <<<"$GRAPHICS"

	print_line "="
	printf "\n"
	# Reset color.
	printf "$NC"
}

# Main loop of the script. It will loop until user input 'q' or 'Q'.
# The syntax of while loop in bash is:
# 
# 	while CONTROL-COMMAND; do 
#		CONSEQUENT-COMMANDS; 
#	done
#
while true; do
	printf "You can input one of the four options to indicate what information you want.\n"
	printf "c|C -- CPU INFO\n"
	printf "d|D -- DISK USAGE\n"
	printf "g|G -- GPU INFO\n"
	printf "m|M -- MEMORY USAGE\n"
	printf "q|Q -- Quit\n"
	# Read the user input.
	read OPTION
	# I think it a good idea to use switch case to provide a menu of options. So I explored
	# how to use switch case in bash. 
	# The syntax of bash switch case is:
	#
	# 	case EXPRESSION in 
	#		CASE1) 
	#			COMMAND-LIST
	#			;; 
	#		CASE2) 
	#			COMMAND-LIST
	#			;; 
	#		... 
	#		CASEN) 
	#			COMMAND-LIST
	#			;; 
	#		esac
	#
	case $OPTION in
	"d" | "D")
		printf "OK, let me check DISK USAGE.\n"
		# Call the function 'disk_usage' we defined above.
		disk_usage
		;;
	"c" | "C")
		printf "OK, let me check CPU INFO.\n"
		cpu_info
		;;
	"m" | "M")
		printf "OK, let me check MEMORY USAGE.\n"
		mem_usage
		;;
	"g" | "G")
		printf "OK, let me check GPU INFO.\n"
		gpu_info
		;;
	"q" | "Q")
		printf "Goodbye!\n"
		break
		;;
	*)
		printf "Sorry, unknown option.\n"
		;;
		# Reverse of 'case', meaning the end of switch case.
	esac
done

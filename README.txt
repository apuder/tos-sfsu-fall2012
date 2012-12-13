This is the README file for tos operating system created by SFSU 
in the fall of 2012 for CSC720. This version of tos has two main functions.
There is a chat window as well as a pong game. Each of these interact with
a java program that is running outside of tos. Both tos and the java versions
communicate with each other through the network by sending UDP packets. In order to download our version of tos it is recommended to use git. If there are any
problems the best place to troubleshoot is at http://pear.sfsu.edu/csc720/tos/ or the 
github location https://github.com/apuder/tos-sfsu-fall2012.

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

1. Downloading TOS
git clone git://github.com/apuder/tos-sfsu-fall2012.git
cd tos-sfsu-fall2012
sh ./configure
may need to install nasm and javac, g++, bochs-x, bochs-sdl

2. Starting bochs
sudo bochs
enter in user password

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

3. Bochs command line
Tab to change from different windows
Running Chat from shell: > chat
Running Pong from shell: > pong "username"
	      		 > coin

	A. Inside chat window:
		i. To change name of user: > name "username"
	B. Inside pong window:
		i. "Q" to move paddle up
		j. "A" to move paddle down 

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

4. Chat in terminal
java -jar tools/UDPChat/chat.jar incomingPort outgoingPort IP
recommended: 
java -jar tools/UDPChat/chat.jar 10001 10002 192.168.1.2

To change the name of the user within chat window:
name "username"

5. Pong in terminal
java -jar tools/pong/pong.jar Java incomingPort outgoingPort IP
recommended:
java -jar tools/pong/pong.jar Java 9876 9875 192.168.1.2
Once inside the window use up and down arrows on the keyboard to control the paddle
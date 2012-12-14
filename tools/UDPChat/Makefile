
SRC = \
	src/sfsu/edu/os/ChatWindow.java \
	src/sfsu/edu/os/SenderThread.java \
	src/sfsu/edu/os/UDPComm.java \
	
all: chat.jar

chat.jar: $(SRC)
	javac -source 1.6 -d . -sourcepath ./src src/sfsu/edu/os/ChatWindow.java
	jar cfm chat.jar MANIFEST.MF sfsu/edu/os/*.class 
	rm -rf sfsu 
	cp chat.jar ../..

clean:
	rm -f chat.jar

depend:



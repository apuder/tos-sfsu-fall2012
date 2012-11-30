
SRC = \
	src/com/sfsu/Main.java \
	src/com/sfsu/Consts.java \
	src/com/sfsu/Pong.java \
	src/com/sfsu/UDPComm.java \
	
all: pong.jar

pong.jar: $(SRC)
	javac -source 1.6 -d . -sourcepath ./src src/com/sfsu/Main.java
	jar cfm pong.jar MANIFEST.MF com/sfsu/*.class 
	rm -rf com
	#cp ttc.jar ../..

clean:
	rm -f pong.jar

depend:



package sfsu.edu.os;

public class SenderThread extends Thread {

	UDPComm objSender;

	public SenderThread(UDPComm objSender) {
		super();
		this.objSender = objSender;
		System.out.println("created sender with the sender objecrt");
	}

	public void run() {

	}

	public void btnPressed(byte [] toSend) {
		
		objSender.send(toSend);

	}

	public SenderThread() {
		super();
	}

}

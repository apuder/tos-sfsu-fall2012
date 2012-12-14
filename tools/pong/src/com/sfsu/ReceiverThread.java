package com.sfsu;

public class ReceiverThread extends Thread {

	UDPComm obj;
	String receiveData;
	boolean isReceived;
	int y1Paddle;

	public int getY1Paddle() {
		return y1Paddle;
	}

	public void setY1Paddle(int y1Paddle) {
		this.y1Paddle = y1Paddle;
	}

	public boolean isReceived() {
		return isReceived;
	}

	public void setReceived(boolean isReceived) {
		this.isReceived = isReceived;
	}

	public UDPComm getObj() {
		return obj;
	}

	public void setObj(UDPComm obj) {
		this.obj = obj;
	}

	public String getReceiveData() {
		return receiveData;
	}

	public void setReceiveData(String receiveData) {
		this.receiveData = receiveData;
	}

	public ReceiverThread(UDPComm obj) {
		super();
		this.obj = obj;
		isReceived = false;
		receiveData = "";
	}

	public ReceiverThread() {
		super();
	}

	public void run() {
		receiveData();
	}

	public void receiveData() {

		while (true) {

			// System.out.println("waiting for the data");
			byte[] receiveData = obj.receive();
			isReceived = true;
			if (receiveData[0] == 0) {
				y1Paddle=receiveData[1];
				//System.out.println("received the data with y1"+ y1Paddle);
			}
			if (receiveData[0] != 0) {
				String stData = new String(receiveData);
				this.setReceiveData(stData.trim());
				// System.out.println("receiving data from tos/ receiver is "
				// + stData.trim());
			}

		}

	}

}

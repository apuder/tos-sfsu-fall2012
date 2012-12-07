package sfsu.edu.os;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.UnknownHostException;

public class UDPComm {

	// Member variables of the class
	private int SenderPort;
	private int ReceiverPort;
	private String host;
	private byte[] data;

	public int getSenderPort() {
		return SenderPort;
	}

	public void setSenderPort(int senderPort) {
		SenderPort = senderPort;
	}

	public int getReceiverPort() {
		return ReceiverPort;
	}

	public void setReceiverPort(int receiverPort) {
		ReceiverPort = receiverPort;
	}

	public String getHost() {
		return host;
	}

	public void setHost(String host) {
		this.host = host;
	}

	public byte[] getData() {
		return data;
	}

	public void setData(byte[] data) {
		this.data = data;
	}



	// Declaring local variables
	DatagramSocket SocketSender;
	DatagramSocket SocketReceiver;
	InetAddress IPAddress;

	public void PrepareConnToSend() throws UnknownHostException,
			SocketException {

		IPAddress = InetAddress.getByName(host);
		SocketSender = new DatagramSocket();

	}

	public void PrepareConnToReceive() throws UnknownHostException,
			SocketException {

		SocketReceiver = new DatagramSocket(this.ReceiverPort);

	}

	public void send(byte[] arr) {

		DatagramPacket sendPacket = new DatagramPacket(arr, arr.length,
				IPAddress, SenderPort);
		try {

			SocketSender.send(sendPacket);

		} catch (IOException e) {
			e.printStackTrace();
		}

	}

	public byte[] receive() {

		byte[] receiveData = new byte[1024];
		try {

			DatagramPacket receivePacket = new DatagramPacket(receiveData,
					receiveData.length);
			SocketReceiver.receive(receivePacket);
			receiveData = receivePacket.getData();

		} catch (Exception e) {
			e.printStackTrace();
		}
		return receiveData;

	}

	public void closeConnection() {
		if (!SocketSender.isClosed())
			SocketSender.close();
		if (!SocketReceiver.isClosed())
			SocketReceiver.close();
	}

}

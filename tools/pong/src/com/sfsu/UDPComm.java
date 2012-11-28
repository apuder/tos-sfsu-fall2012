package com.sfsu;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.UnknownHostException;

public class UDPComm {

	// Member variables of the class
	private int port;
	private String host;
	byte[] data;

	// Declaring local variables
	DatagramSocket SocketSender;
	DatagramSocket SocketReceiver;
	InetAddress IPAddress;

	public UDPComm() {
		super();
	}

	public UDPComm(int port, String host) {
		super();
		this.port = port;
		this.host = host;

	}

	public UDPComm(int port) {
		super();
		this.port = port;
	}

	public byte[] getData() {
		return data;
	}

	public void setData(byte[] data) {
		this.data = data;
	}

	public int getPort() {
		return port;
	}

	public void setPort(int port) {
		this.port = port;
	}

	public String getHost() {
		return host;
	}

	public void setHost(String host) {
		this.host = host;
	}

	public void PrepareConnToSend() throws UnknownHostException,
			SocketException {

		IPAddress = InetAddress.getByName(host);
		SocketSender = new DatagramSocket();

	}

	public void PrepareConnToReceive() throws UnknownHostException,
			SocketException {

		SocketReceiver = new DatagramSocket(this.port);

	}

	public void send(byte[] arr) {

		DatagramPacket sendPacket = new DatagramPacket(arr, arr.length,
				IPAddress, port);
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
		if  (!SocketSender.isClosed())
		SocketSender.close();
		if(!SocketReceiver.isClosed())
			SocketReceiver.close();
	}

}

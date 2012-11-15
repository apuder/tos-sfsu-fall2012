package com.tos.udp;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;

public class UDPserver {
	private final static int PACKETSIZE = 1024 ;
	/**
	 * @param args the command line arguments
	 */

	public static void main(String args[]) throws Exception
	{
		DatagramSocket socket = null;
		
		socket = new DatagramSocket(9000);
		byte[] receiveData = new byte[PACKETSIZE];
		byte[] sendData = new byte[PACKETSIZE];
		while(true)
		{
			//wait for packet to recieve
			DatagramPacket receivePacket = new DatagramPacket(receiveData, receiveData.length);
			socket.receive(receivePacket);
			//convert to string
			String sentence = new String( receivePacket.getData());
			System.out.println("FROM CLIENT: " + sentence);
			//get address
			InetAddress IPAddress = receivePacket.getAddress();
			int port = receivePacket.getPort();
//			System.out.println("ON PORT: " + port);
			String capitalizedSentence = sentence.toUpperCase();
			//send back to client
			sendData = capitalizedSentence.getBytes();
			DatagramPacket sendPacket =
					new DatagramPacket(sendData, sendData.length, IPAddress, port);
//			socket.send(sendPacket);
		}
		
	}
}

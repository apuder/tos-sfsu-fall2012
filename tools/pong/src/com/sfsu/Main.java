package com.sfsu;

import java.net.SocketException;
import java.net.UnknownHostException;

/**
 * @since 11/18/2012
 * 
 */
public class Main {

	/**
	 * main program for PONG game creates object of pong game, and of UDP
	 * connection.
	 * 
	 * @param1 Sender, Receiver Sender begins a pong sender window,which send
	 *         information to another listener pong window, running at @param3
	 *         host on @param2 port default is sender(Default client - 9879,
	 *         server 9876)
	 * @param2 is the ip the port number of the application, default is
	 *         localhost 127.0.0.1
	 * @param3 is the host, in case of sender
	 * 
	 */

	@SuppressWarnings("null")
	public static void main(String[] args) {

		String host = "127.0.0.1";
		int portSender = 9876;
		int portReceiver = 9875;
		UDPComm objUDPComm = null;
		Pong p;
		String name = "Sender";

		if (args.length < 1) {
			System.out.println("Defaul type is sender");
			System.out.println("Usage: UDPClient " + "Now using host = " + host
					+ ", Ports# = " + portSender + "and " + portReceiver);
		}

		else {
			name = String.valueOf(args[0]);
			portSender = Integer.valueOf(args[1]).intValue();
			portReceiver = Integer.valueOf(args[2]).intValue();
			host = String.valueOf(args[3]);
		}

		try {

			UDPComm objComm = new UDPComm();
			objComm.setSenderPort(portSender);
			objComm.setReceiverPort(portReceiver);
			objComm.setHost(host);
			objComm.PrepareConnToSend();
			objComm.PrepareConnToReceive();
			

			p = new Pong(objComm, name);
			p.setNameSelf(name);
			p.play();

		} catch (SocketException e) {
			e.printStackTrace();
		} catch (UnknownHostException e) {
			e.printStackTrace();
		} finally {
			objUDPComm.closeConnection();
		}

	}

}

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

	public static void main(String[] args) {

		String host = "127.0.0.1";
		int port = 9876;
		UDPComm objUDPComm = null;
		String role = Consts.ROLE;
		Pong p;

		if (args.length < 1) {
			System.out.println("Defaul is type sender typer");
			System.out.println("Usage: UDPClient " + "Now using host = " + host
					+ ", Port# = " + port);
		}

		else {

			role = String.valueOf(args[0]);
			port = Integer.valueOf(args[1]).intValue();
			host = String.valueOf(args[2]);

			System.out.println("Usage: UDPClient " + "Now using host = " + host
					+ ", Port# = " + port);
		}

		try {

			if (role.equals("Receiver")) {
				objUDPComm = new UDPComm(port);
				objUDPComm.PrepareConnToReceive();
			} else if (role.equals("Sender")) {

				objUDPComm = new UDPComm(port, host);
				objUDPComm.PrepareConnToSend();
			}

			p = new Pong(objUDPComm, role);
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

package com.tos.udp;

import java.io.*;
import java.net.*;

public class UDP {
	private final static int PACKETSIZE = 100 ;
    /**
     * @param args the command line arguments
     */
	 public static void main( String args[] )
	   {

	      DatagramSocket socket = null ;

	      try
	      {
	         // Convert the arguments first, to ensure that they are valid
	         InetAddress host = InetAddress.getByName( args[0] ) ;
	         int port         = Integer.parseInt( args[1] ) ;

	         // Construct the socket
	         socket = new DatagramSocket() ;

	         // Construct the datagram packet
	         byte [] data = args[2].getBytes() ;
	         DatagramPacket packet = new DatagramPacket( data, data.length, host, port ) ;

	         // Send it
	         socket.send( packet ) ;

	         // Set a receive timeout, 2000 milliseconds
	         socket.setSoTimeout( 2000 ) ;
	         
	      }
	      catch( Exception e )
	      {
	         System.out.println( e ) ;
	      }
	      finally
	      {
	         if( socket != null )
	            socket.close() ;
	      }
	   }
}

package com.tos.udp;

import java.io.*;
import java.net.*;

public class UDP {

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        try {
            String[] destintationIp = args[0].split(".", 4);
            int byte1 = Integer.valueOf(destinationIp[0]).intValue();
            int destintationPort = Integer.valueOf(args[1]).intValue();
            String content = args[2];
            int port = 90;

            byte[] message = "Java Source and Support".getBytes();

            // Get the internet address of the specified host
            InetAddress address = InetAddress.getByName(host);

            // Initialize a datagram packet with data and address
            DatagramPacket packet = new DatagramPacket(message, message.length,
                    address, port);

            // Create a datagram socket, send the packet through it, close it.
            DatagramSocket dsocket = new DatagramSocket();
            dsocket.send(packet);
            dsocket.close();
        } catch (Exception e) {
            System.err.println(e);
        }
    }
}

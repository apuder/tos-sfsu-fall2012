package sfsu.edu.os;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;

public class ChatWindow extends JFrame implements ActionListener {

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	// Creating various components
	JButton btnSend = new JButton("Send");
	JButton exitChat = new JButton("Exit");
	JTextArea curText = new JTextArea();
	JTextArea chatList = new JTextArea(" ");
	JPanel bottomPanel = new JPanel();
	JPanel topPanel = new JPanel();
	JPanel holdAll = new JPanel();
	JLabel rename = new JLabel("To change user name type \"name 'username'\"");
	JScrollPane sbrText = new JScrollPane(chatList);
	private String userName = "me";

	// Creating sender and receiver
	SenderThread objSender;

	/**
	 * The constructor.
	 */
	public ChatWindow(UDPComm objComm) {

		objSender = new SenderThread(objComm);

		bottomPanel.setLayout(new FlowLayout());
		bottomPanel.add(btnSend);
		bottomPanel.add(exitChat);
		bottomPanel.add(rename);

		topPanel.setLayout(new FlowLayout());
		topPanel.add(chatList);
		topPanel.add(curText);

		// Customizing components
		curText.setPreferredSize(new Dimension(350, 100));
		curText.setLineWrap(true);
		curText.setWrapStyleWord(true);

		chatList.setPreferredSize(new Dimension(350, 350));
		chatList.setEditable(false);
		chatList.setLineWrap(true);
		chatList.setWrapStyleWord(true);

		sbrText.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);

		bottomPanel.setPreferredSize(new Dimension(350, 100));

		holdAll.setLayout(new BorderLayout());
		holdAll.add(bottomPanel, BorderLayout.SOUTH);
		holdAll.add(topPanel, BorderLayout.CENTER);

		getContentPane().add(holdAll, BorderLayout.CENTER);

		btnSend.addActionListener(this);
		exitChat.addActionListener(this);

		setDefaultCloseOperation(DISPOSE_ON_CLOSE);
	}

	/*
	 * @param1 = sender port
	 * 
	 * @param2 = receiver port
	 * 
	 * @param3 = host
	 */
	public static void main(String[] args) {
		int portSender = 9875;
		int portReceiver = 9876;
		String host = "localhost";

		if (args.length < 1) {
			System.out.println("Usage: UDPClient " + "Now using host = " + host
					+ ", SenderPort = " + portSender + "  ReceiverPort = "
					+ portReceiver);
		}
		// Get the port number to use from the command line
		else {

			portSender = Integer.valueOf(args[0]).intValue();
			portReceiver = Integer.valueOf(args[1]).intValue();
			host = String.valueOf(args[2]);
			System.out.println("Usage: UDPClient " + "\nhost = " + host
					+ " Sending Port =  " + portSender
					+ "  Recieving Port = " + portReceiver);
		}

		// Get the IP address of the local machine - we will use this as the
		// address to send the data to
		try {
			UDPComm objComm = new UDPComm();
			objComm.setSenderPort(portSender);
			objComm.setReceiverPort(portReceiver);
			objComm.setHost(host);
			objComm.PrepareConnToSend();
			objComm.PrepareConnToReceive();

			ChatWindow myApplication = new ChatWindow(objComm);
			myApplication.setLocation(10, 10);
			myApplication.setSize(400, 600);
			myApplication.setVisible(true);
			myApplication.setResizable(false);
			myApplication.listen(objComm);
		}

		catch (Exception e) {

			e.printStackTrace();
		}

	}

	/**
	 * Each non abstract class that implements the ActionListener must have this
	 * method.
	 * 
	 * @param e
	 *            the action event.
	 */
	public void actionPerformed(ActionEvent e) {
		
		if (e.getSource() == btnSend) {
			String userMessage =  curText.getText().trim() + "\0";
			byte[] data = userMessage.getBytes();
			if(userMessage.startsWith("name ")){
				this.userName = userMessage.substring(5, userMessage.length()-1);
			}
			chatList.setText(chatList.getText() + "\n" + this.userName +" : "
					+ curText.getText().trim());
			objSender.btnPressed(data);
			curText.setText("");
		}

		if (e.getSource() == exitChat) {
			curText.setText("Exiting Chat");
			System.exit(0);
		}
	}

	public void listen(UDPComm objComm) {
		String sender = "Friend";
		boolean isAssignedName = true;

		while (true) {

			String text = new String(objComm.receive()).trim();

			if (text.length() > 5
					&& text.substring(0, 4).toUpperCase().equals("NAME")
					&& isAssignedName) {
				sender = text.substring(5);
				
				// isAssignedName = false;
				text = " joined the chat";

			}
			chatList.setText(chatList.getText() + "\n" + sender + " : " + text);

		}
	}

}

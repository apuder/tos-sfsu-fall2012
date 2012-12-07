package com.sfsu;

import java.awt.Canvas;
import javax.swing.JFrame;
import java.awt.event.KeyListener;
import java.awt.event.KeyEvent;
import java.awt.Color;
import java.awt.Graphics;

public class Pong implements KeyListener {

	private final int WIDTH = 800;
	private final int HEIGHT = 200;
	private final int DELTA = 8;
	private final int PADDLE_WIDTH = 5;
	private final int PADDLE_HEIGHT = 20;
	private final int PUCK_RADIUS = 10;

	Graphics g;
	UDPComm objComm;

	private int x1 = 0; // location of player A's paddle
	private int y1 = 80;
	private int x2 = 795; // location of player B's paddle
	private int y2 = 80;

	private double x = 60.0; // location of ball
	private double y = 140.0;
	private double vx = 2.0; // velocity of ball
	private double vy = 1.0;

	private int playerOneScore;
	private int playerTwoScore;

	private String nameSelf;
	private String nameOther = "No player yet joined !!!!";
	ReceiverThread objReceiver;

	public String getNameSelf() {
		return nameSelf;
	}

	public void setNameSelf(String nameSelf) {
		this.nameSelf = nameSelf;
	}

	public Pong(UDPComm Comm, String name) {
		this.nameSelf = name;
		objComm = Comm;
		JFrame f = new JFrame();
		f.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		f.setTitle("Pong");
		f.setResizable(false);
		Canvas c = new Canvas();
		c.addKeyListener(this);
		c.setSize(800, 200);
		f.add(c);
		f.pack();
		f.setVisible(true);
		g = c.getGraphics();
		f.addKeyListener(this);
		draw();

	}

	public void keyPressed(KeyEvent e) {
		
		//System.out.println("key pressed !!!!");
		if (e.getKeyCode() == KeyEvent.VK_UP)
			y2 = y2 - DELTA;
		if (y2 < 0) {
			y2 = 0;
		} else if (e.getKeyCode() == KeyEvent.VK_DOWN)
			y2 = y2 + DELTA;
		if (y2 > HEIGHT - PADDLE_HEIGHT) {
			y2 = HEIGHT - PADDLE_HEIGHT;
		} else if (e.getKeyChar() == 'i')
			y1 = y1 - DELTA;
		if (y1 < 0) {
			y1 = 0;
		} else if (e.getKeyChar() == 'k')
			y1 = y1 + DELTA;
		if (y1 > HEIGHT - PADDLE_HEIGHT) {
			y1 = HEIGHT - PADDLE_HEIGHT;
		}
	}

	// These two are required by the compiler, but will not be used in our
	// game.
	public void keyReleased(KeyEvent e) {
	}

	public void keyTyped(KeyEvent e) {
	}

	public void draw() {
		g.setColor(Color.black);
		g.fillRect(0, 0, WIDTH, HEIGHT);
		g.setColor(Color.white);
		g.fillRect(x1, y1, PADDLE_WIDTH, PADDLE_HEIGHT);
		g.setColor(Color.white);
		g.fillRect(x2, y2, PADDLE_WIDTH, PADDLE_HEIGHT);
		g.setColor(Color.white);
		g.fillOval((int) x, (int) y, PUCK_RADIUS, PUCK_RADIUS);
		g.drawString(nameOther + " -->" + Integer.toString(playerTwoScore), 5,
				10);
		g.drawString(nameSelf + " -->" + Integer.toString(playerOneScore), 700,
				10);

	}

	public boolean detectCollision() {
		// Test for collision with first paddle
		if (y + vy > y1 && y + vy < y1 + PADDLE_HEIGHT
				&& x + vx < x1 + PADDLE_WIDTH && x + vx > x1) {
			return true;
		}

		// Test for collision with second paddle
		else if (y + vy > y2 && y + vy < y2 + PADDLE_HEIGHT
				&& x + vx + PUCK_RADIUS > x2
				&& x + vx + PUCK_RADIUS < x2 + PADDLE_WIDTH) {
			return true;
		} else

			return false;

	}

	public void play() {

		objReceiver = new ReceiverThread(objComm);
		objReceiver.start();
		PlaySender();

		// waiting to start game till other player join in.
	}

	public void PlaySender() {

		String xStr;
		long begin = System.currentTimeMillis();
		long current = 0;
		long diff = 0;

		while (true) {

			if (detectCollision()) {
				vx = -vx;
			}

			if (y + vy < 0 || y > (HEIGHT - PUCK_RADIUS)) {
				vy = -vy;

			}

			if (x + vx > (WIDTH - PUCK_RADIUS)) {

				// ball touches right side - increment counters for score
				// for left player
				playerOneScore = playerOneScore + 1;
			//	System.out.println("first player score: " + playerOneScore);
				try {
					Thread.sleep(300);
					x = 0 + (int) (Math.random() * (801));
					y = 0 + (int) (Math.random() * (201));
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}

			}
			if (x + vx < 0) {
				// increment score of right player

				playerTwoScore = playerTwoScore + 1;
				// System.out.println("second player score: " + playerTwoScore);

				try {
					Thread.sleep(300);
					x = 0 + (int) (Math.random() * (801));
					y = 0 + (int) (Math.random() * (201));
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}

			x = x + vx;
			y = y + vy;
			this.y1=objReceiver.getY1Paddle()*10;
			
			draw();

			try {

				// preparing package to send -
				// 0 - > means it is with the data
				// 1 -> means it is with the name

				// sending the name at some time interval so that it always
				// reaches the receiver
				current = System.currentTimeMillis();

				diff = (current - begin) % 1000;
				if (diff > 800 && diff < 850) {
					xStr = nameSelf;
					byte[] outName = new byte[xStr.getBytes().length + 1];
					outName[0] = 1;
					System.arraycopy(xStr.getBytes(), 0, outName, 1,
							xStr.length());
					objComm.send(outName);

					//System.out.println("Sending name");

					//System.out.println("objReceiver.isReceived() -->>>>>"
					//		+ objReceiver.isReceived());
					//System.out.println("name is "+ objReceiver.getReceiveData());
					
					this.nameOther = objReceiver.getReceiveData().toString();
					

				}
				byte[] arr = { (byte) 0, (byte) (x / 10), (byte) (y / 10),
						(byte) (y2 / 10), (byte) playerOneScore };

				//System.err.println("sending other");

				objComm.send(arr);
				Thread.sleep(30);
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
	}

}
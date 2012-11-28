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
	private final int PADDLE_HEIGHT = 50;
	private final int PUCK_RADIUS = 10;

	Graphics g;
	UDPComm objComm;
	String role;

	private int x1 = 0; // location of player A's paddle
	private int y1 = 80;
	private int x2 = 795; // location of player B's paddle
	private int y2 = 80;

	private double x = 60.0; // location of ball
	private double y = 140.0;
	private double vx = 2.0; // velocity of ball
	private double vy = 1.0;

	public Pong(UDPComm Comm, String role) {
		objComm = Comm;
		this.role = role;
		JFrame f = new JFrame();
		f.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		f.setTitle("Pong");
		f.setResizable(false);
		Canvas c = new Canvas();
		c.setSize(800, 200);
		f.add(c);
		f.pack();
		f.setVisible(true);
		g = c.getGraphics();
		f.addKeyListener(this);
		draw();

	}

	public void keyPressed(KeyEvent e) {

		if (role.equals("Sender")) {
			if (e.getKeyCode() == KeyEvent.VK_UP)
				y2 = y2 - DELTA;
			else if (e.getKeyCode() == KeyEvent.VK_DOWN)
				y2 = y2 + DELTA;
			else if (e.getKeyChar() == 'i')
				y1 = y1 - DELTA;
			else if (e.getKeyChar() == 'k')
				y1 = y1 + DELTA;
		}

	}

	// These two are required by the compiler, but will not be used in your
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

		if (role.equals("Sender")) {

			PlaySender();
		}
		if (role.equals("Receiver")) {

			PlayReceiver();

		}

	}

	public void PlaySender() {
		while (true) {

			if (x + vx < 0 || x + vx > WIDTH || detectCollision())
				vx = -vx;
			if (y + vy < 0 || y + vy > HEIGHT)
				vy = -vy;

			x = x + vx;
			y = y + vy;
			draw();
			try {
				byte[] arr = { (byte) (x / 10), (byte) (y / 10),
						(byte) (y2 / 10) };

				objComm.send(arr);
				Thread.sleep(30);
			} catch (Exception e) {
				e.printStackTrace();
			}
		}

	}

	public void PlayReceiver() {
		
		byte[] receiveData = new byte[3];

		while (true) {

			try {

				receiveData = objComm.receive();

				// setting new coordinates
				x = receiveData[0] * 10;
				y = receiveData[1] * 10;
				y2 = receiveData[2] * 10;

				draw();
				Thread.sleep(30);
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
	}

}
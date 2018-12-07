# Plotting serial data from 3-sensor force plate - V1.0 by Raaj Chatterjee DVG
#import serial
import time
import numpy as np
import matplotlib.axes as axes
import matplotlib.pyplot as plt
from datetime import datetime

length = 8;
i = 0;

# Initialize graphs
plt.ion()
fig = plt.figure()
ax = fig.add_subplot(111,aspect='equal')
plt.title("Inverted Pendulum Visualizer", loc = 'center')
plt.xlabel("X (cm)")
plt.ylabel("Y (cm)")

# Create Pole and cart
pole = plt.Line2D((0, 0), (0, 6), lw=10, c='m')
cart = plt.Rectangle((-2,-1), 4,2, zorder = 0)

plt.gca().add_line(pole)
plt.gca().add_patch(cart)

annTheta = plt.annotate("A",  xy=(1, -2))
annX = plt.annotate("A", xy=(-2, -2))

plt.ylim(-3,10)
plt.xlim(-20,20)

while True:		
	# Simulated data (replace with serial data)
	i = i+2;
	if i >360:
		i = 0
	x = 5*np.sin(np.deg2rad(i))
	y = 0
	theta = -30*np.sin(np.deg2rad(i))
	# Optional changing colour
	colorVal = 50

	# Delete old figure
	cart.set_visible(False)
	pole.set_visible(False)
	annTheta.remove()
	annX.remove()
	
	# Plot new lines
	pole = plt.Line2D((x, x+length*np.sin(np.deg2rad(theta))), (0, length*np.cos(np.deg2rad(theta))), lw=10, c='m')
	cart = plt.Rectangle((-2+x,-1), 4,2, zorder = 0)
	plt.gca().add_patch(cart)
	plt.gca().add_line(pole)
	
	# Plot Annotations
	annTheta = plt.annotate("Theta: {:.3}°".format(theta), xy=(5, -2), size = 8)
	annX = plt.annotate("X: {:.2f} m".format(x,y), xy=(-5, -2), size = 8)

	plt.show
	plt.pause(0.001)
		
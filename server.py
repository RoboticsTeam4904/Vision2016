from __future__ import division #Might take time to import
import SocketServer, subprocess, time, cv2, math
import numpy as np

pi = False
gui = False

if pi:
	from picamera.array import PiRGBArray
	from picamera import PiCamera


if pi:
	# initialize the camera and grab a reference to the raw camera capture
	camera = PiCamera()
	camera.resolution = (640, 480)
	camera.framerate = 15
	rawCapture = PiRGBArray(camera, size=camera.resolution)

# constants
nativeResolution = (2592, 1944)
nativeAngle = (math.radians(53.5), math.radians(41.41))
mountAngle = (0, math.radians(45))
shift = (13.25, 2.5)
goalHeight = 8 * 12
cameraHeight = 296 / 25.4 #to inches

cascPath = "cascade.xml"

# Create the haar cascade
goalCascade = cv2.CascadeClassifier(cascPath)


def getImage():
	image = None

	if pi:
		for frame in camera.capture_continuous(rawCapture, format="bgr", use_video_port=True):
			# grab the raw NumPy array representing the image, then initialize the timestamp
			# and occupied/unoccupied text
			image = frame.array

			# show the frame
			if debug:
				cv2.imshow("Frame", image)

			# clear the stream in preparation for the next frame
			rawCapture.truncate(0)
	else:
		image = cv2.imread("latest.jpg")

	return image

def angle_and_dist(goal):
	# [0] = X, [1] = Y, goal[i] = ith corner of highgoal
	# Uses camera.resolution
	degPerPxl = (nativeAngle[0] / camera.resolution[0], nativeAngle[1] / camera.resolution[1])
	goalPixel = ((goal[0].x + goal[1].x + goal[2].x + goal[3].x) / 4, camera.resolution[1] - (goal[0].y + goal[1].y + goal[2].y + goal[3].y) / 4)
	goalAngle = (mountAngle[0] + degPerPxl[0] * (goalPixel[0] - camera.resolution[0] / 2), mountAngle[1] + degPerPxl[1] * (goalPixel[1] - camera.resolution[1] / 2))
	cameraDistance = (goalHeight - cameraHeight) / math.tan(goalAngle[1])
	shift = math.sqrt(shift[0] * shift[0] + shift[1] * shift[1])
	cameraAngle = math.pi - goalAngle[0] - math.atan(shift[0] / shift[1])
	distance = math.sqrt(cameraDistance * cameraDistance + shift * shift - 2 * cameraDistance * shift * math.cos(cameraAngle))
	offAngle = math.asin(math.sin(cameraAngle) * cameraDistance / distance)
	offAngle += math.atan(shift[1] / shift[0]) - math.pi / 2
	return (offAngle, distance)

def processImage(src):
	gray = cv2.cvtColor(src, cv2.COLOR_BGR2GRAY)

	# Detect faces in the image
	goals = goalCascade.detectMultiScale(
		gray,
		scaleFactor=1.1,
		minNeighbors=5,
		minSize=(30, 30),
		flags = cv2.cv.CV_HAAR_SCALE_IMAGE
	)

	print "Found {0} goals!".format(len(goals))

	# Draw a rectangle around the faces
	for (x, y, w, h) in goals:
		cv2.rectangle(src, (x, y), (x+w, y+h), (0, 255, 0), 2)
	
	cv2.imshow("img", src)
	cv2.waitKey(0)



class MyTCPHandler(SocketServer.BaseRequestHandler):
	# Responds to requests for the view of the goal.

	def handle(self):
		# self.request is the TCP socket connected to the client
		#process = subprocess.Popen(["./highgoal.bin", "latest"], stdout=subprocess.PIPE)

		response = process.stdout.read()
		# just send back the same data, but upper-cased
		self.request.sendall(response)

if __name__ == "__main__":
	HOST, PORT = "0.0.0.0", 9999

	processImage(getImage())
	# Create the server, binding to localhost on port 9999
	server = SocketServer.TCPServer((HOST, PORT), MyTCPHandler)

	# Activate the server this will keep running until you
	# interrupt the program with Ctrl-C
	#server.serve_forever()

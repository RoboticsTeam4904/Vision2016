from __future__ import division #Might take time to import
import SocketServer, subprocess, time, cv2, math, cv2.cv
import numpy as np

pi = False
gui = True
webcam = False

if pi:
	from picamera.array import PiRGBArray
	from picamera import PiCamera


if pi:
	# initialize the camera and grab a reference to the raw camera capture
	camera = PiCamera()
	camera.resolution = (640, 480)
	camera.framerate = 15
	rawCapture = PiRGBArray(camera, size=camera.resolution)

if not pi and webcam:
	cap = cv2.VideoCapture(0)
# constants
cameraResolution = (640, 480)
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
	elif webcam:
		ret, image = cap.read()
	else:
		image = cv2.imread("img0176.jpg")

	return image

def angle_and_dist((x, y, w, h)):
	# [0] = X, [1] = Y, goal[i] = ith corner of highgoal
	# Uses camera.resolution
	degPerPxl = (nativeAngle[0] / cameraResolution[0], nativeAngle[1] / cameraResolution[1])
	goalPixel = (x + w/2, y + h/2)
	goalAngle = (mountAngle[0] + degPerPxl[0] * (goalPixel[0] - cameraResolution[0] / 2), mountAngle[1] + degPerPxl[1] * (goalPixel[1] - cameraResolution[1] / 2))
	cameraDistance = (goalHeight - cameraHeight) / math.tan(goalAngle[1])
	shiftTotal = math.sqrt(shift[0] * shift[0] + shift[1] * shift[1])
	cameraAngle = math.pi - goalAngle[0] - math.atan(shift[0] / shift[1])
	distance = math.sqrt(cameraDistance * cameraDistance + shiftTotal * shiftTotal - 2 * cameraDistance * shiftTotal * math.cos(cameraAngle))
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
	if len(goals) > 0:
		largest_area = 0
		largest = (0, 0, 0, 0)
		for (x, y, w, h) in goals:
			# cv2.rectangle(src, (x, y), (x + w, y + h), (0, 255, 0), 2)
			if w*h > largest_area:
				largest_area = w * h
				largest = (x, y, w, h)
		data = angle_and_dist(largest)
		print "1::" + str(math.degrees(data[0])) + "::" + str(data[1])
	else:
		print "0::0::0"

	if gui:
		cv2.rectangle(src, (largest[0], largest[1]), (largest[0]+largest[2], largest[1]+largest[3]), (0, 255, 0), 2)
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

	while True:
		processImage(getImage())
		if cv2.waitKey(1):
			break
	# Create the server, binding to localhost on port 9999
	server = SocketServer.TCPServer((HOST, PORT), MyTCPHandler)

	# Activate the server this will keep running until you
	# interrupt the program with Ctrl-C
	#server.serve_forever()

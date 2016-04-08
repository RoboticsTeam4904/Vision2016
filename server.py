from __future__ import division #Might take time to import
import SocketServer, subprocess, time, cv2, math
import numpy as np

from flask import Flask
app = Flask(__name__)

pi = False
gui = True
webcam = False

if pi:
	from picamera.array import PiRGBArray
	from picamera import PiCamera

def nothing(x):
	pass

h,s,v = 64,25,43

cv2.imshow("result", cv2.imread("a00071.jpg"))
cv2.namedWindow('result', cv2.WINDOW_NORMAL)
cv2.createTrackbar('h', 'result',0,179,nothing)
cv2.createTrackbar('s', 'result',0,255,nothing)
cv2.createTrackbar('v', 'result',0,255,nothing)

if pi:
	# initialize the camera and grab a reference to the raw camera capture
	camera = PiCamera()
	camera.resolution = (640, 480)
	camera.framerate = 15
	rawCapture = PiRGBArray(camera, size=camera.resolution)
	#camera.start_preview()
	camera.exposure_mode = 'sports'

if not pi and webcam:
	cap = cv2.VideoCapture(0)
	cap.set(3,640)
	cap.set(4,480)

# constants
cameraResolution = (640, 480)
nativeResolution = (2592, 1944)
nativeAngle = (math.radians(53.5), math.radians(41.41))
mountAngle = (0, math.radians(45))
cameraToShooterDist = (13.25, 2.5)
goalHeight = 8 * 12
cameraHeight = 296 / 25.4 #to inches

def getImage():
	image = None

	if pi:
		
			# grab the raw NumPy array representing the image, then initialize the timestamp
			# and occupied/unoccupied text
			image = frame.array

			# clear the stream in preparation for the next frame
			rawCapture.truncate(0)
	elif webcam:
		ret, image = cap.read()
		#cv2.imwrite("/Users/erik/"+str(time.time())+".jpg", image)
	else:
		image = cv2.imread("a00071.jpg")

	return image
def angle_and_dist(goal):
	# [0] = X, [1] = Y, goal[i] = ith corner of highgoal
	degPerPxl = [nativeAngle[i] / cameraResolution[i] for i in range(2)]
	#print goal

	x = 0
	y = 0
	count = 0
	for i in goal:
		#print i
		x += i[0][0]
		y += i[0][1]
		count += 1
	x /= count
	y /= count

	centerOfGoalPixelCoords = (x, cameraResolution[1] - y)
	print centerOfGoalPixelCoords
	
	
	goalAngle = [mountAngle[i] + degPerPxl[i] * (centerOfGoalPixelCoords[i] - cameraResolution[i] / 2) for i in range(2)]
	goalAngleLeftToRight = goalAngle[0]
	goalAngleUpAndDown = goalAngle[1]
	cameraToGoalDistance = (goalHeight - cameraHeight) / math.tan(goalAngleUpAndDown)
	cameraToGoalX = math.sin(goalAngleLeftToRight) * cameraToGoalDistance
	cameraToGoalY = math.cos(goalAngleLeftToRight) * cameraToGoalDistance
	shooterToGoalX = cameraToGoalX - cameraToShooterDist[0]
	shooterToGoalY = cameraToGoalY + cameraToShooterDist[1]
	shooterToGoalDist = math.sqrt(shooterToGoalX * shooterToGoalX + shooterToGoalY * shooterToGoalY)
	shooterToGoalAngle = math.atan(shooterToGoalX / shooterToGoalY)
	return (shooterToGoalAngle, shooterToGoalDist)

def processImage(src):
	thresholdValue = 200
	max_thresh = 255
	blob_size = 3

	# get info from track bar and apply to result
	h = cv2.getTrackbarPos('h','result')
	s = cv2.getTrackbarPos('s','result')
	v = cv2.getTrackbarPos('v','result')
	print h,s,v
	lower_green = np.array([h,s,v])
	upper_green = np.array([120, 255, 255])


	blurred = cv2.blur(src, (3, 3))
	hsv = cv2.cvtColor(blurred, cv2.COLOR_BGR2HSV)
	thresholded = cv2.inRange(hsv, lower_green, upper_green)
	thresholded = cv2.bitwise_not(thresholded)
	

	# grayscale = cv2.cvtColor(src, cv2.COLOR_BGR2GRAY)    #TODO: change to stripping just reds or something compute-easy convert image to black and white
	# blurred = cv2.blur(grayscale, (3, 3))    # blur image
	# ret, thresholded = cv2.threshold(blurred, thresholdValue, max_thresh, cv2.THRESH_BINARY)


	contours, hierarchy = cv2.findContours(thresholded, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

	if gui:
		# cv2.drawContours(grayscale, contours, -1, (0,255,0), 3)
		# cv2.imshow("grayscaleafter", grayscale)
		cv2.imshow("thresholded", thresholded)

	thresh_filled = np.zeros(thresholded.shape, dtype=np.uint8)
	thick_thresh = np.zeros(thresholded.shape, dtype=np.uint8)
	cv2.drawContours(thresh_filled, contours, -1, (255,255,255), cv2.cv.CV_FILLED, 8)
	cv2.drawContours(thick_thresh, contours, -1, (255,255,255), 3)

	hull = [cv2.convexHull(contour, False) for contour in contours]
	convex = np.zeros(thresholded.shape, dtype=np.uint8)
	cv2.drawContours(convex, hull, -1, (255,255,255), cv2.cv.CV_FILLED, 8)
	subtracted = cv2.bitwise_and(cv2.bitwise_and(convex, cv2.bitwise_not(thresh_filled)), cv2.bitwise_not(thick_thresh))

	if gui:
		#cv2.imshow("convex", convex)
		pass#cv2.imshow("subtracted", subtracted)

	# blob callback
	# blobbed = np.zeros(subtracted.shape, dtype=np.uint8)
	# element = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (2 * blob_size + 1, 2 * blob_size + 1), (blob_size, blob_size))
	# print element
	# cv2.erode(subtracted, blobbed, element)
	# if gui:
	#     cv2.imshow("blobbedBeforeDilate", blobbed)
	# cv2.dilate(blobbed, blobbed, element)
	#
	# if gui:
	#     cv2.imshow("blobbed", blobbed)

	contours, hierarchy = cv2.findContours(subtracted, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

	if len(contours) > 0:
		# Find largest contour.
		largest_contour = contours[0]
		largest_area = cv2.contourArea(contours[0], False)
		for i in range(1, len(contours)):
			temp_area = cv2.contourArea(contours[i], False)
			if (temp_area > largest_area):
				largest_contour = contours[i]
				largest_area = temp_area
		eps=2
		goal = cv2.approxPolyDP(largest_contour, eps, True)
		if len(goal)>4:
			while eps<7:
				eps=eps+1
				goal = cv2.approxPolyDP(largest_contour, eps, True)
				if len(goal)==4:
					break

		data = angle_and_dist(goal)
		print "1::" + str(math.degrees(data[0])) + "::" + str(data[1])
		if gui:
			for contour in contours:
				for i in range(len(contour)):
					#print goal[i][0]
					cv2.line(src, (contour[i][0][0], contour[i][0][1]), (contour[(i+1)%len(contour)][0][0], contour[(i+1)%len(contour)][0][1]), (255, 0, 0), eps*2)
	
	else:
		print "0::0::0"
	
	if gui:
		cv2.imshow("result", src)

	#if gui:
	#	cv2.waitKey(0)
	#	cv2.destroyAllWindows()

	return 0
@app.route('/autonomous')
def autonomous():
	return processImage(getImage())

@app.route('/')
def autonomous2():
	return processImage(getImage())


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
	if gui:
		if pi:
			for frame in camera.capture_continuous(rawCapture, format="bgr", use_video_port=True):
				processImage(getImage())
				if cv2.waitKey(1) != -1:
					break
		else:
			while True:
				processImage(getImage())
				if cv2.waitKey(200) != -1:
					cap.release()
					cv2.destroyAllWindows()
					break
	else:
		app.run(host=HOST,port=PORT)
		# socket server
		#server = SocketServer.TCPServer((HOST, PORT), MyTCPHandler)
		#server.serve_forever()

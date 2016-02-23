import SocketServer
import subprocess
# currentImage=subprocess.Popen("latest")
while (True): #while (currentImage!==subprocess.Popen("latest"))
    # currentImage=subprocess.Popen("latest")
    process = subprocess.Popen(["./highgoal.bin", "latest"], stdout=subprocess.PIPE)
    response = process.stdout.read()
    test = open("test.txt","w+")
    test.write(response)
# I'm not sure how to actually load the image and save it to currentImage, and if we can compare to images with the !==, but I might be able to write a really quick algorithm to compare them (another opencv file), and shaheen can figure out the loading image part

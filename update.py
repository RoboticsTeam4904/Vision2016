import SocketServer
import subprocess
while (True): #make a temp var of the latest image, then say while tempvar!==latest, do that stuff
    process = subprocess.Popen(["./highgoal.bin", "latest"], stdout=subprocess.PIPE)
    response = process.stdout.read()
    test = open("test.txt","w+")
    test.write(response)

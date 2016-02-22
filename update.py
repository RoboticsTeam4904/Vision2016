import SocketServer
import subprocess
while (True):
    process = subprocess.Popen(["./highgoal.bin", "latest"], stdout=subprocess.PIPE)
    response = process.stdout.read()
    test = open("test.txt","w+")
    test.write(response)

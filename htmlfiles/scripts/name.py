import sys
import signal
TIMEOUT = 1 # seconds
signal.signal(signal.SIGALRM, input)
signal.alarm(TIMEOUT)

try:
    for line in sys.stdin:
        print(line)
except:
    ignorar = True


print("Bienvenido ", end="")
for arg in sys.argv:
	if arg!=sys.argv[0]:
		print(arg, end=" ")
print(" !!!", end="")

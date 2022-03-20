import sys
import signal

from numpy import double
TIMEOUT = 1 # seconds
signal.signal(signal.SIGALRM, input)
signal.alarm(TIMEOUT)

try:
    for line in sys.stdin:
        print(line)
except:
    ignorar = True


if len(sys.argv) !=2:
    print("EL VALOR DEBE DE SER MAYOR O IGUAL QUE 0")
else:
    
    valor = float(sys.argv[1])
    if valor>=0:

   
        dolar=valor*(1.11)
        print(valor, "euros son", round(dolar, 4), "USD")

    else:
        print("EL VALOR DEBE DE SER MAYOR O IGUAL QUE 0")


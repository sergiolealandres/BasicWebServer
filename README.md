# practica1

Práctica 1 Asignatura Redes de Comunicación II, Pareja 6
Autores: Pablo Soto y Sergio Leal

La wiki con toda la documentación de la se encuentra [aquí](https://git.eps.uam.es/redes2/2022/2301/06/practica1/-/wikis/Introducci%C3%B3n).

Para ejecutar el servidor hacer make. El ejecutable se llama servidor, para ejecutar llamar a ./servidor.

Si el servidor no está demonizado, el servidor se cierra enviando la señal SIGINT (Ctrl + C en la terminal).

Si el servidor está demonizado, para cerrar el servidor hay que mandarle la señal SIGINT de nuevo. Para ello, tal y como se explica en
la [wiki](https://git.eps.uam.es/redes2/2022/2301/06/practica1/-/wikis/Implementaciones-adicionales), Una forma de enviar SIGINT al 
servidor para cerrarlo de forma correcta es mediante la siguiente secuencia de comandos desde la terminal: _pgrep servidor_, que retorna
el pid del proceso . Una vez conocido este servidor se ejecuta el comando kill -2 pid, siendo pid el id del proceso retornado por el comando
anterior.

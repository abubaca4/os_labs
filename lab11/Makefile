all:
	gcc udp_cilent.c -o udp_cilent
	gcc udp_serv.c -o udp_serv
	./udp_serv 32615 &
	sleep 3
	./udp_cilent 127.0.0.1:32615
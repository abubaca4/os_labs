all:
	gcc udp_serv.c -o udp_serv
	gcc udp_cilent.c -o udp_cilent
	./udp_serv 32615 &
	sleep 1
	./udp_cilent 127.0.0.1:32615
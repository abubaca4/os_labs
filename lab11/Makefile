all:
	gcc udp_serv.c -o udp_serv
	gcc udp_cilent.c -o udp_cilent
	./udp_serv 32615 &
	echo " 10000 \n -1000 \n 1000 \n" > input.txt
	sleep 1
	./udp_cilent 127.0.0.1:32615 < input.txt
	rm input.txt
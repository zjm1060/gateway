#!/bin/sh

instance=$1

var=`dtctl config.network.gprs[$instance].power`
pwrctl=${var:-0}
var=`dtctl config.network.gprs[$instance].apn`
apn=${var:-'cmnet'}
user=`dtctl config.network.gprs[$instance].user`
passwd=`dtctl config.network.gprs[$instance].passwd`
var=`dtctl config.network.gprs[$instance].port`
port=${var:-'/dev/null'}

PPPID=0
MUXID=0
CIMI="000000000000"

echo "pwrctl:$pwrctl"
echo "apn:$apn"
echo "user:$user"
echo "passwd:$passwd"
echo "port:$port"

echo $pwrctl > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio$pwrctl/direction

PPP_OPTIOS="115200 noauth nodetach nocrtscts noipdefault usepeerdns "

#echo pppd $port $PPP_OPTIOS unit $instance user '$user' password '$passwd' connect "chat -v -f /tmp/connect.$instance" 

set_power()
{
	echo $1 > /sys/class/gpio/gpio$pwrctl/value
}

check_module()
{
	ASK=`echo -e "at+creg?\r" | microcom /dev/mux$instance.0 -s 115200 -t 200 | grep "+CREG" |  awk -F ',' '{print $2}'`
	# echo "ASK:$ASK"
	if [ "$ASK" -eq 1 ] || [ "$ASK" -eq 5 ]; then
		return 1
	fi
	return 0
}

start_mux()
{
	echo "start gsmmux"
	gsmmux -p $1 -b 115200  -w -r -s /dev/mux$instance. /dev/ptmx /dev/ptmx /dev/ptmx
	
	MUXID=`ps | grep "[g]smmux -p $1" | awk '{print $1}'`
	
	echo $MUXID
}

get_cimi()
{
	CIMI=`echo -e "at+cimi\r" | microcom /dev/mux$instance.1 -s 115200 -t 200 | awk '/[0-9]/{printf $1}'`
	
	echo $CIMI > /tmp/sim.$instance
}

get_signal()
{
	SIG=`echo -e "at+csq\r" | microcom /dev/mux$instance.1 -s 115200 -t 200 | grep "[0-9]" |  awk -F '[:,]' '{print $2}'`
	
	echo "$CIMI,$SIG" > /tmp/sim.$instance
}

start_pppd()
{
	echo "start ppp..."
	pppd /dev/mux$instance.0 $PPP_OPTIOS unit $instance user "$user" password "$passwd" connect "chat -v -f /tmp/connect.$instance" &

	PPPID=$!
	return
}

prepare_ppp()
{
# var=`dtctl config.network.gprs[$instance].apn`
# apn=${var:-'cmnet'}
cat <<EOT > /tmp/connect.$instance
TIMEOUT 5
ABORT 'BUSY'
ABORT 'NO CARRIER'
ABORT 'NO ANSWER'
ABORT 'ERROR'
ABORT '+CME ERROR: 100'
''   AT
OK   AT+CGDCONT=1,"IP","$apn"
OK   ATD*99***1#
CONNECT
EOT

}

check_ppp()
{
	ppp=`ifconfig | grep ppp$instance`

	if [ "$ppp" ]; then
		return 1
	fi

	return 0
}

doPPP()
{
	failure=0
	while [[ true ]]; do
		prepare_ppp

		start_pppd

		while [[ true ]]; do
			sleep 5

			# get_signal

			check_ppp

			if [ $? -eq 0 ]; then
				break
			fi
		done

		echo "pppd.$instance is off!"

		return ;

		sleep 10

		let failure++

		if [[ $failure = 3 ]]; then
			return
		fi

	done

}

while [[ true ]]; do
	# stop_gsmMuxd
	echo "(Re)start ppp.$instance..."

	var=`dtctl config.network.gprs[$instance].apn`
	apn=${var:-'cmnet'}
	user=`dtctl config.network.gprs[$instance].user`
	passwd=`dtctl config.network.gprs[$instance].passwd`

	set_power 0
	sleep 1
	set_power 1
	sleep 3

	failure=0
	
	start_mux $port

	while [[ true ]]; do
		check_module

		# echo "ASK:$?"
		if [ $? -eq 1 ]; then
			get_cimi 
			doPPP
			break
		fi

		sleep 5

		let failure++

		if [[ $failure = 5 ]]; then
			echo "module not finded..."
			break
		fi

	done

	if [ $PPPID != 0 ]; then
		kill -9 $PPPID
	fi
	
	if [ $MUXID != 0 ]; then
		kill $MUXID
	fi

	sleep 3
done

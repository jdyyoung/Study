#!/bin/bash

user_pwd="\[Sugrman\]"
target_ip=
pc_dir=
target_dir=
cmd_flag=0
ip_flag=0
scp_flag=0

function show_usage(){
	echo "+---------------------------------------------------------------------+"
	echo "|    usage: ./ssh_tool.sh [option]                                    |"
	echo "|           [option] is following options :                           |"
	echo "|                                                                     |"
	echo "|           ssh :                                                     |"
	echo "|           ./ssh_tool.sh <target_ip>                                 |"
	echo "|                                                                     |"
	echo "|           scp pc to target :                                        |"
	echo "|           ./ssh_tool.sh <target_ip> <pc_dir> <target_dir>           |"
	echo "|           ./ssh_tool.sh <target_dir> <pc_dir> <target_ip>           |"
	echo "|                                                                     |"
	echo "|           scp target to pc :                                        |"
	echo "|           ./ssh_tool.sh <target_ip> <target_dir> <pc_dir>           |"
	echo "|           ./ssh_tool.sh <pc_dir> <target_dir> <target_ip>           |"
	echo "|                                                                     |"
	echo "|           <target_ip>      :config target_ip only last num          |"
	echo "|           <pc_dir>         :config pc_dir                           |"
	echo "|           <target_dir>     :config target_dir                       |"
	echo "|                                                                     |"
	echo "|    Note: your current config is this                                |"
	echo "|          target_ip=192.168.1.X (you should add last num)            |"
	echo "|                                                                     |"
	echo "+---------------------------------------------------------------------+"
}

function judge_ip(){
	local tmp_ip=$1
	local is_number=`echo $tmp_ip | sed 's/[0-9]//g'`
	if [ -n "$is_number" ];then
		ip_flag=0
	else
		local last_ip=$((tmp_ip))
		if [ 256 -gt $last_ip ] && [ $last_ip -gt 0 ];then	
			target_ip="192.168.1.""$1"
			ip_flag=1
		else
			ip_flag=0
		fi
	fi
}

function judge_file(){
	local first_file=$1
	local second_file=$2
	if [ -f $first_file ];then
		pc_dir=$first_file
		target_dir=$second_file
		scp_flag=1
	else
		if [ -d $second_file ] || [ -f $second_file ];then
			target_dir=$first_file
			pc_dir=$second_file
			scp_flag=2
		else
			scp_flag=0
		fi
	fi
}

if [ $# -eq 0 ];then
	show_usage
	exit 1
elif [ $# -eq 1 ];then
	judge_ip $1
	if [ $ip_flag -eq 1 ];then	
		cmd_flag=1
	else
		show_usage
		exit 1
	fi	
elif [ $# -eq 3 ];then
	judge_ip $1
	if [ $ip_flag -eq 1 ];then	
		judge_file $2 $3
		if [ $scp_flag -eq 0 ];then
			show_usage 
			exit 1
		fi
	else
		judge_ip $3
		if [ $ip_flag -eq 1 ];then	
			judge_file $1 $2
			if [ $scp_flag -eq 0 ];then
				show_usage 
				exit 1
			fi
		else
			show_usage
			exit 1
		fi
	fi
fi

if [ $cmd_flag = 1 ]; then
	echo "spawn ssh root@$target_ip"
	#ssh -p 14401 root@$target_ip 
expect -c "
set timeout 5
spawn ssh -p 14401 -oStrictHostKeyChecking=no root@${target_ip} 
expect \"password:\"
send \"${user_pwd}\r\"
expect eof
interact
"

else
	if [ $scp_flag -eq 1 ];then
	#scp -P 14401 $pc_dir root@$target_ip:$target_dir
expect -c "
set timeout 300
spawn scp -P 14401 -oStrictHostKeyChecking=no $pc_dir root@${target_ip}:$target_dir
expect \"password:\"
send \"${user_pwd}\r\"
expect eof
"
	elif [ $scp_flag -eq 2 ];then
	#scp -P 14401 root@$target_ip:$target_dir $pc_dir
expect -c "
set timeout 300
spawn scp -P 14401 -oStrictHostKeyChecking=no root@${target_ip}:$target_dir $pc_dir
expect \"password:\"
send \"${user_pwd}\r\"
expect eof
"
	
	fi
	if [ $? != 0 ]; then
		echo "################################################[ scp fail!!! ]#############################################"
		exit 1
	fi  
fi

echo ""
echo "###############################################[ Done. ]#############################################"
echo ""

# C test exercise

## Task
C program that will run in an OpenWRT environment and have the following functions:
1. Ubus : program is connected to ubus  and implemented some request handler function;
2. UCI: saving the period parameter to the config file;
3. MQTT: program has connection to MQTT server and with period period from the previous points send data about its state (cpu, memory) to some topik (MQTT server and topik are configured also from uci)
4. use multithreading

## How to run 

Use Vagrant with virtualbox as the provider with all the hosts that we want to target (Linux, Mac\*, Windows). 

 1. Install [Virtualbox](https://www.virtualbox.org/wiki/Downloads) (tested with 6.1.40 r154048)
 2. Install [Vagrant](https://www.vagrantup.com/downloads) (tested with 2.3.4)

Then run:

```bash
# go to 'test_exercise' folder
cd env
./connect.sh -m $PROJECT_PATH
```

Where:

* The -m options are only needed for the first time, then they're cached (`PROJECT_PATH` - full path to your test_exercise)
In my case:
```bash
./connect.sh -m /home/vanaluk/Work/test_exercise/
```

If the `-m` parameter is skipped the provisioner will take no action. You will not have a firmware directory linked to your virtual machine. This value is cached in `.cache`.

* I've given the virtual machine 4 cores and 4 GB of memory. 
* This will use lots of download data, you've been warned! This takes 30 minutes on my machine.

Once that completes you will be dropped into a terminal within the vagrant box (ssh connection to virtualbox machine). The first thing you need to do is reboot. 

Close the open ssh session that we landed inafter the vagrant install:

```bash
exit
```

Turn off the machine

```bash
# stop the virtual machine, run from inside the env folder
./connect.sh -s
```

Start up the virtual machine again using:

```bash
./connect.sh
```

`./connect.sh` to start and/or connect to your box using cash dir.

`./connect.sh -s` shut down your virtual machine call.

`./connect.sh -d` erase the cache and destroy the vagrant machine.

You can now take a look around. The project folder is mounted to `~/test_exercise`. These are the same folders as those on your host machine. The build commands will need to be run from the VM.

# Build test exercise

```bash
cd ~/test_exercise/build
make all
```

This will generate `wimark_test_exercise` execute file

# Run test exercise

1. Run test_exercise app

```bash
sudo ubusd &
sudo ./wimark_test_exercise
```

This command run `ubus` and `wimark_test_exercise` with default properties on the virtual machine:
```
config wimark_config 'wimark_test_exercise'
	option enabled '1'
	option server 'localhost'
	option topic '/my/secret/topic'
	option period '60'
```
`option enabled '1'` - means mqtt client will report CPU load each `period '60'` seconds.
If `option enabled` set `0`, then mqtt client will not report. 

2. Connect to virtual machine using other ssh terminal and subscripe to mosquitto topic:

```bash
./connect.sh -c 'mosquitto_sub -t "/my/secret/topic"'
```

* messages like this should appear:
```
CPU: 2.638071%
CPU: 1.446801%
```

3. Connect to virtual machine using other ssh terminal

```bash
./connect.sh 
```

* Check wimark-ubus status:
```bash
sudo ubus call wimark-ubus status
```

* Change period report value using ubus:
```bash
sudo ubus call wimark-ubus add '{"period" : "45"}'
```


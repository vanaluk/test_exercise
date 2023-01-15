# Firmware Virtual Box Dev Environment

Use Vagrant with virtualbox as the provider with all the hosts that we want to target (Linux, Mac\*, Windows). 

 1. Install [Virtualbox](https://www.virtualbox.org/wiki/Downloads) (tested with 6.1.34 r150636)
 2. Install [Vagrant](https://www.vagrantup.com/downloads) (tested with 2.2.19)

Then run:

```bash
# you should already be in 'test_exercise' folder
cd env
./connect.sh -m $PROJECT_PATH -k $KEY_DIR
```

Where:

* the -k and -m options are only needed for the first time, then they're cached (`PROJECT_PATH` - full path to your test_exercise)
In my case:
```bash
./connect.sh -m /home/vanaluk/Work/test_exercise/ -k /home/vanaluk/Work/test_exercise/key
```

If the `-m` parameter is skipped the provisioner will take no action. You will not have a firmware directory linked to your virtual machine. This value is cached in `.cache`.

If the `-k` parameter is skipped the provisioner will take no action. You will not have a keys directory linked to your virtual machine. This value is cached in `.keys_cache`.

* I've given the virtual machine 2 cores and 4 GB of memory. 
* This will use lots of download data, you've been warned!
* At the moment I don't think the keys are properly used, I've been using the unity keys folder for now since Io doesn't have its own set yet. If you don't provide it, it simply won't be mounted for you.
* Grab a coffee and come back maybe? This takes 10 minutes on my machine.

Once that completes you will be dropped into a terminal within the vagrant box (ssh connection to virtualbox machine). The first thing you need to do is reboot. We also have some manual configs to set in the virtualbox GUI that requires the vm be turned off. Lets do both quickly.

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

Once the paths are cached you can simply call `./connect.sh` to start and/or connect to your box.

If you'd like to shut down your virtual machine call `./connect.sh -s`.

If for some reason you need to start from scratch `./connect.sh -d` will erase the cache and destroy the vagrant machine.

You can now take a look around. The project folder is mounted to `~/test_exercise` and the keys folder to `~/keys`. These are the same folders as those on your host machine. You can use your favorite editor(s) or IDE(s) in either the host or vm. The build commands will need to be run from the VM.

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

2. Connect to virtual machine using other ssh terminal:

* Subscripe to mosquitto topic:
```bash
./connect.sh -c 'mosquitto_sub -t "/my/secret/topic"'
```

* messages like this should appear:
```
CPU: 2.638071%
CPU: 1.446801%
```

3. Connect to virtual machine using other ssh terminal (the same as step 2)

* Check wimark-ubus status:
```bash
./connect.sh -c 'sudo ubus call wimark-ubus status'
```

* Change period report value using ubus:
```bash
./connect.sh 
sudo ubus call wimark-ubus add '{"period" : "45"}'
```


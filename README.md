
# rpmsg-test

Example how to benchmark rpmsg endpoints. Linux part. SHARC firmware is provided as seperate repository that contains CCES studio project.  



## Compile

There is no extra dependencies that required. 

```bash
make
```

## Load firmwar for SHARC

```
cd /lib/firmware
echo stop > /sys/class/remoteproc/remoteproc0/state
echo adi_adsp_core1_fw.ldr > /sys/class/remoteproc/remoteproc0/firmware
echo start > /sys/class/remoteproc/remoteproc0/state
```

```
root@adsp-sc598-som-ezkit:/lib/firmware# echo stop > /sys/class/remoteproc/remoteproc0/state
[  258.594698] ADI Reset Control Unit 3108c000.rcu: Timeout waiting for remote core 1 to IDLE!
[  258.603149] remoteproc remoteproc0: stopped remote processor core1-rproc
root@adsp-sc598-som-ezkit:/lib/firmware# ls
adi_adsp_core1_fw.ldr  adi_adsp_core2_fw.ldr
root@adsp-sc598-som-ezkit:/lib/firmware# echo adi_adsp_core1_fw.ldr  >  /sys/class/remoteproc/remoteproc0/firmware
root@adsp-sc598-som-ezkit:/lib/firmware# echo start > /sys/class/remoteproc/remoteproc0/state
[  309.727098] remoteproc remoteproc0: powering up core1-rproc
[  309.744737] remoteproc remoteproc0: Booting fw image adi_adsp_core1_fw.ldr, size 52944
[  309.794974] virtio_rpmsg_bus virtio0: creating channel sharc-echo addr 0x120
[  309.802109] virtio_rpmsg_bus virtio0: creating channel sharc-echo-cap addr 0x160
[  309.809471] virtio_rpmsg_bus virtio0: creating channel sharc-echo addr 0x121
[  309.816463] virtio_rpmsg_bus virtio0: creating channel sharc-echo-cap addr 0x161
[  309.823842] virtio_rpmsg_bus virtio0: creating channel sharc-echo addr 0x122
[  309.830869] virtio_rpmsg_bus virtio0: creating channel sharc-echo-cap addr 0x162
[  309.838256] virtio_rpmsg_bus virtio0: creating channel sharc-echo addr 0x123
[  309.845279] virtio_rpmsg_bus virtio0: creating channel sharc-echo-cap addr 0x163
[  309.852661] virtio_rpmsg_bus virtio0: creating channel sharc-echo addr 0x124
[  309.859690] virtio_rpmsg_bus virtio0: creating channel sharc-echo-cap addr 0x164
[  309.867091] virtio_rpmsg_bus virtio0: creating channel sharc-echo addr 0x125
[  309.874107] virtio_rpmsg_bus virtio0: creating channel sharc-echo-cap addr 0x165
[  309.881484] virtio_rpmsg_bus virtio0: creating channel sharc-echo addr 0x126
[  309.888532] virtio_rpmsg_bus virtio0: creating channel sharc-echo-cap addr 0x166
[  309.895900] virtio_rpmsg_bus virtio0: creating channel sharc-echo addr 0x127
[  309.902926] virtio_rpmsg_bus virtio0: creating channel sharc-echo-cap addr 0x167
[  309.910301] virtio_rpmsg_bus virtio0: creating channel sharc-echo addr 0x128
[  309.917332] virtio_rpmsg_bus virtio0: creating channel sharc-echo-cap addr 0x168
[  309.924727] virtio_rpmsg_bus virtio0: creating channel sharc-echo addr 0x129
[  309.931743] virtio_rpmsg_bus virtio0: creating channel sharc-echo-cap addr 0x169
[  309.939121] virtio_rpmsg_bus virtio0: creating channel sharc-echo addr 0x12a
[  309.946153] virtio_rpmsg_bus virtio0: creating channel sharc-echo-cap addr 0x16a
[  309.953547] virtio_rpmsg_bus virtio0: creating channel sharc-echo addr 0x12b
[  309.960565] virtio_rpmsg_bus virtio0: creating channel sharc-echo-cap addr 0x16b
[  309.967942] virtio_rpmsg_bus virtio0: creating channel sharc-echo addr 0x12c
[  309.974977] virtio_rpmsg_bus virtio0: creating channel sharc-echo-cap addr 0x16c
[  309.982370] virtio_rpmsg_bus virtio0: creating channel sharc-echo addr 0x12d
[  309.989390] virtio_rpmsg_bus virtio0: creating channel sharc-echo-cap addr 0x16d
[  309.996762] virtio_rpmsg_bus virtio0: creating channel sharc-echo addr 0x12e
[  310.003793] virtio_rpmsg_bus virtio0: creating channel sharc-echo-cap addr 0x16e
[  310.011184] virtio_rpmsg_bus virtio0: creating channel sharc-echo addr 0x12f
[  310.018214] virtio_rpmsg_bus virtio0: creating channel sharc-echo-cap addr 0x16f
[  310.025587] virtio_rpmsg_bus virtio0: creating channel sharc-echo addr 0x130
[  310.032615] virtio_rpmsg_bus virtio0: creating channel sharc-echo-cap addr 0x170
[  310.040006] virtio_rpmsg_bus virtio0: creating channel sharc-echo addr 0x131
[  310.047024] virtio_rpmsg_bus virtio0: creating channel sharc-echo-cap addr 0x171
[  310.054421] virtio_rpmsg_bus virtio0: creating channel sharc-echo addr 0x132
[  310.061443] virtio_rpmsg_bus virtio0: creating channel sharc-echo-cap addr 0x172
[  310.068835] virtio_rpmsg_bus virtio0: creating channel sharc-echo addr 0x133
[  310.075855] virtio_rpmsg_bus virtio0: creating channel sharc-echo-cap addr 0x173
[  310.083229] virtio_rpmsg_bus virtio0: creating channel sharc-echo addr 0x134
[  310.090261] virtio_rpmsg_bus virtio0: creating channel sharc-echo-cap addr 0x174
[  310.097649] virtio_rpmsg_bus virtio0: creating channel sharc-echo addr 0x135
[  310.104672] virtio_rpmsg_bus virtio0: creating channel sharc-echo-cap addr 0x175
[  310.112051] virtio_rpmsg_bus virtio0: creating channel sharc-echo addr 0x136
[  310.119079] virtio_rpmsg_bus virtio0: creating channel sharc-echo-cap addr 0x176
[  310.126471] virtio_rpmsg_bus virtio0: creating channel sharc-echo addr 0x137
[  310.133503] virtio_rpmsg_bus virtio0: creating channel sharc-echo-cap addr 0x177
[  310.140876] virtio_rpmsg_bus virtio0: creating channel sharc-echo addr 0x138
[  310.147904] virtio_rpmsg_bus virtio0: creating channel sharc-echo-cap addr 0x178
[  310.155305] virtio_rpmsg_bus virtio0: creating channel sharc-echo addr 0x139
[  310.162323] virtio_rpmsg_bus virtio0: creating channel sharc-echo-cap addr 0x179
[  310.169697] virtio_rpmsg_bus virtio0: creating channel sharc-echo addr 0x13a
[  310.176726] virtio_rpmsg_bus virtio0: creating channel sharc-echo-cap addr 0x17a
[  310.184118] virtio_rpmsg_bus virtio0: creating channel sharc-echo addr 0x13b
[  310.191148] virtio_rpmsg_bus virtio0: creating channel sharc-echo-cap addr 0x17b
[  310.198517] virtio_rpmsg_bus virtio0: creating channel sharc-echo addr 0x13c
[  310.205550] virtio_rpmsg_bus virtio0: creating channel sharc-echo-cap addr 0x17c
[  310.212944] virtio_rpmsg_bus virtio0: creating channel sharc-echo addr 0x13d
[  310.219993] virtio_rpmsg_bus virtio0: creating channel sharc-echo-cap addr 0x17d
[  310.227360] virtio_rpmsg_bus virtio0: creating channel sharc-echo addr 0x13e
[  310.234392] virtio_rpmsg_bus virtio0: creating channel sharc-echo-cap addr 0x17e
[  310.241791] virtio_rpmsg_bus virtio0: creating channel sharc-echo addr 0x13f
[  310.248821] virtio_rpmsg_bus virtio0: creating channel sharc-echo-cap addr 0x17f
[  310.257936] systemd-journald[219]: /dev/kmsg buffer overrun, some messages lost.
[  310.265405] virtio_rpmsg_bus virtio0: rpmsg host is online
[  310.274996]  remoteproc0#vdev0buffer: registered virtio0 (type 7)
[  310.294910] remoteproc remoteproc0: remote processor core1-rproc is now up
```

## rpmsg-bind-chardev

### List of rpmgsg devices 

```
ls /sys/bus/rpmsg/devices/
```

```
virtio0.rpmsg_chrdev.0.0/      virtio0.sharc-echo-cap.-1.161/ virtio1.rpmsg_chrdev.0.0/      virtio1.sharc-echo-cap.-1.162/ 
virtio0.rpmsg_ns.53.53/        virtio0.sharc-echo.-1.151/     virtio1.rpmsg_ns.53.53/        virtio1.sharc-echo.-1.152/
```

### How to use

```bash
rpmsg-bind-chardev -p virtio0.sharc-echo-cap.-1. -n 32 -e 352 -s 61
rpmsg-bind-chardev -p virtio0.sharc-echo.-1. -n 32 -e 288 -s 61
```

List created devices in /dev

```
ls /dev/rpmsg*
ls /dev/rpmsg_ctrl*
```

## rpmsg-xmit-p

### How to use

```bash
rpmsg-xmit -p /dev/rpmsg -n 32 -e 0 -t 1000
```


```
ls /dev/rpmsg*
```


### Test single echo-cap endpoint

#### Bind channels

Bind 1 channel

```
rpmsg-bind-chardev -p virtio0.sharc-echo.-1. -n 1 -e 288 -s 100
```

Bind 8 channel

```
rpmsg-bind-chardev -p virtio0.sharc-echo.-1. -n 8 -e 288 -s 100
```

Bind 32 channel

```
rpmsg-bind-chardev -p virtio0.sharc-echo.-1. -n 32 -e 288 -s 100
```


#### Test multi channels 

Run 1 channel
```
rpmsg-xmit -p /dev/rpmsg -n 1 -e 0 -t 1000000
```

Run 8 channel
```
rpmsg-xmit -p /dev/rpmsg -n 8 -e 0 -t 1000000
```

Run 32 channel 
```
rpmsg-xmit -p /dev/rpmsg -n 32 -e 0 -t 1000000
```


#### Test different total transfer sizes

Run 10k transfers
```
rpmsg-xmit -p /dev/rpmsg -n 1 -e 0 -t 10000
```

Run 100k transfers
```
rpmsg-xmit -p /dev/rpmsg -n 1 -e 0 -t 100000
```

Run 1M transfers
```
rpmsg-xmit -p /dev/rpmsg -n 1 -e 0 -t 1000000
```


#### Test different packet size transfers

Set packet size 1
```
rpmsg-xmit -p /dev/rpmsg -n 1 -e 0 -t 1000000 -s 1
```

Set packet size 8
```
rpmsg-xmit -p /dev/rpmsg -n 1 -e 0 -t 1000000 -s 8
```

Set packet size 128
```
rpmsg-xmit -p /dev/rpmsg -n 1 -e 0 -t 1000000 -s 128
```

Set packet size 496
```
rpmsg-xmit -p /dev/rpmsg -n 1 -e 0 -t 1000000 -s 496
```


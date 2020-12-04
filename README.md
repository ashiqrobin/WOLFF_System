# WOLFF Project

WOLFF is a LoRa based data communication system. Contributors of this codebase are Rishabh Mahajani, Bill Eason.  

**1. Loopback**

This section describes the steps required to complete a LoRa loopback
which will enable us to measure roundtrip times for our LoRa pathway.
The parts required to complete the loopback are the gateway, 2 nodes, 3
ESP devices, and all the USB cables to connect these devices.

*1.1 Gateway Setup*

Before we flash all our devices with code we need to setup our gateway.
To be able to reach the initial gateway GUI page refer to the two videos
below:

<https://www.youtube.com/watch?v=Q_25lZWcSWI>

<https://www.youtube.com/watch?v=H18pVF8qIaY&t=577s>

Once you have successfully reached the GUI navigate to LoRaWAN \>
Network Settings. Make sure that your page looks like the picture shown
below:

![Alt text](images/lora-gateway-settings.PNG?raw=true "Title")

Keep in mind that you may have to manually turn on the LoRa network
server. In this case open an ssh session for your gateway, navigate to
the /etc/init.d directory and then type ./lora-network-server start to
start the network server.

*1.1 Roadblocks*

Problem: Sometimes if you try to start the network server from the GUI
it doesn't work

Solution: Use putty to ssh into your gateway and turn on the network
server from the /etc/init.d directory

Problem: Once you have navigated to the /etc/init.d directory the
command ./lora-network-server may not find your lora card

Solution: Use the command ./lora-network-server restart to fix this

*1.2 Grab Node EUIs*

Now download the things network library and run the device info code.
You should be able to get the EUIs of your devices.

*1.3 Add Nodes to the Gateway*

Now navigate to LoRaWAN \> Devices in your gateway's GUI. First add each
of your nodes under the End Devices section. Below is an example of the
settings that you should insert.

![Alt text](images/node_end_device.PNG?raw=true "Title")

Then, create a device session for each of your nodes. Below is a picture
of the settings used while creating a session for your node. Note that
some of these options are auto filled after creating the device session.

![Alt text](images/node_device_session.PNG?raw=true "Title")

*1.5 Setup NodeRed*

Under Apps in your gateway GUI you have an option to launch NodeRed.
NodeRed allows you to setup workflows for data coming in and going out
of the gateway. In order to complete this step you will need to install
the ttn-lora addon. The steps for installing this module can be found
here: <https://www.npmjs.com/package/node-red-contrib-ttn>

Below is the workflow you will want for the loopback.

![Alt text](images/nodered-config.PNG?raw=true "Title")

The uplink block is a lora in block, the Node 1 and Node 2 blocks are
lora out blocks, and the filter1 and filter2 blocks are function blocks.
Now lets go through the configuration settings for each block.

Uplink Block:

![Alt text](images/uplink_block.PNG?raw=true "Title")

Node 1 block:

![Alt text](images/node1_block.PNG?raw=true "Title")

For device EUI above simply insert the device EUI associated with your
first node in the loopback.

Node 2 block:

![Alt text](images/node2_block.png?raw=true "Title")

For the device EUI field above simply enter the device EUI associated
with the second node in the loopback.

Filter 1 block:

if(msg.deveui != "\<insert device EUI for Node 2\>" \|\|
msg.payload.length == 1) {\
    return;\
}\
\
msg.eui =  \"\<insert device EUI for Node 1\>\";\
\
return msg;

Let us discuss the purpose of this filter 1 block. Note that both the
nodes are constantly polling the gateway with 1 byte of information.
This is because we are dealing with class A LoRa devices whose receive
window is only open once they send a message. So, each of these devices
need to continuously poll the gateway to keep their receive windows open
for incoming messages. This filter ignores any 1 byte poll messages by
returning if msg.payload.length == 1. Additionally this filter also
ignores messages originating from Node 1 because we only want to send
messages received from Node 2 to Node 1.

Filter 2 block:

if(msg.deveui != "\<insert device EUI for Node 1\>" \|\|
msg.payload.length == 1) {\
    return;\
}\
\
msg.eui =  \"\<insert device EUI for Node 2\>\";\
\
return msg;

In this filter block we are still ignoring 1 byte poll messages and
ignoring any messages sent from Node 2 because we only want to send
messages from Node 1 to Node 2.

You should add debug nodes in your workflow to check whether the gateway
is receiving data.

*1.6 Flash all the devices with code*

All the code for flashing devices can be found in the Arduino directory.

Now flash your two nodes with the node\_1 and node\_2 code, flash 2 of
your ESP devices with the esp\_1 and esp\_2 code, and flash your final
ESP device with the LoopbackMQTT\_pubsub code.

*1.5/1.6 Roadblocks*

Problem: You don't see any messages arriving at the gateway.

Solutions: Check whether your nodes are properly sending the polling
bytes. Try reflashing your nodes. Try restarting your gateway. Check the
frame counter of the messages being sent by the nodes and check the last
seen frame counter of the nodes shown in the devices portal. If the
frame counter of the messages sent by the nodes are smaller than the one
on the devices portal the gateway will not receive any messages sent by
that node. In this case you will have to delete the device session and
recreate it which will reset the frame counter to 0.

Problem: NodeRed constantly restarts or it only receives the first
message sent by your node

Solution: Restart the lora network server. If this doesn't work then
restart the entire gateway.

Problem: Either the ESP 1 wifi network or ESP 2 wifi network is not up

Solution: Reset your ESP 1 and ESP 2 devices

*1.7 Run the PAHO code from a computer*

Now connect to the ESP\_Access\_Point network. The password for this
network is helloworld. Then, run mqtt\_end\_device.py which can be found
in the PAHO folder. You should be able to see the roundtrip time when
you run this code and you can keep pressing enter to keep resending the
message and get new roundtrip times.

*1.7 Roadblocks*

Before you start reading this section make sure that your gateway is
getting all the 1 byte poll messages which it should get right after you
finish 1.6. If this is not happening, please refer to the 1.5/1.6
Roadblocks section.

Problem: The Loopback ESP device doesn't receive the message.

Solution: Since we have already verified that the gateway is receiving
messages from the nodes this is probably a serial communication issue
between the node and the ESP. Check the serial monitors of the devices
to see which device is not receiving the data. Isolate that device and
test the serial communication for that device. Also, make sure that the
loopback device has connected to the ESP\_Access\_Point\_2 network and
has connected to the broker running on the second ESP.

Problem: The code immediately times out

Solution: This probably means that your computer is not on the
ESP\_Access\_Point network.

Problem: Your computer hangs and doesn't receive an ACK.

Solution: First, try exit and rerunning the code because the loopback
isn't completely reliably. However, if the problem persists then follow
the troubleshooting steps outlined for the first problem in this
section.

**2. Proxy Server and Etsy Store Setup**

The proxy server takes requests from the LoRa pathway and forwards them
as API requests to your etsy store. This section describes how to setup
the proxy server and etsy store.

*2.1 Setup an Etsy Store*

Start off by following the instructions in this link:
<https://www.fundera.com/blog/opening-an-etsy-shop> to create an etsy
store.

Make sure that you start off with at least one listing in your etsy
store. Then navigate to your store and go to settings \> Shipping
settings \> Shipping profiles. Here you can add a shipping profile. Once
you do this you can click on the number under active listings. This will
lead to a link like this :
<https://www.etsy.com/your/shops/gtetsystore/tools/listings/shipping_profile_id:81440677322>

Note how the shipping profile id is contained in this link. Save this
number.

2.1 Roadblocks

Problem: Your active listings number is not associated with a link

Solution: Make sure at least one listing is associated with this
shipping profile.

*2.2 Setup Proxy Server*

Create an account with amazon web services. Click on EC2 under All
services \> Compute. Click on instances in the left panel and then click
on Launch Instance. You can select any of the basic Amazon linux
machines and then create your machine with the default settings. Now we
need to edit the security rules for our machine to allow all inbound and
outbound traffic. So, go back to instances and check the name of the
security group of your instance. Then navigate to network and security
\> security groups and edit the inbound and outbound rules of the
security group associated with your machine to allow all traffic. Now
ssh into your proxy server and grab the proxy server code from this
repo: <https://github.com/rmahajani31/LoRa-proxy-server>

Then, navigate to this website: <https://www.etsy.com/developers>

Now you can register an application on this website using this guide:
<https://www.etsy.com/developers/documentation/getting_started/register>

Once you have done this the site will give you a keystring and a shared
secret associated with your app.

Go to clients/client\_1/etsy/oauth1/keys.tsv and change the client key
and client secret to the one associated with the app that you just
registered. Now navigate to
clients/client\_1/etsy/shipping\_template\_id/shipping\_template\_id.txt
and change it to the one you retrieved from your store in the previous
section. Now navigate to scripts/get\_etsy\_keys.py. You should run this
file with the flag --key\_file which will indicate the location of your
keys.tsv file. This code will spit out a url and if you go to that url
you can authorize access for your app. Once you authorize access you
will be redirected to another url which you will need to paste into the
terminal. Now, your keys.tsv file will contained additional information
to authorize api requests to your etsy store.

Now install mosquito using your local package manager and run it with
the command mosquito. Simultaneously also run the mqtt\_server\_main.py
file. You should see that it has connected to the mosquito mqtt broker.

*2.2 Roadblocks*

Problem: The get\_etsy\_keys.py file errors out

Solution: Make sure that there are no spaces and only tabs in your
keys.tsv file. Also, make sure there are no newline characters in the
keys.tsv file.

Problem: The mqtt\_server\_main.py file doesn't connect to the broker

Solution: Make sure that the broker is running in a separate terminal

**3. Proxy Server LoRa Pathway Integration**

In this section we will try to integrate parts 1 and 2. We want to be
able to post a listing from an area without internet access. We will
work off of the setup in the previous 2 parts to achieve this goal.

*3.1 Restructure LoRa pathway*

In our LoRa pathway we can disconnect the loopback device. We will also
need to reflash the second esp to connect to the broker running on the
proxy server and publish incoming messages to this broker. In the LoRa
project repo under the Arduino file we will reflash ESP 2 with the file
esp\_2\_proxy. Note that we will need to change one line in this code
which specifies the public IP Address of the proxy server. Whenever you
restart a machine on AWS its public IP changes. After starting your
machine on the instances panel on AWS you can check the public IPv4
Address at the very bottom under the Description tab. The line shown
below needs to be updated to reflect the correct IPv4 address of the
proxy server.

![Alt text](images/mqtt_server_line.PNG?raw=true "Title")

Now the ESP 2 device can be reflashed. Considering that your computer
won't have internet access and will be connected to the esp access point
you will need to open terminals on other devices to monitor the status
of your proxy server. Open the 2 terminals on another device and run the
mosquito broker as well as the mqtt\_server\_main.py file on these
terminals.

Once your computer is connected to the esp access point you can run
mqtt\_end\_device.py under the PAHO folder. Once this message arrives at
the server the terminal will print out received message as well as the
complete message that will be used to make an api request.

Note below is the message structure used for each request.

quantity,title,description,price,who\_made field,is\_supply
field,when\_made field,shipping\_template\_id

*3.1 Roadblocks*

Problem: The message makes it through the entire pathway but isn't
received at the server.

Solution: Check whether the broker is running and the ESP 2 device has
connected to the broker. It could be the case that this second ESP
device wasn't able to connect to the internet. Also, make sure that the
server has connected to the broker. Make sure that you have followed the
above message structure when sending your message. Make sure that the
shipping template id matches your template id. Make sure your
application is still active on the developer portal. If not, you will
have to recreate your app.

#!/usr/bin/python
# -*- coding: utf-8 -*-

# Copyright (c) 2010-2013 Roger Light <roger@atchoo.org>
#
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Distribution License v1.0
# which accompanies this distribution.
#
# The Eclipse Distribution License is available at
#   http://www.eclipse.org/org/documents/edl-v10.php.
#
# Contributors:
#    Roger Light - initial implementation
# Copyright (c) 2010,2011 Roger Light <roger@atchoo.org>
# All rights reserved.

# This shows a simple example of an MQTT subscriber.
import paho.mqtt.client as mqtt
import time

start_time = 0
end_time = 0

def on_connect(mqttc, obj, flags, rc):
    # Once this computer has connected to the broker on the first ESP, it creates the first payload and subscribes to the topic esp_messages
    print("Connected: " + str(rc))
    payload = "1,title_1,desc_1,16.04,collective,t,1990s,81440677322"
    mqttc.subscribe("esp_messages")
    # publish the payload to the topic device messages and record the start time
    global start_time
    start_time = time.time()
    mqttc.publish("device_messages", payload)
    print("publishing start time", start_time)

def on_message(mqttc, obj, msg):
    # record the end time and publish stats if ACK is received at this device
    global start_time
    global end_time
    end_time = time.time()
    print("first 5 characters in payload are " + str(msg.payload)[0:5])
    if str(msg.payload)[0:5] == "b'ACK":
        print("Received ACK in", end_time - start_time)
        print(msg.topic + " " + str(msg.qos) + " " + str(msg.payload))
    y = input("enter key to send")
    # provide an option to resend the payload below
    if y != 'q':
        start_time = time.time()
        payload = "1,title_1,desc_1,16.04,collective,t,1990s,81440677322"
        mqttc.publish("device_messages", payload)
        print("publishing start time", start_time)

def on_publish(mqttc, obj, mid):
    print("published: " + str(mid))

def on_subscribe(mqttc, obj, mid, granted_qos):
    print("Subscribed: " + str(mid) + " " + str(granted_qos))


def on_log(mqttc, obj, level, string):
    print(string)


# If you want to use a specific client id, use
# mqttc = mqtt.Client("client-id")
# but note that the client id must be unique on the broker. Leaving the client
# id parameter empty will generate a random id for you.
# connect to the client and set all the callbacks
mqttc = mqtt.Client("paho_client" + str(time.time()))
mqttc.on_message = on_message
mqttc.on_connect = on_connect
mqttc.on_publish = on_publish
mqttc.on_subscribe = on_subscribe
# Uncomment to enable debug messages
mqttc.on_log = on_log
mqttc.connect("192.168.4.1", 1883, 60)
mqttc.loop_forever()
import mqtt from 'mqtt';


// initialize the MQTT client
var client = mqtt.connect("mqtt://6f73b3569de64e8c94164637cdc7a301.s2.eu.hivemq.cloud", { protocol: "mqtts", username: "ucontrol", password: "Ucontrol123", port: 8883, reconnectPeriod: 5000 });

// setup the callbacks
client.on("connect", function () {
    console.log("Connected");
});

client.on("error", function (error) {
    console.log(error);

});

client.on("offline", () => {
    console.log("Client is offline");
});

client.on("reconnect", () => {
    console.log("Reconnecting to MQTT broker");
});

client.on("end", () => {
    console.log("Connection to MQTT broker ended");
});


client.on("message", function (topic, message) {
    // called each time a message is received
    console.log("Received message:", topic, message.toString());
});

// subscribe to topic "my/test/topic"
client.subscribe("Agua/Bano Mujeres/Lab. Prototipos");

// publish message "Hello" to topic "my/test/topic"
client.publish("Agua/Bano Mujeres/Lab. Prototipos", "1", { retain: true, qos: 0 }, (err) => {
    if (err) {
        console.error("Failed to publish message:", err);
    } else {
        console.log("Message published with retain flag set to true");
    }
});
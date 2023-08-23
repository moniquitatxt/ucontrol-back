import express from "express";
import Device from "../models/Device.js";
import Space from "../models/Space.js";
import mongoose from "mongoose";
import mqtt from "mqtt";
import dotenv from "dotenv";
dotenv.config();

const router = express.Router();

const ip = process.env.IP;
const username = process.env.USER;
const password = process.env.PASS;

const client = mqtt.connect(`mqtt://${ip}:1884`, {
	username: username,
	password: password,
});

router.post("/createDevice", async (req, res) => {
	const { deviceData, spaceId, userName } = req.body;

	try {
		// Check if the associated space exists
		const space = await Space.findById(spaceId);
		if (!space) {
			return res
				.status(400)
				.json({ success: false, message: "Space not found" });
		}

		// Create the new device and add it to the associated space
		const device = new Device(deviceData);

		if (device.type == "aire" || device.type == "luz") {
			client.publish("Topico extra", device.topic + " / Switch", (err) => {
				if (err) {
					console.error("Error publishing message:", err);
				} else {
					console.log("Mensaje enviado");
				}
			});
		}

		if (device.type == "controlAcceso") {
			client.publish("Topico extra", device.topic + " / Permiso", (err) => {
				if (err) {
					console.error("Error publishing message:", err);
				} else {
					console.log("Mensaje enviado");
				}
			});
		}

		if (device.type == "tempHum") {
			client.publish("Topico extra", device.topic + " / Temperatura", (err) => {
				if (err) {
					console.error("Error publishing message:", err);
				} else {
					console.log("Mensaje enviado");
				}
			});
			client.publish("Topico extra", device.topic + " / Humedad", (err) => {
				if (err) {
					console.error("Error publishing message:", err);
				} else {
					console.log("Mensaje enviado");
				}
			});
		} else {
			client.publish("Nuevo dispositivo", device.topic, (err) => {
				if (err) {
					console.error("Error publishing message:", err);
				} else {
					console.log("Mensaje enviado");
				}
			});
		}

		const savedDevice = await device.save();

		const historyEntry = {
			updatedBy: userName,
			field: [`Se agregó el dispositivo ${device.name}`], // Assuming spaceUpdate contains fields to be updated
		};
		space.history.push(historyEntry);

		space.devices.push(savedDevice._id);
		await space.save();

		res.status(201).json({
			success: true,
			message: "Device created successfully",
			data: savedDevice,
		});
	} catch (err) {
		res.status(500).json({ success: false, message: err.message });
	}
});

router.get("/getSpaceByDeviceId/:deviceId", async (req, res) => {
	try {
		const { deviceId } = req.params;
		const device = await Device.findById(deviceId);

		// Check if the device exists
		if (!device) {
			return res.status(404).json({ message: "Device not found" });
		}

		// Find the space that contains the device
		const space = await Space.findOne({ devices: deviceId });

		// Check if the space exists
		if (!space) {
			return res.status(404).json({ message: "Space not found" });
		}

		// Return the space id
		res.json({ spaceId: space._id });
	} catch (error) {
		// Handle any errors
		res.status(500).json({ message: error.message });
	}
});

router.post("/getAllDevicesBySpace", async (req, res) => {
	try {
		const { spaceId } = req.body;

		// Check if the spaceId is provided and valid
		if (!spaceId) {
			return res.status(400).json({
				success: false,
				message: "Please provide a valid spaceId in the request body.",
			});
		}

		// Check if the associated space exists
		const space = await Space.findById(spaceId);
		if (!space) {
			return res
				.status(404)
				.json({ success: false, message: "Space not found." });
		}

		// Filter devices by spaceId if the space exists
		const devices = await Device.find({ _id: { $in: space.devices } });

		// Check if there are devices associated with the space
		if (devices.length === 0) {
			return res.status(200).json({
				success: true,
				message: "No devices found for this space.",
				data: devices,
			});
		}

		res.status(200).json({
			success: true,
			message: "Devices retrieved successfully",
			data: devices,
		});
	} catch (err) {
		res.status(500).json({ success: false, message: err.message });
	}
});

router.post("/devicesByUser", async (req, res) => {
	const { userId } = req.body;

	try {
		// Find all devices with the given createdBy value (userId)
		const devices = await Device.find({ createdBy: userId }).exec();

		if (devices.length === 0) {
			return res
				.status(404)
				.json({ success: false, message: "No devices found for this user." });
		}

		res.status(200).json({ success: true, data: devices });
	} catch (err) {
		res.status(500).json({ success: false, message: err.message });
	}
});

router.post("/getDeviceById", async (req, res) => {
	const { id } = req.body;

	try {
		const device = await Device.findById(id);
		if (!device) {
			return res
				.status(404)
				.json({ success: false, message: "Device not found" });
		}

		res.status(200).json({
			success: true,
			message: "Device retrieved successfully",
			data: device,
		});
	} catch (err) {
		res.status(500).json({ success: false, message: err.message });
	}
});

router.patch("/updateDevice", async (req, res) => {
	const { id } = req.body;
	const { name, description, dvt, topic, fields, conditions, userName } =
		req.body;

	try {
		const device = await Device.findByIdAndUpdate(
			id,
			{ name, description, dvt, topic, conditions },
			{ new: true }
		);
		if (!device) {
			return res
				.status(404)
				.json({ success: false, message: "Device not found" });
		}
		// Create a history entry for the update
		const historyEntry = {
			updatedBy: userName,
			field: fields, // Assuming spaceUpdate contains fields to be updated
		};

		const spacesToUpdate = await Space.find({ devices: device._id });

		const historySpaceEntry = {
			updatedBy: userName,
			field: [`Se modificó el dispositivo ${device.name}`], // Assuming spaceUpdate contains fields to be updated
		};
		// Remove the device ID from the devices array of each associated space
		for (const space of spacesToUpdate) {
			space.history.push(historySpaceEntry);
			await space.save();
		}

		device.history.push(historyEntry);
		await device.save();

		res.status(200).json({
			success: true,
			message: "Device updated successfully",
			data: device,
		});
	} catch (err) {
		res.status(500).json({ success: false, message: err.message });
	}
});

router.delete("/deleteDevice", async (req, res) => {
	const { id, userName } = req.body;

	try {
		// Check if the provided ID is a valid ObjectId
		if (!mongoose.Types.ObjectId.isValid(id)) {
			return res
				.status(400)
				.json({ success: false, message: "Invalid device ID provided." });
		}

		const device = await Device.findByIdAndDelete(id);
		if (!device) {
			return res
				.status(404)
				.json({ success: false, message: "Dispositivo no encontrado" });
		}

		// Find all spaces that have the device ID in their devices array
		const spacesToUpdate = await Space.find({ devices: device._id });

		const historyEntry = {
			updatedBy: userName,
			field: [`Se eliminó el dispositivo ${device.name}`], // Assuming spaceUpdate contains fields to be updated
		};
		// Remove the device ID from the devices array of each associated space
		for (const space of spacesToUpdate) {
			space.history.push(historyEntry);
			space.devices.pull(device._id);
			await space.save();
		}

		res.status(200).json({
			success: true,
			message: "Dispositivo eliminado exitosamente",
			data: device,
		});
	} catch (err) {
		res.status(500).json({ success: false, message: err.message });
	}
});

export default router;

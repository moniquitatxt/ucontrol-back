import express from "express";
import Device from "./device.js";
import Space from "./space.js";

const router = express.Router();

router.post("/createDevice", async (req, res) => {
	const { deviceData, spaceId } = req.body;

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
		const savedDevice = await device.save();
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

router.get("/getAllDevices ", async (req, res) => {
	try {
		// Filter devices by space if spaceId param is provided
		const { spaceId } = req.query;
		const filter = spaceId ? { _id: { $in: space.devices }, spaceId } : {};

		const devices = await Device.find(filter);
		res.status(200).json({
			success: true,
			message: "Devices retrieved successfully",
			data: devices,
		});
	} catch (err) {
		res.status(500).json({ success: false, message: err.message });
	}
});

router.get("/getDeviceById", async (req, res) => {
	const { id } = req.params;

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

router.put("/updateDevice", async (req, res) => {
	const { id } = req.params;
	const { name, description, dvt, type } = req.body;

	try {
		const device = await Device.findByIdAndUpdate(
			id,
			{ name, description, dvt, type },
			{ new: true }
		);
		if (!device) {
			return res
				.status(404)
				.json({ success: false, message: "Device not found" });
		}

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
	const { id } = req.params;

	try {
		const device = await Device.findByIdAndDelete(id);
		if (!device) {
			return res
				.status(404)
				.json({ success: false, message: "Device not found" });
		}

		// Remove the device from its associated space
		const space = await Space.findById(device.spaceId);
		space.devices.pull(device._id);
		await space.save();

		res.status(204).json();
	} catch (err) {
		res.status(500).json({ success: false, message: err.message });
	}
});

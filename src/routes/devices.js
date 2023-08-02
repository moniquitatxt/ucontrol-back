import express from "express";
import Device from "../models/Device.js";
import Space from "../models/Space.js";
import mongoose from "mongoose";

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

router.get("/getAllDevicesBySpace", async (req, res) => {
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


router.get('/devicesByUser', async (req, res) => {
	const { userId } = req.body;

	try {
		// Find all devices with the given createdBy value (userId)
		const devices = await Device.find({ createdBy: userId }).exec();

		if (devices.length === 0) {
			return res.status(404).json({ success: false, message: 'No devices found for this user.' });
		}

		res.status(200).json({ success: true, data: devices });
	} catch (err) {
		res.status(500).json({ success: false, message: err.message });
	}
});

router.get("/getDeviceById", async (req, res) => {
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
	const { id } = req.body;

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

		// Remove the device from its associated space
		if (device.spaceId) {
			const space = await Space.findById(device.spaceId);
			if (!space) {
				return res
					.status(404)
					.json({ success: false, message: "Espacio no encontrado." });
			}

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

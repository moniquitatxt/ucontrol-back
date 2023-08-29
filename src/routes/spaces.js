import express from "express";
import Space from "../models/Space.js";
import Permission from "../models/Permissions.js";

const router = express.Router();

//crear espacio
router.post("/createSpace", async (req, res) => {
	try {
		// Create a new space
		const space = new Space(req.body.space);
		await space.save();

		// Create a new permissions collection for the space
		const permission = new Permission({
			spaceId: space._id,
			userId: req.body.userId,
			permission: "readWrite",
		});
		await permission.save();

		res.status(201).json({
			success: true,
			message: "Espacio creado exitosamente",
			data: space,
		});
	} catch (err) {
		res.status(400).json({ success: false, message: err.message });
	}
});

// crear subespacio
router.post("/createSubspace", async (req, res) => {
	const { spaceId, newSubspace } = req.body;
	try {
		const space = await Space.findById(spaceId);
		if (!space) {
			throw new Error("Espacio no encontrado");
		}

		newSubspace.parentSpace = space._id;
		const subSpace = new Space(newSubspace);
		space.subSpaces.push(subSpace._id);
		await subSpace.save();
		await space.save();
		res.status(201).json({
			success: true,
			message: "Espacio creado exitosamente",
			data: subSpace,
		});
	} catch (err) {
		res.status(500).json({ success: false, message: err.message });
	}
});

// Get all parent spaces
router.get("/getParentSpaces", async (req, res) => {
	try {
		const parentSpaces = await Space.find({ parentSpace: null });
		res.status(200).json({
			success: true,
			message: "Espacios obtenidos exitosamente",
			data: parentSpaces,
		});
	} catch (err) {
		res.status(500).json({ success: false, message: err.message });
	}
});

// Obtener todos los dispositivos de todos los subespacios directos de un espacio padre
router.get("/getSpaceDevices", async (req, res) => {
	const { spaceId } = req.body;
	try {
		const space = await Space.findById(spaceId);
		if (!space) {
			throw new Error("Espacio no encontrado");
		}
		const devices = space.devices;
		for (const subSpaceId of space.subSpaces) {
			const subSpace = await Space.findById(subSpaceId);
			if (subSpace) {
				devices.push(...subSpace.devices);
			}
		}
		res.status(200).json({
			success: true,
			message: "Dispositivos obtenidos exitosamente",
			data: devices,
		});
	} catch (err) {
		res.status(500).json({ success: false, message: err.message });
	}
});

router.get("/getAllSpaces", async (req, res) => {
	try {
		// Find all spaces, including both parent spaces and subspaces
		const allSpaces = await Space.find({});

		res.status(200).json({
			success: true,
			message: "Todos los espacios obtenidos exitosamente",
			spaces: allSpaces,
		});
	} catch (err) {
		res.status(500).json({ success: false, message: err.message });
	}
});

// Update a space
router.patch("/updateSpace", async (req, res) => {
	const { spaceId, fields, spaceUpdate, userName } = req.body;
	try {
		const space = await Space.findByIdAndUpdate(spaceId, spaceUpdate, {
			new: true,
		});
		if (!space) {
			throw new Error("Espacio no encontrado");
		}

		// Create a history entry for the update
		const historyEntry = {
			updatedBy: userName,
			field: fields, // Assuming spaceUpdate contains fields to be updated
		};

		space.history.push(historyEntry);
		await space.save();

		res.status(200).json({
			success: true,
			message: "Espacio actualizado exitosamente",
			data: space,
		});
	} catch (err) {
		res.status(500).json({ success: false, message: err.message });
	}
});

// Delete a space
router.delete("/deleteSpace", async (req, res) => {
	const { spaceId } = req.body;
	try {
		const space = await Space.findByIdAndDelete(spaceId);

		if (!space) {
			throw new Error("Espacio no encontrado");
		}

		// Delete devices associated with the space
		for (const deviceId of space.devices) {
			await Device.findByIdAndDelete(deviceId);
		}

		await Permission.deleteMany({ spaceId });

		// If the space has a parent space, remove itself from the parent's subSpaces array
		if (space.parentSpace) {
			const parentSpace = await Space.findById(space.parentSpace);
			if (parentSpace) {
				parentSpace.subSpaces.pull(spaceId);
				await parentSpace.save();
			}
		}

		res.status(200).json({
			success: true,
			message: "Espacio eliminado exitosamente",
			data: space,
		});
	} catch (err) {
		res.status(500).json({ success: false, message: err.message });
	}
});

//Get a space
router.get("/getSpaceById/:spaceId", async (req, res) => {
	const spaceId = req.params.spaceId;

	try {
		const space = await Space.findById(spaceId);
		if (!space) {
			throw new Error("Espacio no encontrado");
		}
		res.status(200).json({
			success: true,
			message: "Espacio obtenido exitosamente",
			data: space,
		});
	} catch (err) {
		res.status(500).json({ success: false, message: err.message });
	}
});

export default router;

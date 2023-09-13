import express from "express";
import mongoose from "mongoose";
import crypto from "crypto";
import nodemailer from "nodemailer";
import Permission from "../models/Permissions.js";
import Device from "../models/Device.js";
import User from "../models/user.js";
import Space from "../models/Space.js";

const router = express.Router();

// Crear un nuevo permiso
router.post("/permission", async (req, res) => {
	const { email, spaceId, permission } = req.body;

	// Find the user by email
	const user = await User.findOne({ email });

	if (!user) {
		return res.status(404).send({
			success: false,
			message: "Correo inválido el usuario no existe",
		});
	}

	// Check if the user is the creator of the space
	const space = await Space.findById(spaceId);

	if (!space) {
		return res.status(404).send({
			success: false,
			message: "Espacio no encontrado",
		});
	}

	if (space.createdBy === user._id.toString()) {
		return res.status(403).send({
			success: false,
			message:
				"No puedes otorgar permisos en un espacio que tú mismo has creado",
		});
	}

	// Check if the user already has permission in the space
	const existingPermission = await Permission.findOne({
		spaceId,
		userId: user._id,
	});

	if (existingPermission) {
		return res.status(400).send({
			success: false,
			message: "El usuario ya tiene permisos en este espacio",
		});
	}

	// Create and save the new permission
	const permissionSaved = new Permission({
		spaceId,
		userId: user._id,
		permission,
	});

	try {
		await permissionSaved.save();

		// Send email
		const transporter = nodemailer.createTransport({
			service: "Gmail",
			auth: {
				user: "ucontrol.iotsystem@gmail.com",
				pass: "jhszraxaqmsmrdpl",
			},
		});

		const mailOptions = {
			from: "ucontrol.iotsystem@gmail.com",
			to: email,
			subject: "Invitación a espacio",
			text: `Hola! Usted ha sido autorizado para acceder al espacio en el sistema de inmótica UControl`,
		};

		const info = await transporter.sendMail(mailOptions);

		res.status(201).json({
			success: true,
			message: "La invitación se ha enviado exitosamente",
		});
	} catch (error) {
		res.status(500).json({ success: false, message: error.message });
	}
});

// Get all permissions for a space
router.get("/getSpacePermissions", async (req, res) => {
	const { spaceId } = req.body;
	try {
		const permissions = await Permission.find({ spaceId });
		res.status(200).json({
			success: true,
			message: "Los permisos se han obtenido exitosamente",
			data: permissions,
		});
	} catch (err) {
		res.status(500).json({ success: false, message: err.message });
	}
});

// Get all permissions for a user
router.get("/getUserPermissions", async (req, res) => {
	const { userId } = req.body;
	try {
		const permissions = await Permission.find({ userId });
		res.status(200).json({
			success: true,
			message: "Los permisos se han obtenido exitosamente",
			data: permissions,
		});
	} catch (err) {
		res.status(500).json({ success: false, message: err.message });
	}
});
// Recursive function to get all subspaces
export const getAllSubspaces = async (spaceId) => {
	const subspaceIds = await Space.find({ parentSpace: spaceId }).distinct(
		"_id"
	);
	let allSubspaces = subspaceIds;

	for (const subId of subspaceIds) {
		const nestedSubspaces = await getAllSubspaces(subId);
		allSubspaces = allSubspaces.concat(nestedSubspaces);
	}

	return allSubspaces;
};

router.get("/getUserParentSpaces/:userId", async (req, res) => {
	const { userId } = req.params;

	try {
		// Find space IDs for which the user has permissions
		const permissionSpaces = await Permission.find({ userId }).distinct(
			"spaceId"
		);

		// Find and return the spaces corresponding to the retrieved IDs
		const spaces = await Space.find({
			$or: [{ _id: { $in: permissionSpaces } }],
		});

		res.status(200).json({
			success: true,
			message: "Spaces retrieved successfully",
			data: spaces,
		});
	} catch (err) {
		res.status(500).json({ success: false, message: err.message });
	}
});

// Route to get all spaces the user has permissions to, including subspaces
router.get("/getUserSpaces/:userId", async (req, res) => {
	const { userId } = req.params;

	try {
		// Find space IDs for which the user has permissions
		const permissionSpaces = await Permission.find({ userId }).distinct(
			"spaceId"
		);

		// Retrieve all subspaces recursively for each space
		let allSubspaces = [];
		for (const spaceId of permissionSpaces) {
			const subspaces = await getAllSubspaces(spaceId);
			allSubspaces = allSubspaces.concat(subspaces);
		}

		// Find and return the spaces corresponding to the retrieved IDs
		const spaces = await Space.find({
			$or: [{ _id: { $in: allSubspaces } }, { _id: { $in: permissionSpaces } }],
		});

		res.status(200).json({
			success: true,
			message: "Spaces retrieved successfully",
			data: spaces,
		});
	} catch (err) {
		res.status(500).json({ success: false, message: err.message });
	}
});

// Route to get all devices from spaces the user has permissions to
router.get("/getUserDevices/:userId", async (req, res) => {
	const { userId } = req.params;

	try {
		// Find space IDs for which the user has permissions
		const permissionSpaces = await Permission.find({ userId }).distinct(
			"spaceId"
		);

		// Retrieve all subspaces recursively for each space
		let allSubspaces = [];
		for (const spaceId of permissionSpaces) {
			const subspaces = await getAllSubspaces(spaceId);
			allSubspaces = allSubspaces.concat(subspaces);
		}

		const spaces = await Space.find({
			$or: [{ _id: { $in: allSubspaces } }, { _id: { $in: permissionSpaces } }],
		});

		const devices = [];

		// Fetch devices for each space
		for (const spaceData of spaces) {
			const deviceIds = spaceData.devices.map((deviceId) =>
				deviceId.toString()
			);
			const spaceDevices = await Device.find({
				_id: { $in: deviceIds },
			});
			devices.push(...spaceDevices);
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
// Get all users for a space
router.get("/getSpaceUsers", async (req, res) => {
	const { spaceId } = req.body;
	try {
		const permissions = await Permission.find({ spaceId });
		const userIds = permissions.map((permission) => permission.userId);
		const users = await User.find({ _id: { $in: userIds } });
		res.status(200).json({
			success: true,
			message: "Usuarios obtenidos exitosamente",
			data: users,
		});
	} catch (err) {
		res.status(500).json({ success: false, message: err.message });
	}
});

// Update a permission for a space and user
router.put("/updatePermission", async (req, res) => {
	const { spaceId, userId } = req.params;
	try {
		const permission = await Permission.findOneAndUpdate(
			{ spaceId, userId },
			req.body,
			{ new: true }
		);
		if (!permission) {
			throw new Error("Permission not found");
		}
		res.status(200).json({
			success: true,
			message: "Permission updated successfully",
			data: permission,
		});
	} catch (err) {
		res.status(500).json({ success: false, message: err.message });
	}
});

// Delete a permission for a space and user
router.delete(
	"/spaces/:spaceId/users/:userId/permissions",
	async (req, res) => {
		const { spaceId, userId } = req.params;
		try {
			const permission = await Permission.findOneAndDelete({ spaceId, userId });
			if (!permission) {
				throw new Error("Permission not found");
			}
			res.status(200).json({
				success: true,
				message: "Permission deleted successfully",
				data: permission,
			});
		} catch (err) {
			res.status(500).json({ success: false, message: err.message });
		}
	}
);

export default router;

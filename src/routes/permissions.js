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
	try {
		const { permission, email } = req.body;
		const permissionSaved = new Permission(permission);
		await permissionSaved.save();

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
			data: permission,
		});
	} catch (err) {
		res.status(500).json({ success: false, message: err.message });
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

		// Check if the user is an admin
		const user = await User.findById(userId);
		if (user && user.type === "admin") {
			// If the user is an admin, add spaces they've created
			const adminSpaces = await Space.find({ creatorId: userId });
			// Filter out any duplicates by merging adminSpaces and spaces
			spaces = [
				...adminSpaces,
				...spaces.filter(
					(space) =>
						!adminSpaces.some(
							(adminSpace) => adminSpace._id.toString() === space._id.toString()
						)
				),
			];
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

		// Check if the user is an admin
		const user = await User.findById(userId);
		if (user && user.type === "admin") {
			// If the user is an admin, add spaces they've created
			const adminSpaces = await Space.find({ creatorId: userId });
			// Filter out any duplicates by merging adminSpaces and spaces
			permissionSpaces.push(
				...adminSpaces.map((adminSpace) => adminSpace._id.toString())
			);
		}

		const devices = [];

		// Fetch devices for each space
		for (const spaceId of permissionSpaces) {
			const space = await Space.findById(spaceId);
			if (space) {
				const deviceIds = space.devices.map((deviceId) => deviceId.toString());
				const spaceDevices = await Device.find({
					_id: { $in: deviceIds },
				});
				devices.push(...spaceDevices);
			}
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

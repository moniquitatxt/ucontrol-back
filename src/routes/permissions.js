import express from "express";
import mongoose from "mongoose";
import crypto from "crypto";
import nodemailer from "nodemailer";
import Permission from "../models/Permissions.js";
import User from "../models/user.js";
import Space from "../models/Space.js";

const router = express.Router();

// Crear un nuevo permiso
router.post("/permissions", async (req, res) => {
	try {
		const { permission, email } = new Permission(req.body);

		await permission.save();

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

// Get all spaces for a user
router.get("/getUserSpaces", async (req, res) => {
	const { userId } = req.body;
	try {
		const permissions = await Permission.find({ userId });
		const spaceIds = permissions.map((permission) => permission.spaceId);
		const spaces = await Space.find({ _id: { $in: spaceIds } });
		res.status(200).json({
			success: true,
			message: "Espacios obtenidos exitosamente",
			data: spaces,
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

import express from "express";
import mongoose from "mongoose";
import AccessControlUser from "../models/AccessControlUser.js";
import AccessControlSpace from "../models/AccessControlSpace.js";

const router = express.Router();

router.post("/createAccessControlUser", async (req, res) => {
	try {
		const { name, email, role, career, eCard, ci } = req.body;

		// Check if the email, eCard, and ci are unique
		const existingUser = await AccessControlUser.findOne({
			$or: [{ email }, { eCard }, { ci }],
		});

		if (existingUser) {
			return res.status(400).json({
				success: false,
				message: "Email, eCard, or ci ya están registrados.",
			});
		}

		// Create a new access control user
		const accessControlUser = new AccessControlUser({
			name,
			email,
			role,
			career,
			eCard,
			ci,
		});

		// Save the user to the database
		await accessControlUser.save();

		res.status(201).json({
			success: true,
			message: "Access control user created successfully.",
			data: accessControlUser,
		});
	} catch (error) {
		res.status(500).json({ success: false, message: error.message });
	}
});

router.patch("/changeStatusSpace/:deviceId", async (req, res) => {
	try {
		const { deviceId } = req.params;
		// Find the access control space by deviceId
		const accessControlSpace = await AccessControlSpace.findOne({ deviceId });

		// Check if the access control space exists
		if (!accessControlSpace) {
			return res.status(404).json({
				success: false,
				message: "Access control space not found",
			});
		}

		// Toggle the status
		accessControlSpace.status = !accessControlSpace.status;

		// Save the updated space
		await accessControlSpace.save();

		res.status(201).json({
			success: true,
			message: "Se ha cambiado el estado del espacio",
		});
	} catch (error) {
		res.status(500).json({ success: false, message: error.message });
	}
});

export default router;

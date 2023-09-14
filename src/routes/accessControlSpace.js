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

router.get("/getAccessControlSpace/:deviceId", async (req, res) => {
	try {
		const { deviceId } = req.params;

		// Find the AccessControlSpace with the provided deviceId
		const accessControlSpace = await AccessControlSpace.findOne({ deviceId });

		if (!accessControlSpace) {
			return res.status(404).json({
				success: false,
				message: "Access control space not found for the given deviceId",
			});
		}

		// If found, send it as a response
		res.status(200).json({
			success: true,
			message: "Access control space retrieved successfully",
			data: accessControlSpace,
		});
	} catch (error) {
		res.status(500).json({ success: false, message: error.message });
	}
});

router.patch("/changeStatusSpace/:deviceId", async (req, res) => {
	try {
		const { deviceId } = req.params;
		const { status } = req.body;

		// Find the access control space by deviceId
		const accessControlSpace = await AccessControlSpace.findOne({ deviceId });

		// Check if the access control space exists
		if (!accessControlSpace) {
			return res.status(404).json({
				success: false,
				message: "No existe un espacio de control de acceso",
			});
		}

		if (accessControlSpace.status === status) {
			return res.status(404).json({
				success: false,
				message: "El espacio ya se encuentra en ese estado",
			});
		}
		// Toggle the status
		accessControlSpace.status = status;

		// Save the updated space
		await accessControlSpace.save();

		const currentTime = new Date();

		const historyEntry = accessControlSpace.history.find((entry) =>
			entry.opened.some(
				(openedTime) =>
					openedTime.getFullYear() === currentTime.getFullYear() &&
					openedTime.getMonth() === currentTime.getMonth() &&
					openedTime.getDate() === currentTime.getDate()
			)
		);

		// Update the history entry based on the new status and current time
		if (status) {
			// Add the current time to the opened array
			if (historyEntry) {
				historyEntry.opened.push(currentTime);
			} else {
				accessControlSpace.history.push({
					opened: [currentTime],
					closed: [],
				});
			}
		} else {
			// Add the current time to the closed array
			if (historyEntry) {
				historyEntry.closed.push(currentTime);
			} else {
				accessControlSpace.history.push({
					opened: [],
					closed: [currentTime],
				});
			}
		}
		// Save the updated space with history
		await accessControlSpace.save();
		res.status(201).json({
			success: true,
			message: "Se ha cambiado el estado del espacio",
		});
	} catch (error) {
		res.status(500).json({ success: false, message: error.message });
	}
});

router.get("/getAccessControlUser/:eCard", async (req, res) => {
	try {
		const { eCard } = req.params;

		// Find the AccessControlUser with the provided eCard
		const accessControlUser = await AccessControlUser.findOne({ eCard });

		if (!accessControlUser) {
			return res.status(404).json({
				success: false,
				message: "Access control user not found for the given eCard",
			});
		}

		// If found, send it as a response
		res.status(200).json({
			success: true,
			message: "Access control user retrieved successfully",
			data: accessControlUser,
		});
	} catch (error) {
		res.status(500).json({ success: false, message: error.message });
	}
});

export default router;

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

router.get("/getAccessControlSpaceUsers/:deviceId", async (req, res) => {
	const { deviceId } = req.params;

	try {
		// Find the AccessControlSpace with the given deviceId
		const accessControlSpace = await AccessControlSpace.findOne({ deviceId });

		if (!accessControlSpace) {
			return res.status(404).json({
				success: false,
				message: "AccessControlSpace not found for the specified deviceId.",
			});
		}

		// Get the userHistory for the found AccessControlSpace
		const userHistory = accessControlSpace.userHistory;

		// Filter userHistory entries for today's date and status=true
		const currentDate = new Date().toDateString();
		const currentUsers = userHistory.filter(
			(entry) =>
				entry.entered[0]?.toDateString() === currentDate &&
				entry.status === true
		);

		// Get the list of user IDs from the filtered entries
		const userIds = currentUsers.map((entry) => entry.userId);

		// Fetch user details for the identified user IDs
		const users = await AccessControlUser.find({ _id: { $in: userIds } });

		// Create an array of objects with user information
		const userData = users.map((user) => ({
			userId: user._id,
			userName: user.name,
			userEmail: user.email,
			userCareer: user.career,
			userCi: user.ci,
			userECard: user.eCard,
		}));

		res.status(200).json({
			success: true,
			message:
				"Users currently inside AccessControlSpace retrieved successfully",
			data: userData,
		});
	} catch (error) {
		res.status(500).json({ success: false, message: error.message });
	}
});

router.get("/getAccessControlSpaceUserHistory/:deviceId", async (req, res) => {
	const { deviceId } = req.params;

	try {
		// Find the AccessControlSpace with the given deviceId
		const accessControlSpace = await AccessControlSpace.findOne({ deviceId });

		if (!accessControlSpace) {
			return res.status(404).json({
				success: false,
				message: "AccessControlSpace not found for the specified deviceId.",
			});
		}

		// Get the userHistory for the found AccessControlSpace
		const userHistory = accessControlSpace.userHistory;

		// Sort userHistory by entry time in ascending order
		userHistory.sort((a, b) => a.entered - b.entered);

		// Create an array of objects with user information and entry/exit times
		const userHistoryData = [];
		for (const historyEntry of userHistory) {
			const userId = historyEntry.userId;
			const user = await AccessControlUser.findById(userId);

			if (user) {
				userHistoryData.push({
					state: historyEntry.state,
					entered: historyEntry.entered,
					gotOut: historyEntry.gotOut,
					userId: user._id,
					name: user.name,
					email: user.email,
					career: user.career,
					ci: user.ci,
					eCard: user.eCard,
				});
			}
		}

		res.status(200).json({
			success: true,
			message: "AccessControlSpace user history retrieved successfully",
			data: userHistoryData,
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

router.post("/passCarnet", async (req, res) => {
	const { topic, message } = req.body;

	const eCard = message.toString().trim();
	try {
		const accessControlUser = await AccessControlUser.findOne({ eCard });
		if (!accessControlUser) {
			throw new Error("Usuario no existe");
		}

		const accessControlSpace = await AccessControlSpace.findOne({ topic });
		if (!accessControlSpace.status) {
			throw new Error("El espacio esta cerrado");
		}

		if (!accessControlSpace.allowedUsers.includes(accessControlUser._id)) {
			const newUserHistory = {
				userId: accessControlUser._id,
				entered: new Date(),
				status: false,
				state: "Acceso denegado",
			};
			accessControlSpace.userHistory.push(newUserHistory);

			await accessControlSpace.save();
			throw new Error("El usuario no tiene acceso a este espacio");
		}
		// Get today's date without the time (just the date)
		const currentTime = new Date();

		// Find the user's history entry for today
		const userHistoryToday = accessControlSpace.userHistory.find(
			(entry) =>
				entry.userId.toString() === accessControlUser._id.toString() &&
				entry.entered.some(
					(openedTime) =>
						openedTime.getFullYear() === currentTime.getFullYear() &&
						openedTime.getMonth() === currentTime.getMonth() &&
						openedTime.getDate() === currentTime.getDate()
				)
		);

		if (!userHistoryToday) {
			// No entry for today, create a new one
			const newUserHistory = {
				userId: accessControlUser._id,
				entered: new Date(),
				status: true, // User is entering
				state: "Acceso concedido",
			};
			accessControlSpace.userHistory.push(newUserHistory);
		} else {
			// Entry for today exists, update status and gotOut accordingly
			if (userHistoryToday.status === false) {
				// User is entering
				userHistoryToday.status = true;
				userHistoryToday.entered.push(new Date());
			} else {
				// User is getting out
				userHistoryToday.status = false;
				userHistoryToday.gotOut.push(new Date());
			}
		}

		await accessControlSpace.save();

		return res.status(200).json({
			success: true,
			message: "Carnet con permiso",
			data: accessControlUser,
		});
	} catch (error) {
		res.status(500).json({ success: false, message: error.message });
	}
});
export default router;

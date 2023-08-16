import express from "express";
import User from "../models/user.js";

const router = express.Router();

router.get("/getAllUsers", async (req, res) => {
	try {
		const allUsers = await User.find({});

		res.status(200).json({
			success: true,
			message: "Todos los usuarios obtenidos exitosamente",
			users: allUsers,
		});
	} catch (err) {
		res.status(500).json({ success: false, message: err.message });
	}
});

// Get a user by ID
router.get("/getUserById/:id", async (req, res) => {
	const { id } = req.params;

	try {
		const user = await User.findById(id);

		if (!user) {
			return res
				.status(404)
				.json({ success: false, message: "Usuario no encontrado" });
		}

		res.status(200).json({
			success: true,
			message: "Usuario obtenido exitosamente",
			data: user,
		});
	} catch (err) {
		res.status(500).json({ success: false, message: err.message });
	}
});

export default router;

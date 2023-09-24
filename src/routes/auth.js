import express from "express";
import bcrypt from "bcrypt";
import User from "../models/user.js";
import nodemailer from "nodemailer";

function generatePin() {
	let pin = "";
	for (let i = 0; i < 6; i++) {
		pin += Math.floor(Math.random() * 10);
	}
	return pin;
}

const router = express.Router();

router.post("/createUser", async (req, res) => {
	const { name, email } = req.body;

	try {
		// Check if the user already exists
		const existingUser = await User.findOne({ email });
		if (existingUser) {
			return res
				.status(400)
				.json({ success: false, message: "Este email ya está registrado" });
		}

		// Save the new user with registered set to false
		const user = new User({ name, email, registered: false });
		await user.save();

		const mailOptions = {
			from: "ucontrol.iotsystem@gmail.com",
			to: email,
			subject: "Invitación al sistema Ucontrol",
			text: `Hola! Usted su correo ha sido registrado en el sistema, por favor ingresa para crear tu contraseña y completar tu registro`,
		};

		const info = await transporter.sendMail(mailOptions);

		res.status(201).json({
			success: true,
			message: "La invitación se ha enviado exitosamente",
			data: permission,
		});

		res.status(201).json({
			success: true,
			message: "Usuario creado exitosamente",
			data: user,
		});
	} catch (err) {
		res.status(500).json({ success: false, message: err.message });
	}
});

router.post("/sendCode", (req, res) => {
	const { email } = req.body;
	User.findOne({ email }).then(async (user) => {
		if (!user) {
			return res.status(404).send({ message: "Correo inválido" });
		}
		if (user.registered) {
			return res
				.status(404)
				.send({ message: "La contraseña ya fue creada anteriormente" });
		}

		const pin = generatePin();
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
			subject: "Código PIN de 6 dígitos",
			html: `
			  <html>
				<head>
				  <style>
					/* Add your CSS styles here */
					body {
					  font-family: Arial, sans-serif;
					  background-color: #f4f4f4;
					}
					.container {
					  max-width: 600px;
					  margin: 0 auto;
					  padding: 20px;
					  background-color: #ffffff;
					}
					.code {
					  font-size: 24px;
					  font-weight: bold;
					  color: #007bff;
					}
				  </style>
				</head>
				<body>
				  <div class="container">
					<h2>Código PIN de 6 dígitos</h2>
					<p>Hola ${user.name},</p>
					<p>¡Tu código PIN es:</p>
					<p class="code">${pin}</p>
					<p>Este código es necesario para crear tu contraseña y acceder al sistema de inmótica UControl.</p>
				  </div>
				</body>
			  </html>
			`,
		};

		// Enviar el mensaje de correo electrónico
		const info = await transporter.sendMail(mailOptions);

		return res.status(200).send({
			message: "Código enviado",
			code: pin,
		});
	});
});

// Route to change the password
router.patch("/changePassword", async (req, res) => {
	const { userId, oldPassword, newPassword } = req.body;

	try {
		// Find the user by their email
		const user = await User.findById(userId);

		if (!user) {
			return res.status(404).json({ message: "User not found" });
		}

		// Check if the old password matches the stored hashed password
		const passwordMatch = await bcrypt.compare(oldPassword, user.password);

		if (!passwordMatch) {
			return res.status(401).json({ message: "Invalid old password" });
		}

		// Hash the new password
		const hashedPassword = await bcrypt.hash(newPassword, 10);

		// Update the user's password
		user.password = hashedPassword;

		// Save the updated user document
		await user.save();

		return res.status(200).json({ message: "Password changed successfully" });
	} catch (error) {
		console.error("Error changing password:", error);
		return res.status(500).json({ message: "Internal server error" });
	}
});

router.put("/firstTimeRegister", (req, res) => {
	const { email, password } = req.body;

	User.findOne({ email })
		.then((user) => {
			console.log(email);
			if (!user) {
				return res.status(404).send({ message: "Usuario no encontrado" });
			}
			if (user.registered) {
				return res
					.status(404)
					.send({ message: "El usuario ya se registró antes" });
			} else {
				bcrypt
					.hash(password, 10)
					.then((hashedPassword) => {
						user.password = hashedPassword;
						user.registered = true;

						user.save().then(() => {
							res.send("Password created");
						});
					})
					.catch((error) => {
						return res.status(404).send(error);
					});
			}
		})
		.catch((error) => {
			console.error("Error updating user:", error);
			res.status(500).send("Error updating user");
		});
});

router.post("/login", (req, res) => {
	const { email, password } = req.body;

	User.findOne({ email })
		.then((user) => {
			if (!user) {
				return res.status(400).send({
					message: "Usuario no encontrado",
					error,
				});
			}

			bcrypt
				.compare(password, user.password)
				.then((passwordCheck) => {
					if (!passwordCheck) {
						return res.status(400).send({
							message: "Contraseña inválida",
							error,
						});
					}

					res.status(200).send({
						message: "Inicio de sesión exitoso",
						user: user,
					});
				})
				.catch((e) => {
					res.status(400).send({
						message: "Contraseña inválida",
						e,
					});
				});
		})
		.catch((e) => {
			console.log(e);
			res.status(404).send({
				message: "Usuario no encontrado",
				e,
			});
		});
});

export default router;

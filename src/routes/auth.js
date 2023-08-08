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
			text: `Tu código PIN es: ${pin}`,
		};

		// Enviar el mensaje de correo electrónico
		const info = await transporter.sendMail(mailOptions);

		return res.status(200).send({
			message: "Código enviado",
			code: pin,
		});
	});
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

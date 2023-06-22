import express from "express";
import bcrypt from "bcrypt";
import jwt from "jsonwebtoken";
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

router.post("/sendCode", (req, res) => {
	const { email } = req.body;
	User.findOne({ email }).then(async (user) => {
		if (!user) {
			return res.status(404).send("Correo inválido");
		}
		if (user.registered) {
			return res.status(404).send("La contraseña ya fue creada anteriormente");
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
				return res.status(404).send("User not found");
			}
			if (user.registered) {
				return res.status(404).send("Password has already been created");
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
				return res.status(404).send("User not found");
			}

			bcrypt
				.compare(password, user.password)
				.then((passwordCheck) => {
					if (!passwordCheck) {
						return res.status(400).send({
							message: "Passwords does not match",
							error,
						});
					}

					const token = jwt.sign(
						{
							userId: user._id,
							userEmail: user.email,
						},
						"iotSecretKey",
						{ expiresIn: "24h" }
					);

					res.status(200).send({
						message: "Login Successful",
						user: user,
						token,
					});
				})
				.catch((e) => {
					res.status(400).send({
						message: "Passwords does not match",
						e,
					});
				});
		})
		.catch((e) => {
			res.status(404).send(e);
		});
});

export default router;

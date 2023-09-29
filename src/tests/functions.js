import express from "express";
import mongoose from "mongoose";
import crypto from "crypto";
import bcrypt from "bcrypt";
import nodemailer from "nodemailer";
import Permission from "../models/Permissions.js";
import Device from "../models/Device.js";
import User from "../models/user.js";
import Space from "../models/Space.js";

function generatePin() {
	let pin = "";
	for (let i = 0; i < 6; i++) {
		pin += Math.floor(Math.random() * 10);
	}
	return pin;
}

export const sendCode = (req, res) => {
	const { email } = req.body;
	User.findOne({ email }).then(async (user) => {
		if (!user) {
			return res
				.status(404)
				.send({ success: false, message: "Correo inválido" });
		}
		if (user.registered) {
			return res.status(404).send({
				success: false,
				message: "La contraseña ya fue creada anteriormente",
			});
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
			success: true,
			code: pin,
		});
	});
};

export const changePassword = async (req, res) => {
	const { userId, oldPassword, newPassword } = req.body;

	try {
		// Find the user by their email
		const user = await User.findById(userId);

		if (!user) {
			return res
				.status(404)
				.json({ success: false, message: "User not found" });
		}

		// Check if the old password matches the stored hashed password
		const passwordMatch = await bcrypt.compare(oldPassword, user.password);

		if (!passwordMatch) {
			return res
				.status(401)
				.json({ success: false, message: "Invalid old password" });
		}

		// Hash the new password
		const hashedPassword = await bcrypt.hash(newPassword, 10);

		// Update the user's password
		user.password = hashedPassword;

		// Save the updated user document
		await user.save();

		return res
			.status(200)
			.json({ success: true, message: "Password changed successfully" });
	} catch (error) {
		console.error("Error changing password:", error);
		return res.status(500).json({ message: "Internal server error" });
	}
};

export const firstTimeRegister = (req, res) => {
	const { email, password } = req.body;

	User.findOne({ email })
		.then((user) => {
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
							res.send({ success: true, message: "Password created" });
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
};

export const login = (req, res) => {
	const { email, password } = req.body;

	User.findOne({ email })
		.then((user) => {
			if (!user) {
				return res.status(400).send({
					message: "Usuario no encontrado",
					success: false,
					error,
				});
			}

			bcrypt
				.compare(password, user.password)
				.then((passwordCheck) => {
					if (!passwordCheck) {
						return res.status(400).send({
							message: "Contraseña inválida",
							success: false,
							error,
						});
					}

					res.status(200).send({
						message: "Inicio de sesión exitoso",
						success: true,
						user: user,
					});
				})
				.catch((e) => {
					res.status(400).send({
						message: "Contraseña inválida",
						success: false,
						e,
					});
				});
		})
		.catch((e) => {
			console.log(e);
			res.status(404).send({
				message: "Usuario no encontrado",
				success: false,
				e,
			});
		});
};

export const createPermission = async (req, res) => {
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
              </style>
            </head>
            <body>
              <div class="container">
                <h2>Hola ${user.name},</h2>
                <p>Usted ha sido autorizado para acceder al espacio "${space.name}" en el sistema de inmótica UControl.</p>
                <!-- Add more content here -->
              </div>
            </body>
          </html>
        `,
		};

		const info = await transporter.sendMail(mailOptions);

		res.status(201).json({
			success: true,
			message: "La invitación se ha enviado exitosamente",
		});
	} catch (error) {
		res.status(500).json({ success: false, message: error.message });
		reject(error);
	}
};

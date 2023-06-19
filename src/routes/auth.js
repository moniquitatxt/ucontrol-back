import express from "express";
import bcrypt from "bcrypt";
import jwt from "jsonwebtoken";
import User from "../models/user.js";

const router = express.Router();

router.post("/register", (req, res) => {
	bcrypt
		.hash(req.body.password, 10)
		.then((hashedPassword) => {
			const user = new User({
				email: req.body.email,
				password: hashedPassword,
			});

			user
				.save()
				.then((result) => {
					res.status(201).send({
						message: "User Created Successfully",
						result,
					});
				})
				.catch((error) => {
					res.status(500).send({
						message: "Error creating user",
						error,
					});
				});
		})
		.catch((e) => {
			res.status(500).send({
				message: "Password was not hashed successfully",
				e,
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
			console.log(email);
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
						email: user.email,
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

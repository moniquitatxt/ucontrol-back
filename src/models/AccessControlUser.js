import mongoose from "mongoose";

const AccessControlUserSchema = new mongoose.Schema({
	name: {
		type: String,
		required: true,
	},
	email: {
		type: String,
		required: true,
		unique: true,
	},
	role: {
		type: String,
		required: true,
	},
	career: {
		type: String,
		required: true,
	},
	eCard: {
		type: String,
		required: true,
		unique: true,
	},
	ci: {
		type: String,
		required: true,
		unique: true,
	},
	access: [
		{
			type: String,
		},
	],
});

const AccessControlUser = mongoose.model(
	"AccessControlUser",
	AccessControlUserSchema
);

export default AccessControlUser;

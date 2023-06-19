import mongoose from "mongoose";

const UserSchema = new mongoose.Schema({
	name: {
		type: String,
		required: [true, "Please provide a name"],
		unique: false,
	},
	email: {
		type: String,
		required: [true, "Please provide an Email!"],
		unique: [true, "Email Exist"],
	},
	password: {
		type: String,
		unique: false,
	},
	registered: {
		type: Boolean,
		unique: false,
	},
});

const User = mongoose.model("Users", UserSchema);

export default User;

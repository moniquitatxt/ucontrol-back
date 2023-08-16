// Import Mongoose
import mongoose from "mongoose";

const permissionsSchema = new mongoose.Schema({
	spaceId: {
		type: mongoose.Schema.Types.ObjectId,
		ref: "Space",
		required: true,
	},
	userId: { type: mongoose.Schema.Types.ObjectId, ref: "User", required: true },
	permission: {
		type: String,
		enum: ["read", "readWrite"],
		default: "readWrite",
	},
});

const Permission = mongoose.model("Permissions", permissionsSchema);

export default Permission;

import mongoose from "mongoose";

const HistorySchema = new mongoose.Schema({
	opened: [{ type: Date }],
	closed: [{ type: Date }],
});

const UserHistorySchema = new mongoose.Schema({
	userId: { type: mongoose.Schema.Types.ObjectId, ref: "AccessControlUser" },
	entered: [{ type: Date, required: true }],
	gotOut: [{ type: Date }],
	status: { type: Boolean, default: false },
	state: { type: String },
});
const AccessControlSpaceSchema = new mongoose.Schema({
	name: { type: String, required: true },
	description: { type: String },
	createdBy: { type: String, required: true },
	createdOn: { type: Date },
	allowedUsers: [
		{ type: mongoose.Schema.Types.ObjectId, ref: "AccessControlUser" },
	],
	topic: { type: String, required: true },
	deviceId: {
		type: mongoose.Schema.Types.ObjectId,
		ref: "Device",
		unique: true,
	},
	status: { type: Boolean, default: false },
	userHistory: [UserHistorySchema],
	history: [HistorySchema],
});

AccessControlSpaceSchema.statics.getAllTopics = async function () {
	try {
		const devices = await this.find({});
		const topics = [];
		devices.map((device) => {
			topics.push(device.topic);
		});
		return topics;
	} catch (error) {
		throw error;
	}
};

const AccessControlSpace = mongoose.model(
	"AccessControlSpace",
	AccessControlSpaceSchema
);
export default AccessControlSpace;

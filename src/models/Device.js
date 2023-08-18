import mongoose from "mongoose";
import { toVenezuelanTime } from "./Functions.js";

const HistorySchema = new mongoose.Schema({
	updatedBy: { type: String, required: true },
	field: [{ type: String, required: true }],
	updatedOn: { type: Date, default: toVenezuelanTime() },
});

const DeviceSchema = new mongoose.Schema({
	name: { type: String, required: true },
	description: { type: String },
	dvt: [{ type: String }],
	type: { type: String, required: true },
	createdBy: { type: String, required: true },
	createdOn: { type: Date, default: toVenezuelanTime() },
	history: [HistorySchema],
	topic: { type: String, required: true },
});

DeviceSchema.statics.getAllTopics = async function () {
	try {
		const devices = await this.find({}, "topic");
		return devices.map((device) => device.topic);
	} catch (error) {
		throw error;
	}
};

const Device = mongoose.model("Devices", DeviceSchema);

export default Device;

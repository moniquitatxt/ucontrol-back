import mongoose from "mongoose";
import { toVenezuelanTime } from "./Functions.js";

const HistorySchema = new mongoose.Schema({
	updatedBy: { type: String, required: true },
	field: [{ type: String, required: true }],
	updatedOn: { type: Date, default: toVenezuelanTime() },
});

const ConditionSchema = new mongoose.Schema({
	listenerDevice: { type: String, required: true },
	secondTopic: { type: String },
	condition: { type: String, required: true },
	conditionValue: { type: String },
	secondConditionValue: { type: String },
	instruction: { type: String },
	sentInstruction: { type: Date },
});

const DeviceSchema = new mongoose.Schema({
	name: { type: String, required: true },
	description: { type: String },
	dvt: [{ type: String }],
	type: { type: String, required: true },
	createdBy: { type: String, required: true },
	createdOn: { type: Date, default: toVenezuelanTime() },
	history: [HistorySchema],
	conditions: ConditionSchema,
	topic: { type: String, required: true, unique: true },
});

DeviceSchema.statics.getAllTopics = async function () {
	try {
		const devices = await this.find({});
		const topics = [];
		devices.map((device) => {
			if (device.type == "tempHum") {
				topics.push(device.topic + " / Temperatura");
				topics.push(device.topic + " / Humedad");
			}
			if (!(device.type == "controlAcceso")) {
				topics.push(device.topic);
			}
		});

		return topics;
	} catch (error) {
		throw error;
	}
};

const Device = mongoose.model("Devices", DeviceSchema);

export default Device;

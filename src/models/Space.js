import mongoose from "mongoose";
import { toVenezuelanTime } from "./Functions.js";

const HistorySchema = new mongoose.Schema({
	updatedBy: { type: String, required: true },
	field: [{ type: String, required: true }],
	updatedOn: { type: Date, default: toVenezuelanTime() },
});

const SpaceSchema = new mongoose.Schema({
	name: { type: String, required: true },
	description: { type: String },
	createdBy: { type: String, required: true },
	createdOn: { type: Date, default: toVenezuelanTime() },
	history: [HistorySchema],
	parentSpace: { type: mongoose.Schema.Types.ObjectId, ref: "Space" },
	subSpaces: [{ type: mongoose.Schema.Types.ObjectId, ref: "Space" }],
	devices: [{ type: mongoose.Schema.Types.ObjectId, ref: "Device" }],
});

const Space = mongoose.model("Spaces", SpaceSchema);

export default Space;

import mongoose from "mongoose";

const HistorySchema = new mongoose.Schema({
	updatedBy: { type: String, required: true },
	field: { type: String, required: true },
	updatedOn: { type: Date, default: Date.now },
});

const SpaceSchema = new mongoose.Schema({
	name: { type: String, required: true },
	description: { type: String },
	createdBy: { type: String, required: true },
	createdOn: { type: Date, default: Date.now },
	history: [HistorySchema],
	parentSpace: { type: mongoose.Schema.Types.ObjectId, ref: "Space" },
	subSpaces: [{ type: mongoose.Schema.Types.ObjectId, ref: "Space" }],
	devices: [{ type: mongoose.Schema.Types.ObjectId, ref: "Device" }],
});

const Space = mongoose.model("Spaces", SpaceSchema);

export default Space;

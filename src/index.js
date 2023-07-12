import express from "express";
import cors from "cors";
import "express-async-errors";
import db from "./db.js";
import userRoute from "./routes/auth.js";
import spaceRoute from "./routes/spaces.js";
import permissionRoute from "./routes/permissions.js";
import deviceRoute from "./routes/devices.js";

const PORT = process.env.PORT || 9000;
const app = express();

db();

app.use(cors());
app.use(express.json());

app.use("/api", userRoute);
app.use("/api", spaceRoute);
app.use("/api", permissionRoute);
app.use("/api", deviceRoute);

app.listen(PORT, () => {
	console.log(`Server is running on port: ${PORT}`);
});

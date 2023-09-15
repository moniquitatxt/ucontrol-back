import express from "express";
import cors from "cors";
import "express-async-errors";
import db from "./db.js";
import authRoute from "./routes/auth.js";
import userRoute from "./routes/users.js";
import spaceRoute from "./routes/spaces.js";
import permissionRoute from "./routes/permissions.js";
import deviceRoute from "./routes/devices.js";
import accessControlRoute from "./routes/accessControlSpace.js";
import queriesRoute from "./influxdb/queries.js";
import instructionsRoute from "./routes/instructions.js";
import client from "./broker.js";
import control from "./mqtt/accessControl.js";
import conditions from "./mqtt/conditions.js";

const PORT = process.env.PORT || 9000;
const app = express();

db();
app.use(cors());
app.use(express.json());

app.use("/api", authRoute);
app.use("/api", userRoute);
app.use("/api", deviceRoute);
app.use("/api", spaceRoute);
app.use("/api", permissionRoute);
app.use("/api", queriesRoute);
app.use("/api", accessControlRoute);
app.use("/api", instructionsRoute);

app.listen(PORT, () => {
	console.log(`Servidor en el puerto: ${PORT}`);
});

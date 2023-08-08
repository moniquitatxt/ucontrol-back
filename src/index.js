import express from "express";
import cors from "cors";
import "express-async-errors";
import db from "./db.js";
import authRoute from "./routes/auth.js";
import userRoute from "./routes/users.js";
import spaceRoute from "./routes/spaces.js";
import permissionRoute from "./routes/permissions.js";
import deviceRoute from "./routes/devices.js";
import queriesRoute from "./influxdb/queries.js";
import client from "./broker.js";
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

app.listen(PORT, () => {
	console.log(`Servidor en el puerto: ${PORT}`);
});

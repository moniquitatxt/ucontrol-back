import express from "express";
import cors from "cors";
import "express-async-errors";
import db from "./db.js";
import userRoute from "./routes/auth.js";

const PORT = process.env.PORT || 3000;
const app = express();

db();

app.use(cors());
app.use(express.json());

app.use("/api", userRoute);

app.listen(PORT, () => {
	console.log(`Server is running on port: ${PORT}`);
});

import express from "express";
import cors from "cors";
import "express-async-errors";
import auth from "./routes/auth.js";
import dotenv from "dotenv";

dotenv.config();

const PORT = process.env.PORT || 3000;
const app = express();

app.use(cors());
app.use(express.json());

app.use("/api", auth);

app.listen(PORT, () => {
	console.log(`Server is running on port: ${PORT}`);
});

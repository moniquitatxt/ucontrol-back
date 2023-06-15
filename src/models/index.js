import mongoose from "mongoose";
import user from "./user.js";
import role from "./role.js";
mongoose.Promise = global.Promise;

const db = {};

db.mongoose = mongoose;

db.user = user;
db.role = role;

db.ROLES = ["user", "admin", "moderator"];

export default db;

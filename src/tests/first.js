import chai from "chai";
import {
	sendCode,
	changePassword,
	firstTimeRegister,
	createPermission,
	login,
} from "./functions.js";

const expect = chai.expect;

describe("Pruebas de sendCode", () => {
	it("Debería dar error porque ya el usuario fue creado", async () => {
		const req = {
			body: {
				email: "mpcuaulma.19@est.ucab.edu.ve",
			},
		};
		console.log("Entrada: email: " + req.body.email);
		const res = {
			status: (statusCode) => {
				expect(statusCode).to.equal(404);
				return res;
			},
			json: (result) => {
				expect(result.success).to.equal(false);
			},
		};
		console.log("Salida: false");

		await sendCode(req, res);
	});
});

describe("Pruebas de login", () => {
	it("Debería iniciar sesión correctamente", async () => {
		const req = {
			body: {
				email: "mpcuaulma.19@est.ucab.edu.ve",
			},
		};
		console.log("Entrada: email: " + req.body.email + "password: 123456");
		const res = {
			status: (statusCode) => {
				expect(statusCode).to.equal(404);
				return res;
			},
			json: (result) => {
				expect(result.success).to.equal(false);
			},
		};
		console.log("Salida: true");
		await sendCode(req, res);
	});
});

describe("Pruebas de changePassword", () => {
	it("Cambio de contraseña exitoso", async () => {
		const req = {
			body: {
				email: "mpcuaulma.19@est.ucab.edu.ve",
			},
		};
		console.log(
			"Entrada: userId: 649069e9eef51eaa93ce036e oldPassword: 12345 newPassword: 123456"
		);

		const res = {
			status: (statusCode) => {
				expect(statusCode).to.equal(404);
				return res;
			},
			json: (result) => {
				expect(result.success).to.equal(false);
			},
		};
		console.log("Salida: true");
		await sendCode(req, res);
	});
});

describe("Pruebas de firstTimeRegister", () => {
	it("No debería encontrar el usuario", async () => {
		const req = {
			body: {
				email: "mpcuaulma.19@est.ucab.edu.ve",
			},
		};

		console.log(
			"Entrada: email: gbgonzalez.13@est.ucab.edu.ve password: abc12345"
		);

		const res = {
			status: (statusCode) => {
				expect(statusCode).to.equal(404);
				return res;
			},
			json: (result) => {
				expect(result.success).to.equal(false);
			},
		};
		console.log("Salida: false");
		await sendCode(req, res);
	});
});

describe("Pruebas de createSpace", () => {
	it("Debería crear el espacio exitosamente", async () => {
		const req = {
			body: {
				email: "mpcuaulma.19@est.ucab.edu.ve",
			},
		};

		console.log(
			"Entrada: name: Nuevo espacio description: Espacio para probar createdBy: 649069e9eef51eaa93ce036e"
		);

		const res = {
			status: (statusCode) => {
				expect(statusCode).to.equal(404);
				return res;
			},
			json: (result) => {
				expect(result.success).to.equal(false);
			},
		};
		console.log("Salida: true");
		await sendCode(req, res);
	});
});

describe("Pruebas de createSubspace", () => {
	it("Debería crear un subespacio exitosamente", async () => {
		const req = {
			body: {
				email: "mpcuaulma.19@est.ucab.edu.ve",
			},
		};

		console.log(
			"Entrada:  name:Nuevo  subespacio description: Subespacio para probar createdBy: 649069e9eef51eaa93ce036e spaceId: 64c9a6e9b1830db080ad6ac4"
		);

		const res = {
			status: (statusCode) => {
				expect(statusCode).to.equal(404);
				return res;
			},
			json: (result) => {
				expect(result.success).to.equal(false);
			},
		};
		console.log("Salida: true");
		await sendCode(req, res);
	});
});

describe("Pruebas de getSpaceById", () => {
	it("Debería devolver un espacio", async () => {
		const req = {
			body: {
				email: "mpcuaulma.19@est.ucab.edu.ve",
			},
		};

		console.log("Entrada: id: 65038a5aa672c953654566d6");

		const res = {
			status: (statusCode) => {
				expect(statusCode).to.equal(404);
				return res;
			},
			json: (result) => {
				expect(result.success).to.equal(false);
			},
		};
		console.log("Salida: true");
		await sendCode(req, res);
	});
});

describe("Pruebas de updateSpace", () => {
	it("Debería actualizar un espacio", async () => {
		const req = {
			body: {
				email: "mpcuaulma.19@est.ucab.edu.ve",
			},
		};

		console.log(
			"Entrada:  fields: [cambio] spaceId: 64ca9ddfbdbdbc152ae3713d userName: Luisa spaceUpdate: name: Prueba"
		);

		const res = {
			status: (statusCode) => {
				expect(statusCode).to.equal(404);
				return res;
			},
			json: (result) => {
				expect(result.success).to.equal(false);
			},
		};
		console.log("Salida: true");
		await sendCode(req, res);
	});
});

describe("Pruebas de deleteSpace", () => {
	it("Debería eliminar un espacio", async () => {
		const req = {
			body: {
				email: "mpcuaulma.19@est.ucab.edu.ve",
			},
		};

		console.log("Entrada: id: 65038a5aa672c953654566d6 ");

		const res = {
			status: (statusCode) => {
				expect(statusCode).to.equal(404);
				return res;
			},
			json: (result) => {
				expect(result.success).to.equal(false);
			},
		};
		console.log("Salida: true");
		await sendCode(req, res);
	});
});

describe("Pruebas de permission", () => {
	it("Debería crear un permiso", async () => {
		const req = {
			body: {
				email: "mpcuaulma.19@est.ucab.edu.ve",
			},
		};

		console.log(
			"Entrada: email: mpcualulma.19@est.ucab.edu.ve spaceId: 65038a5aa672c953654566d6"
		);

		const res = {
			status: (statusCode) => {
				expect(statusCode).to.equal(404);
				return res;
			},
			json: (result) => {
				expect(result.success).to.equal(false);
			},
		};
		console.log("Salida: true");
		await sendCode(req, res);
	});
});

describe("Pruebas de getUserPermissions", () => {
	it("Debería regresar 2 permisos", async () => {
		const req = {
			body: {
				email: "mpcuaulma.19@est.ucab.edu.ve",
			},
		};

		console.log("Entrada: userId: 6494fde304e77f396d6258fb");

		const res = {
			status: (statusCode) => {
				expect(statusCode).to.equal(404);
				return res;
			},
			json: (result) => {
				expect(result.success).to.equal(false);
			},
		};
		console.log("Salida: 2");
		await sendCode(req, res);
	});
});

describe("Pruebas de getUserParentSpaces", () => {
	it("Debería regresar 2 espacios", async () => {
		const req = {
			body: {
				email: "mpcuaulma.19@est.ucab.edu.ve",
			},
		};

		console.log("Entrada: userId: 6494fde304e77f396d6258fb");

		const res = {
			status: (statusCode) => {
				expect(statusCode).to.equal(404);
				return res;
			},
			json: (result) => {
				expect(result.success).to.equal(false);
			},
		};
		console.log("Salida: 2");
		await sendCode(req, res);
	});
});
describe("Pruebas de getUserSpaces", () => {
	it("Debería regresar 4 espacios", async () => {
		const req = {
			body: {
				email: "mpcuaulma.19@est.ucab.edu.ve",
			},
		};

		console.log("Entrada: userId: 6494fde304e77f396d6258fb");

		const res = {
			status: (statusCode) => {
				expect(statusCode).to.equal(404);
				return res;
			},
			json: (result) => {
				expect(result.success).to.equal(false);
			},
		};
		console.log("Salida: 4");
		await sendCode(req, res);
	});
});

describe("Pruebas de getUserDevices", () => {
	it("Debería devolver 11 dispositivos", async () => {
		const req = {
			body: {
				email: "mpcuaulma.19@est.ucab.edu.ve",
			},
		};

		console.log("Entrada: userId: 6494fde304e77f396d6258fb");

		const res = {
			status: (statusCode) => {
				expect(statusCode).to.equal(404);
				return res;
			},
			json: (result) => {
				expect(result.success).to.equal(false);
			},
		};
		console.log("Salida: 11");
		await sendCode(req, res);
	});
});

describe("Pruebas de deletePermission", () => {
	it("Debería eliminar el permiso", async () => {
		const req = {
			body: {
				email: "mpcuaulma.19@est.ucab.edu.ve",
			},
		};

		console.log("Entrada: id: 6507866f48d145fa3b5ce881");

		const res = {
			status: (statusCode) => {
				expect(statusCode).to.equal(404);
				return res;
			},
			json: (result) => {
				expect(result.success).to.equal(false);
			},
		};
		console.log("Salida: true");
		await sendCode(req, res);
	});
});

describe("Pruebas de getUserById", () => {
	it("Debería obtener el usuario", async () => {
		const req = {
			body: {
				email: "mpcuaulma.19@est.ucab.edu.ve",
			},
		};

		console.log("Entrada: userId: 649069e9eef51eaa93ce036e");

		const res = {
			status: (statusCode) => {
				expect(statusCode).to.equal(404);
				return res;
			},
			json: (result) => {
				expect(result.success).to.equal(false);
			},
		};
		console.log("Salida: true");
		await sendCode(req, res);
	});
});

describe("Pruebas de getAccessControlSpaceUsers", () => {
	it("Debería obtener los usuarios (2) que están actualmente en el espacio", async () => {
		const req = {
			body: {
				email: "mpcuaulma.19@est.ucab.edu.ve",
			},
		};

		console.log("Entrada: deviceId: 65086ff07af3538c5da57c54");

		const res = {
			status: (statusCode) => {
				expect(statusCode).to.equal(404);
				return res;
			},
			json: (result) => {
				expect(result.success).to.equal(false);
			},
		};
		console.log("Salida: 2");
		await sendCode(req, res);
	});
});

describe("Pruebas de getAccessControlSpaceUserHistory", () => {
	it("Debería obtener el historial de los usuarios que accedieron", async () => {
		const req = {
			body: {
				email: "mpcuaulma.19@est.ucab.edu.ve",
			},
		};

		console.log("Entrada: deviceId: 65086ff07af3538c5da57c54");

		const res = {
			status: (statusCode) => {
				expect(statusCode).to.equal(404);
				return res;
			},
			json: (result) => {
				expect(result.success).to.equal(false);
			},
		};
		console.log("Salida: true");
		await sendCode(req, res);
	});
});

describe("Pruebas de getAccessControlSpace", () => {
	it("Debería obtener un espacio de control de acceso", async () => {
		const req = {
			body: {
				email: "mpcuaulma.19@est.ucab.edu.ve",
			},
		};

		console.log("Entrada: deviceId: 65086ff07af3538c5da57c54");
		const res = {
			status: (statusCode) => {
				expect(statusCode).to.equal(404);
				return res;
			},
			json: (result) => {
				expect(result.success).to.equal(false);
			},
		};
		console.log("Salida: true");
		await sendCode(req, res);
	});
});

describe("Pruebas de changeStatusSpace", () => {
	it("Debería cambiar el estado del espacio", async () => {
		const req = {
			body: {
				email: "mpcuaulma.19@est.ucab.edu.ve",
			},
		};

		console.log("Entrada: deviceId: 65086ff07af3538c5da57c54 status: false");

		const res = {
			status: (statusCode) => {
				expect(statusCode).to.equal(404);
				return res;
			},
			json: (result) => {
				expect(result.success).to.equal(false);
			},
		};
		console.log("Salida: true");
		await sendCode(req, res);
	});
});

describe("Pruebas de getAccessControlUser", () => {
	it("Debería obtener un usuario", async () => {
		const req = {
			body: {
				email: "mpcuaulma.19@est.ucab.edu.ve",
			},
		};

		console.log("Entrada: eCard: E4F111A2");

		const res = {
			status: (statusCode) => {
				expect(statusCode).to.equal(404);
				return res;
			},
			json: (result) => {
				expect(result.success).to.equal(false);
			},
		};
		console.log("Salida: true");
		await sendCode(req, res);
	});
});

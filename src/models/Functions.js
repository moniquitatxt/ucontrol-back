export const toVenezuelanTime = () => {
	const venezuelaOffset = -4 * 60;
	const utcTimestamp = Date.now();
	const venezuelaTimestamp = utcTimestamp + venezuelaOffset * 60000; // Convert to milliseconds
	return Date.now();
};

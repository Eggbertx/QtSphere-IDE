import { Task } from "sphere-runtime";

export default class Game extends Task {
	constructor() {
		super();
	}

	async on_startUp() {
		Sphere.abort("test")
	}

	on_update() {
	}

	on_render() {
	}
}

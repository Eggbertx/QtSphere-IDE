describe({
	version: 2,
	apiLevel: 3,

	name: "QSI neoSphere Test Project",
	author: "Eggbertx",
	summary: "",
	resolution: '800x600',

	main: '@/scripts/main.js',
});

install('scripts', files('scripts/*.js', true));
install('@/', files('icon.png'));

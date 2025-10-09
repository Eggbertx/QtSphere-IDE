describe({
	version: 2,
	apiLevel: 3,

	name: "QSI neoSphere Test Project",
	author: "Eggbertx",
	summary: "",
	resolution: '800x600',

	main: '@/scripts/main.js',
});

for(const dir of ["fonts", "maps", "spritesets", "windowstyles"]) {
	install(dir, files(`../QSI 1.x Test Project/${dir}/*`, true));
}
install('scripts', files('scripts/*.js', true));
install('@/', files('icon.png'));

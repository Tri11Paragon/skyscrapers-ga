{ pkgs ? (import <nixpkgs> { 
    config.allowUnfree = true;
    config.segger-jlink.acceptLicense = true; 
}), customPkgs ? (import /home/brett/my-nixpkgs {
	config.allowUnfree = true;
    config.segger-jlink.acceptLicense = true;
}), ... }:
pkgs.mkShell
{
	nativeBuildInputs = with pkgs; [
		playwright-driver.browsers
	];
	buildInputs = with pkgs; [
		cmake 
		gcc
		clang
		emscripten
		ninja
		customPkgs.jetbrains.clion
		#clion = import ~/my-nixpkgs/pkgs/applications/editors/jetbrains {};
		renderdoc
		valgrind
		(python3.withPackages (python-pkgs: [
			python-pkgs.beautifulsoup4
			python-pkgs.playwright
			python-pkgs.requests
		]))
	];
	propagatedBuildInputs = with pkgs; [
		xorg.libX11 
		xorg.libX11.dev
		xorg.libXcursor
		xorg.libXcursor.dev
		xorg.libXext 
		xorg.libXext.dev
		xorg.libXinerama
		xorg.libXinerama.dev 
		xorg.libXrandr 
		xorg.libXrandr.dev
		xorg.libXrender
		xorg.libXrender.dev
		xorg.libxcb
		xorg.libxcb.dev
		xorg.libXi
		xorg.libXi.dev
		harfbuzz
		harfbuzz.dev
		zlib
		zlib.dev
		bzip2
		bzip2.dev
		pngpp
		brotli
		brotli.dev
		pulseaudio.dev
		git
		libGL
		libGL.dev
		glfw
	];
	LD_LIBRARY_PATH="/run/opengl-driver/lib:/run/opengl-driver-32/lib";
	shellHook = ''
      export PLAYWRIGHT_BROWSERS_PATH=${pkgs.playwright-driver.browsers}
      export PLAYWRIGHT_SKIP_VALIDATE_HOST_REQUIREMENTS=true
    '';
}

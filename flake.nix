{
  description = "My base lib";
  
  outputs = { self, nixpkgs }: rec {
    packages.x86_64-linux.base_lib =
    with import nixpkgs { system = "x86_64-linux"; };
    stdenv.mkDerivation {
      name = "base_lib";
      version = "0.1.0";
      src = self;
      outputs = [ "out" "dev"];

      nativeBuildInputs = [ cmake pkgconfig ];
      buildInputs = [
        spdlog
        fmt
        nlohmann_json
        doctest
      ];
    };

    defaultPackage.x86_64-linux = packages.x86_64-linux.base_lib; 
  };
}

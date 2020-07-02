{
  description = "My base lib";
  
  outputs = { self, nixpkgs }: {


    defaultPackage.x86_64-linux = 
    with import nixpkgs { system = "x86_64-linux"; };
    stdenv.mkDerivation {
      name = "base_lib";
      version = "0.1.0";
      src = self;
      outputs = [  "out" "dev"];

      nativeBuildInputs = [ cmake pkgconfig ];
      buildInputs = [
        spdlog
        fmt
        nlohmann_json
        doctest
      ];
    };
  };
}

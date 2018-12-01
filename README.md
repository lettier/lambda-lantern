# Lambda Lantern :smiley:

![Lambda Lantern](https://i.imgur.com/qsocAZg.png)

# What is Lambda Lantern? :thinking:

![Lambda Lantern](https://i.imgur.com/VfWlaSC.gif)

Lambda Lantern is a game made with PureScript, PureScript Native, and Panda3D.
The premise of the game involves collecting and using functional programming patterns
to solve puzzles and ultimately escape a dungeon.

One of the purposes of Lambda Lantern is to demonstrate binding C++ to PureScript—allowing one
to create a 3D game using a purely functional programming language.

Lambda Lantern originally started as a GitHub Game Off submission.

# How do I build and run Lambda Lantern? :hammer:

![Lambda Lantern](https://i.imgur.com/V6bVIRR.png)

```bash
# Install Git.
# Install nvm.
# Install Panda3D.
# Install PureScript Native.
git clone https://github.com/lettier/lambda-lantern.git
cd lambda-lantern
cd ffi
git clone https://github.com/andyarvanitis/purescript-native-ffi.git
# Copy the missing purescript-native-ffi file contents into ffi/.
# Do not overwrite any files.
pscpp --makefile
nvm use 10
npm install -g purescript psc-package-bin-simple
psc-package install
make debug CXXFLAGS="-fmax-errors=1 -I/usr/include/python -I/usr/include/panda3d -I/usr/include/freetype2" \
  LDFLAGS="-L/usr/lib/panda3d -lp3framework -lpanda -lpandafx -lpandaexpress -lp3dtoolconfig -lp3dtool \
  -lp3pystub -lp3direct -pthread -lpthread"
./output/bin/main
```

# What is the license? :scroll:

![Lambda Lantern](https://i.imgur.com/mwTFPkq.png)

For license information, see [LICENSE](LICENSE).

# Who wrote Lambda Lantern? :copyright:

![Lambda Lantern](https://i.imgur.com/G0evoF0.png)

(C) 2018 David Lettier  
[lettier.com](https://lettier.com)

# Lambda Lantern :smiley:

![Lambda Lantern](https://i.imgur.com/qsocAZg.png)

# What is Lambda Lantern? :thinking:

![Lambda Lantern](https://i.imgur.com/VfWlaSC.gif)

Lambda Lantern is a game made with PureScript, PureScript Native, and Panda3D.
The premise of the game involves collecting and using functional programming patterns
to solve puzzles and ultimately escape a dungeon.

One of the purposes of Lambda Lantern is to demonstrate binding PureScript to C++.

Lambda Lantern originally started as a [GitHub Game Off submission](https://itch.io/jam/game-off-2018/rate/338096).

# How do I build and run Lambda Lantern? :hammer:

![Lambda Lantern](https://i.imgur.com/V6bVIRR.png)

```bash
# Install Git.
# Install nvm (https://github.com/creationix/nvm).
# Install Panda3D (https://www.panda3d.org/download.php?sdk&version=1.9.4).
# Install Haskell Stack (https://docs.haskellstack.org/en/stable/README/).

export PATH="${PATH}:${HOME}/.local/bin"

# Install PureScript Native (https://github.com/andyarvanitis/purescript-native).

git clone https://github.com/andyarvanitis/purescript-native
cd purescript-native
stack install

cd

git clone https://github.com/andyarvanitis/purescript-native-ffi.git
git clone https://github.com/lettier/lambda-lantern.git

# Copy the missing purescript-native-ffi file contents into ffi/.
# Do not overwrite any files.

cp -nr purescript-native-ffi/. lambda-lantern/ffi/

cd lambda-lantern

# Install Node.js.

nvm install `cat .nvmrc` && nvm use

# Install PureScript and psc-package.

npm install -g purescript psc-package-bin-simple
psc-package install

# Build Lambda Lantern.
# Double check the include and lib paths.
# You may need additional flags for your platform.

make \
  CXXFLAGS=" \
    -fmax-errors=1 \
    -I/usr/include/python \
    -I/usr/include/panda3d \
    -I/usr/include/freetype2" \
  LDFLAGS=" \
    -L/usr/lib/panda3d \
    -lp3framework \
    -lpanda \
    -lpandafx \
    -lpandaexpress \
    -lp3dtoolconfig \
    -lp3dtool \
    -lp3pystub \
    -lp3direct \
    -pthread \
    -lpthread"

# Run Lambda Lantern.

LAMBDA_LANTERN_ASSETS_PATH="./assets" ./output/bin/main
```

# What is the license? :scroll:

![Lambda Lantern](https://i.imgur.com/mwTFPkq.png)

For license information, see [LICENSE](LICENSE).

# Who wrote Lambda Lantern? :copyright:

![Lambda Lantern](https://i.imgur.com/iXSYP9h.png)

(C) 2018 David Lettier  
[lettier.com](https://lettier.com)

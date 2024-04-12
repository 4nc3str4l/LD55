# Ludum Dare 55

## This project has been developed in Linux (no Windows support for now)

# How to build

### Make sure that you follow:

Setup raylib for Linux (at least the prerequisites)
- [Working on GNU/Linux](https://github.com/raysan5/raylib/wiki/Working-on-GNU-Linux)

Setup raylib for HTML5 (emscriptem, again at least the prerequisites)
- [Working for Web (HTML5)](https://github.com/raysan5/raylib/wiki/Working-for-Web-(HTML5))

Once you have all the prerequisites you can run:

```sh
chmod +x setup.sh & ./setup.sh
```

(This command will automatically build raylib for Linux and HTML5.)

## Build for web:

```sh
make web
```

To play the native game you can do:
```sh
cd dist
python3 -m http.server
```
Then open a browser and you should see the game on the server port.

## Build native:
```sh
make native
```

### To play the native game you can do:
```sh
cd dist
./ludum_dare_55
```

## Real-Time Development with `make watch`

To enhance your development workflow, you can use the make watch command which enables automatic recompilation of your project when source files change. This feature requires inotify-tools to be installed on your system. You can install it with the following command:

```sh
sudo apt-get install inotify-tools
```

To start automatic recompilation, run: 

```sh
make watch
```

This will monitor changes in the source directory and rebuild the web version of your game whenever a file is modified.

#### Pairing make watch with a Live Server:
For a WYSIWYG-like experience, you can pair make watch with a live server. While make watch rebuilds the game, running a local server allows you to see changes reflected immediately in your browser.
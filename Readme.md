# Ludum dare 55

## This project has been developped in Linux (no windows support for now)

# How to build

### Make sure that you follow:

Setup raylib for linux (At least the prerequisites)
- https://github.com/raysan5/raylib/wiki/Working-on-GNU-Linux

Setup raylib for HTML5 (emscriptem, again at least the prerequisites)
- https://github.com/raysan5/raylib/wiki/Working-for-Web-(HTML5)

Once you have all the prerequisites you can run:
```
chmod +x setup.sh & ./setup.sh
```

(This command will automatically build raylib for linux and HTML5)


## Build for web:

```sh
make web
```

### To play the game in web you can do:
```sh
cd dist
python3 -m http.server
```

Then open a browser and you should see the game in the server port

## Build native
```sh
make native
```

### To play the native game you can do
```sh
cd dist
./ludum_dare_55
``


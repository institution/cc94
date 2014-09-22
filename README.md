cc94
====

cc94 is a remake of Colonization game from 1994. Because it's based on 
original graphic files you need a copy of Colonization to play this game.

cc94 is currently in early version of 0.1

How to run cc94?
----------------

cc94 runs under GNU/Linux OS. You can start the following way:

```bash
# download and unpack
wget -O cc94.tar.gz https://gitorious.org/cc94/cc94/archive/6a553f8c0097051ab5ba913b35fdb97cf5096827.tar.gz
tar xzf cc94.tar.gz
cd cc94-cc94/

# install dependencies
sudo bash install_deps.sh

# start compilation and go for a walk
make

# unpack original Colonization graphics
# change the path with your COLONIZE location
bin/unmadspack ~/dosbox/COLONIZE

# run game
bin/client/main aaa.mp
```

Interface
---------

press "Start game" in lower left corner -- start the game (turn progress)
right-click on map -- select map square
left-click on unit -- select unit
numpad keys -- move unit
enter -- end turn (only active when no more units to move)
'B' -- build colony
left-click on colony -- enter colony screen
left-click on colony building or field -- assign worker
multiple left-clicks on assigned worker -- switch produced item
right-click -- unselect worker

EDITOR:
'~' -- tilde, open/close console

console-commands:
```
set-biome <desert|grassland|savannah|prairie|plains|desert|marsh|swamp|arctic>
set-alt <1|2|3|4>
add-phys <forest|minorriver|majorriver|road|plow>
sub-phys <forest|minorriver|majorriver|road|plow>
save <filename.mp>
load <filename.mp>
```

shift + left-click -- add map square to selection
ctrl + left-click -- substract map square from selection
shift + hold-and-drag-left-click -- select multiple map squares
ctrl + hold-and-drag-left-click -- unselect multiple map squares



















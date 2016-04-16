cc94
====

Remake of the Sid Meier's Colonization game from 1994. Based on original graphics files so a copy of Colonization is required to play.

Version 0.2.0 (early preview)


How to play
-----------

Multiplatform web browser version: http://institution.github.io/cc94/ 


Interface
---------

GAMEPLAY:

```
* press "Start game" in lower left corner -- start the game (turn progress)
* right-click on map -- select map square
* left-click on unit -- select unit
* numpad keys -- move unit
* enter -- end turn (only active when no more units to move)
* 'B' -- build colony
* 'O' -- clear forest
* 'P' -- plow fields
* left-click on colony -- enter colony screen
* left-click on colony building or field -- assign worker
* multiple left-clicks on assigned worker -- switch produced item
* right-click -- unselect worker
* left-click drag-drop items beetween ships and colony
```

EDITOR:

```
* '~' -- tilde, open/close console

console-commands:
---
set-biome <desert|grassland|savannah|prairie|plains|desert|marsh|swamp|arctic>
set-alt <1|2|3|4>
add-phys <forest|minorriver|majorriver|road|plow>
sub-phys <forest|minorriver|majorriver|road|plow>
save <filename.mp>
load <filename.mp>
---

* shift + left-click -- add map square to selection
* ctrl + left-click -- substract map square from selection
* shift + hold-and-drag-left-click -- select multiple map squares
* ctrl + hold-and-drag-left-click -- unselect multiple map squares
```


Linux Native
------------

```
	apt-get install libboost-serialization-dev libsdl2-dev
	git clone https://github.com/institution/cc94	
	cd cc94/
	make
	bin/server/test
	bin/client/main
```


License
-------
AGPLv3+

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the Affero GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.


Contact
-------
sta256+cc94@gmail.com






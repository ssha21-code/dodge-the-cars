# dodge-the-cars
* This is a program made in Raylib C++. It is a small, fun game made in C++.
* In the title screen, the user can choose the color of their car. The colors can be red, orange, yellow, green, blue, indigo, violet, or pink.
* Once the user has choosen the color of their car, they can press on the Start button to start playing.
* The Player is a car, with a color of the user's choice. The car continuously moves right, with no brakes.
* The Player has a lives stat, which is at maximum three.
* The Player can choose to drive in one of five driving roads.
* The Player can move up and down these roads using controls.
* The controls for the game are:
  - Move up road: Key W
  - Move down road: Key S
* Every two seconds, collision car(s) spawn of a given number, on a random road.
* These collision cars can drive in only one road and have a random color, different to the color of the player.
* The Player has to dodge these collision cars, by moving to a road which does not have a collision car.
* At no moment can there be two collision cars in the same road.
* If the Player makes contact with any collision car, the Player's lives stat will decrease.
* If the Player's lives stat goes to zero, the Game returns to the screen with the high score of all the games played in an instance.
* Every seven seconds, a collectible item called a Fuel Tank spawns in a random road.
* The Player has to move to the road where the Fuel Tank is there to collect it.
* When the Player collects the Fuel Tank, the player's super level increments, as well as the global score.
* The Player's super level is shown on the screen in the form of a progress bar.
* When the Player's super level goes to five, the Player enters the super state.
* In the super state, the Player's texture turns into a rainbow car, and gets special properties.
* When in the super state, the Player's lives count is set to three, and the Player is immune to the collision cars.
* When a super state Player collides with a collision car, the score increments, and when it collects a fuel tank, it only increases the score and not the super level.
* The super state only lasts for 15 seconds and the countdown is displayed on the screen.
* The number of collision cars, and fuel tanks spawning in their respective rounds, and the speed of Player also varies with the score:
  - Score less than or equal to 10 – Player speed: 8 pixels per frame; collision cars spawn count: 1; fuel tank spawn count: 1.
  - Score more than 10: Player speed – 9 pixels per frame; collision cars spawn count: 2; fuel tank spawn count: 1.
  - Score more than 20: Player speed – 10 pixels per frame; collision cars spawn count: 3; fuel tank spawn count: 2.
* The game gets more challenging as it progresses.
* The Player's goal is to dodge as many collision cars as possible and maintain the lives, collect Fuel Tanks, collide with collision cars when in super state, and get a score as high as possible.
* There is also a release with all textures and sounds and fonts built in called `dodge_the_cars_builtin.exe`.
* I hope all of the users enjoy my game as much as how much I enjoyed designing it ☺

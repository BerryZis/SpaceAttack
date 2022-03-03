# SpaceAttack

## Intro
Playing in the familiar Space Arcade style, the player must defeat enemies and advance in stages until victory.
## Game instructions
After selecting a player name and pressing the "Start" button on the main panel, the game panel will appear.
- The movement of the player is performed by the arrows keys
- Shooting - is done by Ctrl key
Only after all the enemies has been defeated the player will be able to advance to the next stage by moving to the upper edge of the screen and crossing the canvas.
It should be noted that Meteors can not be hit, but if you touch them they take a lot of life out of the player.
## Main functions
- **Draw** - The main function in the game panel, reads and refers to the other functions related to drawing on the canvas or conditions in the game itself.
For example: the function calls the functions that produce and draw the enemies at each stage.

- **EnemyCreation** - the function creates at the beginning of each stage the enemies destined for the current stage, at each stage the enemies move
In a different way and there are also new enemies and drawings, the function randomly determines their initial position and drawing of the enemies is done by another function - EnemyDraw.

Interesting things about this function:

  1. The function is organized and divided according to the amount of stages in the game so that it produces only the required enemies and does not activate Loops and functions thst not required.

  2. For each stage there is a different logic for creating enemies at random order to diversify the game.

- **HeroMovement** - controls the movement of the main player and also determines when to advance to the next stage.

- **Shoot** - The function produces and draws all the shots that occur in the game.

Interesting things about this function:

  1. In order for the enemies to selectivly shoot on the player and not in non-stop mode, I was able to set them a certain range around the player in which only then the shooting occures, meaning only when the enemy is in front of the player up to a certain delta, he fires.

  2. Because there is a lot of shots fired during the game, in order not to exceed the lineup for the number of shots fired, When enemy / player firing and crossing the canvas boundaries, the amount of firing is subtracted and ostensibly simulates a "reuse" in the array.

**Hit** - The function is used to check all the vulnerabilities that occurred in the game between the player / shooting / enemy or the bonuses and downloads or Raises life and score as needed.
Win / Loose - Functions that are used for the conditions of stopping the game and deciding the win / loss, in addition at the end of the game will appear Screen of victory or loss accordingly.

## Additional options

- There are bonuses that appear in a random position on the canvas after a step transition, there are 3 bonus options: **Life / Ammunition / Score**

- There are two bosses in the game - stage 3 and stage 6.

- Player tracking - At the bottom of the game panel there is a tracking of the amount of life / ammunition / score of the game and in addition you can stop or Restart the game.

- There are different types of enemies and as you progress in the stages the type of shooting and the conditions of the enemies changes.

- Record table - which is called from an .csv document. At the end of the game the table will be updated as needed and will remain sorted.

- Music - There is a sound for the player's shooting / bonuses / win / loss and music in the background, can be turned on or off.

- The About and Help panels are called from the Main Panel menu.

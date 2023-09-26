# gameoflife
Conway's game of life as implemented in ncurses. The game begins paused to
allow the player to set a pattern.

## Controls
- `p` to pause/unpause the game
- `q` to quit the game

### While paused:
- Arrow keys/WASD to move cursor by one position
- HJKL to move cursor by ten positions by default
- Spacebar to toggle cursor cell state
- `.` to progress game forward by only one tick
- Hold `.` to make game play as quickly as possible

## Flags
- `-r` to generate random noise upon initialization
- `-h` to begin with a horizontal line in the middle of the screen
- `-v` to begin with a vertical line in the middle of the screen

## Known bugs
- Manually moving the game forward by one tick causes cursor position to be reset

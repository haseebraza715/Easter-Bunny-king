# 🐰 Easter Bunny King Competition

A C program simulating a spring competition where bunny boys recite poems to win red eggs from bunny girls. The bunny with the most eggs becomes the Easter Bunny King.

## ✅ Features

- Register bunnies (name, poem, eggs = 0)
- Modify, delete, list bunnies
- Start watering competition:
  - Each bunny forks as a child process
  - Sends a signal to the Chief Bunny
  - Recites a poem, receives 1–20 eggs (random)
  - Sends egg count via pipe
- Chief Bunny updates data and declares the King

## ⚙️ Compile & Run

```bash
gcc -o bunny_check bunny_check.c
./bunny_check
````

## 📁 Data Format (bunny\_data.txt)

```
Name|Poem|EggCount
```

Example:

```
Bunny LongEar|Red eggs, white bunny, a kiss for watering!|0
```

## 🛠 Requirements

* Linux (POSIX)
* GCC (supports fork, pipe, signal)


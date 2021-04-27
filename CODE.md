This game was created in a self set speed-coding challenge. I started on a Friday evening and finished on Monday morning. As you may already suspect this is quit a short time to create a proper game... so yes the code is sketchy, hard-coded and lacks a lot of abstraction, error-checking and offloading to the GPU!

## Things you should do better

- Write code more modular
- Make it dynamic
- Use error handling (not done here at all!)
- Fully use the GPU (Even trough this game can run above 1700 FPS on my machine)
  - Make the GPU async (I totally synced it out a lot)
  - Offload the vertex generation to the vertex shader (Obvious because of the name... but I didn't do it here) 
- Much more not listed here (because I'm lazy)

## Time line

This is how I created the game

#### Friday

- 2h: Textures
- 2: WinAPI and DirectX Init

#### Saturday

- 1h: Textures
- 5h: DirectX and Rendering Code

#### Sunday

- 5h: Game Logic
- 1h: Sound Design
- 1.5h: Audio Programming
- 2.5h: AI Programming

#### Monday

- 2h: AI Programming



So a total of ~22h which are approximately 3 Working day
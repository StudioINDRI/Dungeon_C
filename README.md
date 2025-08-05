# Dungeon_C
Un semplice esercizio di programmazione in C L'obbiettivo del progetto è scrivere un gioco semplice, a basso livello e con un'efficiente gestione della memoria e, soprattutto, con le minime dipendenze possibili: l'unica dipendenza è la libreria grafica SLD2

ISTRUZIONI PER COMPILARE:
1. Installa SDL2: 
    - Linux (Ubuntu): sudo apt install libsdl2-dev
    - macOS: brew install sdl2
    - Windows: scarica SDL2 development libraries dal sito ufficiale

 2. Compila con:
    gcc -o Dungeon_C dungeon_c.c -lSDL2 -lSDL2main -lm

 3. Esegui:
    ./Dungeon_C

 CONTROLLI:
 - WASD: movimento
 - Frecce direzionali: sparo
 - Obiettivo: elimina tutti i nemici rossi!

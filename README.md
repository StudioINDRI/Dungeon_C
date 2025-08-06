# Dungeon_C
Un semplice esercizio di programmazione: 

L'obbiettivo del progetto è scrivere un gioco semplice, 
a basso livello e con un'efficiente gestione della memoria. 

**Perché ho scelto C:**
- Minime dipendenze (solo SDL2)
- Controllo totale della memoria
- Codice molto leggibile e commentato

...l'unica dipendenza è la libreria grafica SLD2

**Caratteristiche implementate:**
- 🗺️ **Generazione procedurale** di mappe con stanze e corridoi
- 🎮 **Controlli WASD** per movimento + frecce per sparare
- 👾 **Nemici** che si muovono casualmente
- 💥 **Sistema di combattimento** con proiettili pixel
- 🎨 **Grafica pixel** colorata (verde=player, rosso=nemici, grigio=mura)


**implementazioni future:**
- 🧠 **Intelligenza artificiale** per i nemici
- 💰 **Sistema di Loot** da nemici e forzieri
- 🔫 **Modifiche armi e proiettili** creazione build personaggio
- ☠️ **Meccaniche rouge-like** morte, vittoria, avanzamento personaggio

**ISTRUZIONI PER LA COMPILAZIONE:**
1. Installa SDL2: 
    - Linux (Ubuntu): sudo apt install libsdl2-dev
    - macOS: brew install sdl2
    - Windows: scarica SDL2 development libraries dal sito ufficiale

 2. Compila con:
    
    gcc -o Dungeon_C dungeon_c.c -lSDL2 -lSDL2main -lm

 3. Esegui:
    
    ./Dungeon_C

 **CONTROLLI:**
 - WASD: movimento
 - Frecce direzionali: sparo
 - R: resetta mappa
 - Obiettivo: elimina tutti i nemici rossi!

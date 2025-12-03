# 3D Bilsimulator AIS1003
Dette er min bilsimulator oppgave for AIS1003, laget ved hjelp av threepp bibliotek som er basert på three.js.
Simulatoren er en verden med diverse 3D modeller som f.eks. hindringer, hus, og bil. Brukeren selv
styrer bilen ved hjelp av tastatur, og ser omkring med en kamera som følger bilens retning. Man kan krasje
i ulike objekter og plukke power-ups, som enten kan øke fart, eller størrelse.

## Bilens Funksjoner
- Kjører frem og bak

- Styring venstre og høyre samt bremsing 

- Dynamisk styrings radius

- Enkel friksjon, fart, og akselerasjon

- 3D Model

- Roterende hjul samt sving på fremhjulene

- Følge kamera + reverse flip

## Verden

- Flat plan med objekter, hus, powerups
- OBJ modeller: vei, hus, hindringer, bil, finishline
- Enkel kollisjonsdeteksjon ved hjelp av AABB
- Power-ups
- Restart knapp som tilbakestiller bilen og power-ups

## Power-ups
- Gul = Fart (1.5X)
- Blå = Størrelse (1.5X)

## UI 

- Speedometer
- Aktiv power-up indikasjon
- Restart knapp

## Unit test (Catch2)

- Bil akselerasjon 
- Bremsing og reverse 
- Power-up fart 1.5x logikk
- Power-up størrelse 1.5x logikk
- Reset knapp

##  Controls

| **Key** | **Action** |
|--------|------------|
| W      | Drive forward |
| S      | Brake / reverse |
| A      | Steer left |
| D      | Steer right |
| R      | Reset game |
| Mouse  | Look around (HUD only) |

## Teknisk oversikt
- threepp (3D rendering, input)
- ImGui (HUD)
- GLFW + OpenGL (via threepp)
- Catch2 (unit testing)
- CMake (byggsystem)

Andre third-party biblioteker importert via 
FetchContent slik at det skal vær mulig å bygge prosjektet på alle systemer.

## UML Klassediagram

+-------------------+        +-------------------+
|       Car         |        |       World       |
+-------------------+        +-------------------+
| - currentSpeed    |        | - colliders_      |
| - wheelRotation_  |        | - powerUps_       |
| - speedMultiplier |        +-------------------+
| - sizeMultiplier  |------> | +getPowerUps()    |
+-------------------+        | +getColliders()   |
| +update(dt)       |        +-------------------+
| +getBoundingBox() |
| +resetState()     |
+-------------------+

+--------------------------------------------+
|               Application                  |
+--------------------------------------------+
| - Car car;                                 |
| - World world;                             |
| - PerspectiveCamera camera;                |
| - GLRenderer renderer;                     |
| +main loop:                                |
|    • update car                            |
|    • check collisions                      |
|    • handle powerups                       |
|    • update camera                         |
|    • draw UI                               |
+--------------------------------------------+

## Bygging av prosjektet:
### Krav
- CMake 3.20+
- C++ 20 compiler
- OpenGL system (Windows, macOS, Linux)

### Instruksjoner:

#### I terminal for rotmappa: 

cmake -S . -B build

cmake --build build

#### Kjør programmet:

./build/bilsimulator

#### Kjør tests:

./build/bilsimulator_tests

------------------------------------------

## Refleksjoner

### Hva som gikk bra?

Git versjonskontroll hjalp meg med å holde kontroll på de forrige versjonene av prosjektet.
Å ha en backup er veldig viktig i tilfelle programmet blir ødelagt, eller det oppstår en bug som krever endring av større deler av kode.
Det gjør også mulig å jobbe på flere maskiner samtidig.

Threepp har gjort det enkelt å lage rendering og håndtering av objekter. 
Er også enkelt å importere 3D modeller og justere på de etter krav. 

Inputs, kamera, fysikk til bilen ble veldig bra og intuitivt, samt AABB kollisjoner.

Powerup systemet har fungert som det skal, i tillegg er det enkelt 
å lage forskjellige typer, som endrer på de ulike egenskapene av bilen eller verden.

Jeg synes det gikk spesielt bra med CMake filhåndteringen som gjør det enkelt
å koble sammen filene og automatisk laste ned de nødvendige filene via FetchContent,
som gjør det mulig å bygge prosjektet på de fleste systemene veldig enkelt.

For å lage, og justere 3D modeler, har jeg brukt Blender. Modelene er gratis til å bruke i non-commercial prosjekter.
Modellene var endret etter behov. Vei-systemet var koblet sammen og skiftet farger.
For å lage roterende hjul, så måtte jeg koble av hjulene fra bilen og importere
karrosseri og hjul separat.

### Hva var vanskelig?

Det vanskeligste å få til var fysikken i simulatoren. Før AABB, har jeg prøvd å implementere
bullet-physics i prosjektet. Bullet gir mange muligheter
til å lage realistisk fysikk som bodyroll, friksjon osv.
Jeg har fått til å laste den ned gjennom CMake filen
og koble den til bilen og andre objekter, problemet kom når jeg prøvde å
kjøre programmet. Hver gang jeg har kjørt programmet, så ble fysikken til bilen 
veldig tilfeldig uansset justeringer. Det var ikke en regelmessig problem fordi bilen
oppførte seg forkjellig på hver start av program. Jeg har prøvd å fikse problemet ved
å skifte på de forskjellige variablene som f.eks. spawnpoint til bilen, bilens hitbox, verdens hitbox osv.
Til slutt har eg bestemt meg å skifte til en enklere fysikk system.

### Hva kunne bli bedre? 

Fysikken i denne prosjektet var veldig enkel. Jeg kunne brukt mer tid til debugging og fått til Bullet
i programet, som hadde gitt meg mulighet til å lage realistisk fysikk til bilen og andre objekter.

Styring av bilen kunne bli forbedret ved lage den mer dynamisk. På ekte biler
er styringen redusert ved høye hastigheter pga. lav friksjon. Jeg har lagd
en liten dynamisk kode men den fungerer bare når "currentspeed" er høyere en 20, da blir
"maxsteering" halvert og bilen snur ikke så brått som på lave hastigheter. Her er det mye potensial til
forbedring ved å gradvis redusere "maxsteer" når farten øker.

Har også tenkt å lage flere tester for de forskjellige deler av koden for bedre feilsøking og endringer.

Det er mange ting som man kan legge til og endre på slik at  simulatoren blir bedre og mer behagelig og spille.
Noen av de som jeg har tenkt på er: lyd til bilen, collectible coins, forskjellige 3D modeller av biler, flere verden,
flere powerups osv... 

## Hjelpemidler og kilder

### KI
I dette prosjektet har jeg brukt hjelpemidler som "Clion code suggestion" og ChatGPT(5.1) for å lære og som støtte til:

- feilsøking
- forslag til forbedringer
- generering av enkelte kodeutkast for ting som er nytt for meg, spesielt:
- oppsett av Catch2-testfilen (src/tests/test_car.cpp)
- enkel ImGui-HUD (src/application.cpp)
- enkel AABB kollisjon  (src/world.cpp)

Jeg har tilpasset kodeutkastene til egen struktur og navn, integrert alt manuelt i prosjektet
og sørget for å forstå koden før jeg har brukt den. Bilfysikk, følge-kamera, lys og implementering av 3D modeller
er gjort av meg med feilsøking og litt forslag fra KI som støtte.

### 3D Modeler
Alle 3D modeller i denne prosjektet er hentet fra nett, de er gratis for å bruke i "non commercial projects".

Hindringer: https://www.turbosquid.com/3d-models/concrete-barriers-pack-3d-model-2379024

Vei: https://www.turbosquid.com/3d-models/free-blend-model-street-roads/986319

Hus: https://www.turbosquid.com/3d-models/a-frame-house-3d-1706368

Bil: https://www.turbosquid.com/3d-models/3d-model-stylized-cartoon-car-free-free-1778654

Modellene var justert og skalert i Blender før import
### Kilder 
Disse nettsidene ble brukt for å hente informasjon om hvordan forskjellige bibliotek fungerer og hvordan man bruker de. 

https://github.com/markaren/threepp: Brukt for å sjekke ut diverse demoer og koden bak de.

https://threejs.org/: Brukt for tutorials for threepp



















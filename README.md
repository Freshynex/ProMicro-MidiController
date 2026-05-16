# Pro Micro Midi Controller

## 1. A projekt bemutatása
### 1.1 Általános leírás
Ez a projekt a zenekészítés/zeneszerzés folyamatát segíti a MIDI protokoll felhasználásával.
Ennek a készüléknek a felépítése különböző feladatott ellátó alrészekből áll.

### 1.2 Vezérlő és modulok

<img alt="promicropinout.png" data-hpc="true" src="https://github.com/Freshynex/ProMicro-MidiController/blob/main/promicropinout.png?raw=true" style="max-width: 100%;">

A legfőbb része egy Atmel ATMEGA32u4 ic-vel ellátott Arduino Pro Micro fejlesztői alaplap (továbbiakban MCU).
Az MCU-t egy kifejtő-kártyára helyezve a többi alrész (továbbiakban modul) ráköthető.
A következő modulok lesznek alapból integrálva a készülékben:

 - Kijelző modul (továbbiakban <b>DISP</b>), amely egy egyszerű interfészt biztosít, hogy az aktuális bemeneti
értékeket visszaadja, illetve hogy kényelmesen lehessen konfigurálni azt.

 - Oktáv-váltó modul (továbbiakban <b>OCTSTP</b>), amely egyszerű elérhetőséget ad oktáv-léptetéshez
  
 - Analóg modul (továbbiakban <b>ANAL</b>), egy olyan felület amely kihasználja az MCU saját analog bemeneteit, és
visszaad egy értéket.

 - Billentyűzet modul (továbbiakban <b>KB</b>), amely egy 2 oktávos zongora interfészt ad a felhasználónak. 

## 2. Alkatrészek

MCU: Arduino Pro Micro Atmel ATMEGA32u4
DISP: 128x64 i2c oled display (SSD1306 driver)
Oktáv váltó gombok: TACTS-6x6x7.3 (2 db)
KB gombok: AKKO Stellar Rose Switch – Lubed-5pin (24 db)
Tengelyes potméter: POM1615 LIN 1K B (2 db)
Toló potméter: PTA2043-2015CIB203 (2 db)
PISO shift register: 74HC166 (3 db)

### 2.1 Kapcsolás rajz

<img alt="Schematic.png" data-hpc="true" src="https://github.com/Freshynex/ProMicro-MidiController/blob/main/Schematic.png?raw=true" style="max-width: 100%;">

## 3. Funkcionalitás

A MIDI protokollban 8 bites adatcsomagokkal dolgozunk. Két féle üzenet van: command és data. Az első bit azt jelöli, hogy a kettő közül melyiket küldjük, így adat üzenet esetén 0-127, command üzenet esetén 128-255 a használható értéktartomány. Ezek együttesen alkotnak egy midi üzenetet (message).
Egy üzenet mindig command byte-tal kezdődik, amit a parancs függvényében 0, 1 vagy 2 data byte követ.
Mi esetünkben a fontosabb üzenetek:
<li>
 <ul>
  NoteOn, NoteOff: 
 </ul>
</li>

## Software used for testing
FL Studio 20

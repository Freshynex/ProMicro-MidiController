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

A MIDI protokollban 8 bites adatcsomagokkal dolgozunk. Két féle üzenet van: status és data. Az első bit azt jelöli, hogy a kettő közül melyiket küldjük, így adat üzenet esetén 0-127, command üzenet esetén 128-255 a használható értéktartomány. Ezek együttesen alkotnak egy midi üzenetet (message).

Egy üzenet mindig status byte-tal kezdődik, amit a parancs függvényében 0, 1 vagy 2 data byte követ.
A status byte felső 4 bitje jelöli a kiadott parancsot (aminek az első bitje mindig 1, így az első kódszó decimálisan a 8), a második fele a csatornát jelöli (16 csatornát lehet kezelni, ebben a projektben egynél többre nincs szükségünk).
Számunkra fontosabb status üzenetek:
<ul style="list-style:none;">
 <li>
  NoteOn "hang lejátszás"
  Üzenet felépítése: 
  <ul style="list-style:none;">
   <li>  
    1 status byte (parancs: 0x90)<br> 2 adat byte (pitch - melyik hang, velocity - "lenyomás ereje")
    </li>
  </ul>
 </li>
 <li>
  NoteOff "hang leállítás"
  Üzenet felépítése: 1 status byte (parancs: 0x80), 2 adat byte (pitch, velocity)
 </li>
 <li>
  Control Change "paraméter állítás"
  Üzenet felépítése: 1 status byte (parancs: 0xB0), 1 vagy 2 adat byte (MSB, LSB) attól függően, hogy 7 vagy 14 bit precizitással szeretnénk küldeni az értéket
 </li>
</ul>

## Szoftver a teszteléshez
FL Studio 20

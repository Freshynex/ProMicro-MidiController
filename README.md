# Pro Micro Midi Controller

## 1. A projekt bemutatása
### 1.1 Általános leírás
Ez a projekt a zenekészítés/zeneszerzés folyamatát segíti a MIDI protokoll felhasználásával.
Ennek a készüléknek a felépítése különböző feladatott ellátó alrészekből áll.

### 1.2 Vezérlő és modulok

<img alt="promicropinout.png" data-hpc="true" src="https://github.com/Freshynex/ProMicro-MidiController/blob/main/promicropinout.png?raw=true" style="max-width: 50%;">

A legfőbb része egy Atmel ATMEGA32u4 ic-vel ellátott Arduino Pro Micro fejlesztői alaplap (továbbiakban MCU).
Az MCU-t egy kifejtő-kártyára helyezve a többi alrész (továbbiakban modul) ráköthető.
A következő modulok lesznek alapból integrálva a készülékben:

 - Kijelző modul (továbbiakban <b>DISP</b>), amely egy egyszerű interfészt biztosít, hogy az aktuális bemeneti
értékeket visszaadja, illetve hogy kényelmesen lehessen konfigurálni azt.

 - Oktáv-váltó modul (továbbiakban <b>OCTSTP</b>), amely egyszerű elérhetőséget ad oktáv-léptetéshez
  
 - Analóg modul (továbbiakban <b>ANAL</b>), egy olyan felület amely kihasználja az MCU saját analog bemeneteit, és
visszaad egy értéket.

 - Billentyűzet modul (továbbiakban <b>KB</b>), amely egy 2 oktávos zongora interfészt ad a felhasználónak. 

## 2. Alkatrészek és kapcsolás

### 2.1 Főalkatrészek

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

### 3.1 MIDI Protokoll

A MIDI protokollban 8 bites adatcsomagokkal dolgozunk. Két féle üzenet van: status és data. Az első bit azt jelöli, hogy a kettő közül melyiket küldjük, így adat üzenet esetén 0-127, command üzenet esetén 128-255 a használható értéktartomány. Ezek együttesen alkotnak egy midi üzenetet (message).

Egy üzenet mindig status byte-tal kezdődik, amit a parancs függvényében 0, 1 vagy 2 data byte követ.
A status byte felső 4 bitje jelöli a kiadott parancsot (aminek az első bitje mindig 1, így az első kódszó decimálisan a 8), a második fele a csatornát jelöli (16 csatornát lehet kezelni, ebben a projektben egynél többre nincs szükségünk).

Számunkra fontosabb status üzenetek:

| Status üzenet  | Üzenet kódja | Adat byte 1 | Adat byte 2 | Adat byte 3 |
| -------------  | ------------ | ----------- | ----------- | ----------- |
| NoteOn         | 0x90         | Pitch       | Velocity    |             |
| NoteOff        | 0x80         | Pitch       | Velocity    |             |
| Control Change | 0xB0         | CC number   | MSB         | LSB *       | 

 *<sub> Adat byte 3 (LSB) nem kötelező, de 7 helyett 14 bit pontosságu értéket adhatunk meg vele </sub>

### 3.2 Billentyűzet

A billentyűzet kialakítása egy zongora mintájára készült. 2 oktáv, ami 24 gomb. A gombok mechanikus billentyűkapcsolókkal vannak kivitelezve. A keret illetve gomb kupakok 3D nyomtatással készültek. Három kaszkádolt parallel in serial out shift register segítségével vannak az MCU-ra kötve, így csak 3 pin felhasználása szükséges. Ez a fajta megoldás lehetővé teszi a felhasználónak hogy 3Byte-on visszakapja a gombok állapotát egyszerre.

### 3.3 Oktáv váltás

A zenekészítéshez elengedhetetlenül szükséges hogy a billentyűzet képes legyen több oktávon is játszani. Ehhez az <b>MCU</b> két digitális bemenetét használjuk. A készülék oktáv-váltó gombok hatására a billentyűzeten reprezentált 2 oktávot eltolja, és így a felhasználónak sokkal több hang lesz hozzáférhető.

### 3.4 Analog bevitel

Kettő forgatható és kettő tolható lineáris potméter segítségével, kihasználva Kirchoff törvényeit, képes a készülék adott csatornák állapotait módosítani. A választott <b>MCU</b> analog bemenetei a referencia feszültséghez képest adja vissza a bemeneti értéket. Ezt a készülék 0-1023-as intervallumban elhelyezi (map-olja).

Ez a következő képpen nyílvánul meg:
| Ref. V         | ANAL bemenet | Érték       |
| -------------  | ------------ | ----------- |
|       5V       | 0V           | 0           |
|       -"-      | 2.5V         | 512         |
|       -"-      | 5V           | 1023        |

### 3.5 i²c Kijelző és protokol

Az i²c, egy olyan protokoll, amely MASTER-SLAVE kapcsolatot biztosít <u>kettő vagy több</u> eszköz között. A szállítandó adatot a protokoll címkézi és szállítja az adatvonalon.
Amikor a SLAVE címe egyezik a csomagban elhelyezett célcímmel, akkor azt a SLAVE fogadja és feldogozza. 

A készülék rendelkezik egy kis 1.2"-es OLED kijelző, amely SSD1306-os meghajtású. Jelenleg a <b>DISP</b> beépített címmel rendelkezik, amelynek a módosítása nehézkes.
A mostani címe <b>0x78</b>.

## 4. Bővithetőség
### 4.1 i²c

Az <b>MCU</b>-nak nem az összes pin-jét használtuk, ezért ezekre még kapcsolhatóak eszközök. Amennyiben a beépítendő eszköz i²c-vel kommunikál, így az ráközhető párhuzamosan ugyanarra az ágra, amelyiken rajta van a <b>DISP</b>. Ebben az esetben érdemes figyelni a rákötendő eszköz <u><b>címére</b></u>. Amennyiben a címeknél ütközés történik, úgy nem fog működik a ráhelyezett eszköz, illetve a <b>DISP</b>.

Lehetséges bővítési lehetőségek a jelzett protokollon:
| IC / MODUL név | Funkció                  |
| -------------  | ------------------------ |
|  ADS1115 / -M  | ANAL bővítő (A0 - A3)    |
|PCF8574 / IN830S| DIGITAL bővítő (8db, be) |
|PCF8574 / OC805S| DIGITAL bővítő (8db, ki) |

### 4.2 Nem használt pin-ek


### 4.3 MCU csere



### 4.4 UART (Soros kommunikáció)

Az <b>UART</b> (<b>U</b>niversal <b>A</b>synchronous <b>R</b>eceiver‑<b>T</b>ransmitter) egy olyan kommunikációs protokoll, amelynél két egyenrangú eszköz kétírányú, <b>duplex</b> kapcsolatot alakít ki. Mind a kettő eszközön egy előre definiált közös bit-sebességet (<b>baudrate</b>-et) kell használni. Az közlendő csomagot egy start és stop bit-ekkel határolt üzenetbe építi be az egyik eszköz, és <b>Tx</b> (Transmitt) ágra küldi. Ezt a csomagot a fogadó fél az <b>Rx</b> ágán kapja.

Így ez a fajta kiegészítési mód tudja kínálni a legtöbb lehetőséget, mivel így a már használt <b>MCU</b>-hoz köthető egy másik <b>MCU</b>, amely rendelkezik a saját beállításaival és saját moduljaival.

Bekötése a következő:
|    A eszköz    |   B eszköt   |
| -------------  | ------------ |
|     <b>G</b>   |    <b>G</b>  |
|        Tx      |       Rx     |
|        Rx      |       Tx     |
|        +*      |       +*     |

*<sub>Amennyiben nem akarunk vinni tápellátást protokoll-on, így a + kihagyható</sub>

## 5. Készülék Prezentáció és Demo


## Szoftver a teszteléshez
FL Studio 20

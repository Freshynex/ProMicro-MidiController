

# Pro Micro Midi Controller

## 1. A projekt bemutatása
### 1.1 Általános leírás
Ez a projekt a zenekészítés/zeneszerzés folyamatát segíti a MIDI protokoll felhasználásával.
Ennek a készüléknek a felépítése különböző feladatott ellátó alrészekből áll.

### 1.2 Vezérlő és modulok

<img alt="promicropinout.png" data-hpc="true" src="https://github.com/Freshynex/ProMicro-MidiController/blob/main/promicropinout.png?raw=true" style="height: 50%;width: 50%">

A legfőbb része egy Atmel ATMEGA32u4 ic-vel ellátott Arduino Pro Micro fejlesztői alaplap (továbbiakban MCU).
Az MCU-t egy kifejtő-kártyára helyezve a többi alrész (továbbiakban modul) ráköthető.

A kifejtőkártya:

<img alt="carrier board" data-hpc="true" src="https://github.com/Freshynex/ProMicro-MidiController/blob/main/_pics/CR/jpg/carrier/cb_layoutcheck1.jpg" style="height: 25%;width: 25%">

A következő modulok vannak integrálva a készülékben:

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
 Teszteléshez használt szoftver: FL Studio 20 (Macbook M1-en kicsit bugos, ezért a kép villódzik néha)

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
<img alt="Keyboard" data-hpc="true" src="https://github.com/Freshynex/ProMicro-MidiController/blob/main/_pics/CR/jpg/modules/keyboard/1_kb_module_layout6.jpg" style="width:328px;height:530px;">
A billentyűzet kialakítása egy zongora mintájára készült. 2 oktáv, ami 24 gomb. A gombok mechanikus billentyűkapcsolókkal vannak kivitelezve. A keret illetve gomb kupakok 3D nyomtatással készültek. Három kaszkádolt parallel in serial out shift register segítségével vannak az MCU-ra kötve, így csak 3 pin felhasználása szükséges. Ez a fajta megoldás lehetővé teszi hogy 3Byte-on visszakapjuk a gombok állapotát egyszerre.

### 3.3 Oktáv váltás
<img alt="Octave buttons" data-hpc="true" src="https://github.com/Freshynex/ProMicro-MidiController/blob/main/_pics/CR/jpg/modules/octave_step/0_octstep_module_layoutconnection1.jpg" style="width:328px;height:530px;" >
A zenekészítéshez szükséges, hogy a billentyűzet képes legyen több oktávon is játszani. Ehhez az <b>MCU</b> két digitális bemenetét használjuk. A készülék oktáv-váltó gombok hatására a billentyűzeten reprezentált 2 oktávot eltolja, és így mind a 10 MIDI protokollban használt oktáv elérhető.

### 3.4 Analog bevitel
<img alt="Potmeters" data-hpc="true" src="https://github.com/Freshynex/ProMicro-MidiController/blob/main/_pics/CR/jpg/modules/analog/0_anal_module_rotconnection2.jpg" style="width:328px;height:530px;" >
Kettő forgatható és kettő tolható lineáris potméter segítségével, kihasználva Kirchoff törvényeit, képes a készülék adott csatornák állapotait módosítani. A választott <b>MCU</b> analog bemenetei a referencia feszültséghez képest adja vissza a bemeneti értéket. Ezt a készülék 0-1023-as intervallumban elhelyezi (map-eli).

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
### 4.1 I²C (Inter-Integrated Circuit)

Az <b>MCU</b>-nak nem az összes pin-jét használtuk, ezért ezekre még kapcsolhatóak eszközök. Amennyiben a beépítendő eszköz i²c-vel kommunikál, így az ráközhető párhuzamosan ugyanarra az ágra, amelyiken rajta van a <b>DISP</b>. Ebben az esetben érdemes figyelni a rákötendő eszköz <u><b>címére</b></u>. Amennyiben a címeknél ütközés történik, úgy nem fog működik a ráhelyezett eszköz, illetve a <b>DISP</b>.

Lehetséges bővítési lehetőségek a jelzett protokollon:
| IC / MODUL név | Funkció                  |
| -------------  | ------------------------ |
|  ADS1115 / -M  | ANAL bővítő (A0 - A3)    |
|PCF8574 / IN830S| DIGITAL bővítő (8db, be) |
|PCF8574 / OC805S| DIGITAL bővítő (8db, ki) |

### 4.2 UART (Universal Asynchronous Receiver-Transmitter)

Az UART egy olyan soron protokoll fajta, amely két egyenrangú eszközt köt össze. Bekötéséhez egy adó, egy fogadó és közös test szükséges. Ez a protokoll a közlendő információt egy start- és stop-bittel ellátott bitfolyammá alakítja.
Ehhez a fajta kapcsolathoz nem szükséges külön órajel, mivel a kommunikációt egy közösen előre deklarált jelaránnyal (baudrate) éri el. A Küldő fél a bitfolyamot a saját Tx (Transmit) ágán küldi a másik eszköz Rx (Receive lábára.
Ez igaz fordítottan is, ezért ez a fajta megoldás Full-Duplex-nek minősül.
| A eszköz | B eszköz |
|    Tx    |    Rx    |
|    Rx    |    Tx    |
|    G     |    G     |
|    Vcc   |    Vcc   |*



*<sub>Ha a másik készülék rendelkezik saját táp-ellátással, akkor a Vcc összekötése nem szükséges.</sub>

## 5. Készülék Prezentáció és Demo

### Billentyűzet
https://github.com/user-attachments/assets/6a722bac-2a85-4638-812b-2556ad7aba79

### Potméter
[<video src="_vids/Pot_demo.mp4" width="320" height="240" controls></video>
](https://github.com/user-attachments/assets/77403d33-6a48-4602-aa05-ce928a655359
)
### Billentyűzet, potméter (pitch paraméter állítása), oktáv váltás
https://github.com/user-attachments/assets/33ad1122-0a03-4d51-aa62-576f443cc43e


